/* gpioout.c */

#include <xinu.h>

#define OE_ADDR (void *) 0x44E07134

/*-----gpiooutgetc-----*/
devcall gpiooutgetc(
		 struct dentry *devptr
)
{
	 struct iotentry *iotptr;	 /*ptr to iot device in minor table*/
	 uint32 *csreg;
	 uint32 gpio_pin;
	 uint32 curr_val;

	 iotptr = &iottab[devptr->dvnum];
	 csreg = (uint32*) devptr->dvcsr;
		
	 gpio_pin = (uint32) devptr->dvirq;
	 curr_val = (*csreg & (uint32) (1 << gpio_pin));
	 if (curr_val > 0) {
		 curr_val = 1;
	 } else {
		 curr_val = 0;
	 }
	 iotptr->curr_val=(double) curr_val;

	 return (devcall) curr_val;

}

/*-----gpiooutputc-----*/
devcall gpiooutputc(
		 struct dentry *devptr, bool8 turnon
)
{
	 struct iotentry *iotptr;	 /*ptr to iot device in minor table*/
	 uint32 *csreg;
	 uint32 gpio_pin;
	 
	 
	 iotptr = &iottab[devptr->dvnum];
	 csreg = (uint32*) devptr->dvcsr;
	 gpio_pin = (uint32) devptr->dvirq;

	 if (turnon) {
		*csreg = (*csreg | (uint32) (1 << gpio_pin));}
	 else {
		 *csreg = (*csreg ^ (uint32) (1 << gpio_pin));}

	 return (devcall) OK;
}

/*-----gpiooutinit-----*/
devcall gpiooutinit(
		 struct dentry *devptr
)
{
	 struct iotentry *iotptr;	 /*ptr to iot device in minor table*/
	 uint32 gpio_pin;
	 uint32 *oe_addr;
	 uint32 *csreg;

	 iotptr = &iottab[ devptr->dvnum ];
	 gpio_pin = (uint32) devptr->dvirq;
	 csreg = (uint32*) devptr->dvcsr;
	 
	 iotptr->gpio_in=0; /*GPIO on BBB are initiall all inputs*/
	 oe_addr = OE_ADDR; /*sets the pin to OUT. unnecessary for IN: */ 
	 *oe_addr = (*oe_addr ^ (uint32) (1 << gpio_pin));
	 return (devcall) OK;


}
