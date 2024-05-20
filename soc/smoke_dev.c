#include "control.h"
#include "face_mqueue.h"
#include <stdio.h>
#include "my_uart.h"
#include "smoke_dev.h"
#include <pthread.h>
#include <stdlib.h>
#include "global.h"
#include <unistd.h>
#include <wiringPi.h>
#define SMOKE_PIN 6
#define SMOKE_MODE INPUT

static int smoke_init(void)
{
    printf("smoke_init \n");
    pinMode(SMOKE_PIN, SMOKE_MODE);
    return 0;
}

static void smoke_final(void)
{
    //nothing
}

static void *smoke_get(void *arg){
    int status = HIGH;
    int switch_status = 0;
    unsigned char buffer[6] = {0xAA, 0x55, 0x00, 0x00, 0x55, 0xAA};
    ssize_t byte_send = -1;
    mqd_t mqd = -1;
      ctrl_info_t *ctrl_info = NULL;
      if (NULL != arg)
          ctrl_info = (ctrl_info_t *)arg;
    
      if(NULL != ctrl_info)
     {
          mqd =  ctrl_info->mqd;
     }
      if ((mqd_t)-1 == mqd)
     {
          pthread_exit(0);
     }
      pthread_detach(pthread_self());
      printf("%s thread start\n", __func__);
      while(1)
     {   
          status = digitalRead(SMOKE_PIN);
          if (LOW == status)
         {
            buffer[2] = 0x58;
            buffer[3] = 0x00;
            switch_status = 1;
            byte_send = mq_send(mqd, buffer, 6, 0);
            if (-1 == byte_send)
            {
                continue;
            }
        }
        else if (HIGH == status && 1 == switch_status)
        {
            buffer[2] = 0x58;
            buffer[3] = 0x01;
            printf("smoke relieve!\n");
            switch_status = 0;
            byte_send = mq_send(mqd, buffer, 6, 0);
            if (-1 == byte_send)
            {
                continue;
            }
        }
        sleep(4);

    }
    pthread_exit(0);
}

static void *smoke_set(void *arg)
{
    
}
struct control smoke_control={
    .control_name = "smoke_crl",
    .init = smoke_init,
    .final = smoke_final,
    .get = smoke_get,
    .set = NULL,
    .next = NULL,
};
struct control *add_smoke_to_ctrl_list(struct control *head){
    return add_interface_to_ctrl_list(head,&smoke_control);

};