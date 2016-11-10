/*  main.c  - main processSender processReceiver processAdditionalReceiver*/

#include <xinu.h>

#define OE_ADDR 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_DATAIN 0x138
#define GPIO0_ADDR 0x44E07000
#define GPIO1_ADDR 0x4804C000
#define GPIO2_ADDR 0x481AC000
#define GPIO3_ADDR 0x481AF000

pid32 FOO;
pid32 BAR;

/* This prevents the processes from cutting each other off when printing*/
sid32 printMutex;
#define mprintf(...) wait(printMutex);kprintf (__VA_ARGS__);signal(printMutex)

process foo(){
	mprintf("%d foo\n", getpid());
}

process bar(){
	mprintf("%d bar\n", getpid());
}

/* starts a sender and three receivers that dance around sending 
 * each other messages until they figure they've run through everything.
 * The sender usually sends things, but it technically gets sent
 * messages by receivers 2 and 3.  Otherwise, how would we know if
 * buffers for old message queues could be recycled.*/
process	main(void)
{
	printMutex=semcreate(1);
	mprintf("%d STARTING\n\n", getpid());
	
	recvclr();
	FOO = create(foo, 4096, 50, "foo", 0);
	BAR = create(bar, 4096, 50, "bar", 0);
	
	resched_cntl(DEFER_START);
	resume(FOO);
	resume(BAR);
	resched_cntl(DEFER_STOP);
	
	mprintf("\n%d DONE\n", getpid());
	return OK;
}
