#include "control.h"
#include <stdio.h>
#include "global.h"
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "face.h" 
#include "myoled.h"
#include "face_mqueue.h"
#include <stdlib.h>
#include "mydevice.h"
#include "ini.h"
//#include "bedroom_lamp.h"
//#include "fan_dev.h"
//#include "lock_dev.h"
//#include "beep_dev.h"
//#include "living_room_lamp.h"

typedef struct{
    int msg_len;
    unsigned char *buf ;
    ctrl_info_t *ctrl_info;
}recv_msg_t;
static int oled_fd = -1;
static struct mydevice *pdevhead = NULL;
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{    
    struct mydevice *pdev = NULL;
    if(pdevhead == NULL){
        pdevhead = (struct mydevice*)malloc(sizeof(struct mydevice));        
        pdevhead->next= NULL;
        memset(pdevhead, 0, sizeof(struct mydevice));
        strcpy(pdevhead->dev_name, section);
    }else if(strcmp(section, pdevhead->dev_name)!= 0){
        pdev = (struct mydevice*)malloc(sizeof(struct mydevice));
        memset(pdev, 0, sizeof(struct mydevice));
        strcpy(pdev->dev_name, section);
        pdev->next = pdevhead;
        pdevhead = pdev;
    }
    if(pdevhead!= NULL){
        if(MATCH(pdevhead->dev_name, "key")) {
            sscanf(value, "%x", &pdevhead->key);
            printf("key: %x\n", pdevhead->key);
        }else if(MATCH(pdevhead->dev_name, "gpio_pin")) {
            pdevhead->gpio_pin = atoi(value);
        }else if(MATCH(pdevhead->dev_name, "gpio_mode")) {
            if(strcmp(value, "OUTPUT")==0){
                pdevhead->gpio_mode = 1;
            }else if(strcmp(value, "INPUT")==0){
                pdevhead->gpio_mode = 0;
            }
        }else if(MATCH(pdevhead->dev_name, "gpio_status")) {
            if(strcmp(value, "LOW")==0){
                pdevhead->gpio_mode = 0;
            }else if(strcmp(value, "HIGH")==0){
                pdevhead->gpio_mode = 1;
            }
        }else if(MATCH(pdevhead->dev_name, "check_face_status")) {
            pdevhead->check_face_status = atoi(value);
        }else if(MATCH(pdevhead->dev_name, "voice_set_status")) {
            pdevhead->voice_set_status = atoi(value);
        }
            
    }
    return 1;
}
static int receive_init(void){
    if (ini_parse("/home/orangepi/yuanxueshe/smart_home/face_recg/ini/mydevice.ini", handler, NULL) < 0) {
        printf("Can't load 'test.ini'\n");
        return 1;
    }
    
    oled_fd = myoled_init();
    python_init();
    return 0;
}
static void receive_final(void){
    python_final();
    if(oled_fd != -1){
        myoled_deinit();
        oled_fd = -1;
    }
}
static void *handle_device(void *arg){
    pthread_detach(pthread_self());
    pthread_t voice_tid;
    char *voice_buf=(char *)malloc(sizeof(char)*6);
    struct mydevice *pdevice = NULL;
    recv_msg_t *recv_msg = (recv_msg_t *)arg;
    if(arg != NULL){
        printf("recv_msg->msg_len = %d\n",recv_msg->msg_len);
        printf("recv_msg->buf = %x %x %x %x %x %x \n",recv_msg->buf[0],recv_msg->buf[1]\
        ,recv_msg->buf[2],recv_msg->buf[3],recv_msg->buf[4],recv_msg->buf[5]);
    }
    if(recv_msg->buf != NULL && recv_msg != NULL){
        pdevice = find_mydevice_by_key(pdevhead,recv_msg->buf[2]);
    }
    if(pdevice == NULL){
        pthread_exit(0);
    }

    if(pdevice->check_face_status == 1){
        system("wget http://127.0.0.1:8080/?action=snapshot -O \
						/home/orangepi/yuanxueshe/pic/myface.jpg");
        double ret = face_recg("face", "face_search");
        system("sudo rm -f /home/orangepi/yuanxueshe/pic/myface.jpg");
        if(ret > 0.6){
            printf("face_recgnition success\n");
            recv_msg->buf[2]=0x47;
            recv_msg->buf[3]=0x00;
        }else{
            recv_msg->buf[2]=0x46;
            recv_msg->buf[3]=0x00;
            printf("face_recgnition failed\n");
        }
    }
    oled_demo(recv_msg->buf);
    if(pdevice->voice_set_status == 1){
        while(recv_msg->ctrl_info->ctrl_phead!= NULL){
            if(strstr(recv_msg->ctrl_info->ctrl_phead->control_name,"voice_crl")!= NULL){
                if(recv_msg->buf[2] == 0x58 && recv_msg->buf[3] == 0x00){
                    printf("a risk of fire\n");
                    pthread_create(&voice_tid,NULL,recv_msg->ctrl_info->ctrl_phead->set,recv_msg->buf);
                    break;
                }
                if(recv_msg->buf[2] == 0x47 || recv_msg->buf[2] == 0x46){
                    printf("recv_msg->buf= %x %x %x %x %x %x \n",recv_msg->buf[0],recv_msg->buf[1]\
                    ,recv_msg->buf[2],recv_msg->buf[3],recv_msg->buf[4],recv_msg->buf[5]);
                    voice_buf = recv_msg->buf;
                    printf("voice_buf = %x %x %x %x %x %x \n",voice_buf[0],voice_buf[1]\
                    ,voice_buf[2],voice_buf[3],voice_buf[4],voice_buf[5]);
                    pthread_create(&voice_tid,NULL,recv_msg->ctrl_info->ctrl_phead->set,voice_buf);
                    break;
                }
            }else{
                recv_msg->ctrl_info->ctrl_phead = recv_msg->ctrl_info->ctrl_phead->next;
            }
        }
    }
    pdevice->gpio_status = recv_msg->buf[3] == 0 ? LOW : HIGH;
    if(recv_msg->buf[2] == 0x47 ){
        pdevice->gpio_status=LOW;
        set_gpio_mydevice_status(pdevice);
        sleep(5);
        pdevice->gpio_status=HIGH;
        set_gpio_mydevice_status(pdevice);
        recv_msg->buf[2]=0x46;
    }
    if(recv_msg->buf[2] != 0x47 && recv_msg->buf[2] != 0x46){
        set_gpio_mydevice_status(pdevice);
    }
    printf("recv_msg->buf= %x %x %x %x %x %x \n",recv_msg->buf[0],recv_msg->buf[1]\
            ,recv_msg->buf[2],recv_msg->buf[3],recv_msg->buf[4],recv_msg->buf[5]);
    oled_demo(recv_msg->buf);
    pthread_exit(0);
    
}
static void *receive_get(void *arg){
    pthread_detach(pthread_self());
    pthread_t tid;
    struct mq_attr attr;
    int read_len;
    char *buffer;
    recv_msg_t *recv_msg;
    if(arg !=  NULL)
    {
        recv_msg = (recv_msg_t *)malloc(sizeof(recv_msg_t));
        recv_msg->ctrl_info = (ctrl_info_t *)arg;
        recv_msg->msg_len = -1;
        recv_msg->buf = NULL;
    }else{
        pthread_exit(0);
    }
    if(mq_getattr(recv_msg->ctrl_info->mqd,&attr) == -1){
        pthread_exit(0);    
    }

    recv_msg->buf = (unsigned char *)malloc(attr.mq_msgsize);
    buffer = (unsigned char *)malloc(attr.mq_msgsize);
    memset(buffer,0,attr.mq_msgsize);
    memset(recv_msg->buf,0,attr.mq_msgsize);
    while(1){
        if((read_len = mq_receive(recv_msg->ctrl_info->mqd,buffer,attr.mq_msgsize,NULL)) == -1){
            if(errno == EAGAIN){
                printf("mqueue is empty\n");
            }else{
                break;
            }
        }else if(buffer[0]==0xAA && buffer[1]==0x55 && buffer[4]==0x55 && buffer[5]==0xAA){

            recv_msg->msg_len = read_len;
            memcpy(recv_msg->buf,buffer,read_len);
            pthread_create(&tid,NULL,handle_device,(void *)recv_msg);
        }
    }
       
    pthread_exit(0);
}

struct control receive_control={
    .control_name = "receive_crl",
    .init = receive_init,
    .final = receive_final,
    .get = receive_get,
    .set = NULL,
    .next = NULL,
};
struct control *add_receive_to_ctrl_list(struct control *head){
    return add_interface_to_ctrl_list(head,&receive_control);

};