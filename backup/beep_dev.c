#include "mydevice.h"
#include "control.h"
struct mydevice beep_device = {
    .dev_name = "beep",
    .key = 0x58,
    .gpio_pin = 9,
    .gpio_mode = OUTPUT,
    .gpio_status = HIGH,
    .check_face_status = 0,
    .voice_set_status = 1,
};
struct mydevice *add_beep_to_ctrl_list(struct mydevice *head){
    return add_device_to_mydevice_list(head,&beep_device);

};
