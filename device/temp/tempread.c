/* tempread.c - tempread*/
#include <xinu.h>

#define TSC_ADC_START 0x44e0D000

#define FIFO0DATA (TSC_ADC_START+0x100)


devcall tempread(
		struct dentry *devptr,
		char *buff,
		int32 count
){
	return *((uint32*)FIFO0DATA);
}
