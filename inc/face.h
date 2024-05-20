#ifndef FACE_H
#define FACE_H
#define TAKE_PHOTO "wget http://127.0.0.1:8080/?action=snapshot -O \
						/home/orangepi/yuanxueshe/pic/image.jpg"
#define CLEAN_PHOTO "sudo rm -f /home/orangepi/yuanxueshe/pic/image.jpg"
#define FACE_FILE "/home/orangepi/yuanxueshe/pic/image.jpg"
void python_init();
double face_recg(char *model, char *function);
void python_final();
#endif