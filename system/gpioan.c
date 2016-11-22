/* gpioan.c */

#include <xinu.h>

#define OE_ADDR 0x44E070134

/*-----gpioangetc-----*/
devcall gpioangetc(
		 struct dentry *devptr
)
{
	 struct iotentry *iotptr;	 /*ptr to iot device in minor table*/
	 uint32 *csreg;
	 uint32 gpio_pin;
	 iotptr = &iottab[devptr->dvminor];


	 iotptr = &iottab[ devptr->dvminor ];
	 csreg = (uint32*) devptr->dvcsr;
	 gpio_pin = (uint32) devptr->dvirq;



	 int32 output = *csreg & (1 << gpio_pin) ? '1' : '0';
	 return (devcall) output;

}

/*-----gpioanputc-----*/
devcall gpioanputc(
		 struct dentry *devptr
)
{
	 struct iotentry *iotptr;	 /*ptr to iot device in minor table*/
	 uint32 *csreg;
	 uint32 gpio_pin;
	 iotptr = &iottab[devptr->dvminor];


	 iotptr = &iottab[ devptr->dvminor ];
	 csreg = (uint32*) devptr->dvcsr;
	 gpio_pin = (uint32) devptr->dvirq;


	 *csreg = *csreg & (1 << gpio_pin);
	 return (devcall) OK;
}

/*-----gpioaninit-----*/
devcall gpioaninit(
		 struct dentry *devptr
)
{
	 struct iotentry *iotptr;	 /*ptr to iot device in minor table*/
	 uint32 *csreg;
	 uint32 gpio_pin;
	 uint32 *oe_addr;


	 iotptr = &iottab[ devptr->dvminor ];
	 csreg = (uint32*) devptr->dvcsr;
	 gpio_pin = (uint32) devptr->dvirq;


	 /*iotptr->gpio_in=CHANGE;*/ /*GPIO on BBB are initiall all inputs*/
	 iotptr->curr_val=0;
	 oe_addr = (uint32*) OE_ADDR;
	 /**oe_addr = *oe_addr ^ (CHANGE << gpio_pin);*/
	 return (devcall) OK;


}
