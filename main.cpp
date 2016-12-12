#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <semaphore.h>

using namespace std;

#define PLATE_NUMS 2
#define CHILD_NUMS 3
#define MOM_FEED 4

int plateState[PLATE_NUMS]={0};
int currentMomFeed = 0;
bool momIsGoingToBeCalled = false;
pthread_mutex_t printMutex;
pthread_mutex_t dishes[PLATE_NUMS];
pthread_mutex_t momIsCalled;
sem_t waitToCallMom;

int ready_to_eat(int i);

void *child(void *arguments);

void xOut(string s , int i);

void mother();

bool checkForEmptyDish();

void food_ready() ;

int main()
{
    pthread_t ch[CHILD_NUMS];
    pthread_mutex_init(&momIsCalled,NULL);
    sem_init(&waitToCallMom,0,1);
    pthread_mutex_init(&printMutex,NULL);
    for(int i=0;i<PLATE_NUMS;i++){
        pthread_mutex_init(&dishes[i],NULL);
    }
    for ( int i = 0; i < CHILD_NUMS; ++i ) {
        int *arg = (int *) malloc(sizeof(*arg));
        if (arg == NULL) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }
        *arg = i;
        if (pthread_create(&ch[i], NULL, &child, arg)) {
            printf("\n ERROR creating child ");
            exit(1);
        }
    }
    for(int i =0 ; i<CHILD_NUMS ; i++){
        if(pthread_join(ch[i], NULL))  /* wait for the thread 1 to finish */
        {
            printf("\n ERROR joining thread");
            exit(1);
        }
    }
}

void *child(void *a){
    int i = *((int *) a);
    xOut("baby crow  started:",i);
    int playingTime;
    int eatingTime;
    while(currentMomFeed<MOM_FEED){
        srand((unsigned int) time(NULL));
        playingTime = 1000000;
        usleep((unsigned int) playingTime);
        xOut("Trying to find food " , i);
        int plateNum=ready_to_eat(i);
        if(plateNum==-1) {
            xOut("Food not found" , i);
            bool IhavaToCallMom = false;
            pthread_mutex_lock(&momIsCalled);
            if (!momIsGoingToBeCalled) {
                momIsGoingToBeCalled = true;
                IhavaToCallMom = true;
            }
            pthread_mutex_unlock(&momIsCalled);
            if(IhavaToCallMom)
            {
                xOut("It's my task to call mom " , i);
                sem_wait(&waitToCallMom);
                mother();
                xOut("Mom call complete" , i);
            }

        } else {
            xOut("Starting to eat food",i);
            eatingTime = 1000000;
            usleep((unsigned int) eatingTime);
            bool empty = checkForEmptyDish();
            if(empty)
            {
                xOut("Calling someone to call mom",i);
                sem_post(&waitToCallMom);
            }
            xOut("Hungry again!",i);
        }
    }
    return NULL;
}

bool checkForEmptyDish() {
    bool result = true;
    for(int i=0;i<PLATE_NUMS;i++){
        pthread_mutex_lock(&dishes[i]);
        if(plateState[i]==true)
        {
            result = false;
        }
        pthread_mutex_unlock(&dishes[i]);
        if(!result)
        {
            return false;
        }
    }
    return true;
}

void xOut(string s, int i)
{
    pthread_mutex_lock(&printMutex);
    cout<<s<<" "<<i<<endl;
    pthread_mutex_unlock(&printMutex);
}


int ready_to_eat(int childNum) {
    int plateNum = -1;
    for(int i=0;i<PLATE_NUMS;i++){
        pthread_mutex_lock(&dishes[i]);
        if(plateState[i]==true)
        {
            plateState[i] = false;
            plateNum = i;

        }
        pthread_mutex_unlock(&dishes[i]);
        if(plateNum!=-1)
        {
            return plateNum;
        }
    }
    return plateNum;
}

void mother() {
    int foodPreparation = 0;
    xOut("Mom is waking up", 0);
    if (currentMomFeed < MOM_FEED) {
        srand((unsigned int) time(NULL));
        foodPreparation = rand() % 1000;
        usleep((unsigned int) foodPreparation);
        food_ready();
        currentMomFeed++;
        xOut("Mom current feed num is:", currentMomFeed);
    } else {
        xOut("Mom is retired", 0);
    }
}

void food_ready() {
    xOut("Reseting foods",0);
    for(int i=0;i<PLATE_NUMS;i++) {
        pthread_mutex_lock(&dishes[i]);
        plateState[i] = true;
        pthread_mutex_unlock(&dishes[i]);
    }
    pthread_mutex_lock(&momIsCalled);
    momIsGoingToBeCalled = false;
    pthread_mutex_unlock(&momIsCalled);
}