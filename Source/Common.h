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

// Prints the stats of the client
void printStats(int numResigned, int numWaiting, int numMaxChairs, int currServed);

/// Adds the element to the new linked-list based queue
/// @param[in] beginning    pointer to the beginning of the queue
/// @param[in] clientId     Value with the client ID (used for debug purpose)
QueuePosition* addToQueue(QueuePosition **beginning, int clientId);

/// Prints the content of the queue
/// @param[in] beginning    pointer to the beginning of the queue
void printQueue(QueuePosition *beginning);