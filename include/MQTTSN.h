
#define MQTTSN_SLOTS 10

void mqttsn_init(void);

uid32	mqttsn_subscribe (MQTTSN_topicid* topic, void (*handler) (MQTTSN_topicid topic, unsigned char * data, int32 datalen));
uid32	mqttsn_publish (MQTTSN_topicid topic, unsigned char * data, int32 datalen);



struct	mqttdata {
	MQTTSN_topicid topic;
	byte used;
	void (*handler) (MQTTSN_topicid, unsigned char * data, int32 datalen);
};

extern	struct	mqttdata mqttsntab[];
