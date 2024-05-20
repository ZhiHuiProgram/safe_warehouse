#include "control.h"
#include "face_mqueue.h"
#include <stdio.h>
#include "my_uart.h"
#include "voice_ctl.h"
#include <pthread.h>
#include <stdlib.h>
#include "global.h"
#include <unistd.h>

static int fd=-1;
static int voice_init(void)
{
    fd = uart_open("/dev/ttyS5", 115200);
    printf("%s| %s |%d",__FILE__,__FUNCTION__,__LINE__);
    return fd;
}

static void voice_final(void)
{
    if(fd > 0){
        close(fd);
    }
}

static void *voice_get(void *arg){
    pthread_detach(pthread_self());
    int len;
    printf("%s thread start\n",__func__);
    ctrl_info_t *ctrl_info = (ctrl_info_t *)arg;
    mqd_t mqd = -1;
    if(arg != NULL){
        ctrl_info = (ctrl_info_t *)arg;
        mqd = ctrl_info->mqd;
    }
    if (-1 == fd){
        fd = voice_init();
        if (-1 == fd)
        {
            pthread_exit(0);
        }
    }
    if(mqd == -1){
        pthread_exit(0);
    }
    char *voice_data=(char*)malloc(6);
    voice_data[2]=0x00;
    voice_data[3]=0x00;
    printf("hello\n");
    while(1){
	    len = uart_getsdata(fd, 6,voice_data);
        if(len>0){
            if(voice_data[0]==0xAA && voice_data[1]==0x55 && \
                voice_data[4]==0x55 && voice_data[5]==0xAA){
                printf("hello voice\n");
                send_msg(ctrl_info->mqd, voice_data, 6);
                if(voice_data[2]==0x01 && voice_data[3]==0x01){
                    write(fd, voice_data, 6);

                }
            }
        }
	}
    pthread_exit(0);
}

static void *voice_set(void *arg)
{
    pthread_detach(pthread_self());
    unsigned char *buffer = (unsigned char *)arg;
    if (-1 == fd){
        fd = voice_init();
        if (-1 == fd)
        {
            pthread_exit(0);
        }
    }
    if(buffer != NULL){
        write(fd, buffer, 6);
    }
    pthread_exit(0);
}
struct control voice_control={
    .control_name = "voice_crl",
    .init = voice_init,
    .final = voice_final,
    .get = voice_get,
    .set = voice_set,
    .next = NULL,
};
struct control *add_voice_to_ctrl_list(struct control *head){
    return add_interface_to_ctrl_list(head,&voice_control);
};