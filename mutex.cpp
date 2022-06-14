#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

pthread_t *crt, *cons;
int *buff, num_crt, num_cons, size_buff, pos = -1;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

int create(int digit){
        int item = 1 + rand() % 100;
        printf("creator %d creates item %d\n", digit, item);
        return item;
}

void add(int item){
        pthread_mutex_lock(&lock);
        while (pos == size_buff - 1){
                pthread_cond_wait(&not_full, &lock);
        }
        pos++;
        *(buff + pos) = item;
        printf("%d added to the buffer\n", item);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&lock);
}

void *creator(void *arg){
        int digit = 0;
        while(!pthread_equal(*(crt + digit), pthread_self()) && digit < num_crt)
        {digit++;}
        while (1){
                int item = create(digit + 1);
                add(item);
                sleep(1 + rand() % 4);
        }
        pthread_exit(0);
}

void consume(int item, int digit){
        printf("item %d was eaten by %d consumer\n", item, digit);
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
        int digit = 0;
        while(!pthread_equal(*(cons + digit), pthread_self()) && digit < num_cons)
        {digit++;}
        while (1){
                int item;
                item = receive();
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
printf("Input buffer size: ");
scanf("%d", &size_buff);

crt = (pthread_t *) malloc(num_crt * sizeof(pthread_t));
cons = (pthread_t *) malloc(num_cons * sizeof(pthread_t));
buff = (int *) malloc(size_buff * sizeof(int));

for (int i = 0; i < num_crt; i++){
        pthread_create(crt + i, NULL, creator, NULL);
}

for (int i = 0; i < num_cons; i++){
        pthread_create(cons + i, NULL, consumer, NULL);
}

sleep(8);

for (int i = 0; i < num_crt; i++){
        pthread_cancel(*(crt + i));
}

for (int i = 0; i < num_cons; i++){
        pthread_cancel(*(cons + i));
}

return 0;
}
