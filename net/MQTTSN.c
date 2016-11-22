/* dns.c - dnslookup, dns_bldq, dns_geta, dns_getrname */

#include <xinu.h>
#include <string.h>

#define BROKER_IP "192.168.1.129"
#define BROKER_PORT 1977



#define MQTTSN_PORT 9835
#define MAX_PACKET_SIZE 200
#define DEFAULT_TIMEOUT_SEC 30
#define BROKER_SLEEP_MSEC 100

#define DEFAULT_TIMEOUT_MSEC DEFAULT_TIMEOUT_SEC*1000

uid32 broker_slot;
uid32 broadcast_slot;
struct	mqttdata mqttsntab[MQTTSN_SLOTS];

unsigned char sendbuf[MAX_PACKET_SIZE];
unsigned char readbuf[MAX_PACKET_SIZE];

sid32 socket_sem;

int32 process_mqttsn(uint32);
int32 waitfor(int32 msg_type);
process local_broker();

void mqttsn_init(void) {
	MQTTSNPacket_connectData options = MQTTSNPacket_connectData_initializer;
	int32 len, res;
	int32 xinu_res;
	int32 lv;
	uint32 gwIP;
	uint16 gwPort;
	unsigned char dummy_byte;
	unsigned short dummy_short;
	unsigned char* dummy_str;

	kprintf("Initializing mqttsn subsystem.\n");

	/* Create semaphore */
	socket_sem = semcreate(1);

	/* Initialize sub slots */
	for (lv = 0; lv < MQTTSN_SLOTS; lv++) 
	{
		mqttsntab[lv].used = 0;
	}

	/* Open a port for broadcast */
	kprintf("Opening port.\n");
	broadcast_slot = udp_register(0, BROKER_PORT, MQTTSN_PORT);
	if (broadcast_slot == SYSERR) panic("Error creating MQTTSN broadcast slot");

	/* Send searchGW broadcast */
	kprintf("Sending searchgw broadcast.\n");
	len = MQTTSNSerialize_searchgw(sendbuf, MAX_PACKET_SIZE, 1); // 1 = gw on immediate network
	if (len <= 0)
		panic("Error serializing MQTTSN searchgw");
	res = udp_sendto(broadcast_slot, IP_BCAST, BROKER_PORT, (char *)sendbuf, len);
	if (res == SYSERR)
		panic("Error broadcasting searchgw.");

	/* Wait for infogw */
	kprintf("Waiting for infogw response.\n");
	res = udp_recvaddr(broadcast_slot, &gwIP, &gwPort, (char*) readbuf, MAX_PACKET_SIZE, DEFAULT_TIMEOUT_MSEC);
	if (res == TIMEOUT)
		panic("Didn't receive infogw within 30 seconds.");
	if (res == SYSERR)
		panic("Error reading packet from broadcast slot.");

	/* Deserialize infogw and then we're good */
	kprintf("Received a packet, making sure it's a gwinfo packet.\n");
	len = MQTTSNDeserialize_gwinfo(&dummy_byte, &dummy_short, &dummy_str, readbuf, MAX_PACKET_SIZE);
	if (len != 1)
		panic("Error deserializing gwinfo packet.");

	kprintf("Correctly received broadcast from IP %d.\n", gwIP);

	udp_release(broadcast_slot);
	broker_slot = udp_register(gwIP, BROKER_PORT, MQTTSN_PORT);
	if (broker_slot == SYSERR) panic("Error creating MQTTSN broker slot");

	kprintf("Connecting to broker.\n");
	/* Connect to broker */
	len = MQTTSNSerialize_connect(sendbuf, MAX_PACKET_SIZE, &options);
	if (len <= 0) 
		panic("Error serializing MQTTSN connect");
	
	kprintf("Sending connect packet.\n");
	res = udp_send(broker_slot, (char *) sendbuf, len);
	if (res == SYSERR) 
		panic("Error flushing MQTTSN connect");

	
	kprintf("Waiting for CONNACK.\n");
	// wait for connectack:
	xinu_res = waitfor(MQTTSN_CONNACK);
	if (xinu_res == SYSERR) 
		panic("Didn't receive CONNACK in within default timeout.");

	
	kprintf("Got CONNACK, verifying.\n");
	// readbuf now has connack, deserialize and make sure OK
	if (MQTTSNDeserialize_connack(&xinu_res, readbuf, MAX_PACKET_SIZE) != 1)
		panic("Received malformed CONNACK.");

	
	/* Create the local broker process */

	resume(create(local_broker, NETSTK, NETPRIO, "local_broker", 0, NULL));
}

// return SYSERR on fail
int32 waitfor(int32 msg_type) {
	uint32 start_clk, cur_clk;
	int32 res;

	start_clk = clktime;
	while (1) {
		// try and read a packet
		res = process_mqttsn(DEFAULT_TIMEOUT_MSEC);

		// check if we processed what we wanted
		if (res == msg_type) {
			return msg_type;
		}
		
		// otherwise check timeout
		cur_clk = clktime;
		if ((cur_clk-start_clk) >= DEFAULT_TIMEOUT_SEC) {
			break;
		}
	}
	return SYSERR;
}

