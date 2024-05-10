#ifndef __LIST_H_
#define __LIST_H_
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef void *(*th_fun)(void *arg);
typedef struct 
{
    double angle; //encoder
    double distance; //laser
    double lenth;//linear
    double Xaxis;//tilt
    double Yaxis;//tilt
    double temp;//tilt
}DATA;

typedef struct mail_data
{
    pthread_t id_of_sender;
    char name_of_sender[256];
    pthread_t id_of_recver;
    char name_of_recver[256];
    DATA data;
}MAIL_DATA;

typedef struct queue
{
    MAIL_DATA data;
    struct queue *next;
}Que,*pQue;

typedef struct thread_node
{
    pthread_t tid;
    char name[256];
    Que *mail_head, *mail_tail;
    th_fun th;
}LIST_DATA;

typedef struct Link
{
    LIST_DATA elem;
    struct Link *next;
}LIST_LINK;

extern LIST_LINK *list_init();
extern LIST_LINK *list_for_each(LIST_LINK *head, char *name);

extern void list_add(LIST_LINK *head, LIST_LINK *info);

#endif