#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>
#include "Common.h"

// mutex used to access data
pthread_mutex_t gChangeNumClientsMutex = PTHREAD_MUTEX_INITIALIZER;

// semaphore used to inform the barber that the client is waiting (if its asleep)
sem_t gClientReadySemaphore;

// number of resigned clients
int gNumResigned = 0;
// number of waiting clients in the queue
int gNumWaiting = 0;
// number of all clients that went into the shop, used also as a ID source
int gNumTotal = 0;
// number of chairs in the waiting room
int gNumMaxChairs = 0;
// get the debug info list of the resigned clients and the queue
bool gDebug = false;
// internal value for currently served client
volatile int gCurrServed;

// Waiting room queue
QueuePosition *gCurrClientQueue = NULL;
// Used to store resigned clients
QueuePosition *gResignedClientQueue = NULL;

// seed for random cut time (taken from the Paweł's code)
int randomizer=0;

// function that simulates the cutting of the hair
void cutHair(QueuePosition *client)
{
	// sleep for random time plus value from the randomizer
    srand(time(NULL));
    int t=(rand()+randomizer)%10;
	randomizer=(randomizer+1)%5;
    sleep(t+1);
	// inform that he/she was cutted.
	sem_post(&client->clientWasCutSemaphore);
}

// shrinks the queue and returns incoming client
QueuePosition* clientIn()
{
    QueuePosition *ret = gCurrClientQueue;
	gCurrClientQueue = gCurrClientQueue->next;
	return ret;
}

void printDebugQueues()
{
	printf("Current clients: ");
	printQueue(gCurrClientQueue);
	printf("Resigned clients: ");
	printQueue(gResignedClientQueue);
}

// thread for the customer, one spawned per client
void *customerThread(void *notUsed)
{
	// We need to change the number of clients, lock the access mutex
	pthread_mutex_lock(&gChangeNumClientsMutex);
	// get client id for new customer
	gNumTotal++;
	int clientId = gNumTotal;
	if (gNumWaiting < gNumMaxChairs)
	{
		gNumWaiting++;
		QueuePosition *newClient = addToQueue(&gCurrClientQueue, gNumTotal);
		printStats(gNumResigned, gNumWaiting, gNumMaxChairs, gCurrServed);
		if (gDebug == true)
			printDebugQueues();
		// Client is read and its waiting, wake the barber
		sem_post(&gClientReadySemaphore);
		// Number state not needed to change, unlock the mutex
		pthread_mutex_unlock(&gChangeNumClientsMutex);
		// Wait for the turn
		sem_wait(&newClient->clientTurnSemaphore);
		// Wait when you're getting cut
		sem_wait(&newClient->clientWasCutSemaphore);
	}
	else // add resigned client to the resigned queue 
	{
		addToQueue(&gResignedClientQueue, gNumTotal);
		gNumResigned++;
		printStats(gNumResigned, gNumWaiting, gNumMaxChairs, gCurrServed);
		if (gDebug == true)
			printDebugQueues();
		// Also unlock the client number data access
		pthread_mutex_unlock(&gChangeNumClientsMutex);
	}
	
	pthread_exit(0);
}

// Main thread for the barber.
void* barberThread(void *notUsed)
{
	while(true)
	{
		// barber sleeps if there's no one in the queue
		sem_wait(&gClientReadySemaphore);
		// We need to change the number of clients, lock the access mutex
		pthread_mutex_lock(&gChangeNumClientsMutex);
		gNumWaiting--;
		QueuePosition *c = clientIn(gCurrClientQueue);
		//printf("NEXT CLIENT!\n");
		gCurrServed = c->clientId;
		printStats(gNumResigned, gNumWaiting, gNumMaxChairs, gCurrServed);
		if (gDebug == true)
			printDebugQueues();
		// Number state not needed to change, unlock the mutex
		pthread_mutex_unlock(&gChangeNumClientsMutex);
		// inform client that its now his/her turn
		sem_post(&c->clientTurnSemaphore);
		cutHair(c);
		// reset the id if its not serving
		gCurrServed = 0;
		free(c);
	}
}

int main(int argc, char *argv[])
{
	gNumMaxChairs = 10;
	pthread_t barberThreadPtr;
	pthread_t customerThreadPtr;
	
	sem_init(&gClientReadySemaphore, 0, 0);
	
	if (argc < 2)
	{
		printf("Usage: MainMutexed [NUMCHAIRS] [--debug]\n\n    --debug - lists the state of the queues\n");
		exit(EXIT_FAILURE);
	}
	
	gNumMaxChairs = atoi(argv[1]);
	
	if(argc == 3)
		if(strncmp(argv[2], "--debug", 6) == 0)
			gDebug = true;
	
	// spawn only ONE barber thread
	if (pthread_create(&barberThreadPtr, NULL, (void*)barberThread, NULL) != 0)
	{
		printf("FATAL: couldn't start barber thread\n");
		exit(EXIT_FAILURE);
	}
	
	while (1)
	{
		sleep(rand()%3);
		// constantly spawn new customer threads. those will always exit
		if (pthread_create(&customerThreadPtr, NULL, (void*)customerThread, NULL) != 0)
		{
			printf("FATAL: couldn't start customer thread\n");
			exit(EXIT_FAILURE);
		}
	}
	pthread_join(barberThreadPtr, NULL);
}