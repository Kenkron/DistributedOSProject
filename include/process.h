/* process.h - isbadpid */

/* Maximum number of processes in the system */

#ifndef NPROC
#define	NPROC		8
#endif		

/* Inbox conf, Lab02 */
#define NINBOX		1
#define NMAXMSGS	10
#define MSGSIZE		sizeof(struct ientry)
#define	TOTALMSGS	(int32) 100
#define	DEFAULT_MSG	999
#define	MSGSLEEP	1
extern sid32	printsem;

/* Process state constants */

#define	PR_FREE		0	/* Process table entry is unused	*/
#define	PR_CURR		1	/* Process is currently running		*/
#define	PR_READY	2	/* Process is on ready queue		*/
#define	PR_RECV		3	/* Process waiting for message		*/
#define	PR_SLEEP	4	/* Process is sleeping			*/
#define	PR_SUSP		5	/* Process is suspended			*/
#define	PR_WAIT		6	/* Process is on semaphore queue	*/
#define	PR_RECTIM	7	/* Process is receiving with timeout	*/

/* Miscellaneous process definitions */

#define	PNMLEN		16	/* Length of process "name"		*/
#define	NULLPROC	0	/* ID of the null process		*/

/* Process initialization constants */

#define	INITSTK		65536	/* Initial process stack size		*/
#define	INITPRIO	20	/* Initial process priority		*/
#define	INITRET		userret	/* Address to which process returns	*/

/* Inline code to check process ID (assumes interrupts are disabled)	*/

#define	isbadpid(x)	( ((pid32)(x) < 0) || \
			  ((pid32)(x) >= NPROC) || \
			  (proctab[(x)].prstate == PR_FREE))

/* Number of device descriptors a process can have open */

#define NDESC		5	/* must be odd to make procent 4N bytes	*/

/* Definition of the process table (multiple of 32 bits) */

struct procent {		/* Entry in the process table		*/
	uint16	prstate;	/* Process state: PR_CURR, etc.		*/
	pri16	prprio;		/* Process priority			*/
	char	*prstkptr;	/* Saved stack pointer			*/
	char	*prstkbase;	/* Base of run time stack		*/
	uint32	prstklen;	/* Stack length in bytes		*/
	char	prname[PNMLEN];	/* Process name				*/
	sid32	prsem;		/* Semaphore on which process waits	*/
	pid32	prparent;	/* ID of the creating process		*/
	umsg32	prmsg;		/* Message sent to this process		*/
	bool8	prhasmsg;	/* Nonzero iff msg is valid		*/
	int16	prdesc[NDESC];	/* Device descriptors for process	*/
	
	bool8	prhasmsgs;	/* Other messages sent to this process	*/
	sid32	prinboxputsem;	/* Semaphore for empty spots in inbox	*/
	sid32	prinboxgetsem; 	/* Semaphore for num mesgs in inbox	*/
	struct ientry *prinboxhead;	/* Address to first struct in head 	*/
};

/* Marker for the top of a process stack (used to help detect overflow)	*/
#define	STACKMAGIC	0x0A0AAAA9

extern	struct	procent proctab[];
extern	int32	prcount;	/* Currently active processes		*/
extern	pid32	currpid;	/* Currently executing process		*/

/* lab03 */

struct topic {
	uint32 num_subscribers;	/*number of subscribers to topic	*/
	handler callbacks[NSUBSCRIBERS];	/* pointers to process callback function addresses */
	pid32 pids[NSUBSCRIBERS]; /*all pids subscribed			*/
	topic16 group_ids[NSUBSCRIBERS]; /*all group IDs in same loc as pid */
	struct pubentry *pubhead;	/* Address of next pub  	*/
	sid32	pubputsem;	/* Semaphore for empty spots in inbox	*/
	sid32	pubgetsem; 	/* Semaphore for num mesgs in inbox	*/
};

extern struct topic topicstab[];
