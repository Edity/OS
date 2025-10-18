#include <unistd.h>

int main() {
    char c;
    while (read(STDIN_FILENO, &c, 1) > 0) {
        if(c <= 'z' && c >= 'a'){
            c = c + 'A' - 'a';
        }
        write(STDOUT_FILENO, &c, 1);
    }
    return 0;
}