/* bufpool.h */

#ifndef	NBPOOLS
#define	NBPOOLS	20		/* Maximum number of buffer pools	*/
#endif

#ifndef	BP_MAXB
#define	BP_MAXB	8192		/* Maximum buffer size in bytes		*/
#endif

#define	BP_MINB	8		/* Minimum buffer size in bytes		*/
#ifndef	BP_MAXN
#define	BP_MAXN	2048		/* Maximum number of buffers in a pool	*/
#endif

struct ientry;
struct	bpentry	{		/* Description of a single buffer pool	*/
	struct	bpentry *bpnext;/* pointer to next free buffer		*/
	sid32	bpsem;		/* semaphore that counts buffers	*/
				/*    currently available in the pool	*/
	uint32	bpsize;		/* size of buffers in this pool		*/
	};

struct 	ientry	{		/* (I)nbox entry in processes inbox	*/
	umsg32	msg;		/* Message				*/
	struct	ientry *nextmsg;/* Next message in mailbox   */		
};

struct 	pubentry {		/* Pending publish entry in processes inbox	*/
	uint32	has_msg;		/* Boolean				*/
	topic16 topic;		/* Topic (group/id)				*/
	char data_seg;		/* Data segment (message)			*/
	uint32 num_segs;	/* num of segments				*/
	struct	pubentry *nextpub;/* Next message in mailbox   			*/		
};

extern	struct	bpentry buftab[];/* Buffer pool table			*/
extern	bpid32	nbpools;	/* current number of allocated pools	*/
extern bpid32 	INBOX_ID;	/* Inbox pool ID			*/
extern bpid32	PPUBLISH_ID;	/* Pending publish pool			*/
