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
#include<unistd.h>

#define MAX_NUM_OF_USERS 50
#define KEY 3
#define WELCOME "Witaj w grze warcaby. Zaloguj sie aby rozpoczac gre.  \nPodaj login: \n"
#define WELCOME2 "Teraz podaj haslo: \n"
#define SUCCESSFUL_LOGIN "Zalogowano sie poprawnie. Witamy\n"
#define FAILED_LOGIN "Bledne haslo. Ponow probe.\n"
#define MENU "Menu glowne \n1.\t Utworz wlasny pokoj \n2.\t Dolacz do utworzonej gry \n3.\t Wyloguj sie\n"
#define SUCCESSFUL_LOGOUT "Wylogowano poprawnie z systemu. Zapraszamy ponownie.\n"
#define BAD_OPTION "Wybrano bledna opcje. Sprobuj ponownie.\n"

char (*logins)[20], (*passwords)[20];

struct msgbuf {
    long type;
    char message[100];
};

int checkIfLoginIsInBase(char* login) {
    for (int i = 0; i < MAX_NUM_OF_USERS; i++) {
        if (strcmp(login, logins[i]) == 0) return 1;
    }
    return 0;
}

int checkIfPwIsCorrect(char* login, char* pw) {
    int i = 0;
    for (; i < MAX_NUM_OF_USERS; i++) {
        printf("%s\n", logins[i]);
        if (strcmp(logins[i], login) == 0) break;
    }
    printf("PASSWORD: %s\n", passwords[i]);
    if (strcmp(passwords[i], pw) == 0) return 1;
    return 0;
}

int main() {
    struct msgbuf wiadomosc, odbior;
    int types[MAX_NUM_OF_USERS * 2] = {0}, msgId, odbiorId, *usersOnline, sharedMemoryIds[4],
        isLoginInBase, isPwCorrect, *numberOfWaitingClients;
    unsigned int sharedMemorySizes[2] = {sizeof(int), MAX_NUM_OF_USERS * 40};
    char login[20];
    
    for (int i = 0; i < 4; i++) {
        if ((sharedMemoryIds[i] = shmget(KEY + i, sharedMemorySizes[i / 2], IPC_CREAT | 0777)) == -1) {
            perror("shmget server\n");
            exit(1);
        }
    }
    if ((usersOnline = shmat(sharedMemoryIds[0], NULL, 0777)) == (void *) -1 ) {
        perror("shmat1 server\n");
        exit(1);
    }
    if ((numberOfWaitingClients = shmat(sharedMemoryIds[1], NULL, 0777)) == (void *) -1 ) {
        perror("shmat2 server\n");
        exit(1);
    }
    if ((logins = shmat(sharedMemoryIds[2], NULL, 0777)) == (void *)-1) {
        perror("shmat3 server\n");
        exit(1);
    }
    if ((passwords = shmat(sharedMemoryIds[3], NULL, 0777)) == (void *)-1) {
        perror("shmat4 server\n");
        exit(1);
    }

    if ((msgId = odbiorId = msgget(KEY, 0777 | IPC_CREAT)) == -1) {
       perror("msgget1 server\n");
       exit(1);
    }

    *usersOnline = 0;
    *numberOfWaitingClients = 0;

    while (1) {
        //memset(wiadomosc.message, 0, strlen(wiadomosc.message));
        //memset(odbior.message, 0, strlen(odbior.message));
        types[*usersOnline] = *usersOnline + 5;
        wiadomosc.type = types[*usersOnline];
        odbior.type = wiadomosc.type + 1;
        printf("Numofclients: %d\n", (*numberOfWaitingClients));
        while ((*numberOfWaitingClients) <= 0) {};
        (*numberOfWaitingClients)--;
        if (fork() == 0) {
            while (1) {
                strcpy(wiadomosc.message, WELCOME);
                if (msgsnd(msgId, &wiadomosc, 69, 0) == -1) {
                    perror("Welcome send server\n");
                    exit(1);
                }
                printf("%ld %d %d\n", wiadomosc.type, msgId, odbiorId);
                if (msgrcv(odbiorId, &odbior, 20, odbior.type, 0) == -1) {
                    perror("Login receive server\n");
                    exit(1);
                }
                isLoginInBase = 0;
                isPwCorrect = 1;
                printf("%s\n", odbior.message);
                isLoginInBase = checkIfLoginIsInBase(odbior.message);
                if (!isLoginInBase) strcpy(logins[wiadomosc.type], odbior.message);
                strcpy(login, odbior.message);
                memset(wiadomosc.message, 0, strlen(wiadomosc.message));
                memset(odbior.message, 0, strlen(odbior.message));
                strcpy(wiadomosc.message, WELCOME2);
                if (msgsnd(msgId, &wiadomosc, 20, 0) == -1) {
                    perror("PW request send server\n");
                    exit(1);
                }
                if (msgrcv(odbiorId, &odbior, 20, odbior.type, 0) == -1) {
                    perror("PW receive server\n");
                    exit(1);
                }
                printf("%s\n", odbior.message);
                if (isLoginInBase) isPwCorrect = checkIfPwIsCorrect(login, odbior.message);
                else strcpy(passwords[wiadomosc.type], odbior.message);
                memset(wiadomosc.message, 0, strlen(wiadomosc.message));
                memset(odbior.message, 0, strlen(odbior.message));
                if (isPwCorrect) {
                    strcpy(wiadomosc.message, SUCCESSFUL_LOGIN);
                    if (msgsnd(msgId, &wiadomosc, 33, 0) == -1) {
                        perror("PW succesful login result send server\n");
                        exit(1);
                    }
                    while(1) {
                        memset(wiadomosc.message, 0, strlen(wiadomosc.message));
                        strcpy(wiadomosc.message, MENU);
                        if (msgsnd(msgId, &wiadomosc, 85, 0) == -1)
                        {
                            perror("Menu send server\n");
                            exit(1);
                        }
                        if (msgrcv(odbiorId, &odbior, 1, odbior.type, 0) == -1)
                        {
                            perror("Option receive server\n");
                            exit(1);
                        }
                        memset(wiadomosc.message, 0, strlen(wiadomosc.message));
                        switch (odbior.message[0])
                        {
                        case '1':
                            printf("Tworzenie gry\n");
                            break;
                        case '2':
                            printf("Dolaczanie do gry\n");
                            break;
                        case '3':
                            strcpy(wiadomosc.message, SUCCESSFUL_LOGOUT);
                            if (msgsnd(msgId, &wiadomosc, 53, 0) == -1)
                            {
                                perror("Logout msg send server\n");
                                exit(1);
                            }
                            break;
                        default:
                            strcpy(wiadomosc.message, BAD_OPTION);
                            if (msgsnd(msgId, &wiadomosc, 40, 0) == -1)
                            {
                                perror("Bad option msg send server\n");
                                exit(1);
                            }
                            break;
                        }
                        if (odbior.message[0] == '3') exit(0);
                    }
                }
                else {
                    strcpy(wiadomosc.message, FAILED_LOGIN);
                    if (msgsnd(msgId, &wiadomosc, 33, 0) == -1) {
                        perror("PW failed login result send server\n");
                        exit(1);
                    }
                }
                memset(wiadomosc.message, 0, strlen(wiadomosc.message));
                memset(odbior.message, 0, strlen(odbior.message));
            }
        }
        (*usersOnline) += 2;
    }
}