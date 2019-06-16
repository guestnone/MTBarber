#include <semaphore.h>

typedef struct QueuePosition
{
	int clientId;
	// Per-client semaphore informing that its the clients turm
	sem_t clientTurnSemaphore;
	// Per-client semaphore informing that the client has been cut
	sem_t clientWasCutSemaphore;
	struct QueuePosition *next;
} QueuePosition;


void printStats(int numResigned, int numWaiting, int numMaxChairs, int currServed);

QueuePosition* addToQueue(QueuePosition **beginning, int clientId);

void printQueue(QueuePosition *beginning);