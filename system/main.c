/*  main.c  - main */

#include <xinu.h>

void handler(MQTTSN_topicid topic, unsigned char * data, int32 datalen)
{
	kprintf("GOT SOME DATA!!!! WITH LEN: %d\n", datalen);
	return;
}

process	main(void)
{
	MQTTSN_topicid topic;
	unsigned char data[2];
	recvclr();

	topic.type = 0;
	topic.data.long_.name = "hey/there";
	topic.data.long_.len = 9;
	data[0] = 0x1;
	data[1] = 0x5;
	kprintf("Trying to publish. ID %d \n", topic.data.id);
	mqttsn_publish (topic, data, 2);

	kprintf("Trying to subscribe.\n");
	mqttsn_subscribe (&topic, &handler);

	kprintf("Trying to publish again.\n");
	mqttsn_publish (topic, data, 2);
	while (1);
	return OK;
    
}
