/* tempread.c - tempread */
#include <xinu.h>

#define TSC_ADC_START 0x44e0D000

#define FIFO1COUNT TSC_ADC_START+0xF0
#define FIFO1THRESHOLD TSC_ADC_START+0xF4
#define DMA1REQ TSC_ADC_START+0xF8
#define FIFO0DATA TSC_ADC_START+0x100
#define FIFO1DATA TSC_ADC_START+0x200


uint32 tempread(){
	return ((uint32*)FIFO0DATA)[0];
}
