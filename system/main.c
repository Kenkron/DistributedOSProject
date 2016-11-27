#include <xinu.h>

extern syscall if_pressed(did32); /* include/button.h and system/button.c */
extern syscall button_status(did32);

extern syscall light_status(did32); /* include/led.h and system/led.c */
extern syscall turn_light_off(did32);
extern syscall turn_light_on(did32);

local process pushbtn(void);

MQTTSN_topicid topic;

/*NOTE: Process checks to see if button is pressed. Checks two functions: if_pressed() and button_status()*/
process pushbtn(void) {
	/*PUSHBTN=push button, found in config/config.h */
	while(TRUE) {
		if (if_pressed((did32) PUSHBTN)) {
			mqttsn_publish (topic, "1", 1);
			sleep(2);
		}
	}
	return OK;
}


process main(void) {
	unsigned char data[2];
	topic.type = 0;
	topic.data.long_.name = "home/bedroom/btn";
	topic.data.long_.len = 16;

	recvclr();

	pid32 btn_id = create(pushbtn,4096,50,"btn",0);

	resched_cntl(DEFER_START);
	resume(btn_id);
	resched_cntl(DEFER_STOP);
}


// process	main(void)
// {
// 	MQTTSN_topicid topic;
// 	unsigned char data[2];
// 	recvclr();

// 	topic.type = 0;
// 	topic.data.long_.name = "hey/there";
// 	topic.data.long_.len = 9;
// 	data[0] = 0x1;
// 	data[1] = 0x5;
// 	kprintf("Trying to publish. ID %d \n", topic.data.id);
// 	mqttsn_publish (topic, data, 2);

// 	kprintf("Trying to subscribe.\n");
// 	mqttsn_subscribe (&topic, &handler2);

// 	kprintf("Trying to publish again.\n");
// 	mqttsn_publish (topic, data, 2);
// 	while (1);

// 	return OK;
    
//}
