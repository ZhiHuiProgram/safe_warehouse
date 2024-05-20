#include <mqueue.h>
typedef struct 
{
	mqd_t mqd;
	struct control *ctrl_phead;
}ctrl_info_t;
