#include <stdio.h>
#include "mydevice.h"
//struct mydevice *add_device_to_mydevice_list(struct mydevice *head,struct mydevice *device ){
//    if(head == NULL){
//        head = device;
//        return head;
//    }else{
//        device->next = head;
//        head = device;
//        return head;
//    }
//
//}
struct mydevice *find_mydevice_by_key(struct mydevice *pdev, unsigned char key){
    struct mydevice *p = NULL;
    if(pdev == NULL){
        return NULL;
    }
    p = pdev;
    while(p != NULL){
        if(p->key == key){
            return p;
        }
        p = p->next;
    }
}
int set_gpio_mydevice_status(struct mydevice *pdev){
    if(pdev == NULL){
        return -1;
    }
    if(pdev->gpio_pin != -1){
        if(pdev->gpio_mode != -1){
            pinMode(pdev->gpio_pin,pdev->gpio_mode);
        }
        if(pdev->gpio_status != -1){
            digitalWrite(pdev->gpio_pin, pdev->gpio_status);
        }
    }
    return 0;
}