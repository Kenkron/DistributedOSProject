/* iot.h - handle iot functions */

#ifndef	Ngpio
#define	Ngpio		NDEVS		/* We are only using iottab for gpioin/gpioout,etc	*/
#endif


struct iotentry {
	double	curr_val;
	bool8	gpio_in;
};
struct	iotentry	iottab[Ngpio];
