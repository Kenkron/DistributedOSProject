#include <xinu.h>

extern syscall light_status(did32); /* include/led.h and system/led.c */
extern syscall turn_light_off(did32);
extern syscall turn_light_on(did32);

sid32 psem; /*print sem*/
MQTTSN_topicid topic;

/*NOTE: Process checks turn_light_off() and turn_light_on() and light_status()*/
void led(MQTTSN_topicid topic, unsigned char * data, int32 datalen)
{
	kprintf("GOT SOME DATA!!!! WITH LEN: %d\n", datalen);
	kprintf("Temp sensor published. Light LED");
	turn_light_on((did32) LED);
	kprintf("Light Status: %d\n",light_status((did32) LED));
	sleep(1);
	turn_light_off((did32) LED);
	kprintf("Light Status: %d\n",light_status((did32) LED));
	sleep(1);
	turn_light_on((did32) LED);
	kprintf("Light Status: %d\n",light_status((did32) LED));
	sleep(1);
	turn_light_off((did32) LED);
	kprintf("Light Status: %d\n",light_status((did32) LED));
	return;
}


process	main(void)
{ 
 	topic.type = 0;
 	topic.data.long_.name = "home/bedroom/temp";
 	topic.data.long_.len = 17;

 	recvclr();

	/*Turn light on for 5 seconds to show activity*/
	turn_light_on((did32) LED);
	sleep(5);
	turn_light_off((did32) LED);

	mqttsn_register(&topic);

	mqttsn_subscribe(&topic, &led);
	return OK;
}

