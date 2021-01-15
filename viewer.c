#include <stdio.h>

#define BLK     "\x1B[30m"      // Black
#define RED     "\x1B[31m"      // Red
#define GRN     "\x1B[32m"      // Green
#define YEL     "\x1B[33m"      // Yel
#define BLU     "\x1B[34m"      // Blue
#define MAG     "\x1B[35m"      // Magenta
#define CYA     "\x1B[36m"      // Cyan
#define WHT     "\x1B[37m"      // White

#define BBLK    "\x1B[30;1m"    // Bright black
#define BRED    "\x1B[31;1m"    // Bright red
#define BGRN    "\x1B[32;1m"    // Bright green
#define BYEL    "\x1B[33;1m"    // Bright yellow
#define BBLU    "\x1B[34;1m"    // Bright blue
#define BMAG    "\x1B[35;1m"    // Bright magenta
#define BCYA    "\x1B[36;1m"    // Bright cyan
#define BWHT    "\x1B[37;1m"    // Bright white

#define RESET   "\x1B[0m"

// Choose your four colours here
const char *colours[] = {
    BRED, BGRN, BBLU, BYEL
};

int main(int argc, char *argv[])
{
    if (argc != 2) return fprintf(stderr, "Usage: %s grid\n", *argv), 1;

    FILE *fp;
    if (!(fp = fopen(argv[1], "r")))
        return fprintf(stderr, "Could not open file\n"), 1;

    for (char c; (c = fgetc(fp)) != EOF; ) {
        switch (c) {
            case '0': case '1':
            case '2': case '3':
                printf("%s%c%s", colours[c - '0'], c, RESET);
                break;

            default:
                putchar(c);
                break;
        }
    }

    fclose(fp);
    return 0;
}
