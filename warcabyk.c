#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<sys/msg.h>

#define KEY 3

struct msgbuf {
    long type;
    char message[100];
};

int main() {
    struct msgbuf odbior, wiadomosc;
    int msgId, odbiorId, sharedMemoryId, *usersOnline;
    char login[20], password[20], option[2];

    if ((sharedMemoryId = shmget(KEY, sizeof(int), IPC_CREAT | 0777)) == -1) {
        perror("shmget client\n");
        exit(1);
    }
    if ((usersOnline = shmat(sharedMemoryId, NULL, SHM_RDONLY)) == (void *) -1 ) {
        perror("shmat server\n");
        exit(1);
    }
    
    if ((msgId = odbiorId = msgget(KEY, 0777 | IPC_CREAT)) == -1) {
       perror("msgget1 client\n");
       exit(1);
    }

    odbior.type = *usersOnline + 5;
    wiadomosc.type = odbior.type + 1;

    while (1) {
        memset(odbior.message, 0, strlen(odbior.message));
        memset(wiadomosc.message, 0, strlen(wiadomosc.message));
        printf("%ld %d %d\n", odbior.type, msgId, odbiorId);
        if (msgrcv(odbiorId, &odbior, 69, odbior.type, 0) == -1) {
            perror("Welcome receive client\n");
            exit(1);
        }
        printf("%ld\n", odbior.type);
        printf("%s", odbior.message);
        scanf("%s", login);
        fflush(0);
        strcpy(wiadomosc.message, login);
        if (msgsnd(msgId, &wiadomosc, 20, 0) == -1) {
            perror("Login send client\n");
            exit(1);
        }
        memset(odbior.message, 0, strlen(odbior.message));
        memset(wiadomosc.message, 0, strlen(wiadomosc.message));
        if (msgrcv(odbiorId, &odbior, 20, odbior.type, 0) == -1) {
            perror("PW request receive client\n");
            exit(1);
        }
        printf("%s", odbior.message);
        scanf("%s", password);
        fflush(0);
        strcpy(wiadomosc.message, password);
        if (msgsnd(msgId, &wiadomosc, 20, 0) == -1) {
            perror("PW send client\n");
            exit(1);
        }
        if (msgrcv(odbiorId, &odbior, 33, odbior.type, 0) == -1) {
            perror("Login result receive client\n");
            exit(1);
        }
        printf("%s", odbior.message);
        if (odbior.message[0] == 'Z') break;
    }
    while (1) {
        memset(odbior.message, 0, strlen(odbior.message));
        memset(wiadomosc.message, 0, strlen(wiadomosc.message));
        if (msgrcv(odbiorId, &odbior, 85, odbior.type, 0) == -1) {
            perror("Menu receive client\n");
            exit(1);
        }
        printf("%s", odbior.message);
        scanf("%s", option);
        fflush(0);
        strcpy(wiadomosc.message, option);
        if (msgsnd(msgId, &wiadomosc, 1, 0) == -1) {
            perror("Option send client\n");
            exit(1);
        }
        memset(odbior.message, 0, strlen(odbior.message));
        if (msgrcv(msgId, &odbior, 100, odbior.type, 0) == -1) {
            perror("Menu feedback client\n");
            exit(1);
        }
        printf("%s\n", odbior.message);
        if (odbior.message[0] == 'W') break;
    }
}