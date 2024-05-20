#include <Python.h>
#include <stdio.h>
#include "face.h"
#include "mydevice.h"
#include "my_uart.h"
#include "control.h"
#include "face_mqueue.h"
#include <mqueue.h>
#include "voice_ctl.h"
#include "receive_ctl.h"
#include "socket_ctl.h"
#include "smoke_dev.h"
#include "global.h"

static int detect_process(const char *process_name);//判断进程是否已经启动
double face_recgnition(char *model, char *function);
int main(){
    int ret_mjpg;
	ret_mjpg = detect_process("mjpg_streamer");//用于判断mjpg_streamer服务是否已经启动
	if ( -1 == ret_mjpg)
	{
		printf("detect process failed\n");
		return -1;
	}
	ctrl_info_t *ctrl_info=NULL;
	ctrl_info = (ctrl_info_t *)malloc(sizeof(ctrl_info_t));
	ctrl_info->ctrl_phead = NULL;
	ctrl_info->mqd = -1;

    ctrl_info->mqd = mqueue_init();
    if (ctrl_info->mqd == -1){
		printf("mqueue_init failed\n");
		return -1;
	}

    struct control *pvoice;
   // struct control *control_head = NULL;

    int node_num = 0;
	ctrl_info->ctrl_phead = add_smoke_to_ctrl_list(ctrl_info->ctrl_phead);
	ctrl_info->ctrl_phead = add_receive_to_ctrl_list(ctrl_info->ctrl_phead);
	ctrl_info->ctrl_phead = add_socket_to_ctrl_list(ctrl_info->ctrl_phead);
    ctrl_info->ctrl_phead = add_voice_to_ctrl_list(ctrl_info->ctrl_phead);

    pvoice = ctrl_info->ctrl_phead;
    while (pvoice != NULL)
    {
		if(pvoice->init != NULL){
       		pvoice->init();
		}
        pvoice = pvoice->next;
        node_num++;
    }
	printf("node_num = %d\n", node_num);
	pthread_t *thread_id = malloc(sizeof(pthread_t) * node_num);
    pvoice = ctrl_info->ctrl_phead;
	for(int i=0; i<node_num; i++){
	    pthread_create(&thread_id[i], NULL, (void *)pvoice->get, (void *)ctrl_info);
		pvoice = pvoice->next;
	}
	while(1);


	pvoice = ctrl_info->ctrl_phead;
	for(int i=0; i<node_num; i++){
		pvoice->final();
        pvoice = pvoice->next;

	}
	for(int i=0; i<node_num; i++){
		pthread_join(thread_id[0], NULL);
	}
	
	mqueue_close(ctrl_info->mqd);
	free(ctrl_info);
	free(thread_id);
//    double ret = face_recgnition("face", "face_search");//人脸识别
 //   printf("%.2f\n", ret);
    return 0;
}
double face_recgnition(char *model, char *function){
    double ret;
    python_init();
    system(TAKE_PHOTO);
    if (0 == access(FACE_FILE, F_OK)){
		ret = face_recg(model, function);
	}else{
		printf("no file\n");
	}
    python_final();
    system(CLEAN_PHOTO);
    return ret;
}
static int detect_process(const char *process_name)
{ 
	int n = -1; 
	FILE *strm; 
	char buf[128]={0}; 
	sprintf(buf,"ps -ax | grep %s|grep -v grep", process_name); 

	if((strm = popen(buf, "r")) != NULL) 
	{ 
		if(fgets(buf, sizeof(buf), strm) != NULL) 
		{
			printf("buf=%s\n",buf);
			n = atoi(buf); 
			printf("n=%d\n",n);
		}
	}
	else
	{
		return -1;
	} 

	pclose(strm); 
	return n; 
}