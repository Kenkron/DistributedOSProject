/* gpioout.c */

#include <xinu.h>

#define OE_ADDR (void *) 0x44E070134

/*-----gpiooutgetc-----*/
devcall gpiooutgetc(
		 struct dentry *devptr
)
{
	 struct iotentry *iotptr;	 /*ptr to iot device in minor table*/
	 uint32 *csreg;
	 uint32 gpio_pin;
	 iotptr = &iottab[devptr->dvnum];/*change to gpio**tab[]??*/


	 iotptr = &iottab[ devptr->dvnum ]; /*change to gpio**tab[]??*/
	 csreg = (uint32*) devptr->dvcsr;
	 gpio_pin = (uint32) devptr->dvirq;



	 int32 output = *csreg & (1 << gpio_pin) ? '1' : '0';
	 return (devcall) output;

}

/*-----gpiooutputc-----*/
devcall gpiooutputc(
		 struct dentry *devptr
)
{
	 struct iotentry *iotptr;	 /*ptr to iot device in minor table*/
	 uint32 *csreg;
	 uint32 gpio_pin;

	 iotptr = &iottab[ devptr->dvnum ]; /*change to gpio**tab[]??*/
	 csreg = (uint32*) devptr->dvcsr;
	 gpio_pin = (uint32) devptr->dvirq;


	 *csreg = *csreg & (1 << gpio_pin);
	 return (devcall) OK;
}

/*-----gpiooutinit-----*/
devcall gpiooutinit(
		 struct dentry *devptr
)
{
	 struct iotentry *iotptr;	 /*ptr to iot device in minor table*/
	 uint32 *csreg;
	 uint32 gpio_pin;
	 uint32 *oe_addr;


	 iotptr = &iottab[ devptr->dvnum ]; /*change to gpio**tab[]??*/
	 csreg = (uint32*) devptr->dvcsr;
	 gpio_pin = (uint32) devptr->dvirq;


	 /*iotptr->gpio_in=IF IN gpiooutinit then =0,else 1;*/ /*GPIO on BBB are initiall all inputs*/
	 iotptr->curr_val=  *csreg & (1 << gpio_pin) ? '1' : '0';
	 oe_addr = (uint32*) OE_ADDR;
	 /**oe_addr = *oe_addr ^ (IF IN gpiooutinit then =0,else no include << gpio_pin);*/
	 return (devcall) OK;


}
