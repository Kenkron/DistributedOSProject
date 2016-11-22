/*led.c*/

#include <xinu.h>

syscall turn_light_on(did32 dev_id){
	bool8 turnon = 0; 
 return gpioin/an/outputc(&devtab[dev_id],turnon);
};


syscall turn_light_off(did32 dev_id){
	bool8 turnon = 0; 
 return gpioin/an/outputc(&devtab[dev_id],turnon);
};


syscall light_status(did32 dev_id){
	return gpioin/an/outgetc(&devtab[dev_id]);
};

