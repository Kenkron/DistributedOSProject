/* receive.c - receive */

#include <xinu.h>
umsg32 receiveMsg(void);
syscall receiveMsgs(umsg32* msgs, uint32 msg_count);

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message and return the message to the caller
 *------------------------------------------------------------------------
 */
umsg32	receive(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/

	mask = disable();
	prptr = &proctab[currpid];
	if (prptr->prhasmsg == FALSE) {
		prptr->prstate = PR_RECV;
		resched();		/* Block until message arrives	*/
	}
	msg = prptr->prmsg;		/* Retrieve message		*/
	prptr->prhasmsg = FALSE;	/* Reset message flag		*/
	restore(mask);
	return msg;
}
/*------------------------------------------------------------------------
 *  receiveMsg  -  Receive arbitrary message, Lab02
 *------------------------------------------------------------------------
 */
umsg32	receiveMsg(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/
	struct ientry *newinboxhead;	/* Recv first message in buffer */
	struct ientry *oldinboxhead;
	struct ientry *prinboxhead;

	
	mask = disable();
	prptr = &proctab[currpid];
#if 0
	msg=prptr->prinboxhead->msg;
	kprintf("default msg: %d\n",msg);
#endif

	wait(prptr->prinboxgetsem);	/* Wait until msg is in buffer */

	prinboxhead = prptr->prinboxhead;

	msg=prinboxhead->msg;	/* MESSAGE */
	wait(printsem);
	kprintf("RECEIVED MSG: %d, RID: %d\n",msg,currpid);
	signal(printsem);	

	/* Remove msg spot from buffer to make new buffer head */
	if(prinboxhead->nextmsg == NULL) { 
		prinboxhead->msg=DEFAULT_MSG; /* empty */
#if DEBUG
		kprintf("r: no next messages\n");
#endif
		prptr->prhasmsgs=FALSE;
		signal(prptr->prinboxputsem); /* open slot for new msg */
		restore(mask);
		return msg; } /* RETURN */ 		
	
	oldinboxhead = prinboxhead; /* If more msgs, move head to next msg in queue*/
	newinboxhead = prinboxhead->nextmsg; /* New head in moved to next msg */
	prptr->prinboxhead=newinboxhead;
	freebuf((char *) oldinboxhead); /* Release buffer space */
	signal(prptr->prinboxputsem); /*open slow for new msg */
	restore(mask);
	return msg;
}

/*------------------------------------------------------------------------
 *  receiveMsgs  -  Receive messages, Lab02
 *------------------------------------------------------------------------
 */
syscall receiveMsgs(umsg32* msgs, uint32 msg_count)
{
	intmask	mask;			/* Saved interrupt mask		*/

	umsg32	msg;			/* Message to return		*/


#if 0 /*old code, instructions confusing, may use*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	struct sentry	msgs_in_buffer;
	prptr = &proctab[currpid];
	msgs_in_buffer = &semtab[prptr->prinboxgetsem];
	while(msgs_in_buffer->scount < msg_count){ sleep(MSGSLEEP);/* busy waiting until all msg_count in buffer */}
#endif

	mask = disable();
	uint32 rcv_count=0;
	for (;rcv_count<msg_count;rcv_count++) { /*mv msgs from array to process's buffer'*/
		msg=*(msgs+rcv_count);
		sendMsg(currpid,msg); /*puts the msg from the array to the buffer*/
	}
	
	for (rcv_count=0;rcv_count<msg_count;rcv_count++) { /*rcv messages at once*/
		msg=receiveMsg();
	}

	restore(mask);
	return OK;

}
