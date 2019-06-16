#include<stdlib.h>
#include<stdio.h>
//#include<pthread.h>
#include <assert.h>
#include <stdlib.h>
#include "myqueue.h"
#include "cond_queue.h"
#include <pthread.h>
#include <string.h>
#include <unistd.h>

int resigned=0;
int currentlyCuttedClient=-1;
int queueSize=10;
int isBarberWorking=0;
struct client * listOfResignedClients=NULL;
int nrOfAllClients=30;
struct cond_queue * listOfClientsInQueue=NULL;

/*mutex odpowiadajacy za czekanie w kolejce dopuki nie nadejdzie kolej klienta*/
pthread_mutex_t mut_waitInqueue;

/*mutex odpowiadajacy za dostemm do kolejki klientow*/
pthread_mutex_t mut_accessqueue;

/*zmienna czekajaca na to az fryzjer bedzie
gotowy by obsluzyc pierwszego klienta
w kolejce i wysylajaca do niego wtedy sygnal*/
pthread_cond_t cond_babrberIsWaiting;


int debug=0;

void waitTime(int maxtime)
{
    srand(time(NULL));
    int t=rand()%maxtime;
    sleep(t+1);
}


int showData()
{
    printf("resigned: %d WRomm: %d/%d [in: %d]\n",resigned,getcondQueueSize(listOfClientsInQueue),queueSize,currentlyCuttedClient);

}


void* clientThread(void* data)
{
    pthread_mutex_lock(&mut_accessqueue);
    int nr=(int*)data;
    if(getcondQueueSize(listOfClientsInQueue)<queueSize)
    {
        struct cond_queue * thisclient;
        thisclient = addElementtoCondQueue(&listOfClientsInQueue,nr);
        showData();
        if(debug==1)
            printQueue(listOfClientsInQueue);
		//wysyłam sygnał do fryzjera, że jest klient
        pthread_cond_signal(&cond_babrberIsWaiting);
        pthread_mutex_unlock(&mut_accessqueue);
        pthread_mutex_lock(&mut_waitInqueue);
		//czekam az dostane sygnal do fryzjera ze przyszła kolej tego klienta
        while(currentlyCuttedClient!=nr)
        {
            pthread_cond_wait(&thisclient->cond,&mut_waitInqueue);
        }

        pthread_mutex_unlock(&mut_waitInqueue);
        pthread_exit(NULL);

    }
    else
    {
		//jesli kolejka jest przepełniona, to dodaję klienta do listy klientów 
		//którzy nie weszli do kolejki
        listOfResignedClients= addClientToQueue(listOfResignedClients,nr);
        showData();
        if(debug==1)
        showList(listOfResignedClients);
        resigned++;
        pthread_mutex_unlock(&mut_accessqueue);
    }
    pthread_exit(NULL);
}
void* barber(void* data)
{

    while(1)
    {
		//blokuję dostęp do kolejki klientów
        pthread_mutex_lock(&mut_accessqueue);
        currentlyCuttedClient=-1;
		//czekam aż pojawi się klient (fryzjer śpi)
        while(isCondQueueEmpty(listOfClientsInQueue))
        {
            pthread_cond_wait(&cond_babrberIsWaiting,&mut_accessqueue);
        }
		//tworzę tymczasową zmienną przechowującą element klienta który będzie 
		//obsługiwany
        struct cond_queue * cuttedClient;

        cuttedClient = listOfClientsInQueue;

        if(cuttedClient==NULL)
        {
            pthread_mutex_unlock(&mut_accessqueue);
            continue;
        }

        if(!isCondQueueEmpty(listOfClientsInQueue));
            listOfClientsInQueue =popCondQueueFront(listOfClientsInQueue);

        currentlyCuttedClient = cuttedClient->nr;
        showData();

        if(debug==1)
            printQueue(listOfClientsInQueue);
		
        pthread_mutex_unlock(&mut_accessqueue);
		//wysyłam sygnał do klienta że nadeszła jego kolej
        pthread_cond_signal(&cuttedClient->cond);
        free(cuttedClient);
        waitTime(4);
    }


}

int main(int argc, char ** argv)
{

    if(argc == 2)
		if(strncmp(argv[1], "--debug", 6) == 0)
    debug = 1;

    pthread_mutex_init(&mut_accessqueue,NULL);
    pthread_mutex_init(&mut_waitInqueue,NULL);

    pthread_cond_init(&cond_babrberIsWaiting, NULL);

    nrOfAllClients = 30;
    pthread_t queue[nrOfAllClients];

     pthread_t hairdresser;
     pthread_create(&hairdresser,NULL,barber,NULL);

int i=0;
pthread_t client_thread_var;
	for(;;)
    {
	i++;
	if(i<0)i=0;
        sleep(1);
        pthread_create(&client_thread_var,NULL,*clientThread,(void*)(i));
    }
    for(int i =0;i<nrOfAllClients;++i)
    {
        pthread_join(queue[i],NULL);
    }

    while(listOfResignedClients)
    {
        struct client * tmp;
        tmp =listOfResignedClients;
        listOfResignedClients=listOfResignedClients->next;
        free (tmp);
    }


}
