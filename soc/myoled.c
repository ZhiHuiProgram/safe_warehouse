#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "myoled.h"
#include "oled.h"
#include "font.h"
static struct display_info disp;
int oled_demo(unsigned char *buf) {
	unsigned char *buffer = buf;
	if(buffer != NULL){
		if(buffer[2] == 0x41 && buffer[3] == 0x00){
			oled_putstrto(&disp, 16, 22, "lr led|   open ");
		}else if(buffer[2] == 0x41 && buffer[3] == 0x01){
			oled_putstrto(&disp, 16, 22, "lr led|   close");
		}
		if(buffer[2] == 0x42 && buffer[3] == 0x01){
			oled_putstrto(&disp, 16, 12, "br led|   close");
		}else if(buffer[2] == 0x42 && buffer[3] == 0x00){
			oled_putstrto(&disp, 16, 12, "br led|   open ");
		}
		if(buffer[2] == 0x43 && buffer[3] == 0x01){
			oled_putstrto(&disp, 16, 42, "fan   |   close");
		}else if(buffer[2] == 0x43 && buffer[3] == 0x00){
			oled_putstrto(&disp, 16, 42, "fan   |   open ");
		}
		if(buffer[2] == 0x46 && buffer[3] == 0x00){
			oled_putstrto(&disp, 16, 52,  "door  |   close");
		}else if(buffer[2] == 0x47 && buffer[3] == 0x00){
			oled_putstrto(&disp, 16, 52,  "door  |   open ");
		}
		if(buffer[2] == 0x58 && buffer[3] == 0x01){
			oled_putstrto(&disp, 16, 32,  "beep  |   close");
		}else if(buffer[2] == 0x58 && buffer[3] == 0x00){
			oled_putstrto(&disp, 16, 32,  "beep  |   open ");
		}
	}
	disp.font = font2;
	oled_send_buffer(&disp);
	return 0;
}

void show_error(int err, int add) {
	//const gchar* errmsg;
	//errmsg = g_strerror(errno);
	printf("\nERROR: %i, %i\n\n", err, add);
	//printf("\nERROR\n");
}
void myoled_deinit() {
	oled_close(&disp);
//	free(disp);
}

int myoled_init() {
	int e;
//	disp = (struct display_info *)malloc(sizeof(struct display_info));
	memset(&disp, 0, sizeof(disp));
	disp.address = OLED_I2C_ADDR;

	e = oled_open(&disp, "/dev/i2c-3");
	disp.font = font2;

	if (e < 0) {
		show_error(1, e);
	} else {
		e = oled_init(&disp);
	}
	oled_clear(&disp);
	oled_putstrto(&disp, 0, 2, "DEVICE:   STATE:");
//	disp.font = font1; 
	oled_putstrto(&disp, 16, 12, "br led|   close");
	oled_putstrto(&disp, 16, 22, "lr led|   close");
	oled_putstrto(&disp, 16, 32, "beep  |   close");
	oled_putstrto(&disp, 16, 42, "fan   |   close");
	oled_putstrto(&disp, 16, 52, "door  |   close");
	oled_putstrto(&disp, 0, 60,  "------------------------");
	oled_send_buffer(&disp);
	return e;
}
