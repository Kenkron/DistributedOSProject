/* iot.h - handle iot functions */

#ifndef	Ngpio
#define	Ngpio		NDEVS		/* iottab will inclue gpioin,gpioout,gpioan. Configuration will produce Ngpioin, Ngpioout and Ngpioan. These would be good if we wanted to created a gpiointab[],gpioouttab[] and gpioantab[] and use the dvminor numbers to reference the fields. To make it easier I am created only an iottab[] that will have a lot of empty fields and we WILL NOT use dvminor to reference sub values.	*/
#endif


struct iotentry {
	double	curr_val;
	bool8	gpio_in;
};
struct	iotentry	iottab[Ngpio];
