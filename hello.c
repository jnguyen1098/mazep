#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

int value = 0;

void Hello(void);
void Increment(void);

int main(int argc, char *argv[])
{
    int thread_count = strtol(argv[1], NULL, 10);

#pragma omp parallel num_threads(thread_count)
    Hello();

#pragma omp parallel num_threads(thread_count)
    Increment();

    printf("value is %d\n", value);

    int global_result = 0;

#pragma omp parallel num_threads(thread_count) reduction(+:global_result)
    global_result += 10;

    printf("global result is %d\n", global_result);

    // TODO: cannot use do-while or while loop. The number
    //       of iterations must be known. for (;;) is bad.

    // TODO: you cannot exit the loop early. The iterations
    //       must be known at the start of the for loop.
    //       break; return, etc. are not allowed here

    // TODO: the index must be an int or a pointer.

    // TODO: the start, end, increment maintain type

    // TODO: the start, end, and increment can't change

    // TODO: the index can only be modified by the increment

    // TODO: this is because only a special for loop can be
    //       parallelized in an easy manner.

    // TODO: you cannot use stuff you calculated earlier because
    //       in a parallel program, you can't rely on the order
#pragma omp parallel for num_threads(thread_count)
    for (int i = 0; i < 4; i++) {
        sleep(1);
    }

    int idk = 0;
#pragma omp parallel for num_threads(thread_count) reduction(+:idk)
    for (int i = 0; i < 400; i++) {
        idk += 10;
    }
    printf("idk = %d\n", idk);

    // default(none) reduction(+:sum)
    // private(k, factor) shared(n)

    return 0;
}

void Hello(void)
{
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();
    printf("Hello world from thread %d/%d\n", my_rank, thread_count);
}

void Increment(void)
{
#pragma omp critical
    for (int i = 0; i < 100; i++) {
        int tmp = value;
        tmp = tmp + 1;
        value = tmp;
    }
}
