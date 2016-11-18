#include <xinu.h>

#define TSC_ADC_START 0x44e0D000
/* 
 * All of the TSC_ADC_SS registers:
 * */

#define REVISION TSC_ADC_START+0x00
#define SYSCONFIG TSC_ADC_START+0x10
#define IRQSTATUS_RAW TSC_ADC_START+0x24
#define IRQSTATUS TSC_ADC_START+0x28
#define IRQENABLE_SET TSC_ADC_START+0x2c
#define IRQENABLE_CLR TSC_ADC_START+0x30
#define IRQWAKEUP TSC_ADC_START+0x34
#define DMAENABLE_SET TSC_ADC_START+0x38
#define DMAENABLE_CLR TSC_ADC_START+0x3C
#define TSC_ADC_CTRL TSC_ADC_START+0x40
#define ADCSTAT TSC_ADC_START+0x44
#define ADCRANGE TSC_ADC_START+0x48
#define ADC_CLKDIV TSC_ADC_START+0x4c
#define ADC_MISC TSC_ADC_START+0x50
#define STEPENABLE TSC_ADC_START+0x54
#define IDLECONFIG TSC_ADC_START+0x58
#define TS_CHARGE_STEPCONFIG TSC_ADC_START+0x5c
#define TS_CHARGE_DELAY TSC_ADC_START+0x60
#define STEPCONFIG1 TSC_ADC_START+0x64
#define STEPDELAY1 TSC_ADC_START+0x68
#define STEPCONFIG2 TSC_ADC_START+0x6c
#define STEPDELAY2 TSC_ADC_START+0x70
#define STEPCONFIG3 TSC_ADC_START+0x74
#define STEPDELAY3 TSC_ADC_START+0x78
#define STEPCONFIG4 TSC_ADC_START+0x7c
#define STEPDELAY4 TSC_ADC_START+0x80
#define STEPCONFIG5 TSC_ADC_START+0x84
#define STEPDELAY5 TSC_ADC_START+0x88
#define STEPCONFIG6 TSC_ADC_START+0x8c
#define STEPDELAY6 TSC_ADC_START+0x90
#define STEPCONFIG7 TSC_ADC_START+0x94
#define STEPDELAY7 TSC_ADC_START+0x98
#define STEPCONFIG8 TSC_ADC_START+0x9c
#define STEPDELAY8 TSC_ADC_START+0xA0
#define STEPCONFIG9 TSC_ADC_START+0xA4
#define STEPDELAY9 TSC_ADC_START+0xA8
#define STEPCONFIG10 TSC_ADC_START+0xAc
#define STEPDELAY10 TSC_ADC_START+0xB0
#define STEPCONFIG11 TSC_ADC_START+0xB4
#define STEPDELAY11 TSC_ADC_START+0xB8
#define STEPCONFIG12 TSC_ADC_START+0xBc
#define STEPDELAY12 TSC_ADC_START+0xC0
#define STEPCONFIG13 TSC_ADC_START+0xC4
#define STEPDELAY13 TSC_ADC_START+0xC8
#define STEPCONFIG14 TSC_ADC_START+0xCc
#define STEPDELAY14 TSC_ADC_START+0xD0
#define STEPCONFIG15 TSC_ADC_START+0xD4
#define STEPDELAY15 TSC_ADC_START+0xD8
#define STEPCONFIG16 TSC_ADC_START+0xDc
#define STEPDELAY16 TSC_ADC_START+0xE0
#define FIFO0COUNT TSC_ADC_START+0xE4
#define FIFO0THRESHOLD TSC_ADC_START+0xE8
#define DMA0REQ TSC_ADC_START+0xEC
#define FIFO1COUNT TSC_ADC_START+0xF0
#define FIFO1THRESHOLD TSC_ADC_START+0xF4
#define DMA1REQ TSC_ADC_START+0xF8
#define FIFO0DATA TSC_ADC_START+0x100
#define FIFO1DATA TSC_ADC_START+0x200

#define CTRL_BASE 0x44E10000

//check out DMA requests

void writeReg(uint32* reg, uint32 val){
	(*reg) = val;
}

void setReg(uint32* reg, uint32 val){
	(*reg) |= val;
}

void clearReg(uint32* reg, uint32 val){
	(*reg) &= val;
}

uint32 readReg(uint32* reg){
	return (reg[0]);
}

void printHex(uint32 hex){
	char output=0;
	uint32 i = 0;
	kputc('0');kputc('x');
	for (i = 0; i<8; i++){
		output = (int)((hex>>4*(7-i))%16)+48;
		if (output>=58) output+=7;
		kputc(output);
	}
}

