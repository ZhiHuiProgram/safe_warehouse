#ifndef _VOICECTRL_H_
#define _VOICECTRL_H_
static int voice_init(void);
static void voice_final(void);
static void *voice_get(void *arg);
static void *voice_set(void *arg);
struct control *add_voice_to_ctrl_list(struct control *head);
#endif
