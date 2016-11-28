/* initialize.c - nulluser, sysinit */

/* Handle system initialization and become the null process */

#include <xinu.h>
#include <string.h>

extern	void	start(void);	/* Start of Xinu code			*/
extern	void	*_end;		/* End of Xinu code			*/

/* Function prototypes */

extern	void main(void);	/* Main is the first process created	*/
static	void sysinit(); 	/* Internal system initialization	*/
extern	void meminit(void);	/* Initializes the free memory list	*/
local	process startup(void);	/* Process to finish startup tasks	*/

/* Declarations of major kernel variables */

struct	procent	proctab[NPROC];	/* Process table			*/
struct	sentry	semtab[NSEM];	/* Semaphore table			*/
struct	topic	topicstab[NTOPICS]; /*Topics table			*/
struct	memblk	memlist;	/* List of free memory blocks		*/

/* Declare inbox, Lab02*/
bpid32	INBOX_ID;		/* Pool ID of inbox*/
sid32 	printsem;		/* Used to control kprintf() statements	*/

/*lab03*/
struct pubentry* get_next_pending_publish(void);
process broker(void);

/* Declare pending publish buffer, Lab03*/
bpid32	PPUBLISH_ID;		/* Pool ID for pending publish events	*/


/* Active system status */

int	prcount;		/* Total number of live processes	*/
pid32	currpid;		/* ID of currently executing process	*/

/* Control sequence to reset the console colors and cusor positiion	*/

#define	CONSOLE_RESET	" \033[0m\033[2J\033[;H"

/*------------------------------------------------------------------------
 * nulluser - initialize the system and become the null process
 *
 * Note: execution begins here after the C run-time environment has been
 * established.  Interrupts are initially DISABLED, and must eventually
 * be enabled explicitly.  The code turns itself into the null process
 * after initialization.  Because it must always remain ready to execute,
 * the null process cannot execute code that might cause it to be
 * suspended, wait for a semaphore, put to sleep, or exit.  In
 * particular, the code must not perform I/O except for polled versions
 * such as kprintf.
 *------------------------------------------------------------------------
 */

void	nulluser()
{
	struct	memblk	*memptr;	/* Ptr to memory block		*/
	uint32	free_mem;		/* Total amount of free memory	*/

	/* Initialize the system */

	sysinit();

	/* Output Xinu memory layout */
	free_mem = 0;
	for (memptr = memlist.mnext; memptr != NULL;
						memptr = memptr->mnext) {
		free_mem += memptr->mlength;
	}
	kprintf("%10d bytes of free memory.  Free list:\n", free_mem);
	for (memptr=memlist.mnext; memptr!=NULL;memptr = memptr->mnext) {
	    kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)memptr, ((uint32)memptr) + memptr->mlength - 1);
	}

	kprintf("%10d bytes of Xinu code.\n",
		(uint32)&etext - (uint32)&text);
	kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)&text, (uint32)&etext - 1);
	kprintf("%10d bytes of data.\n",
		(uint32)&ebss - (uint32)&data);
	kprintf("           [0x%08X to 0x%08X]\n\n",
		(uint32)&data, (uint32)&ebss - 1);

	/* Enable interrupts */

	enable();

	/* Initialize the network stack and start processes */

	net_init();

	/* Create a process to finish startup and start main */

	resume(create((void *)startup, INITSTK, INITPRIO,
					"Startup process", 0, NULL));

	/* Become the Null process (i.e., guarantee that the CPU has	*/
	/*  something to run when no other process is ready to execute)	*/

	while (TRUE) {
		;		/* Do nothing */
	}

}

/*------------------------------------------------------------------------
 *
 * startup  -  Finish startup takss that cannot be run from the Null
 *		  process and then create and resumethe main process
 *
 *------------------------------------------------------------------------
 */
