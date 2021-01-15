#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define GRID_DEFAULT 11
#define THREAD_COUNT 4

#ifndef SERIAL
# include <omp.h>
#endif

#define RAND(x,y)(rand() % (y + 1 - x) + x)

typedef struct coord_struct {
    int x;
    int y;
} Coord;

typedef struct stack_struct {
    int nel;
    int cap;
    Coord data[];
} Stack;

Stack *create_stack(int size);          // Allocate stack
void destroy_stack(Stack *stack);       // De-allocate stack
void push(Stack **stack, Coord coord);  // Push coord to stack
Coord pop(Stack *stack);                // Pop coord from stac
int empty(Stack *stack);                // Check if stack empty

char **create_grid(size_t size);                        // Allocate grid of points
void destroy_grid(char **grid, size_t size);            // De-allocate grid of points
void print_grid(char **grid, size_t size);              // Print grid of points
void paint_grid(char **grid, Coord coord, char value);  // Update a single point on grid
int peek(char **grid, size_t size, int x, int y);       // Look at specific point on grid
void get_children(Coord coord, Coord children[4]);      // Get four neighbours of point
int coord_equals(Coord a, Coord b);                     // Check if two coords are equal
Coord get_fill(Coord origin, Coord destination);        // Get the inner coordinate of point

int main(int argc, char *argv[])
{
    int grid_size = GRID_DEFAULT;

    int opt;
    while ((opt = getopt(argc, argv, ":s:n:")) != -1) {
        switch (opt) {
            case 's':
                srand(atoi(optarg));
                break;
            case 'n':
                grid_size = atoi(optarg);
                break;
            case ':':
                printf("option needs a value\n");
                break;
            case '?':
                printf("unknown argument specified\n");
                break;
        }
    }

    // Create our central grid
    char **grid = create_grid(grid_size);

    // Keep track of claims only in parallel
    #ifndef SERIAL
    int score[4] = {0, 0, 0, 0};
    #endif

    // Start each thread
    #ifndef SERIAL
    # pragma omp parallel num_threads(THREAD_COUNT)
    #endif
    {
        // Create one stack per thread
        Stack *stack = create_stack(1);

        // Discriminate between threads
        #ifdef SERIAL
        int my_rank = 0;
        #else
        int my_rank = omp_get_thread_num();
        #endif

        // Starting points
        Coord starting;
        switch (my_rank) {
            case 0:
                starting.x = 1;
                starting.y = 1;
                break;

            case 1:
                starting.x = grid_size - 2;
                starting.y = 1;
                break;

            case 2:
                starting.x = grid_size - 2;
                starting.y = grid_size - 2;
                break;

            case 3:
                starting.x = 1;
                starting.y = grid_size - 2;
                break;
        }

        // Push starting position to stack
        push(&stack, starting);

        // Critical section: painting grid
        #ifndef SERIAL
        # pragma omp critical
        #endif
        paint_grid(grid, starting, my_rank + '0');

        // Count 
        int tally = 0;

        // Start working with stack
        while (!empty(stack)) {

            // Pop an item out of the stack
            Coord popped = pop(stack);

            // Randomly visit each of the four neighbours
            Coord children[4] = {{0}, {0}, {0}, {0}};
            get_children(popped, children);

            // Attempt to claim neighbours
            for (int i = 0; i < 4; i++) {
                int scope;

                // Looking at the grid is a critical section
                #ifndef SERIAL
                # pragma omp critical
                #endif
                scope = peek(grid, grid_size, children[i].x, children[i].y);

                // If free space
                if (scope == '.') {
                    tally++;

                    // Paint the point along with its children
                    #ifndef SERIAL
                    # pragma omp critical
                    #endif
                    {
                        paint_grid(grid, children[i], my_rank + '0');
                        paint_grid(grid, get_fill(popped, children[i]), my_rank + '0');
                    }

                    // For every successful visit, push
                    push(&stack, children[i]);
                }
            }
        }

        // Housekeeping
        destroy_stack(stack);

        #ifndef SERIAL
        // Send claimed count
        score[my_rank] = tally;
        #endif
    }

    // Print grid
    print_grid(grid, grid_size);

    #ifndef SERIAL
    // Print claimed neighbours
    for (int i = 0; i < 4; i++)
        printf("process %d count: %d\n", i, score[i]);
    #endif

    // Housekeeping
    destroy_grid(grid, grid_size);

    return 0;
}

