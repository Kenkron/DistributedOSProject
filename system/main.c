#include <xinu.h>

extern syscall if_pressed(did32); /* include/button.h and system/button.c */
extern syscall button_status(did32);

extern syscall light_status(did32); /* include/led.h and system/led.c */
extern syscall turn_light_off(did32);
extern syscall turn_light_on(did32);

local process pushbtn(void);
local process status_print(void);
local process led(void);

sid32 psem; /*print sem*/

/*NOTE: Process checks to see if button is pressed. Checks two functions: if_pressed() and button_status()*/
process pushbtn(void) {
	
	/*PUSHBTN=push button, found in config/config.h */
	while(TRUE) {
#if 1
		if (if_pressed((did32) PUSHBTN)) {
			wait(psem);
			kprintf("PUSHBTN pushed!\n");
			uint32 btn_status = button_status((did32) PUSHBTN);
			kprintf("PUSHBTN status: %d\n",btn_status);
			signal(psem);
		}
#endif
	}
	return OK;
}

/*NOTE: Process checks to make sure button_status() will output '0' if btn NOT pressed*/
process status_print(void){
	while(TRUE) {
		sleep(3);
		uint32 btn_status = button_status((did32) PUSHBTN);
		wait(psem);
		kprintf("PUSHBTN status: %d\n",btn_status);
		signal(psem);
	}
	return OK;
}


/*NOTE: Process checks turn_light_off() and turn_light_on() and light_status()*/
process	led(void)
{
	/*LED=led, found in config/config.h */
	while(TRUE) {
		sleep(1);
		turn_light_off((did32) LED);
		wait(psem);
		kprintf("Light Status: %d\n",light_status((did32) LED));
		signal(psem);
		sleep(1); /*sleep 2 seconds*/
		turn_light_on((did32) LED);
		wait(psem);
		kprintf("Light Status: %d\n",light_status((did32) LED));
		signal(psem);
	}

	return OK;
}

process main(void) {


	recvclr();
	kprintf("Starting LED and PUSHBTN demo...\n");
	
	psem = semcreate(1);
	pid32 led_id = create(led,4096,50,"led",0);
	pid32 btn_id = create(pushbtn,4096,50,"btn",0);
	pid32 status_id = create(status_print,4096,50,"status",0);

	resched_cntl(DEFER_START);
	resume(led_id);
	resume(btn_id);
	resume(status_id);
	resched_cntl(DEFER_STOP);

	return OK;
}
