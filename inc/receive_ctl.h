#ifndef _RECEIVE_CTRL_H_
#define _RECEIVE_CTRL_H_

#include "control.h"
static int receive_init(void);
static void receive_final(void);
static void *receive_get(void *arg);
struct control *add_receive_to_ctrl_list(struct control *head);
#endif