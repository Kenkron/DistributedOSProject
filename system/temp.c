/* temp.c - gettemp */

#include <xinu.h>

int32 gettemp(){
	struct	dentry	*devptr;
	devptr = (struct dentry *) &devtab[TEMP];
	uint32 raw = (devptr->dvread)(devptr, NULL, 0);
	return (raw-1250)/20;
}
