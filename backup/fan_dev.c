#include "mydevice.h"
#include "control.h"
struct mydevice fan_device = {
    .dev_name = "fan",
    .key = 0x43,
    .gpio_pin = 8,
    .gpio_mode = OUTPUT,
    .gpio_status = HIGH,
    .check_face_status = 0,
    .voice_set_status = 0,
};
struct mydevice *add_fan_to_ctrl_list(struct mydevice *head){
    return add_device_to_mydevice_list(head,&fan_device);

};