syscall inittemp(){
	//9   SW steps are not pre-empted by hardware events
	//8   Default event mapping
	//7   Touchscreen disabled
	//6-5 Default AFTe Pen Ctrl Inputs
	//4   AFE is powered up
	//3   ADC uses internal bias
	//2   Setup configuration registers are protected (not writable)
	//1   Don't store StepID
	//0   Enable TSC_ADC_SS
	/*MOV r10,0x44E0D040 //target address: CTRL register
//described in TRM section 12.5.1.10
MOV r12,0x00000005 //set bit 0 = TSC_ADC_SS enable
//and bit 2 = Step conf. registers writeable
SBBO r12,r10,0,4 //store value of r12 in the CTRL register
MOV r12,0xFFFFFFFF //reset r12 to confirm that the followin load
//operation works
LBBO r12,r10,0,12 //Load value from CTRL register to r10
//The C host programm will echo the value of r10*/
	
	kprintf("inittemp:\n");

	kprintf("attempting to write to ");
	printHex(TSC_ADC_CTRL);
	kprintf(":\n");
	writeReg(TSC_ADC_CTRL, 0x05);
	kprintf("...done\n");
	//writeReg(REVISION, 0x0);
	kprintf("attempting to write to anything adc 2: ");
	//writeReg(SYSCONFIG, 0x0);

	
	kprintf("initializing step configuration registers: \n");
	

	
	writeReg(STEPCONFIG1, 0x4000000);
	writeReg(STEPCONFIG2, 0x00);
	writeReg(STEPCONFIG3, 0x00);
	writeReg(STEPCONFIG4, 0x00);
	writeReg(STEPCONFIG5, 0x00);
	writeReg(STEPCONFIG6, 0x00);
	writeReg(STEPCONFIG7, 0x00);
	writeReg(STEPCONFIG8, 0x00);
	writeReg(STEPCONFIG9, 0x00);
	writeReg(STEPCONFIG10, 0x00);
	writeReg(STEPCONFIG11, 0x00);
	writeReg(STEPCONFIG12, 0x00);
	writeReg(STEPCONFIG13, 0x00);
	writeReg(STEPCONFIG14, 0x00);
	writeReg(STEPCONFIG15, 0x00);
	writeReg(STEPCONFIG16, 0x00);
	kprintf("If you're reading this, step configuration is done. 8D\n");
	
	writeReg(TSC_ADC_CTRL, 0x05);
	//setReg(STEPENABLE, 0b10);
	//setReg(STEPCONFIG1, 0x00);
	
	return OK;
}

#define OE_ADDR 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_DATAIN 0x138
#define GPIO0_ADDR 0x44E07000

process TestTemp(sid32 printMutex){
	recvclr();

	/*NOTE: LED is turned on by writing (out) '1' to GPIO pin (voltage turns on!). Push button is opposide and you read (in) a 1 from register to denote pushed in. */
	/*NOTE: This is for an LED on board pin 19==GPIO0_22. Note the '0'. GPIO block 0. Note: board pin is 19, NOT 22. 22 is used for setting the bit locations for this GPIO location. Crazy*/
	/*NOTE: You need a LED in series with a 470 ohm resistor. Path: GRND (board pin 0 or 1)->short leg (LED)->LED->resistor->3.3V (from board pin 19*/

	//uint32 * light_on = (void *)0x44E0713C; /*GPIO0_ADDR+GPIO_DATAOUT.DEFAULT-0,0-off,1-on*/
	//uint32 * enable_dataout = (void *)0x44E07134; /*OE_ADDR.DEFAULT-1,1-IN,0-OUT*/
	//*enable_dataout=0; /*Turns ALL GPIO pins intially set to IN with 0xFFFFFFFF. OUT=0x00000000*/
	/*writeReg(0x44E07134, 0x00);
	
	while(FALSE) {
		kputc('.');
		sleep(2);
		//*light_on=0; /*turn off
		writeReg(0x44E0713C,0x00);
		sleep(2); /*sleep 2 seconds
		//*light_on=0xFFFFFFFF; /*turn ALL GPIO pins on. Obv needs optimization
		writeReg(0x44E0713C,0xFFFFFFFF);
	}*/
	kprintf("setting gpio to input (as a control)\n");
	writeReg(0x44E07134, 0xFFFFFFFF);
	kprintf("...done\n");
	inittemp();
	kprintf("setting gpio to input\n");
	writeReg(0x44E07134, 0xFFFFFFFF);
	kprintf("starting to get temp:\n");
	while(1){
		wait(printMutex);
		printHex(readReg(FIFO0COUNT));
		kputc(':');
		sleep(1);
		printHex(readReg(FIFO0DATA));
		kputc('\n');
		signal(printMutex);
		sleep(1);
	}
	return OK;
}

uint32 gettemp(){
	return ((uint32*)FIFO0DATA)[0];
}
