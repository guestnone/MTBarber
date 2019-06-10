typedef struct client
{
    int nr;
    struct client* next;
} client ;

int getNumberOfClients();
int getQueueFront(struct client* c);
struct client* addClientToQueue(struct client *c,int nr);

struct client* popQueueFront(struct client *c);
int isQueueEmpty(struct client* c);
