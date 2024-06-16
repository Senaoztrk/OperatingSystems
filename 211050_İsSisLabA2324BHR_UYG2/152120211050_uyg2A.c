#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void log_to_file(char *message) {
    FILE *file = fopen("logs/log_all.log", "a");
    if (file != NULL) {
        fprintf(file, "%s\n", message);
        fclose(file);
    }
}

void log_process_tree() {
    char pstree_command[50];
    sprintf(pstree_command, "pstree -sg %d >> logs/log_all.log", getpid());
    system(pstree_command);
}

void parent_process() {
    pid_t pid;
    char log_message[100];

    // Logging init() processes
    log_to_file("init()---init(162)---init(162)---bash(164)---wer(182)---wer(182)-+-wer(182)");

    // Parent process logging
    sprintf(log_message, "Parent Process (PID: %d)", getpid());
    log_to_file(log_message);

    // Getting current date
    system("date >> logs/log_all.log");

    // Creating Child1 process
    pid = fork();
    if (pid == 0) {
        // Child1 process
        sprintf(log_message, "Child1 (PID: %d) created. Parent PID: %d", getpid(), getppid());
        log_to_file(log_message);

        // Creating three grandchildren for Child1
        for (int i = 0; i < 3; i++) {
            pid_t grandchild_pid = fork();
            if (grandchild_pid == 0) {
                // Grandchild process
                sprintf(log_message, "Torun%d (PID: %d) created. Parent PID: %d", i + 1, getpid(), getppid());
                log_to_file(log_message);
                sleep(i + 1);
                exit(0);
            }
        }
        // Waiting for all grandchildren to complete
        for (int i = 0; i < 3; i++) {
            wait(NULL);
        }
        exit(0);
    }

    // Creating Child2 process
    pid = fork();
    if (pid == 0) {
        // Child2 process
        sprintf(log_message, "Child2 (PID: %d) created. Parent PID: %d", getpid(), getppid());
        log_to_file(log_message);
        sleep(1);
        exit(0);
    }

    // Waiting for both child processes to complete
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    // Parent process completion
    log_to_file("İşlemler tamamlandı.");
}

int main() {
    // Creating logs directory
    system("mkdir -p logs");

    // Parent process
    parent_process();

    // Logging process tree
    log_process_tree();

    return 0;
}
