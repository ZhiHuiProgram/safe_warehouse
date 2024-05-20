#ifndef _SOCKET_CTRL_H_
#define _SOCKET_CTRL_H_
static int socket_init(void);
static void socket_final(void);
static void *socket_get(void *arg);
static void *socket_set(void *arg);
struct control *add_socket_to_ctrl_list(struct control *head);
#endif