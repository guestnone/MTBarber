#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include "Common.h"

pthread_mutex_t gBarberBusyMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gReadyToCutMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gChangeNumClientsMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gPrintMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t gClientReadySemaphore;
sem_t gCurrentlyCuttingSemaphore;

int gNumResigned = 0;
int gNumWaiting = 0;
int gNumTotal = 0;
int gNumMaxChairs = 0;
volatile int gCurrServed;

QueuePosition *gCurrClientQueue = NULL;
QueuePosition *gResignedClientQueue = NULL;

int randomizer=0;

void cutHair(QueuePosition *client)
{
    srand(time(NULL));
    int t=(rand()+randomizer)%10;
	randomizer=(randomizer+1)%5;
    sleep(t+1);
	// inform that he/she was cutted.
	sem_post(&client->clientWasCutSemaphore);
}

QueuePosition* clientIn()
{
    QueuePosition *ret = gCurrClientQueue;
	gCurrClientQueue = gCurrClientQueue->next;
	return ret;
}


void *customerThread(void *notUsed)
{
	// We need to change the number of clients, lock the access mutex
	pthread_mutex_lock(&gChangeNumClientsMutex);
	gNumTotal++;
	int clientId = gNumTotal;
	if (gNumWaiting < gNumMaxChairs)
	{
		gNumWaiting++;
		QueuePosition *newClient = addToQueue(&gCurrClientQueue, gNumTotal);
		printStats(gNumResigned, gNumWaiting, gNumMaxChairs, gCurrServed);
		// Client is read and its waiting
		sem_post(&gClientReadySemaphore);
		// Number state not needed to change, unlock the mutex
		pthread_mutex_unlock(&gChangeNumClientsMutex);
		// Wait for his turn
		sem_wait(&newClient->clientTurnSemaphore);
		// Wait when you're getting cut
		sem_wait(&newClient->clientWasCutSemaphore);
	}
	else // add resigned client to the resigned queue 
	{
		addToQueue(&gResignedClientQueue, gNumTotal);
		gNumResigned++;
		printStats(gNumResigned, gNumWaiting, gNumMaxChairs, gCurrServed);
		//PrintQueue(gCurrClientQueue);
		//PrintQueue(gResignedClientQueue);
		// Also unlock the client number data access
		pthread_mutex_unlock(&gChangeNumClientsMutex);
	}
	
	pthread_exit(0);
}

void* barberThread(void *notUsed)
{
	while(true)
	{
		// barber sleeps
		sem_wait(&gClientReadySemaphore);
		// We need to change the number of clients, lock the access mutex
		pthread_mutex_lock(&gChangeNumClientsMutex);
		gNumWaiting--;
		QueuePosition *c = clientIn(gCurrClientQueue);
		//printf("NEXT CLIENT!\n");
		gCurrServed = c->clientId;
		printStats(gNumResigned, gNumWaiting, gNumMaxChairs, gCurrServed);
		//PrintQueue(gCurrClientQueue);
		//PrintQueue(gResignedClientQueue);
		// Number state not needed to change, unlock the mutex
		pthread_mutex_unlock(&gChangeNumClientsMutex);
		// inform client that its now his/her turn
		sem_post(&c->clientTurnSemaphore);
		
		cutHair(c);
		gCurrServed = 0;
	}
}

int main()
{
	gNumMaxChairs = 10;
	pthread_t barberThreadPtr;
	pthread_t customerThreadPtr;
	
	// spawn only ONE barber thread
	if (pthread_create(&barberThreadPtr, NULL, (void*)barberThread, NULL) != 0)
	{
		printf("FATAL: couldn't start barber thread");
		exit(-1);
	}
	
	while (1)
	{
		sleep(rand()%3);
		// constanly spawn new customer threads. those will always exit
		if (pthread_create(&customerThreadPtr, NULL, (void*)customerThread, NULL) != 0)
		{
			printf("FATAL: couldn't start customer thread");
			exit(-1);
		}
	}
	pthread_join(barberThreadPtr, NULL);
}