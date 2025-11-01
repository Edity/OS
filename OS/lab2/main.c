#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

typedef struct {
    int id;
    char *text;
    char *pattern;
    int patternLength;
    int startIndex;
    int endIndex;
} ThreadArgs;

double getRealTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

void *work(void *_args) {
    ThreadArgs *args = (ThreadArgs *)_args;

    for (int i = args->startIndex; i <= args->endIndex - args->patternLength; i++) {
        int found = 1;
        for (int j = 0; j < args->patternLength; j++) {
            if (args->text[i + j] != args->pattern[j]) {
                found = 0;
                break;
            }
        }
    }

    return NULL;
}

void classic(char *text, char *pattern, int patternLength, int textLength) {
    for (int i = 0; i <= textLength - patternLength; i++) {
        int found = 1;
        for (int j = 0; j < patternLength; j++) {
            if (text[i + j] != pattern[j]) {
                found = 0;
                break;
            }
        }
    }
}

void generateRandomText(char *text, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";
    for (int i = 0; i < length; i++) {
        text[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    text[length] = '\0';
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Ошибка некорректного ввода\n");
        exit(EXIT_FAILURE);
    }

    int textLength = atoi(argv[1]);
    int patternLength = atoi(argv[2]);
    int threadsCount = atoi(argv[3]);

    if (patternLength > textLength || threadsCount <= 0 || textLength <= 0) {
        printf("Ошибка некорректных параметров\n");
        return 1;
    }

    char *text = (char *)malloc((textLength + 1) * sizeof(char));
    char *pattern = (char *)malloc((patternLength + 1) * sizeof(char));

    if (!text || !pattern) {
        printf("Ошибка выделения памяти\n");
        return 1;
    }

    srand(time(NULL));
    generateRandomText(text, textLength);
    
    int patternStart = rand() % (textLength - patternLength);
    strncpy(pattern, text + patternStart, patternLength);
    pattern[patternLength] = '\0';

    double startTime, endTime;

    startTime = getRealTime();
    classic(text, pattern, patternLength, textLength);
    endTime = getRealTime();
    double timeResult1 = endTime - startTime;

    startTime = getRealTime();

    pthread_t *threads = (pthread_t *)malloc(threadsCount * sizeof(pthread_t));
    ThreadArgs *thread_args = (ThreadArgs *)malloc(threadsCount * sizeof(ThreadArgs));

    if (!threads || !thread_args) {
        printf("Ошибка выделения памяти для потоков\n");
        return 1;
    }

    int range = textLength / threadsCount;

    for (int i = 0; i < threadsCount; i++) {
        thread_args[i].id = i;
        thread_args[i].text = text;
        thread_args[i].pattern = pattern;
        thread_args[i].patternLength = patternLength;
        
        thread_args[i].startIndex = i * range;
        if (i == threadsCount - 1) {
            thread_args[i].endIndex = textLength - patternLength + 1;
        } else {
            thread_args[i].endIndex = (i + 1) * range;
        }
        
        if (thread_args[i].endIndex > textLength - patternLength + 1) {
            thread_args[i].endIndex = textLength - patternLength + 1;
        }

        if (pthread_create(&threads[i], NULL, work, &thread_args[i]) != 0) {
            printf("Ошибка создания потока\n");
            return 1;
        }
    }

    for (int i = 0; i < threadsCount; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Ошибка завершения потока\n");
            return 1;
        }
    }

    endTime = getRealTime();
    double timeResult2 = endTime - startTime;

    printf("Последовательное время: %.5f мс\n", timeResult1);
    printf("Параллельное время: %.5f мс\n", timeResult2);
    
    if (timeResult2 > 0) {
        double speedup = timeResult1 / timeResult2;
        double efficiency = (speedup / threadsCount) * 100;
        printf("Ускорение: %.2f\n", speedup);
        printf("Эффективность: %.2f%%\n", efficiency);
    }

    free(text);
    free(pattern);
    free(threads);
    free(thread_args);

    return 0;
}