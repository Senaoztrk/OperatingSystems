#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct multiplePrintData {
    pthread_t id;
    int threadid;
    char *printText;
    double waitDuration;
};

void *printCharMultipleTimes(void *data) {
    struct multiplePrintData *pData = (struct multiplePrintData *)data;
    int count = 0;
    FILE *outputFile = fopen("output.txt", "a"); // "a" modu dosyanın sonuna ekleme yapar
    if (outputFile == NULL) {
        printf("Dosya açma hatası!");
        pthread_exit(NULL);
    }

    while (1) {
        count++;
        fflush(stdout);
        printf("Thread %d: %s\n", pData->threadid, pData->printText);
        fprintf(outputFile, "Thread %d: %s\n", pData->threadid, pData->printText);
        fflush(outputFile);
        usleep((int)(pData->waitDuration * 1000000));
    }

    fclose(outputFile);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 4 || (argc % 2) != 0) {
        printf("Usage: %s total_duration concurrent_tasks [print_text1 duration1 ...]\n", argv[0]);
        return 1;
    }

    int totalDuration = atoi(argv[1]);
    int numTasks = atoi(argv[2]);

    if (numTasks != (argc - 3) / 2) {
        printf("Number of tasks doesn't match the specified count.\n");
        return 1;
    }

    struct multiplePrintData tasks[numTasks];

    for (int i = 0; i < numTasks; i++) {
        tasks[i].threadid = i + 1;
        tasks[i].printText = argv[i * 2 + 3];
        tasks[i].waitDuration = atof(argv[i * 2 + 4]);
        pthread_create(&tasks[i].id, NULL, printCharMultipleTimes, (void *)&tasks[i]);
    }

    sleep(totalDuration);

    for (int i = 0; i < numTasks; i++) {
        pthread_cancel(tasks[i].id);
    }

    return 0;
}
