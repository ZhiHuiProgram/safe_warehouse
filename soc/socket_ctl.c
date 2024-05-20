#include "socket_ctl.h"
#include "control.h"
#include "server.h"
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include "global.h"
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "face_mqueue.h"
static int s_fd=-1;
static int socket_init(void){

    s_fd = face_socket();
    if(s_fd < 0){
        perror("socket");
        return -1;
    }
}
static void socket_final(void){
    close(s_fd);
}
static void *socket_get(void *arg){
    pthread_detach(pthread_self());
    int c_fd,len,ret;
    struct sockaddr_in c_addr;
    char buf[1024];
    printf("hello socket\n");
    if(s_fd < 0){
        s_fd = socket_init();
        if(s_fd < 0){
            perror("socket");
            return NULL;
        }
    }
    ctrl_info_t *ctrl_info = (ctrl_info_t *)arg;
    mqd_t mqd = -1;
    if(arg != NULL){
        ctrl_info = (ctrl_info_t *)arg;
        mqd = ctrl_info->mqd;
    }
    memset(&c_addr,0,sizeof(c_addr));
    len = sizeof(c_addr);
    while(1){
        printf("connecting...\n");
		c_fd=accept(s_fd,(struct sockaddr *)&c_addr , &len);
        printf("connect success\n");
        int keepalive = 1; // 开启TCP KeepAlive功能
        int keepidle = 5; // tcp_keepalive_time 3s内没收到数据开始发送心跳包
        int keepcnt = 3; // tcp_keepalive_probes 每次发送心跳包的时间间隔,
        int keepintvl = 3;  // tcp_keepalive_intvl 每3s发送一次心跳包
        setsockopt(c_fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));
        setsockopt(c_fd, SOL_TCP, TCP_KEEPIDLE, (void *) &keepidle, sizeof (keepidle));
        setsockopt(c_fd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcnt, sizeof (keepcnt));
        setsockopt(c_fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepintvl, sizeof (keepintvl));
        printf("%s|%s|%d: Accept a connection from %s:%d\n", __FILE__, __func__, \
                __LINE__, inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
        if(c_fd == -1)
        {
            perror("accept");
            continue;
        }
        while(1){
            memset(buf,0,sizeof(buf));
            ret = read(c_fd, buf, sizeof(buf));
            if(ret <= 0){
                break;
            }else{
                if(buf[0]==0xAA && buf[1]==0x55 && \
                buf[4]==0x55 && buf[5]==0xAA){
                    printf("hello socket");//消息队列替换
                    send_msg(mqd, buf, len);
                }
            }
            printf("socket send_buf is %s\n",buf);   
            
            if(strcmp(buf,"exit")==0){
                break;
            }
        }

	}
}

struct control socket_control={
    .control_name = "socket_crl",
    .init = socket_init,
    .final = socket_final,
    .get = socket_get,
    .set = NULL,
    .next = NULL,
};
struct control *add_socket_to_ctrl_list(struct control *head){
    return add_interface_to_ctrl_list(head,&socket_control);

};