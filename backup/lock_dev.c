#include "mydevice.h"
#include "control.h"
struct mydevice lock_device = {
    .dev_name = "lock",
    .key = 0x44,
    .gpio_pin = 5,
    .gpio_mode = OUTPUT,
    .gpio_status = HIGH,
    .check_face_status = 1,
    .voice_set_status = 1,
};
struct mydevice *add_lock_to_ctrl_list(struct mydevice *head){
    return add_device_to_mydevice_list(head,&lock_device);

};