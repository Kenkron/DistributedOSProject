#include <xinu.h>

extern syscall if_pressed(did32); /* include/button.h and system/button.c */
extern syscall button_status(did32);

extern syscall light_status(did32); /* include/led.h and system/led.c */
extern syscall turn_light_off(did32);
extern syscall turn_light_on(did32);

local process temp(void);

MQTTSN_topicid topic;
MQTTSN_topicid btn_topic;
sid32 psem; /*print sem*/

unsigned char temperature_string[32];

void	broadcast_temp(MQTTSN_topicid topicid, unsigned char* text, int32 datalen){
		wait(psem);
		sprintf((char*)temperature_string,"%d",gettemp());
		mqttsn_publish (topic, temperature_string, 32, 0);
		kprintf("Temperature: %d c\n",gettemp());
		signal(psem);
}

/*NOTE: Process polls the temperature (gettemp)*/
process	temp(void)
{
	while(TRUE) {
		broadcast_temp(topic, temperature_string, 0);
		sleep(20);
		kprintf("Scheduled publish: ");
	}
}

process main(void) {
	kprintf("main...\n");
	topic.type = 0;
	topic.data.long_.name = "home/temp";
	topic.data.long_.len = 9;
	mqttsn_register(&topic);
	
	
	btn_topic.type = 0;
	btn_topic.data.long_.name = "home/btn";
	btn_topic.data.long_.len = 8;
	
	mqttsn_register(&btn_topic);
	mqttsn_subscribe(&btn_topic, &broadcast_temp);
	
	recvclr();
	kprintf("Starting LED and PUSHBTN demo...\n");
	
	psem = semcreate(1);
	pid32 temp_id = create(temp,4096,50,"temp",1,psem);
	
	resched_cntl(DEFER_START);
	resume(temp_id);
	resched_cntl(DEFER_STOP);
}
