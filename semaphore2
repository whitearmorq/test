#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

pthread_t *prod, *cons;
int *buff, num_prod, num_cons, size_buff, pos = -1;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

int produce(int numb){
        int item = 1 + rand() % 100;
        printf("Producer %d creates item %d\n", numb, item);
        return item;
}

void append(int item){
        pthread_mutex_lock(&lock);
        while (pos == size_buff - 1){
                pthread_cond_wait(&not_full, &lock);
        }
        pos++;
        *(buff + pos) = item;
        printf("%d appended to the buffer\n", item);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&lock);
}

void *producer(void *arg){
        int numb = 0;
        while(!pthread_equal(*(prod + numb), pthread_self()) && numb < num_prod)
        {numb++;}
        while (1){
                int item = produce(numb + 1);
                append(item);
                sleep(1 + rand() % 4);
        }
        pthread_exit(0);
}

void consume(int item, int numb){
        printf("item %d was eaten by %d consumer\n", item, numb);
}

int receive(){
        int item;
        pthread_mutex_lock(&lock);
        while (pos == -1){
                pthread_cond_wait(&not_empty, &lock);
        }
        item = *(buff + pos);
        printf("buffer--> ");
        for (int i = 0; i <= pos; ++i) printf("%d ", *(buff + i));
        pos--;
        printf("\nitem %d received from the buffer\n", item);
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&lock);
        return item;
}

void *consumer(void *arg){
        int numb = 0;
        while(!pthread_equal(*(cons + numb), pthread_self()) && numb < num_cons)
        {numb++;}
        while (1){
                int item;
                item = receive();
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
printf("Input buffer size: ");
scanf("%d", &size_buff);

prod = (pthread_t *) malloc(num_prod * sizeof(pthread_t));
cons = (pthread_t *) malloc(num_cons * sizeof(pthread_t));
buff = (int *) malloc(size_buff * sizeof(int));

for (int i = 0; i < num_prod; i++){
        pthread_create(prod + i, NULL, producer, NULL);
}

for (int i = 0; i < num_cons; i++){
        pthread_create(cons + i, NULL, consumer, NULL);
}

sleep(8);

for (int i = 0; i < num_prod; i++){
        pthread_cancel(*(prod + i));
}

for (int i = 0; i < num_cons; i++){
        pthread_cancel(*(cons + i));
}

return 0;
}
