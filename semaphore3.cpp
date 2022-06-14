#include <pthread.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <signal.h>

int FLAG = 0;
int iter = 10000;
int c_thr;
double pi = 0.0;
pthread_mutex_t pi_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct Parts 
{
        double n1;
        double n2;
        int step;
        double partial_sum;
} Parts;

void *thread_funk(void *arg)
{
    int iterations_count = 0;
        Parts *st = (Parts *)arg;
        for (int i = 0; i < iter; i++)
    {
                st->partial_sum = st->partial_sum + 4 * (1 / st->n1 - 1 / st->n2);
                st->n1 = st->n1 + st->step;
                st->n2 = st->n2 + st->step;
        if (i == iter - 1)
        {
          if (FLAG == 1)
          {
            break;
          }
          i = 0;
        }
        }
        pthread_mutex_lock(&pi_lock);
        pi = pi + st->partial_sum;
        pthread_mutex_unlock(&pi_lock);
    pthread_exit(0);
}

void signalhandler(int param)
{
  FLAG = 1;
}

int main(int argc, char *argv[])
{

        if (argc != 2)
    {
                std::cout << "Wrong input.\nSample: ./pi_calculate 'number of threads'" << std::endl;
                return 0;
}
        else 
    {
                c_thr = atoi(argv[1]);
        }
    
        pthread_t *threads = (pthread_t *) malloc(c_thr * sizeof(pthread_t));
        Parts *p = (Parts *) malloc(c_thr * sizeof(Parts));
    
        for (int i = 0; i < c_thr; i++)
    {
                *(p + i) = {1.0 + 4.0 * i, 3.0 + 4.0 * i, 4 * c_thr, 0 };
                pthread_create(threads + i, NULL, thread_funk, p + i);
        }
    
    signal(SIGINT, signalhandler);
    
        for (int i = 0; i < c_thr; i++)
    {
                pthread_join(*(threads + i), NULL);
        }
    
    
        std::cout << std::setprecision(16);
        std::cout << pi << std::endl;

        return 0;
}
