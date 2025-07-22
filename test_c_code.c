#include <stdio.h>

// This is a single-line comment in C
int main() {
    /*
     * This is a multi-line comment.
     * It spans across several lines.
     */
    int number = 10;
    if (number > 5) {
        printf("Number is greater than 5: %d\n", number);
    } else {
        printf("Number is 5 or less.\n");
    }
    for (int i = 0; i < 3; i++) {
        printf("Loop iteration: %d\n", i);
    }
    return 0;
}
