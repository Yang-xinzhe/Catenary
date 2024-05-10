#include "mailbox.h"

LIST_LINK *end_list = NULL;
MBS* create_mail_box_system()
{
    MBS *temp =(MBS*)malloc(sizeof(MBS));

    if(NULL ==  temp)
    {
        perror("create_mail_box_system mutex malloc failure\n");
        return NULL;
    }
    int ret = pthread_mutex_init(&temp->mutex,NULL);
    if(0 != ret)
    {
        perror("create_mail_box_system mutex init failure\n");
        return NULL;
    }
    temp->thread_list = malloc(sizeof(LIST_LINK));
   // memset(temp->thread_list, 0, sizeof(LIST_LINK));
    temp->thread_list->next = NULL;
    printf("mail box create ok!! \n");
    return temp;
}

int destroy_mail_box_system(MBS*mbs)
{
    pthread_mutex_destroy(&mbs->mutex);
    LIST_LINK *temp = NULL;
    LIST_LINK *find = mbs->thread_list;
    while(find !=  NULL)
    {
        temp = find;
        find = find->next;
        free(temp);
    }
    free(find);
    return 0;
}
int  register_to_mail_system(MBS *mbs,char name[],th_fun th)
{
    LIST_LINK* temp =  malloc(sizeof(LIST_LINK));
    if(NULL == temp)
    {
        perror("register to mail malloc  \n");
        return -1;
    }
    strcpy(temp->elem.name ,name);
    temp->elem.th = th;
    temp->next = NULL;
    init_que(temp);

    pthread_t ret = pthread_create(&(temp->elem.tid),NULL,th,NULL);
    if(0!=ret)
    {
        perror("register to mail thread create\n");
        return -1;
    }

    list_add(mbs->thread_list, temp);
    printf("register mail system  |%s|  ok \n", temp->elem.name);

    return 0;
}

int unregister_from_mailbox(MBS*msb,char*name)
{
    LIST_LINK* find=msb->thread_list->next;
    LIST_LINK *temp = NULL;

    while(find !=  NULL)
    {
        temp = find;
        find = find->next;
        if(0 == strcmp(temp->elem.name ,name))
        {
            destroy(find);
            free(temp);
            return 0;
        }
    }

    if(0 == strcmp(find->elem.name ,name))
    {
        destroy(find);
        free(find);
        return 0;
    }
    return -1;
}

int wait_all_end(MBS*msb)
{
    LIST_LINK *find=msb->thread_list->next;
    LIST_LINK *end=end_list;
    while(find != end)
    {
        // pthread_join(find,NULL);

        pthread_join(find->elem.tid,NULL);
        find = find->next;
    }
    pthread_join(find->elem.tid,NULL);
    return 0;
}



int send_msg(MBS*msb, char*recvname, DATA *data)
{
    MAIL_DATA* temp =  malloc(sizeof(MAIL_DATA));

    memcpy(&temp->data, data,sizeof(DATA));

    temp->id_of_sender = pthread_self();

    LIST_LINK *find = list_for_each(msb->thread_list, recvname);
    if (find == NULL)
    {
        printf("can,t find msg \n");
    }

    char* name = get_th_name(msb);
    strcpy(temp->name_of_sender,name);
    strcpy(temp->name_of_recver,recvname);
    ENTER_CRITICAL_AREA(&msb->mutex);
    in_queue(find, temp);
    QUIT_CRITICAL_AREA(&msb->mutex);
//    printf("send msg is ok |%s| msg is %s\n", temp->name_of_recver, temp->data);
    return 0;

}

int recv_msg(MBS*msb,char*sendname,DATA *data)
{
    MAIL_DATA* temp =  malloc(sizeof(MAIL_DATA));
    pthread_t tid =  pthread_self();

    LIST_LINK *find = msb->thread_list;

    while(find != NULL)
    {
        if( find->elem.tid == tid)
            break;
        find = find->next;
    }

    if( find->elem.tid == tid)
    {
        while (1)
        {
            if(find->elem.mail_head != find->elem.mail_tail)
            {
                ENTER_CRITICAL_AREA(&msb->mutex);
                out_queue(find, temp);
                QUIT_CRITICAL_AREA(&msb->mutex);
                break;
            }
        }
    }

    strcpy(sendname, temp->name_of_sender);
    memcpy(data, &temp->data,sizeof(DATA));

    free(temp);

    return 0;

}
char *get_th_name(MBS*msb)
{
//	LIST_LINK * end_list;
    pthread_t tid = pthread_self();
    LIST_LINK *find = msb->thread_list;
    LIST_LINK *end = end_list;
    while(find != end)
    {
        if(find->elem.tid == tid)
            break;
        find = find->next;
    }
    if(find->elem.tid == tid)
    {
        //printf("cant find the recv th\n");
        return find->elem.name;
    }
    else
        return NULL;
}
