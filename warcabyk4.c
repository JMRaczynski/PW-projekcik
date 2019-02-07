#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<sys/msg.h>

#define KEY 3
#define SIZE sizeof(struct msgbuf)-sizeof(long)

struct msgbuf {
    long type;
    char message[1500];
    int erro_n, stat_n;
} odbior, wiadomosc;
int msgId;
pid_t pid;

void catch_kill(int sig_num){
    memset(wiadomosc.message,'\0', sizeof(wiadomosc.message));
    strcpy(wiadomosc.message, "exit");
    if(msgsnd(msgId, &wiadomosc, SIZE, 0)<0){
        perror("catch_exit");exit(1);}
}

void obsluga_gry(struct msgbuf *wiadomosc, struct msgbuf *odbior, int msgId){
    signal(SIGINT, catch_kill);
   do{
        memset(odbior->message,0,sizeof(odbior->message));
        if(msgrcv(msgId,odbior,SIZE,odbior->type,0)<0){
            //perror("odbior podczas gry"); exit(1);
            catch_kill(9);
            continue;
        }
        printf("%s\n",odbior->message);

        //printf("%d aa1\n",odbior->stat_n);
        if(odbior->stat_n==1){
            signal(SIGINT, catch_kill);
            //printf("bb2\n");
            fgets(wiadomosc->message,6,stdin);
            memset(wiadomosc->message,'\0',sizeof(wiadomosc->message));
            fgets(wiadomosc->message,6,stdin);
            //printf("%s %ld\n",wiadomosc->message, sizeof(wiadomosc->message));
            //if(wiadomosc->message[0]=='e' && wiadomosc->message[1]=='x' && wiadomosc->message[2]=='i' && wiadomosc->message[3]=='t')odbior->stat_n=5;
            if(msgsnd(msgId,wiadomosc,SIZE,0)<0){
                perror("odsylanie gra");exit(1);}
            //printf("cc3 w%ld, o%ld\n",wiadomosc->type,odbior->type);
        }
   }while(odbior->stat_n<3); 
   //printf("wyszedl\n");
}

