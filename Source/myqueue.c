#include "myqueue.h"

#include<stdlib.h>
#include <stdio.h>




int getNumberOfClients(struct client* c)
{
    int nr=0;
    while(c)
    {
        c=c->next;
        nr++;
    }
    return nr;
}
int getQueueFront(struct client* c)
{
    if(c)
    return c->nr;
    return -1;
}
struct client* addClientToQueue(struct client *c,int nr)
{
    //struct client *c=first;
    //assert(c);
	if(getNumberOfClients(c)==0)
	{
        //assert(c);
		c = malloc(sizeof(struct client));
		c->next=0;
		c->nr = nr;
		return c;
	}
	struct client *tmp = c;
	while(c->next!=0)
    {
        c=c->next;
    }
    c->next = malloc(sizeof(struct client));
    c->next->nr=nr;
    c->next->next=0;
    return tmp;
}

struct client* popQueueFront(struct client *c)
{

	//struct client *temp;
	//temp = c->next;
	//free(c);
    //return temp;

    if(c==NULL)
        return NULL;
    struct client *tmp;
    tmp = c;

    tmp=c->next;
    free (c);

    return tmp;




}

int showList(struct client *c)
{
    //struct client *current = &first;


    printf("list of resigned clients:\n");
    while(c)
    {

        printf("%d| ",c->nr);
        c=c->next;
    }
    printf("\n");
    return 0;
}

int isQueueEmpty(struct client* c)
{
    if(!c)return 1;
    return 0;
}
