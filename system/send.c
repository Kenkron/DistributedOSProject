/* send.c - send */

#include <xinu.h>
extern syscall sendMsg(pid32 pid, umsg32 msg);
extern uint32 sendMsgs(pid32 pid, umsg32* msgs, uint32 msg_count);
extern uint32 sendnMsg(uint32 pid_count, pid32* pids, umsg32 msg); 
extern syscall subscribe(topic16 topic, void (*handler) (topic16 topic,void* data,uint32 size));
extern syscall unsubscribe(topic16 topic);
extern syscall publish(topic16 topic, void* data, uint32 size);

/*------------------------------------------------------------------------
 *  send  -  Pass a message to a process and start recipient if waiting
 *------------------------------------------------------------------------
 */
syscall	send(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg		/* Contents of message		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
	if (prptr->prhasmsg) {
		restore(mask);
		return SYSERR;
	}
	prptr->prmsg = msg;		/* Deliver message		*/
	prptr->prhasmsg = TRUE;		/* Indicate message is waiting	*/

	/* If recipient waiting or in timed-wait make it ready */

	if (prptr->prstate == PR_RECV) {
		ready(pid);
	} else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid);
	}
	restore(mask);		/* Restore interrupts */
	return OK;
}

/*------------------------------------------------------------------------
 *  sendMsg  -  Send an arbitrary message (eg mail) in Lab02
 *------------------------------------------------------------------------
 */
syscall	sendMsg(
	pid32 pid,	/* ID of recipient process	*/
	umsg32 msg	/* Contents of message		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	uint16	inbox_leftoverspace;	/* Status of inbox */
	struct	ientry *prmsghead;	/* Spot for message */
	struct 	ientry *newmsg;		/* Msg to put in buffer */
	struct 	ientry *currmsg;	/* Used to find tail */ 


	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
#if DEBUG
		kprintf("s: bad pid= process already dead, need sleep statement\n");
#endif
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
		inbox_leftoverspace = semtab[prptr->prinboxputsem].scount; /*check if space in inbox*/
		if (!inbox_leftoverspace) { 
#if DEBUG
			kprintf("inbox full!\n");
#endif
			restore(mask);
			return SYSERR; } 
		else { /*inbox space available */
			wait(prptr->prinboxputsem); /*only decrements as we already checked slots avaialable. sender never waits*/
			prmsghead = (struct ientry *) prptr-> prinboxhead; /*get inbox head*/
			if(prptr->prhasmsgs) { /* check if inboxhead has at least a msg */
				newmsg = (struct ientry *) getbuf(INBOX_ID); /*create new msg buffer if inboxhead node is full*/
				newmsg->msg = msg; /*construct msg*/
				newmsg->nextmsg = NULL; /*construct msg*/
				currmsg = prmsghead;
				while (currmsg->nextmsg != NULL) { currmsg = currmsg->nextmsg; } /*find place to put msg in tail*/
				currmsg->nextmsg=newmsg; /* Store new message in tail */
			} else { /* put msg in inboxhead */
#if DEBUG
				kprintf("put in inboxhead\n");	
#endif			
				prmsghead->msg=msg;
		}
	
		prptr->prhasmsgs=TRUE;
		if(currpid!=pid){ /*kludegy fix for receiveMsgs() move from array to buffer*/
			
		wait(printsem);
		kprintf("SENT MSG: %d, SID: %d, RID: %d\n",msg,currpid,pid); }
		signal(printsem);
		signal(prptr->prinboxgetsem); /* Make message avaiable to be recd */
		}
	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 *  sendMsgs  -  Send lots of messagess (eg mail) in Lab02
 *------------------------------------------------------------------------
 */
uint32 sendMsgs(pid32 pid, umsg32* msgs, uint32 msg_count) 
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	sentry *space_available; 
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;


	mask = disable();
	
	if (isbadpid(pid)) {
		
#if DEBUG
		kprintf("s: bad pid= process already dead, need sleep statement\n");
#endif
		restore(mask);
		return SYSERR;
	}
	
	prptr=&proctab[pid];
	space_available=&semtab[prptr->prinboxputsem];
	if(space_available->scount < msg_count) { restore(mask);return SYSERR;} /* rule:send either all or none */
	
	uint32 sent_count=0;
	for (;sent_count<msg_count;sent_count++) {
		msg=*(msgs+sent_count); /* get msg from array pointer */
		sendMsg(pid,msg); /*takes care of put/get semaphore counts*/ 
	}

	wait(printsem);
	kprintf("SENT MSG COUNT: %d, SID: %d\n",sent_count,currpid); 
	signal(printsem);
	restore(mask);
	return sent_count;
}