local process	startup(void)
{
	uint32	ipaddr;			/* Computer's IP address	*/
	char	str[128];		/* String used to format output	*/
	/* Use DHCP to obtain an IP address and format it */

	kprintf("Obtaining ip address...\n");
	ipaddr = getlocalip();
	if ((int32)ipaddr == SYSERR) {
		kprintf("Cannot obtain an IP address\n");
	} else {
		/* Print the IP in dotted decimal and hex */
		ipaddr = NetData.ipucast;
		sprintf(str, "%d.%d.%d.%d",
			(ipaddr>>24)&0xff, (ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,        ipaddr&0xff);

		kprintf("Obtained IP address  %s   (0x%08x)\n", str,
								ipaddr);
	}

	/* Initialize mqttsn */
	mqttsn_init();

	/* Create a process to execute function main() */

	resume(create((void *)main, INITSTK, INITPRIO,
					"Main process", 0, NULL));

	/* Start broker function for publish pending (Lab03)*/
	resume(create(broker, 4000, 50, "broker", 0 ));

	/* Startup process exits at this point */


	return OK;
}


/*------------------------------------------------------------------------
 *
 * sysinit  -  Initialize all Xinu data structures and devices
 *
 *------------------------------------------------------------------------
 */
static	void	sysinit()
{
	int32	i;
	int32 	j;
	struct	procent	*prptr;		/* Ptr to process table entry	*/
	struct	sentry	*semptr;	/* Ptr to semaphore table entry	*/

	struct 	topic	*topicptr;	/* Ptr to topic in topics table */
	struct pubentry *pubhead;		/* Head of pending publish buffer */

	struct 	topicentry *topptr;	/* Ptr to topic table entry	*/


	kprintf(CONSOLE_RESET);
	kprintf("\n%s\n\n", VERSION);

	/* Platform Specific Initialization */

	platinit();

	/* Initialize the interrupt vectors */

	initevec();

	/* Initialize free memory list */

	meminit();

	/* Initialize system variables */

	/* Count the Null process as the first process in the system */

	prcount = 1;

	/* Scheduling is not currently blocked */

	Defer.ndefers = 0;

	/* Initialize process table entries free */

	for (i = 0; i < NPROC; i++) {
		prptr = &proctab[i];
		prptr->prstate = PR_FREE;
		prptr->prname[0] = NULLCH;
		prptr->prstkbase = NULL;
		prptr->prprio = 0;
	}

	/* Initialize the Null process entry */

	prptr = &proctab[NULLPROC];
	prptr->prstate = PR_CURR;
	prptr->prprio = 0;
	strncpy(prptr->prname, "prnull", 7);
	prptr->prstkbase = getstk(NULLSTK);
	prptr->prstklen = NULLSTK;
	prptr->prstkptr = 0;
	currpid = NULLPROC;

	/* Lab03 - start broker process */




	/* Initialize semaphores */

	for (i = 0; i < NSEM; i++) {
		semptr = &semtab[i];
		semptr->sstate = S_FREE;
		semptr->scount = 0;
		semptr->squeue = newqueue();
	}

	/* Initialize buffer pools */

	bufinit();

	/* Create inbox feature, Lab02 */

	INBOX_ID=mkbufpool(MSGSIZE, TOTALMSGS);
	printsem=semcreate(1); /*control prints*/

	/* Lab03 - initialize topics table */

	/* Create pending publish buffer feature, Lab03 */

	PPUBLISH_ID=mkbufpool(PUBLISHSIZE, MAXPENDING); /*Pool ID for pending publish */
	topicptr = &topicstab[0];		/*Holds pending publish ptr*/
	topicptr->pubputsem=semcreate(MAXPENDING);
	topicptr->pubgetsem=semcreate(0);
	pubhead = (struct pubentry *)getbuf(PPUBLISH_ID);
	pubhead->data_seg=-1;
	pubhead->topic=-1;
	pubhead->has_msg=0;
	pubhead->nextpub=NULL;
	topicptr->pubhead=pubhead;		/* Set pending publish head address */

	for (i = 0; i < NTOPICS; i++) {
		topicptr = &topicstab[i];
		topicptr->num_subscribers = 0;
		for(j=0;j<NSUBSCRIBERS;j++) {		/*initialize all array values*/
			topicptr->callbacks[j]=NULL;
			topicptr->group_ids[j]=-1;
			topicptr->pids[j]=-1;
		}

	}




	/* Create a ready list for processes */

	readylist = newqueue();

	/* Initialize the real time clock */

	clkinit();

	for (i = 0; i < NDEVS; i++) {
		init(i);
	}
	return;
}

int32	stop(char *s)
{
	kprintf("%s\n", s);
	kprintf("looping... press reset\n");
	while(1)
		/* Empty */;
}

int32	delay(int n)
{
	DELAY(n);
	return OK;
}

struct pubentry* get_next_pending_publish(void)
{
	struct pubentry* next;			/*To be published				*/
	struct topic* topicentry;		/*Entry to publish buffer pool			*/

	topicentry = &topicstab[0];		/*Pending publish buffer pool is in entry 0	*/
	wait(topicentry->pubgetsem);		/* Wait until there are pending publish 	*/
	next = topicentry->pubhead;		/*pending publish is in the current head	*/
	return next;
}

/* broker */
process broker(void)
{
	topic16 topic;				/*Topic: components Group/ID			*/
	topic16 topic_id;			/*Topic ID					*/
	topic16 topic_group;			/*Topic Group					*/
	int32 	i;				/*iterator					*/
	int32 	j;
	struct topic* topicentry;		/*Topic holding pending publish list, entry 0	*/
	struct pubentry* oldpubhead;		/*iterator					*/
	struct pubentry* newpubhead;		/*iterator					*/
	struct pubentry* nexttopic;		/*Current topic to be published			*/
	struct	procent *prptr;			/*For unsubscribe-Ptr to process's table entry	*/
	char	data_seg;
	char*	data_ptr;
	uint32 	num_segs;			/*number of chracters for message		*/


	topicentry = &topicstab[0];




	while(1) {

		/*Lab03 - Unsubscribe from topics (not very efficient as we search all topics! in 256*8 time)*/
		for (j=0; j<NTOPICS;j++) {
			topicentry=&topicstab[j]; 				/*get topic			*/
			for (i=0;i<NSUBSCRIBERS;i++){
				prptr = &proctab[topicentry->pids[i]];
				if(topicentry->pids[i] != -1 && prptr->prstate == PR_FREE) {
						wait(printsem);
						kprintf("(TERMINATED) Process %d unsubscribers from topic/group value of %d/%d\n",topicentry->pids[i],j,topicentry->group_ids[i]);
						signal(printsem);
						--(topicentry->num_subscribers);
						topicentry->pids[i] = -1;	/*Return values to empty	*/
						topicentry->callbacks[i] = NULL;/*Remove from callbacks list	*/
						topicentry->group_ids[i] = -1;	/*Remove from groups list	*/
				}
			}
		}

		/*Get pending publish*/
		nexttopic=get_next_pending_publish();	/*Get pending publish from head		*/


		num_segs=nexttopic->num_segs;
		topic=nexttopic->topic;
		data_seg=nexttopic->data_seg;
		topic_id = topic & 0x00FF; 		/*Topic ID-get last 8 bits 		*/
		topic_group = (topic & 0xFF00)>>8; 	/*Topic Group-get first 8 bits 		*/

		/* Remove msg spot from buffer to make new buffer head */
		if(nexttopic->nextpub == NULL) {
			nexttopic->has_msg=0;
			nexttopic->data_seg=-1;
			nexttopic->topic=-1;
	#if DEBUG
			wait(printsem);
			kprintf("r: no next messages\n");
			signal(printsem);
	#endif
		}

		oldpubhead = nexttopic; /* If more msgs, move head to next msg in queue*/
		newpubhead = nexttopic->nextpub; /* New head in moved to next msg */
		topicentry->pubhead=newpubhead;
		freebuf((char *) oldpubhead); /* Release buffer space */
		signal(topicentry->pubputsem); /*signal there is now an empty spot to publish in*/

		char data[num_segs];
		data_ptr=data;
		data[0]=data_seg;

		for(i=1;i<nexttopic->num_segs;i++){	/*start at i=1 for we have */
			nexttopic=get_next_pending_publish();	/*Get pending publish from head		*/
			data[i]=nexttopic->data_seg;

			/* Remove msg spot from buffer to make new buffer head */
			if(nexttopic->nextpub == NULL) {
				nexttopic->has_msg=0;
				nexttopic->data_seg=-1;
				nexttopic->topic=-1;
		#if DEBUG
				wait(printsem);
				kprintf("r: no next messages\n");
				signal(printsem);
		#endif
			}

			oldpubhead = nexttopic; /* If more msgs, move head to next msg in queue*/
			newpubhead = nexttopic->nextpub; /* New head in moved to next msg */
			topicentry->pubhead=newpubhead;
			freebuf((char *) oldpubhead); /* Release buffer space */
			signal(topicentry->pubputsem); /*signal there is now an empty spot to publish in*/
		}


#if DEBUG
		wait(printsem);
		kprintf("BROKERING TOPIC: %d AND GROUP ID: %d.\n",topic_id,topic_group);
		signal(printsem);
#endif

		/*Error check for id and group done in publish()	*/

		topicentry=&topicstab[topic_id]; 			/*get topic*/
		if (topicentry->num_subscribers == 0) {
#if 1
			wait(printsem);
			kprintf("(ERROR) No subscribers to topic 0x%x!\n",topic);
			signal(printsem);
#endif
			continue;
		}
		if(topic_group==0) { 					/*send to all members of topic*/
			for(i=0;i<NSUBSCRIBERS;i++) {
				if(topicentry->callbacks[i] != NULL) {
					topicentry->callbacks[i](topic,data_ptr,num_segs);
				}
			}

		} else { 						/*send to group members with group_id*/
			for(i=0;i<NSUBSCRIBERS;i++) {
				if(topicentry->group_ids[i] == topic_group) {
					topicentry->callbacks[i](topic,data_ptr,num_segs);
				}
			}
		}

	}
	return OK;
}
