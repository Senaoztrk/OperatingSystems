#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

void sigint_handler(int sig) {
    printf("Parent process: SIGINT (Ctrl+C) signal received. Ignoring...\n");
}

void sigtstp_handler(int sig) {
    printf("Parent process: SIGTSTP (Ctrl+Z) signal received. Ignoring...\n");
}

int main(int argc, char *argv[]) {
    // Pipe için dosya tanımlayıcıları
    int pipefd[2];

    // Pipe oluştur
    if (pipe(pipefd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    // SIGINT (Ctrl+C) sinyali için handler ayarla
    signal(SIGINT, sigint_handler);

    // SIGTSTP (Ctrl+Z) sinyali için handler ayarla
    signal(SIGTSTP, sigtstp_handler);

    // Ebeveyn işlem
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Çocuk işlem
        close(pipefd[1]); // Yazma ucunu kapat

        // Mesajı al
        char msg[100];
        read(pipefd[0], msg, sizeof(msg));

        // Rastgele bir süre bekle
        srand(time(NULL));
        int wait_time = rand() % 10 + 1;
        sleep(wait_time);

        // Mesajı yazdır
        printf("Child process: Received message from parent: %s\n", msg);

        // Alarm süresi
        int msg_length = strlen(msg);
        if (strcmp(msg, "death") == 0) {
            printf("Child process: I'll try to kill you when your time is up!\n");
            // Mesaj içeriği "ölüm" ise çocuk işlemi sonlandır
            exit(EXIT_SUCCESS);
        } else if (strcmp(msg, "life") == 0) {
            printf("Child process: You'll live your life!\n");
            // Mesaj içeriği "yaşam" ise koşulsuz beklemeye başlar
            while(1) {
                sleep(1); // Sonsuz döngüde bekler
            }
        } else {
            printf("Child process: Invalid message received from parent!\n");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]); // Okuma ucunu kapat
        exit(EXIT_SUCCESS);
    } else {
        // Ebeveyn işlem
        close(pipefd[0]); // Okuma ucunu kapat

        // SIGINT (Ctrl+C) ve SIGTSTP (Ctrl+Z) sinyalleri için ignore modunda çalışır
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);

        // Argümanı al
        if (argc != 2) {
            fprintf(stderr, "Usage: %s <integer>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        int number = atoi(argv[1]);

        // Mesajı oluştur
        char msg[100];
        if (number % 2 == 0) {
            strcpy(msg, "death");
        } else {
            strcpy(msg, "life");
        }

        // Mesajı gönder
        write(pipefd[1], msg, strlen(msg) + 1);

        close(pipefd[1]); // Yazma ucunu kapat

        // Alarm süresi
        printf("Parent process: Waiting for the child process...\n");
        sleep(5);

        // Çocuk işleminin sonucunu beklemek için kullan
        int status;
        wait(&status);
        
        // Çocuk işlemin sonucunu ekrana yazdır
        if (WIFEXITED(status)) {
            printf("Parent process: Child process exited with status: %d\n", WEXITSTATUS(status));
        } else {
            printf("Parent process: Child process exited abnormally\n");
        }
    }

    return 0;
}
