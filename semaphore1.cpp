#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

pthread_t *crt, *cons;
int *buff, num_crt, num_cons, size_buff, pos = -1;
sem_t lock, emptyitems, fullitems;

int create(int digit){
        int item = 1 + rand() % 100;
        printf("creator %d creates item %d\n", digit, item);
        return item;
}

void append(int item){
        pos++;
        *(buff + pos) = item;
        printf("%d appended to the buffer\n", item);
}

void *creator(void *arg){
        int digit = 0;
        while(!pthread_equal(*(crt + digit), pthread_self()) && digit < num_crt)
        {digit++;}
        while (1){
                int item = create(digit + 1);
                sem_wait(&emptyitems);
                sem_wait(&lock);
                append(item);
                sem_post(&lock);
                sem_post(&fullitems);
                sleep(1 + rand() % 4);
        }
        pthread_exit(0);
}

void consume(int item, int digit){
        printf("item %d was used by %d consumer\n", item, digit);
}

int receive(){
        int item = *(buff + pos);
        printf("buffer--> ");
        for (int i = 0; i <= pos; ++i) printf("%d ", *(buff + i));
        pos--;
        printf("\nitem %d received from the buffer\n", item);
        return item;
}

void *consumer(void *arg){
        int digit = 0;
        while(!pthread_equal(*(cons + digit), pthread_self()) && num_cons)
        {digit++;}
        while (1){
                int item;
                sem_wait(&fullitems);
                sem_wait(&lock);
                item = receive();
                sem_post(&lock);
                sem_post(&emptyitems);
                consume(item, digit + 1);
                sleep(1 + rand() % 4);
        }
        pthread_exit(0);
}
int main(){

srand(time(NULL));

printf("Input digiter of creators: ");
scanf("%d", &num_crt);
printf("Input digiter of consumers: ");
scanf("%d", &num_cons);
printf("Input size of the buffer: ");
scanf("%d", &size_buff);

crt = (pthread_t *) malloc(num_crt * sizeof(pthread_t));
cons = (pthread_t *) malloc(num_cons * sizeof(pthread_t));
buff = (int *) malloc(size_buff * sizeof(int));

sem_init(&lock, 0, 1);
sem_init(&fullitems, 0, 0);
sem_init(&emptyitems, 0, size_buff);

for (int i = 0; i < num_crt; i++){
        pthread_create(crt + i, NULL, creator, NULL);
}

for (int i = 0; i < num_cons; i++){
        pthread_create(cons + i, NULL, consumer, NULL);
}

sleep(3);

for (int i = 0; i < num_crt; i++){
        pthread_cancel(*(crt + i));
}

for (int i = 0; i < num_cons; i++){
        pthread_cancel(*(cons + i));
}
return 0;
}
