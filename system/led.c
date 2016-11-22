/*led.c*/
#include <xinu.h>

syscall turn_light_on(did32 dev_id){
	bool8 turnon =1;
	return gpiooutputc(&devtab[dev_id],turnon); /*dev_id==LED?*/
}


syscall turn_light_off(did32 dev_id){
	bool8 turnon=0;
	return gpiooutputc(&devtab[dev_id],turnon);
}


syscall light_status(did32 dev_id){
	return (syscall) gpiooutgetc(&devtab[dev_id]);
}

