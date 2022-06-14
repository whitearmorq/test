#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

pthread_t *prod, *cons;
int *buff, num_prod, num_cons, size_buff, pos = -1;
sem_t lock, emptyitems, fullitems;

int produce(int numb){
        int item = 1 + rand() % 100;
        printf("Producer %d creates item %d\n", numb, item);
        return item;
}

void append(int item){
        pos++;
        *(buff + pos) = item;
        printf("%d appended to the buffer\n", item);
}

void *producer(void *arg){
        int numb = 0;
        while(!pthread_equal(*(prod + numb), pthread_self()) && numb < num_prod)
        {numb++;}
        while (1){
                int item = produce(numb + 1);
                sem_wait(&emptyitems);
                sem_wait(&lock);
                append(item);
                sem_post(&lock);
                sem_post(&fullitems);
                sleep(1 + rand() % 4);
        }
        pthread_exit(0);
}

void consume(int item, int numb){
        printf("item %d was eaten by %d consumer\n", item, numb);
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
        int numb = 0;
        while(!pthread_equal(*(cons + numb), pthread_self()) && num_cons)
        {numb++;}
        while (1){
                int item;
                sem_wait(&fullitems);
                sem_wait(&lock);
                item = receive();
                sem_post(&lock);
                sem_post(&emptyitems);
                consume(item, numb + 1);
                sleep(1 + rand() % 4);
        }
        pthread_exit(0);
}
int main(){

srand(time(NULL));

printf("Input number of producers: ");
scanf("%d", &num_prod);
printf("Input number of consumers: ");
scanf("%d", &num_cons);
printf("Input size of the buffer: ");
scanf("%d", &size_buff);

prod = (pthread_t *) malloc(num_prod * sizeof(pthread_t));
cons = (pthread_t *) malloc(num_cons * sizeof(pthread_t));
buff = (int *) malloc(size_buff * sizeof(int));

sem_init(&lock, 0, 1);
sem_init(&fullitems, 0, 0);
sem_init(&emptyitems, 0, size_buff);

for (int i = 0; i < num_prod; i++){
        pthread_create(prod + i, NULL, producer, NULL);
}

for (int i = 0; i < num_cons; i++){
        pthread_create(cons + i, NULL, consumer, NULL);
}

sleep(3);

for (int i = 0; i < num_prod; i++){
        pthread_cancel(*(prod + i));
}

for (int i = 0; i < num_cons; i++){
        pthread_cancel(*(cons + i));
}
return 0;
}
