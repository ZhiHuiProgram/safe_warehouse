#include "mydevice.h"
#include "control.h"
struct mydevice bedlamp_device = {
    .dev_name = "bedroom_lamp",
    .key = 0x42,
    .gpio_pin = 7,
    .gpio_mode = OUTPUT,
    .gpio_status = HIGH,
    .check_face_status = 0,
    .voice_set_status = 0,
};
struct mydevice *add_bedroom_lamp_to_ctrl_list(struct mydevice *head){
    return add_device_to_mydevice_list(head,&bedlamp_device);
};