/*------------------------------------------------------------------------
 *  sendnMsg  -  Send lots of messagess (eg mail) in Lab02
 *------------------------------------------------------------------------
 */
uint32 sendnMsg(uint32 pid_count, pid32* pids, umsg32 msg) 
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	sentry *space_available; 
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	pid32 pid_avail=0;
	for(;pid_avail<pid_count;pid_avail++)
	{
		pid32 pid = *(pids+pid_avail);
		if (isbadpid(pid)) {

#if DEBUG
			kprintf("s: bad pid= process already dead, need sleep statement\n");
#endif
			restore(mask);
			return SYSERR;
		}

		prptr=&proctab[pid];
		space_available=&semtab[prptr->prinboxputsem]; /*check if room in each pid*/
		if(space_available->scount < 1) { restore(mask);return SYSERR;} /* rule:send either all or none */
	}

	pid32 send_pid=0;
	for(;send_pid<pid_count;send_pid++)
	{
		pid32 pid = *(pids+send_pid);
		sendMsg(pid,msg); /*calls gets/put semaphores*/
	}
	
	wait(printsem);
	kprintf("SENT PID COUNT: %d, SID: %d\n",pid_count,currpid); 
	signal(printsem);
	restore(mask);
	return pid_count; /*we basically just return the count if we get to this point as we have a send all/nothing rule*/

}

/*------------------------------------------------------------------------
 *  subscribe - Lab03, subscribe to a topic
 *------------------------------------------------------------------------
 */
