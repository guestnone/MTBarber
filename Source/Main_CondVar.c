#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include <assert.h>
#include <unistd.h>
#include "myqueue.h"

int clientsCounter=0;
int resigned=0;
int currentlyCuttedClient=-1;
int queueSize=10;

struct client * listOfClients;
pthread_mutex_t mut_onChair;//  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_queue;//   = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_fryzjer;//= PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_kolejka;//= PTHREAD_COND_INITIALIZER;
int randomizer=0;
int cutHair()
{
    srand(time(NULL));
    int t=(rand()+randomizer)%10;
	randomizer=(randomizer+1)%5;
    sleep(t+1);
}

int waitToEnterSalon()
{
    srand(time(NULL));
    int t=(rand()+randomizer)%6;
	randomizer=(randomizer+1)%5;
    sleep(1);
}

int showData()
{
    printf("resigned: %d WRomm: %d/%d [in: %d]\n",resigned,getNumberOfClients(listOfClients),queueSize,currentlyCuttedClient);
}

int NewClientOnChair()
{

    if(!isQueueEmpty(listOfClients))
    {
        //assert(listOfClients);
        currentlyCuttedClient=getQueueFront(listOfClients);
        listOfClients=popQueueFront(listOfClients);
    }

    else
        currentlyCuttedClient =-1;

}
void* hairCutter(void* data)
{
//int i = getNumberOfClients(listOfClients);
    while(1)
    {

        pthread_mutex_lock(&mut_onChair);

        while(isQueueEmpty(listOfClients))
        {
            pthread_cond_wait(&cond_fryzjer,&mut_onChair);
        }

        if(currentlyCuttedClient!=-1)
            cutHair();
        //assert(currentlyCuttedClient!=-1);
        NewClientOnChair();
        //i--;
       //printf("%d\n",i);
        //printf("hair cutter: ");
        showData();
        //if(isQueueEmpty(listOfClients)==1)break;
        pthread_mutex_unlock(&mut_onChair);
    }
}
int enterHairdressingSalon()
{
    ///////////// mutex lock

    clientsCounter++;
    if(getNumberOfClients(listOfClients)<queueSize)
    {

        listOfClients=addClientToQueue(listOfClients,clientsCounter);

    }
    else
    {
        resigned++;
    }
    if(getNumberOfClients(listOfClients)==1)
    {
        pthread_cond_signal(&cond_fryzjer);
    }

    //////////// mutex lock
}
void* clients(void* data)
{
    //struct client* c;
    //c=(struct client*)data;
    while(1)
    {
        waitToEnterSalon();
        //lock mutex
        pthread_mutex_lock(&mut_queue);
        enterHairdressingSalon();

        //printf("queue: ");
        showData();
        pthread_mutex_unlock(&mut_queue);

        //lock mutex
    }
}
int goThroughQueue(struct client *c)
{
    //struct client *current = &first;
    printf("resigned: %d\n",resigned);

    printf("list of clients in queue:\n");
    while(c)
    {

        printf("%d\n",c->nr);
        c=c->next;
    }
    return 0;
}

int main()
{
 pthread_mutex_init(&mut_onChair, NULL);
 pthread_mutex_init(&mut_queue, NULL);
pthread_cond_init(&cond_fryzjer, NULL);
pthread_cond_init(&cond_kolejka, NULL);


    //listOfClients = initQueue(listOfClients,1);
    //addClientToQueue(listOfClients,2);
    //addClientToQueue(listOfClients,3);
    //addClientToQueue(listOfClients,4);

    pthread_t hairdresser,queue;
    pthread_create(&hairdresser,NULL,hairCutter,NULL);
    pthread_create(&queue,NULL,clients,NULL);




    pthread_join(hairdresser,NULL);
    pthread_join(queue,NULL);
}
