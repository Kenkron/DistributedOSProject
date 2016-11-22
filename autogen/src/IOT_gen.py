import fileinput
import os.path
import shutil

OUTPUT_FOLDER='./xinu_gen'
GPIO_STRUCT="""
\nstruct iotentry {
\tdouble\tcurr_val;
\tbool8\tgpio_in;
};
"""
XINU_HEADER='#include <iot.h>\n\n'
TAB_STRUCT="""struct	iotentry	iottab[Ngpio];\n"""
CONSTANTS="""
#ifndef	Ngpio
#define	Ngpio		NDEVS		/* iottab will inclue gpioin,gpioout,gpioan. Configuration will produce Ngpioin, Ngpioout and Ngpioan. These would be good if we wanted to created a gpiointab[],gpioouttab[] and gpioantab[] and use the dvminor numbers to reference the fields. To make it easier I am created only an iottab[] that will have a lot of empty fields and we WILL NOT use dvminor to reference sub values.	*/
#endif
"""
SAVE_MSG="""Saved {temp} to {file}"""

class IOT_gen:
    
    def __init__(self,filename,xinuf,data):
        self.genIOT(filename,xinuf,data)

    def genIOT(self,file,xinuf,data):
        self.gen_IOT_header(data,file)
        self.save_xinu_header(xinuf)
        print SAVE_MSG.format(temp='IOT HEADER',file=file)
    
    def gen_IOT_header(self,data,filename):
        for info in data:
            if (info=='Platform'):
                if (data[info]=='BBB'):
                    if (not os.path.isfile(filename)):
                        header = file(filename,'w')
                        header.write('/* iot.h - handle iot functions */\n')
                        header.write(CONSTANTS)
                        header.write(GPIO_STRUCT)
                        header.write(TAB_STRUCT)

    def save_xinu_header(self,file):
        inserted = False
        filename = os.path.basename(file)
        output_path=os.path.join(OUTPUT_FOLDER,filename)
        shutil.copy(file,output_path) #make copy to dest folder and append data
        for line in fileinput.input(output_path,inplace=1):
            tokens=line.split()
            if(len(tokens)>0):
                if (tokens[0]=='#include' and inserted==False):
                    print XINU_HEADER
                    inserted=True
                else:
                    print line #continue printing rest of the file
        fileinput.close()
        print SAVE_MSG.format(temp=XINU_HEADER,file=OUTPUT_FOLDER)






