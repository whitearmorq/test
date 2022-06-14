#include <stdio.h> 

#include <stdlib.h> 

#include <string.h> 

#include <sys/types.h> 

#include <sys/stat.h> 

#include <sys/ipc.h> 

#include <sys/msg.h> 

#include <sys/sem.h> 

#include <sys/shm.h> 

#include <sys/wait.h> 

#include <time.h> 

#include <errno.h> 

#include <unistd.h> 

 

#define PERM 0660 

 

#define MSGDATASZ 4096 

#define SEGSZ     8192 

 

static char labs[] = " ./lab81 ./mutexq ./pimutexf "; 

static int prognum = 4; 

 

struct printq 

{ 

    int cpid; 

    char buf[MSGDATASZ]; 

}; 

 

struct printbf 

{ 

    long printt; 

    struct printq printd; 

}; 

 

union semun 

{ 

    int val; 

    struct semid_ds *buf; 

    unsigned short *array; 

}; 

 

struct sembuf p = { 0, -1, SEM_UNDO }; 

struct sembuf v = { 0, +1, SEM_UNDO }; 

 

char *getword(char *s, int n) 

{ 

    static char buf[100] = {0}; 

    char *p; 

 

    strcpy(buf, s); 

    p = strtok(buf, " "); 

    while(n > 1) 

    { 

        p = strtok(NULL, " "); 

        --n; 

    } 

     

    return p; 

} 

 

int main(int argc, char **argv) 

{ 

    int i, cpid; 

    union semun u; 

    long procnum, n; 

    struct printbf msg; 

    int msqid, semid, shmid; 

    char *endptr, *str, *sh_mem; 

 

 

    if(argc < 2) 

    { 

        fprintf(stderr, "Usage: %s <procnum>\n", argv[0]); 

        exit(EXIT_FAILURE); 

    } 

 

    str = argv[1]; 

    errno = 0; 

 

    procnum = strtol(str, &endptr, 10); 

 

    if(errno != 0) 

    { 

        perror("strtol"); 

        exit(EXIT_FAILURE); 

    } 

 

    if(endptr == str) 

    { 

        fprintf(stderr, "No digits were found\n"); 

        exit(EXIT_FAILURE); 

    } 

 

    msqid = msgget(IPC_PRIVATE, PERM | IPC_CREAT); 

    if(msqid == -1) 

    { 

        perror("msgget"); 

        exit(EXIT_FAILURE); 

    } 

 

    semid = semget(IPC_PRIVATE, 1, PERM | IPC_CREAT); 

    if(semid == -1) 

    { 

        perror("semget"); 

        exit(EXIT_FAILURE); 

    } 

 

    shmid = shmget(IPC_PRIVATE, SEGSZ, PERM | IPC_CREAT); 

    if(shmid == -1) 

    { 

        perror("shmget"); 

        exit(EXIT_FAILURE); 

    } 

    printf("msqid = %d\nsemid = %d\nshmid = %d\n\n", msqid, semid, shmid); 

 

    u.val = 1; 

    if(semctl(semid, 0, SETVAL, u) == -1) 

    { 

        perror("semctl"); 

        exit(EXIT_FAILURE); 

    } 

 

    sh_mem = shmat(shmid, 0, 0); 

    if(!sh_mem) 

    { 

        perror("shmat"); 

        exit(EXIT_FAILURE); 

    } 

    strcpy(sh_mem, labs); 

 

    for(i = 0; i < procnum; ++i) 

    { 

        cpid = fork(); 

        if(cpid == -1) { 

            perror("fork"); 

            exit(EXIT_FAILURE); 

        } else if(cpid == 0) { 

            break; 

        } 

    } 

     

    if(cpid == 0) 

    { 

        srand(time(NULL) ^ (getpid()<<16)); 

        msg.printt = 1; 

        msg.printd.cpid = getpid(); 

        n = rand() % (prognum + 1 - 1) + 1; 

        str = getword(sh_mem, n); 

        strcat(msg.printd.buf, str); 

        if(semop(semid, &p, 1) == -1) 

        { 

            perror("semop p"); 

            exit(EXIT_FAILURE); 

        } 

        if(msgsnd(msqid, &msg, sizeof(struct printq), 0) == -1) 

        { 

            perror("msgsnd"); 

            exit(EXIT_FAILURE); 

        } 

        execlp(str, str, NULL); 

        perror("execvp"); 

        exit(EXIT_FAILURE); 

    } 

    else 

    { 

        n = procnum; 

        while(n > 0) 

        { 

            if(msgrcv(msqid, &msg, sizeof(struct printq), 0, 0) == -1) 
            if(msgrcv(msqid, &msg, sizeof(struct printq), 0, 0) == -1) 

            { 

                perror("msgrcv"); 

                exit(EXIT_FAILURE); 

            } 

            printf("%d executed program ``%s''\n", msg.printd.cpid,  

                msg.printd.buf); 

            --n; 

        } 

        for(i = 0; i < procnum; ++i) 

        { 

            wait(NULL); 

            if(semop(semid, &v, 1) == -1) 

            { 

                perror("semop v"); 

                exit(EXIT_FAILURE); 

            } 

        } 

        msgctl(msqid, IPC_RMID, NULL); 

    } 

 

    shmdt(sh_mem); 

    shmctl(shmid, IPC_RMID, 0); 

    return 0; 

} 
