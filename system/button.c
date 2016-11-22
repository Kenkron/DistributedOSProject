/*button.c*/

#include <xinu.h>

syscall button_status(did32 dev_id){
	return gpioingetc(&devtab[dev_id]);
};


syscall if_pressed(did32 dev_id){
	return gpioingetc(&devtab[dev_id]);
};

/*Note I do not think this works as intended. I think when you write a '1' to DATAIN. The register gets rewritten at next time clock. This makes sense because if you click a button that does not stay down, it will only register once*/
syscall virtual_button_press(did32 dev_id,bool8 engage){
	return gpioinputc(&devtab[dev_id],engage);
};

