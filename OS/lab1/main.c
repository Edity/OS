#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

const char* error_msg1 = "ERROR_PIPE\n";
const char* error_msg2 = "ERROR_CHILD\n";
const char* error_msg3 = "ERROR_DUP\n";
const char* prompt_msg = "Введите строку: ";

int main() {
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1) {
        write(STDERR_FILENO, error_msg1, strlen(error_msg1));
        exit(-1);
    }
    if (pipe(pipe2) == -1) {
        write(STDERR_FILENO, error_msg1, strlen(error_msg1));
        exit(-1);
    }
    pid_t child1 = fork();
    if (child1 == -1) {
        write(STDERR_FILENO, error_msg2, strlen(error_msg2));
        exit(-1);
    }
    if (child1 == 0) {
        close(pipe1[1]);
        close(pipe2[0]);
        int pipe3[2];
        if (pipe(pipe3) == -1) {
            write(STDERR_FILENO, error_msg1, strlen(error_msg1));
            exit(-1);
        }
        pid_t child2 = fork();
        if (child2 == -1) {
            write(STDERR_FILENO, error_msg2, strlen(error_msg2));
            exit(-1);
        }
        if (child2 == 0) {
            close(pipe3[0]);
            close(pipe2[1]);
            if (dup2(pipe1[0], STDIN_FILENO) == -1) {
                write(STDERR_FILENO, error_msg3, strlen(error_msg3));
                exit(-1);
            }
            if (dup2(pipe3[1], STDOUT_FILENO) == -1) {
                write(STDERR_FILENO, error_msg3, strlen(error_msg3));
                exit(-1);
            }
            execl("./child1", "child1", NULL);
            write(STDERR_FILENO, error_msg2, strlen(error_msg2));
            exit(-1);
        } else {
            close(pipe1[0]);
            close(pipe3[1]);
            if (dup2(pipe3[0], STDIN_FILENO) == -1) {
                write(STDERR_FILENO, error_msg3, strlen(error_msg3));
                exit(-1);
            }
            if (dup2(pipe2[1], STDOUT_FILENO) == -1) {
                write(STDERR_FILENO, error_msg3, strlen(error_msg3));
                exit(-1);
            }
            execl("./child2", "child2", NULL);
            write(STDERR_FILENO, error_msg2, strlen(error_msg2));
            exit(-1);
        }
    } else {
        close(pipe1[0]);
        close(pipe2[1]);
        write(STDOUT_FILENO, prompt_msg, strlen(prompt_msg));
        char c;
        int bytes_read;
        while ((bytes_read = read(STDIN_FILENO, &c, 1)) > 0) {
            write(pipe1[1], &c, 1);
            read(pipe2[0], &c, 1);
            write(STDOUT_FILENO, &c, 1);
            
            if (c == '\n') {
                break;
            }
        }
        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL);
    }
    
    return 0;
}