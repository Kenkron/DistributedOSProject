#include <xinu.h>

extern syscall if_pressed(did32); /* include/button.h and system/button.c */
extern syscall button_status(did32);

extern syscall light_status(did32); /* include/led.h and system/led.c */
extern syscall turn_light_off(did32);
extern syscall turn_light_on(did32);

local process pushbtn(void);

local process status_print(void);
local process led(void);
local process temp(void);
extern process TestTemp(void);

MQTTSN_topicid topic;

/*NOTE: Process checks to see if button is pressed. Checks two functions: if_pressed() and button_status()*/
process pushbtn(void) {
	/*PUSHBTN=push button, found in config/config.h */
	while(TRUE) {
		if (if_pressed((did32) PUSHBTN)) {
			kprintf("Sending button press\n");
			mqttsn_publish (topic, "1", 1, 0);
			sleep(2);
		}
	}
	return OK;
}


process main(void) {
	unsigned char data[2];
	topic.type = 0;
	topic.data.long_.name = "home/btn";
	topic.data.long_.len = 8;
	mqttsn_register(&topic);
	topic.type = 1;

	recvclr();

	pid32 btn_id = create(pushbtn,4096,50,"btn",0);

	resched_cntl(DEFER_START);
	resume(btn_id);

	resched_cntl(DEFER_STOP);
}