unsigned char eqls(MQTTSN_topicid* top1, MQTTSN_topicid* top2) {
	// these two are the same because of union
	if (top1->data.id == top2->data.id) return 1;
	if (top1->data.short_name[0] == top2->data.short_name[0] &&
		top1->data.short_name[1] == top2->data.short_name[1]) return 1;
	return 0;
}

// read incoming packet, process it, and return value
// if packet is rec of some data, add it to mqttsntab
// returns -1 on timeout.
int32 process_mqttsn(uint32 timeout) {
	int32 res, actual_len;
	int32 len;
	int32 packet_type, lv;
	MQTTSN_topicid topicid;
	unsigned char* payload;
	int32 payloadLen;
	unsigned char dup, retained;
	int32 qos;
	unsigned short packetId;

	// read packet
	len = udp_recv(broker_slot, (char*) readbuf, MAX_PACKET_SIZE, timeout);
	if (len == SYSERR)
		return -1;
	if (len == TIMEOUT)
		return -1;

	res = MQTTSNPacket_decode(readbuf, MAX_PACKET_SIZE, &actual_len);
	if (actual_len != len)
		return -1;

	// process packet
	packet_type = readbuf[res];
	switch (packet_type) {
		// don't really need to process ACKs
		case MQTTSN_CONNACK:
		case MQTTSN_PUBACK:
		case MQTTSN_SUBACK:
			break;
		case MQTTSN_PUBLISH:
			if (MQTTSNDeserialize_publish(&dup, &qos, &retained, &packetId, &topicid, &payload, &payloadLen, readbuf, MAX_PACKET_SIZE) != 1)
			{
				// incorrect packet or something
				return -1;
			}

			for (lv = 0; lv < MQTTSN_SLOTS; lv++)
			{
				if (mqttsntab[lv].used != 0 && eqls(&mqttsntab[lv].topic, &topicid) == 1)
				{
					mqttsntab[lv].handler(topicid, payload, payloadLen);
				}
			}
			break;
		case MQTTSN_PINGRESP:
			// TODO: ping response...
			break;
		// missing register (unused), regack (unused), and pubrec (qos2)
	}
	return packet_type;
}

process local_broker() {
	kprintf("Initialized local broker.\n");
	while (1) {
		wait(socket_sem);
		process_mqttsn(1);
		sleepms(BROKER_SLEEP_MSEC);
		signal(socket_sem);
	}
}

uid32	mqttsn_subscribe (
	MQTTSN_topicid* topic, 
	void (*handler) (MQTTSN_topicid, unsigned char * data, int32 datalen)
	)
{
	int32 slot, len, res, qos;
	unsigned char rc;
	unsigned short packId;
	unsigned short topId;
	static unsigned short nextId = 0;
		
	wait(socket_sem);

	/* Find unused slot */
	for (slot = 0; slot < MQTTSN_SLOTS; slot++)
	{
		if (mqttsntab[slot].used == 0)
		{
			break;
		}
	}	
	
	/* No slots left for subscribers */
	if (slot == MQTTSN_SLOTS) 
	{
		signal(socket_sem);
		return SYSERR;
	}

	nextId++;
	if (nextId >= 65536) nextId = 1;

	/* Create packet */
	len = MQTTSNSerialize_subscribe(sendbuf, MAX_PACKET_SIZE, 0, 0, nextId, topic);
	if (len <= 0)
	{
		signal(socket_sem);
		return SYSERR;
	}	


	/* Send the packet */
	res = udp_send(broker_slot, (char *) sendbuf, len);
	if (res == SYSERR) {
		signal(socket_sem);
		return SYSERR;
	}

	res = waitfor(MQTTSN_SUBACK);
	if (res == SYSERR) 
	{
		signal(socket_sem);
		return SYSERR;
	}

	if (MQTTSNDeserialize_suback(&qos, &topId, &packId, &rc, readbuf, MAX_PACKET_SIZE) != 1)
	{
		signal(socket_sem);
		return SYSERR;
	}

	if (rc != MQTTSN_RC_ACCEPTED)
	{
		signal(socket_sem);
		return SYSERR;
	}		

	// if we arent using short names, set topic id to the accepted one
	if (topic->type != 2) 
	{
		topic->data.id = topId;
	}

	/* Fill mqttsntab */
	mqttsntab[slot].used = 1;
	mqttsntab[slot].handler = handler;
	mqttsntab[slot].topic = *topic;

	signal(socket_sem);
	return OK;
}


// topic can only use topicid or shortname.
// if longname, register a topicid
uid32	mqttsn_publish (
	MQTTSN_topicid topic, 
	unsigned char * data, 
	int32 datalen
	)
{
	int32 len, res;
	wait(socket_sem);
	
	/* Create packet */
	len = MQTTSNSerialize_publish(sendbuf, MAX_PACKET_SIZE, 0, 0, 0, 0, topic, data, datalen);
	if (len <= 0) {
		signal(socket_sem);
		return SYSERR;
	}

	/* Send the packet */
	res = udp_send(broker_slot, (char *) sendbuf, len);
	if (res == SYSERR) {
		signal(socket_sem);
		return SYSERR;
	}

	// TODO: IF WE USE QOS1, WAITFOR PUBACK

	signal(socket_sem);
	return SYSERR;
}


