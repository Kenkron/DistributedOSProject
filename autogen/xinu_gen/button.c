/*button.c*/

#include <xinu.h>

syscall button_status(did32 dev_id){
	return gpioin/an/outgetc(&devtab[dev_id]);
};


syscall if_pressed(did32 dev_id){
	return gpioin/an/outgetc(&devtab[dev_id]);
};

