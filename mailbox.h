#ifndef __MAILBOX_H_
#define __MAILBOX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "list.h"
#include "queue.h"

#define ENTER_CRITICAL_AREA(mutex) do{pthread_mutex_lock(mutex);}while(0);
#define QUIT_CRITICAL_AREA(mutex) do{pthread_mutex_unlock(mutex);}while(0);
extern unsigned pthread_index;

typedef struct mail_box_system
{
    pthread_mutex_t mutex;
    LIST_LINK *thread_list;
}MBS;

extern MBS* create_mail_box_system();
extern int destroy_mail_box_system(MBS*mbs);
extern int register_to_mail_system(MBS *mbs,char name[],th_fun th);
extern int unregister_from_mailbox(MBS*msb,char*name);
extern int wait_all_end(MBS*msb);
extern int send_msg(MBS*msb, char*recvname, DATA *data);
extern int recv_msg(MBS*msb,char*sendname,DATA *data);
extern char *get_th_name(MBS*msb);

#endif