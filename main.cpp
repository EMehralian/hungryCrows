#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <unistd.h>

#define NITER 1000000
#define M 1

int cnt = 0;
int state[2]={0}; // state 0 : playing (initial state)  state 1 : hungry  state 2 : eating
bool plate[M]={false};

sem_t childSM[2];

struct argstruct{
    int arg;
};

int ready_to_eat(int i);

int test(int i);

void finish_eating(int i , int plateNum);

void *child(void *arguments);

int main(int argc, char * argv[])
{

    pthread_t ch1, ch2;
    sem_init(&childSM[0], 0, 1);
    sem_init(&childSM[1], 0, 1);

    struct argstruct args;
    args.arg=0;
    if(pthread_create(&ch1, NULL, &child, (void *) &(args)))
    {
        printf("\n ERROR creating child 1");
        exit(1);
    }
    args.arg=1;
    if(pthread_create(&ch1, NULL, &child, (void *) &(args)))
    {
        printf("\n ERROR creating child 2");
        exit(1);
    }

    if(pthread_join(ch1, NULL))	/* wait for the thread 1 to finish */
    {
        printf("\n ERROR joining thread");
        exit(1);
    }

    if(pthread_join(ch2, NULL))        /* wait for the thread 2 to finish */
    {
        printf("\n ERROR joining thread");
        exit(1);
    }

    pthread_exit(NULL);
}

void *child(void *arguments){

    struct argstruct *args = (argstruct *) arguments;
    int i = args -> arg;
    int plateNum;
    while(true){
        srand(time(NULL));
        int playingTime = rand()%1000+1000;
        sleep(playingTime);
        plateNum=ready_to_eat(i);
        int EatingTime = rand()%1000+1000;
        sleep(playingTime);
        finish_eating(i,plateNum);
    }

}


void finish_eating(int i , int plateNum) {
    sem_t mutex;
    sem_init(&mutex,0,1);
    sem_wait (&mutex);
    state[i]=0;//playing
    plate[plateNum]= false;
    sem_post(&mutex);
}

int ready_to_eat(int i) {
    int plateNum;
    sem_t mutex;
    sem_init(&mutex,0,1);
    sem_wait (&mutex);
    state[i]=1;// hungry
    plateNum = test(i);
    sem_post(&mutex);
    sem_wait(&childSM[i]);
    return plateNum;

}

int test(int i) {   // check is there any full plate to eat?
    for(int j=0 ; j < M ; j++)
        if(plate[j]){
            state[i]=2;
            sem_post (&childSM[i]);
            return j;
        }
    return -1;
}