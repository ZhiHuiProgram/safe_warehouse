#include<stdio.h>
#include<mqueue.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
mqd_t mqueue_init(){
    mqd_t mq;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 1024;
    attr.mq_curmsgs = 0;
    mq = mq_open("/my_queue", O_CREAT | O_RDWR, 0666, &attr);
    if(mq == -1){
        perror("mq_open");
        return -1;
    }
    printf("%s| %s|%d",__FILE__,__func__,__LINE__); 
    return mq;
}
void mqueue_close(mqd_t mq){
    mq_close(mq);
    mq_unlink("/my_queue");
}
int send_msg(mqd_t mq, char *msg, int len){
    int ret;
    ret = mq_send(mq,msg,len,0);
    if(ret<0){
        perror("mq_send");
        pthread_exit(NULL);
    }
    return ret;
}
char *recv_msg(mqd_t mq, int len){
    int ret;
    char *msg = (char *)malloc(len);
    ret = mq_receive(mq,msg,len,NULL);
    if(ret<0){
        perror("mq_receive");
        pthread_exit(NULL);
    }
    printf("recv msg:%s\n",msg);
    return msg;
}