syscall subscribe(topic16 topic, void (*handler) (topic16 topic,void* data,uint32 size)) 
{

	topic16 topic_id;		/*Topic ID - last 8 bits	*/
	topic16 topic_group;		/*Topid Group - first 8 bits	*/
	intmask mask;	
	int32 	i;			/*iterator			*/
	pid32 	pid;			/*pid of caller of subscribe()	*/
	struct 	topic* topicentry; 	/*Topic holder			*/ 
	int32 	alreadySubscribed=0;	/*Do not allow multiple subs to same topic tho diff groups*/
	
	mask=disable();
	pid=getpid(); 						/*Store current process for unsubscribing	*/
	topic_id = topic & 0x00FF; 				/*Get Topic ID - get last 8 bits 		*/
	topic_group = (topic & 0xFF00)>>8; 			/*Get Group ID - get first 8 bits 		*/	
		
	if(topic_id > NTOPICS || topic_group > NGROUPS) { 	/*topic_id and topic_group must be < NTOPICS (=255)*/
		wait(printsem);
		kprintf("ERROR: INVALID TOPIC- %d OR GROUP ID: %d (>255)!\n",topic_id,topic_group);
		signal(printsem);
		restore(mask);
		return SYSERR;
	}

	topicentry=&topicstab[topic_id]; 			/*get topic entry				*/
	if(topicentry->num_subscribers==NSUBSCRIBERS) {		/*check if max subscribers met			*/
		wait(printsem);
		kprintf("ERROR (MAX SUBS):Process %d unable to subscribe to topic16 value of 0x%X\n",pid,topic);
		signal(printsem);
		restore(mask);
		return SYSERR;
	}

	for (i=0;i<NSUBSCRIBERS;i++){ 				/*Disallow multiple subscriptions*/
		if(topicentry->pids[i] == pid) {			
			wait(printsem);
			kprintf("ERROR (ALREADY SUB):Process %d unable to subscribe to topic16 value of 0x%X\n",pid,topic);
			signal(printsem);
			alreadySubscribed=1;
			restore(mask);  
			return SYSERR;
		}
	}
	
	if(!alreadySubscribed) {					/*Add to subscription				*/
		for (i=0;i<NSUBSCRIBERS;i++){ 				/*find first empty spot in array. 		*/
			if(topicentry->pids[i] == -1) {			/* Allows multiple subscriptions to the same topic */
				++(topicentry->num_subscribers); 	/*(eg. PID 1 can subscribe to Gr1 and Gr2 of topic 1)*/
	#if DEBUG
				wait(printsem);
				kprintf("TOPIC: %d GROUP: %d SUBS: %d\n",topic_id,topic_group,topicentry->num_subscribers); 
				signal(printsem);
	#endif
				topicentry->pids[i] = pid;		/*Store pid			*/
				topicentry->callbacks[i] = handler;	/*Store callback handler	*/
				topicentry->group_ids[i] = topic_group; /*Store topic group for pid	*/
				break;  
			}
		}
	}
	
	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 *  unsubscribe -  Lab03, unsubscirbe from a topic
 *------------------------------------------------------------------------
 */
syscall unsubscribe(topic16 topic)
{
	topic16 topic_id;		/*Topic ID - last 8 bits	*/
	topic16 topic_group;		/*Topid Group - first 8 bits	*/
	intmask mask;	
	int32 i;			/*iterator			*/
	pid32 pid;			/*pid of caller of subscribe()	*/
	struct topic* topicentry; 	/*Topic holder			*/ 
	int32 didUnsubscribe = 0; 	/*Error Case: in case pid who did not unsubscribe tries*/

	
	mask=disable();
	pid=getpid(); 				/*current process, used later for unsubscribing*/
	topic_id = topic & 0x00FF; 		/*get last 8 bits */
	topic_group = (topic & 0xFF00) >> 8; 	/*get first 8 bits */	
		
	if(topic_id > NTOPICS || topic_group > NGROUPS) { 	/*topic_id and topic_group must be < NTOPICS (=255)*/
		wait(printsem);
		kprintf("ERROR: INVALID TOPIC- %d OR GROUP ID: %d (>255)!\n",topic_id,topic_group);
		signal(printsem);
		restore(mask);
		return SYSERR;
	}

	topicentry=&topicstab[topic_id]; 				/*get topic					*/

	for (i=0;i<NSUBSCRIBERS;i++){ 					/*find matching pid AND group id for topic	*/
		if(topicentry->pids[i] == pid) {
			if(topicentry->group_ids[i]==topic_group) { 	/*NOTE: PID COULD BE IN MULTIPLE GROUPS!	*/
				wait(printsem);
				kprintf("Process %d unsubscribers from topic16 value of 0x%X\n",pid,topic);
				signal(printsem);
				--(topicentry->num_subscribers);
				topicentry->pids[i] = -1;		/*Return values to empty			*/
				topicentry->callbacks[i] = NULL;	/*Remove from callbacks list			*/
				topicentry->group_ids[i] = -1;		/*Remove from groups list			*/
				didUnsubscribe=1;
				break;  
			}
		}
	}
	
	if(!didUnsubscribe) {
		wait(printsem);
		kprintf("ERROR: Process %d was NEVER subscribed to topic16 value of 0x%X\n",pid,topic);
		signal(printsem);				/*If process attempts to unsubsc but never subs*/
		restore(mask);
		return SYSERR;
	}
	
	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 *  publish - publish data to a topic (published data stored in buffer pool, distributed by broker process)
 *------------------------------------------------------------------------
 */
syscall publish(topic16 topic, void* data_in, uint32 size)
{

	topic16 topic_id;		/*Topic ID - last 8 bits	*/
	topic16 topic_group;		/*Topic Group - first 8 bits	*/
	intmask mask;	
	struct topic* topicentry;	/*Topic entry			*/
	struct pubentry* pubhead;	/*Head of pending publish queue	*/
	struct pubentry* newpub;	/*Topic to be published		*/
	struct pubentry* currpub;	/*iterator			*/
	uint32	pub_leftoverspace;	/*Num spots possible publish to	*/
	uint32 	i;			/*iterator			*/
	char* data;
	

	mask=disable();
	data=(char *)data_in;
	topic_id = topic & 0x00FF; 		/*Get Topic ID-get last 8 bits		*/
	topic_group = (topic & 0xFF00)>>8; 	/*Get Group ID - get first 8 bits 	*/	
		
	if(topic_id > NTOPICS || topic_group > NGROUPS) { /*topic_id and topic_group must be < NTOPICS (=255)*/
		wait(printsem);
		kprintf("ERROR: INVALID TOPIC- %d OR GROUP ID: %d (>255)!\n",topic_id,topic_group);
		signal(printsem);
		restore(mask);
		return SYSERR;
	}

	/*REQUIRE: allow buffer to expand until memory overflow. I am using pools that have a max number of buffers of BP_MAXN, thus there is a limit unless I dynamically create new buffer pools...*/
	topicentry = &topicstab[0]; 					/*!Only entry 0 contains pending publish data!	*/
	for (i=0;i<size;i++) { 						/*add chars messages				*/
		pub_leftoverspace = semtab[topicentry->pubputsem].scount; 	/*check space avaialabe 	*/
		if (!pub_leftoverspace) { 
	#if DEBUG
			wait(printsem);
			kprintf("(SOFT ERROR) PENDING PUBLISH BUFFER FULL. CREATING NEW POOL?!\n");
			signal(printsem);
	#endif
	#if 0
			/*Code attempt to dynamically create new pools. Problem: using pubputsem counts and edge case where we use only 1 buffer in pool 2, pop the head, then need to figure out where to put next pending publish. Unfinished!*/
			bpdid32 temp_pool =mkbufpool(PUBLISHSIZE, MAXPENDING); /*Pool ID for pending publish */
			pubhead = (struct pubentry *)getbuf(temp_pool);
	#endif
				 
			restore(mask);
			return SYSERR; 
		} 
		else { 							/*Publish space available			*/
			wait(topicentry->pubputsem);			/*Sender never waits. Only decremeter		*/
			pubhead = (struct pubentry *) topicentry-> pubhead; /*Get pending publish head			*/
			if(pubhead->has_msg != 0) { 			/*Put publish in tail 				*/
				newpub = (struct pubentry *) getbuf(PPUBLISH_ID);
				newpub->data_seg = *(data+i);		/*store char i as message		*/
				wait(printsem);

				signal(printsem);
				newpub->num_segs = size; 
				newpub->topic = topic; 
				newpub->nextpub=NULL;
				newpub->has_msg=1;
				currpub = pubhead;
				while (currpub->nextpub != NULL) { currpub = currpub->nextpub; } /*find place tail*/
				currpub->nextpub=newpub; 					 /* Store  in tail 	*/
				signal(topicentry->pubgetsem);

			} else { 					/*Put publish in head 			*/
	#if DEBUG
				wait(printsem);
				kprintf("put in pending pubhead\n");	
				signal(printsem);
	#endif			
				pubhead->data_seg = *(data+i); 
				wait(printsem);

				signal(printsem);
				pubhead->num_segs = size; 
				pubhead->topic = topic; 
				pubhead->nextpub=NULL;
				pubhead->has_msg=1;
				signal(topicentry->pubgetsem);

			}
		}
	}
	restore(mask);
	return OK;
}








