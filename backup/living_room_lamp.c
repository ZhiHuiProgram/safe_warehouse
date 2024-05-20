#include "mydevice.h"
#include "control.h"
struct mydevice livingled_device = {
    .dev_name = "living_room_lamp",
    .key = 0x41,
    .gpio_pin = 2,
    .gpio_mode = OUTPUT,
    .gpio_status = HIGH,
    .check_face_status = 0,
    .voice_set_status = 0,
};
struct mydevice *add_living_room_lamp_to_ctrl_list(struct mydevice *head){
    return add_device_to_mydevice_list(head,&livingled_device);

};
