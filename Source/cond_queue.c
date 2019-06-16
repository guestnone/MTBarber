#include "cond_queue.h"
#include <stdlib.h>
#include <stdio.h>




int getcondQueueSize(struct cond_queue* c)
{
    int nr=0;
    while(c)
    {
        c=c->next;
        nr++;
        //printf("nr: %d ",nr);
    }
    return nr;
}
///to na dole niepotrzebne ???

struct cond_queue * getFirstElementOfCondQueue(struct cond_queue **c)
{
    return *c;
}

struct cond_queue* getLastElementOfCondQueue(struct cond_queue **c)
{
    struct cond_queue * temp = *c;
    if(temp==NULL)return NULL;
    while(temp->next!=NULL)
    {
        temp = temp->next;
    }
    return temp;


}


struct cond_queue* addElementtoCondQueue(struct cond_queue **c,int nr)
{
    //struct mythread *c=first;

    //assert(c);
	if(*c==NULL)
	{
        //assert(c);
        struct cond_queue *temp = malloc(sizeof(struct cond_queue));
        temp->next=NULL;
        temp->nr=nr;
        pthread_cond_init(&temp->cond,NULL);
        (*c) = temp;
        //printf("nr value: %d\n",temp->nr);
        //first = c;
        return temp;
		//*c = malloc(sizeof(struct cond_queue));
		//*c->next=NULL;
		//*c->nr =nr;
		//pthread_cond_init(&c->cond,NULL);
		//return c;
	}
	struct cond_queue *tmp = *c;

	while(tmp->next!=NULL)
    {
        tmp=tmp->next;
    }
    struct cond_queue * mynew = malloc(sizeof(struct cond_queue));
    mynew->nr=nr;
    mynew->next=NULL;
    pthread_cond_init(&mynew->cond,NULL);
    tmp->next=mynew;




    return mynew;
}

struct cond_queue* popCondQueueFront(struct cond_queue *c)
{
    if(c==NULL)
        return NULL;
	//free(*c);
    //struct cond_queue * tmp;
    //tmp = c;
    c=c->next;
    //free(tmp);

    return c;

    //temp = (*c);
	//(c)=(*c)->next;

    //c=c->next;
    //return c;
}


void printQueue(struct cond_queue *c)
{
//printf("printing list of clients in queue \n");
    struct cond_queue * temp;
    temp = c;
    if(isCondQueueEmpty(c))
    {
       // printf("queue is empty - cant print it\n");
        return;
    }
    while(temp !=NULL)
    {
        printf("%d | ",temp->nr);
        temp=temp->next;
    }
   // printf("done printing queue\n");
}
int isCondQueueEmpty(struct cond_queue* c)
{
    if(c==NULL)return 1;
    return 0;
}
