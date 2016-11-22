/* gpioin.c */
#include <xinu.h>

#define OE_ADDR (void *) 0x44E07134

/*-----gpioingetc-----*/
devcall gpioingetc(
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

	 curr_val = (*csreg & (uint32) (1<<gpio_pin));
	 if (curr_val > 0) {
		 curr_val=1;
	 } else {
		 curr_val=0;
	 }
	 iotptr->curr_val= (double) curr_val;
	 return (devcall) curr_val;

}

/*-----gpioinputc-----*/
devcall gpioinputc(
		 struct dentry *devptr, bool8 engage
)
{
	 struct iotentry *iotptr;	 /*ptr to iot device in minor table*/
	 uint32 *csreg;
	 uint32 gpio_pin;
	 iotptr = &iottab[devptr->dvnum];


	 iotptr = &iottab[ devptr->dvnum ];
	 csreg = (uint32*) devptr->dvcsr;
	 gpio_pin = (uint32) devptr->dvirq;

	 if (engage) {/*button in down position*/
	 	*csreg = ( *csreg | (uint32) (1 << gpio_pin)); /*turns on with virtual_button_press*/
	 } else {
		 *csreg = ( *csreg ^ (uint32) (1 << gpio_pin));
	 }
	 return (devcall) OK;
}

/*-----gpioininit-----*/
devcall gpioininit(
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


	 iotptr->gpio_in=1;
	 oe_addr = OE_ADDR;
	 *oe_addr = *oe_addr | (1 << gpio_pin); /*makes it an input by turning to 1*/
	 return (devcall) OK;


}
