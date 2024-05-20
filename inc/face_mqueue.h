#include <mqueue.h>
mqd_t mqueue_init();
void mqueue_close(mqd_t mq);
int send_msg(mqd_t mq, char *msg, int len);
int recv_msg(mqd_t mq, int len);