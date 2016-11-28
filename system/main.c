#include <xinu.h>

extern syscall if_pressed(did32); /* include/button.h and system/button.c */
extern syscall button_status(did32);

extern syscall light_status(did32); /* include/led.h and system/led.c */
extern syscall turn_light_off(did32);
extern syscall turn_light_on(did32);

local process temp(void);

MQTTSN_topicid topic;

sid32 psem; /*print sem*/

unsigned char temperature_string[32];

/*NOTE: Process polls the temperature (gettemp)*/
process	temp(void)
{
	while(TRUE) {
		sleep(5);
		wait(psem);
		sprintf((char*)temperature_string,"%d",gettemp());
		mqttsn_publish (topic, temperature_string, 32, 0);
		kprintf("Temperature: %d c\n",gettemp());
		signal(psem);
	}
}

process main(void) {
	kprintf("main...\n");
	topic.type = 1;
	topic.data.long_.name = "home/temp";
	topic.data.long_.len = 9;
	mqttsn_register(&topic);
	
	recvclr();
	kprintf("Starting LED and PUSHBTN demo...\n");
	
	psem = semcreate(1);
	pid32 temp_id = create(temp,4096,50,"temp",1,psem);
	
	resched_cntl(DEFER_START);
	resume(temp_id);
	resched_cntl(DEFER_STOP);
}