Stack *create_stack(int size) {
    // Allocate and initialize stack
    if (size < 1) return NULL;
    Stack *stack = calloc(1, sizeof(Stack) + (sizeof(Coord) * size));
    stack->cap = size;
    return stack;
}

void destroy_stack(Stack *stack) {
    free(stack);
}

void push(Stack **stack, Coord coord) {
    // Double capacity of stack if limit reached
    if ((*stack)->nel == (*stack)->cap) {
        int new_cap = (*stack)->cap * 2;
        *stack = realloc(*stack, sizeof(Stack) + (sizeof(Coord) * new_cap));
        (*stack)->cap = new_cap;
    }

    // Write data then increment element count
    (*stack)->data[(*stack)->nel].x = coord.x;
    (*stack)->data[(*stack)->nel].y = coord.y;

    (*stack)->nel++;
}

Coord pop(Stack *stack) {
    // Write coord then decrement element count
    Coord result = {stack->data[stack->nel - 1].x, stack->data[stack->nel - 1].y};
    stack->nel--;
    return result;
}

int empty(Stack *stack) {
    return !stack->nel;
}

char **create_grid(size_t size) {
    char **grid = malloc(sizeof(char *) * size);

    for (size_t i = 0; i < size; i++) {
        grid[i] = malloc(size);
        memset(grid[i], '.', size);
    }

    return grid;
}

void destroy_grid(char **grid, size_t size) {
    for (size_t i = 0; i < size; i++)
        free(grid[i]);
    free(grid);
}


void print_grid(char **grid, size_t size) {
    puts("");
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++)
            printf("%c", grid[i][j]);
        puts("");
    }
}

void paint_grid(char **grid, Coord coord, char value) {
    grid[coord.x][coord.y] = value;
}

int peek(char **grid, size_t size, int x, int y) {
    // Only access grid if the x and y are in bounds
    if (x > 0 && x < (int)size - 1 && y > 0 && y < (int)size - 1)
        return grid[x][y];
    return -1;
}

void get_children(Coord coord, Coord children[4]) {
    // Create all four possible candidates. It doesn't matter
    // if they're valid or not (we use peek() to determine that)
    Coord candidates[] = {
        {coord.x - 2, coord.y},
        {coord.x, coord.y + 2},
        {coord.x + 2, coord.y},
        {coord.x, coord.y - 2},
    };

    int choice[] = {0, 0, 0, 0};

    int n = 0;
    // Choose randomly until all four choices have been made
    while (choice[0] + choice[1] + choice[2] + choice[3] != 4) {
        int pick = RAND(0, 3);
        if (choice[pick] == 0) {
            choice[pick] = 1;
            children[n].x = candidates[pick].x;
            children[n].y = candidates[pick].y;
            n++;
        }
    }
}

int coord_equals(Coord a, Coord b) {
    return a.x == b.x && a.y == b.y;
}

Coord get_fill(Coord origin, Coord destination) {
    // Return for North, South, East, West respectively
    if (coord_equals(origin, (Coord){destination.x - 2, destination.y}))
        return (Coord){destination.x - 1, destination.y};

    if (coord_equals(origin, (Coord){destination.x + 2, destination.y}))
        return (Coord){destination.x + 1, destination.y};

    if (coord_equals(origin, (Coord){destination.x, destination.y + 2}))
        return (Coord){destination.x, destination.y + 1};

    if (coord_equals(origin, (Coord){destination.x, destination.y - 2}))
        return (Coord){destination.x, destination.y - 1};
    fprintf(stderr, "get_fill point not related. Exiting.\n");
    exit(1);
}
