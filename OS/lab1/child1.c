#include <unistd.h>

int main() {
    char c;
    while (read(STDIN_FILENO, &c, 1) > 0) {
        if (c == ' ' || c == '\t') {
            c = '_';
        }
        write(STDOUT_FILENO, &c, 1);
    }
    return 0;
}