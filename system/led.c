/*led.c*/
#include <xinu.h>

syscall turn_light_on(did32 dev_id){
	bool8 turnon =1;
	return putc(&devtab[dev_id],turnon); /*dev_id==LED?*/
}


syscall turn_light_off(did32 dev_id){
	bool8 turnon=0;
	return putc(&devtab[dev_id],turnon);
}


syscall light_status(did32 dev_id){
	return (syscall) getc(&devtab[dev_id]);
}

