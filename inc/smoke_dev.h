#ifndef _SMOKEDEV_H_
#define _SMOKEDEV_H_
static int smoke_init(void);
static void smoke_final(void);
static void *smoke_get(void *arg);
static void *smoke_set(void *arg);
struct control *add_smoke_to_ctrl_list(struct control *head);
#endif