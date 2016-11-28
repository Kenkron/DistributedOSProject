/* temp.c - gettemp */

#include <xinu.h>

int32 gettemp(){
	struct	dentry	*devptr;
	devptr = (struct dentry *) &devtab[TEMP];
	return ((devptr->dvread)(devptr, NULL, 0)-1250)/20;
}
