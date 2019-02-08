#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <string.h>

#define MAX_TRESC 1024
#define SERWER 0   // ID nadawcy
#define REQUEST 1  // żądanie do serwera
#define ANSWER 2   // odpowiedź serwera

char server_name[] = "server_1234";

struct Message {
    long mtype;             // flaga wiadomości
    int id_nadawcy;         // ID jednoznacznie identyfikujące nadawcę
    char tresc[MAX_TRESC];  // wiadomość lub komenda dla serwera
};

int msgid, i;
volatile int waza=1; // zmienna w funkcji while procesu wysyłającego wiadomości
struct Message wyslij, odbierz; // stworzenie 2 obiektów wiadomosci 1 do wysylania 2 do odbierania

//obsługa sygnału SIGINT jeszcze niedokończone
void sig_handler(int signo)
{
    strcpy(wyslij.tresc, "quit");
	//printf("PRZECHWYCILEM");
	//printf("%s",wyslij.tresc);
	fflush(stdout);
    msgsnd(msgid,&wyslij,sizeof(wyslij)-sizeof(wyslij.mtype),1);
}

void *glowna_petla()
{
	//printf("\nutworzylem nowy proces");
	fflush(stdout);
	while(strncmp(odbierz.tresc, "EXIT", 4)!=0)
	{	
		msgrcv(msgid,&odbierz,sizeof(odbierz),ANSWER,0);
		printf("\n%s",odbierz.tresc);
		fflush(stdout);
	}
	msgctl(msgid, IPC_RMID, NULL);
	waza=0;
	pthread_exit(NULL);
}

pthread_t tid[2];

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);
	if (signal(SIGINT, sig_handler) == SIG_ERR) printf("\ncan't catch SIGINT\n");
	int server_fifo;
	// long i = 0;
	char nick[128];

	msgid = msgget(IPC_PRIVATE, IPC_CREAT|IPC_EXCL|0600);
	while(msgid == 0)
	{
		msgctl(msgid,IPC_RMID,NULL);
		msgid = msgget(IPC_PRIVATE,  IPC_CREAT|IPC_EXCL|0600);
	}
	printf("\n%d",msgid);
	fflush(stdout);

		wyslij.mtype = REQUEST;
		wyslij.id_nadawcy=msgid;
		sprintf(wyslij.tresc, "%d", msgid);
		server_fifo = open(server_name, O_WRONLY);
		write(server_fifo, &msgid, sizeof(msgid));
		close(server_fifo);
		msgrcv(msgid,&odbierz,sizeof(odbierz),ANSWER,0);
		printf("\n%s",odbierz.tresc);
		fflush(stdout);
		if(strncmp(odbierz.tresc, "DENIAL", 6)==0)
		{
			return 0;
		}
		else
		{
		    //podawanie nicku
			if(strncmp(odbierz.tresc, "ACCEPT", 6)==0)
			{
				printf("\nPodaj nick: ");
				fflush(stdout);
				scanf("%s", nick);
				sprintf(wyslij.tresc, "nick %s\n",nick);
				//printf("\n%s",wyslij.tresc);
				fflush(stdout);
				msgsnd(msgid,&wyslij,sizeof(wyslij)-sizeof(wyslij.mtype),wyslij.mtype);
				msgrcv(msgid,&odbierz,sizeof(odbierz),ANSWER,0);

                //próbuje podawać póki nie będzie poprawny
				while(strncmp(odbierz.tresc, "ACCEPT", 6)!=0)
				{
					printf("\n%s",odbierz.tresc);
					fflush(stdout);
					printf("\nPodaj nick: ");
					fflush(stdout);
					scanf("%s", nick);
					sprintf(wyslij.tresc,"nick %s\n", nick);
					msgsnd(msgid,&wyslij,sizeof(wyslij)-sizeof(wyslij.mtype),wyslij.mtype);
					msgrcv(msgid,&odbierz,sizeof(odbierz),ANSWER,0);
				}
				//tworzenie wątku
				pthread_t id;

				//utworzenie 2 wątku
				//printf("\nzaraz utworze proces");
				fflush(stdout);
				if(pthread_create(&id, NULL,glowna_petla , NULL)) {
					fprintf(stderr, "Error creating thread\n");
					return 1;
				}
				char tresc[1024];
				int c;
				while ((c = getchar()) != '\n' && c != EOF) { }
				while(waza)
				{
					//printf("%s", wyslij.tresc);
					fgets(tresc,1024,stdin);
					strcpy(wyslij.tresc,tresc);
					//printf("%s",wyslij.tresc);
					fflush(stdout);
					
					msgsnd(msgid,&wyslij,sizeof(wyslij)-sizeof(wyslij.mtype),wyslij.mtype);
				}
			}
		}

	return 0;
}
