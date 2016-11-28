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

/*NOTE: Process polls the temperature (gettemp)*/
process	temp(void)
{
	while(TRUE) {
		sleep(1);
		wait(psem);
		kprintf("Temperature: %d c\n",gettemp());
		signal(psem);
	}
}

void handler2(MQTTSN_topicid topic, unsigned char * data, int32 datalen)
{
	kprintf("GOT SOME DATA!!!! WITH LEN: %d\n", datalen);
	return;
}

process main(void) {
	recvclr();
	kprintf("Starting LED and PUSHBTN demo...\n");
	
	psem = semcreate(1);
	pid32 led_id = create(led,4096,50,"led",0);
	pid32 btn_id = create(pushbtn,4096,50,"btn",0);
	pid32 status_id = create(status_print,4096,50,"status",0);
	pid32 temp_id = create(temp,4096,50,"temp",1,psem);
	
	resched_cntl(DEFER_START);
	resume(led_id);
	resume(btn_id);
	resume(status_id);
	resume(temp_id);
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
