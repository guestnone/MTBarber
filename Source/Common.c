#include "Common.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

void printStats(int numResigned, int numWaiting, int numMaxChairs, int currServed)
{
	if (currServed != 0)
		printf("Resigned: %d Waiting Room: %d/%d Currently: %d\n", numResigned, numWaiting, numMaxChairs, currServed);
	else
		printf("Resigned: %d Waiting Room: %d/%d\n", numResigned, numWaiting, numMaxChairs);
}

QueuePosition* addToQueue(QueuePosition **beginning, int clientId)
{
	QueuePosition *new = malloc(sizeof(QueuePosition));
	new->clientId = clientId;
	new->next = NULL;
	sem_init(&new->clientTurnSemaphore, 0, 0);
	sem_init(&new->clientWasCutSemaphore, 0, 0);
	if (*beginning == NULL)
		(*beginning) = new;
	else 
	{
		QueuePosition *tmp = *beginning;
		while (tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = new;
	}
	return new;
}

void printQueue(QueuePosition *beginning)
{
	QueuePosition *curr = NULL;
	if (beginning == NULL) 
	{	
		printf("EMPTY");
	}
	else 
	{
		curr = beginning;
		while (curr != NULL)
		{
			printf("%d ", curr->clientId);
			curr = curr->next;
		}
	}
	printf("\n");
}