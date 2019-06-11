#include <semaphore.h>

typedef struct QueuePosition
{
	int clientId;
	sem_t clientTurnSemaphore;
	sem_t clientWasCutSemaphore;
	struct QueuePosition *next;
} QueuePosition;


void printStats(int numResigned, int numWaiting, int numMaxChairs, int currServed);

QueuePosition* addToQueue(QueuePosition **beginning, int clientId);

void printQueue(QueuePosition *beginning);