int main() {
    //struct msgbuf odbior, wiadomosc;
    int odbiorId, roomid, sharedMemoryIds[2], *usersOnline, *numberOfWaitingClients;
    char login[20], password[20], option[2];
    long oldtype;

    for (int i = 0; i < 2; i++) {
        if ((sharedMemoryIds[i] = shmget(KEY + i, sizeof(int), IPC_CREAT | 0777)) == -1) {
            perror("shmget client\n");
            exit(1);
        }
    }
    if ((usersOnline = shmat(sharedMemoryIds[0], NULL, SHM_RDONLY)) == (void *) -1 ) {
        perror("shmat1 client\n");
        exit(1);
    }
    if ((numberOfWaitingClients = shmat(sharedMemoryIds[1], NULL, 0777)) == (void *) -1 ) {
        perror("shmat2 client\n");
        exit(1);
    }
    if ((msgId = odbiorId = msgget(KEY, 0777 | IPC_CREAT)) == -1) {
       perror("msgget1 client\n");
       exit(1);
    }

    odbior.type = *usersOnline + 5;
    wiadomosc.type = odbior.type + 1;
    (*numberOfWaitingClients)++;

    while (1) {
        memset(odbior.message, 0, strlen(odbior.message));
        memset(wiadomosc.message, 0, strlen(wiadomosc.message));
        printf("%ld %d %d\n", odbior.type, msgId, odbiorId);
        if (msgrcv(odbiorId, &odbior, SIZE, odbior.type, 0) == -1) {
            perror("Welcome receive client\n");
            exit(1);
        }
        printf("%ld\n", odbior.type);
        printf("%s", odbior.message);
        scanf("%s", login);
        fflush(0);
        strcpy(wiadomosc.message, login);
        if (msgsnd(msgId, &wiadomosc, SIZE, 0) == -1) {
            perror("Login send client\n");
            exit(1);
        }
        memset(odbior.message, 0, strlen(odbior.message));
        memset(wiadomosc.message, 0, strlen(wiadomosc.message));
        if (msgrcv(odbiorId, &odbior, SIZE, odbior.type, 0) == -1) {
            perror("PW request receive client\n");
            exit(1);
        }
        printf("%s", odbior.message);
        scanf("%s", password);
        fflush(0);
        strcpy(wiadomosc.message, password);
        if (msgsnd(msgId, &wiadomosc, SIZE, 0) == -1) {
            perror("PW send client\n");
            exit(1);
        }
        if (msgrcv(odbiorId, &odbior, SIZE, odbior.type, 0) == -1) {
            perror("Login result receive client\n");
            exit(1);
        }
        printf("%s", odbior.message);
        if (odbior.message[0] == 'Z') break;
    }
    while (1) {
        memset(odbior.message, 0, strlen(odbior.message));
        memset(wiadomosc.message, 0, strlen(wiadomosc.message));
  //      printf("menu_rec %ld\n",odbior.type);
        if (msgrcv(odbiorId, &odbior, SIZE, odbior.type, 0) == -1) {
            perror("Menu receive client\n");
            exit(1);
        }
        printf("%s", odbior.message);
        scanf("%s", option);
        fflush(0);
        strcpy(wiadomosc.message, option);
        if (msgsnd(msgId, &wiadomosc, SIZE, 0) == -1) {
            perror("Option send client\n");
            exit(1);
        }
	memset(odbior.message, 0, strlen(odbior.message));
//     printf("menu_feed %ld\n",odbior.type);
	    if (msgrcv(msgId, &odbior, SIZE, odbior.type, 0) == -1) {
	        perror("Menu feedback client\n");
	        exit(1);
	    }
	printf("%s\n", odbior.message);
	
	switch(option[0])
	{
	case '1':
	  
	    memset(odbior.message, 0, strlen(odbior.message));
	    if (msgrcv(msgId, &odbior, SIZE, odbior.type, 0) == -1) {
	        perror("Game welcome receive player1\n");
	        exit(1);
	    }
	    printf("%s\n", odbior.message);
         obsluga_gry(&wiadomosc, &odbior, msgId);
	    break;
	    
	case '2':
	  
	    if(odbior.message[0] == 'N'){}//Jesli nie ma zadnego pokoju do dolaczenia trzeba wrocic do odbierania menu
	    else{//przypadek kiedy pokoj jest
	        scanf("%d", &roomid);
	        memset(wiadomosc.message, 0, strlen(wiadomosc.message));
	        sprintf(wiadomosc.message, "%d %ld", roomid, odbior.type);
	        //printf("%s\n", wiadomosc.message);
	        if (msgsnd(msgId, &wiadomosc, SIZE, 0) == -1) {
	            perror("Player2 roomid send client\n");
	            exit(1);
	        }
		if (msgrcv(msgId, &odbior, SIZE, odbior.type, 0) == -1) {
		  perror("Game welcome message receive client\n");
		  exit(1);
		}
		if(odbior.message[0] == 'P'){
		  printf("%s\n", odbior.message);
		}
		else{
		
		  oldtype = wiadomosc.type;
		  wiadomosc.type = roomid;
		  memset(odbior.message, 0, strlen(odbior.message));
		  if (msgrcv(msgId, &odbior, SIZE, odbior.type, 0) == -1) {
		    perror("Game welcome message receive client\n");
		    exit(1);
		  }
		  printf("%s\n", odbior.message);
		  obsluga_gry(&wiadomosc, &odbior, msgId);
		  //       printf("poza_funkcja\n");
		  wiadomosc.type = oldtype;
		}
	    }
	    break;
	    
	default:
	    printf("Nic specjalnego\n");
	    break;
	}    
    
        if (odbior.message[0] == 'W') break;
//        printf("za break\n");
    }
}
