import fileinput
import os.path
import shutil

OUTPUT_FOLDER='./xinu_gen'
DEVICE_TEMPLATE="""{name} is {template} on top csr {loc} -irq {pin}"""
GPIO_TEMPLATE="""
{type}:
\ton top
\t\t-i {init}\t-o ionull\t-c ionull
\t\t-r ionull\t-g {get}\t-p {put}
\t\t-w ionull\t-s ionull\t-n ionull
\t\t-intr ionull
"""
SAVE_MSG="""Saved {temp} to {file}"""
GPIO_FILE="""{gp}.c"""
GPIOGETC="""{gp}getc"""
GPIOPUTC="""{gp}putc"""
GPIOINIT="""{gp}init"""

GPIO_FUNC_INIT="""
/*-----{gp}-----*/
devcall {gp}(
\t\t struct dentry *devptr
)
{{
\t struct iotentry *iotptr;\t /*ptr to iot device in minor table*/
\t uint32 *csreg;
\t uint32 gpio_pin;
\t uint32 *oe_addr;\n

\t iotptr = &iottab[ devptr->dvnum ]; /*change to gpio**tab[]??*/
\t csreg = (uint32*) devptr->dvcsr;
\t gpio_pin = (uint32) devptr->dvirq;\n

\t /*iotptr->gpio_in=IF IN gpiooutinit then =0,else 1;*/ /*GPIO on BBB are initiall all inputs*/
\t iotptr->curr_val=  *csreg & (1 << gpio_pin) ? '1' : '0';
\t oe_addr = (uint32*) OE_ADDR;
\t /**oe_addr = *oe_addr ^ (IF IN gpiooutinit then =0,else no include << gpio_pin);*/
\t return (devcall) OK;\n

}}
"""

GPIO_FUNC_GETC="""
/*-----{gp}-----*/
devcall {gp}(
\t\t struct dentry *devptr
)
{{
\t struct iotentry *iotptr;\t /*ptr to iot device in minor table*/
\t uint32 *csreg;
\t uint32 gpio_pin;
\t iotptr = &iottab[devptr->dvnum];/*change to gpio**tab[]??*/\n

\t iotptr = &iottab[ devptr->dvnum ]; /*change to gpio**tab[]??*/
\t csreg = (uint32*) devptr->dvcsr;
\t gpio_pin = (uint32) devptr->dvirq;\n


\t int32 output = *csreg & (1 << gpio_pin) ? '1' : '0';
\t return (devcall) output;\n
}}
"""

GPIO_FUNC_PUTC="""
/*-----{gp}-----*/
devcall {gp}(
\t\t struct dentry *devptr
)
{{
\t struct iotentry *iotptr;\t /*ptr to iot device in minor table*/
\t uint32 *csreg;
\t uint32 gpio_pin;

\t iotptr = &iottab[ devptr->dvnum ]; /*change to gpio**tab[]??*/
\t csreg = (uint32*) devptr->dvcsr;
\t gpio_pin = (uint32) devptr->dvirq;\n

\t *csreg = *csreg & (1 << gpio_pin);
\t return (devcall) OK;
}}
"""

class CONFIGURATION_gen:
    
    def __init__(self,filename,data,bdata):
        self.genCONFIGURATION(filename,data,bdata)

    def genCONFIGURATION(self,file,data,bdata):
        self.gen_proto_temp(file,data,bdata)
        self.create_gpio_templates(file,bdata)
        self.build_gpio_functions(bdata)
        print SAVE_MSG.format(temp='CONFIGS',file=OUTPUT_FOLDER)
    
    def gen_proto_temp(self,file,data,bdata):
        for info in data:
            platform=data['Platform']
            if (info=='Devices'):
                for ty in data[info]:
                    if (ty=='Type'):
                        for styles in data[info][ty]:
                            style=styles['Style']
                            for sens in styles['Sensor']:
                                filename=sens['Filename']
                                name=sens['Driver_ID']
                                b_pin=sens['Board_pin']
                                IO=sens['IO']
                                ty=sens['Type']
                                head=sens['Header']
                                funcs=sens['functions']
                                csr,irq = self.get_gpio_info(platform,b_pin,IO,ty,head,bdata)
                                self.build_device(name,IO,csr,irq,file)

    #build devices from driver template in CONFIG_FILE
    def build_device(self,name,IO,loc,pin,config_file):
        gpioFound=False
        break_counter=0 #look for second '##'
        
        filename = os.path.basename(config_file)
        output_path=os.path.join(OUTPUT_FOLDER,filename)
        if not os.path.exists(output_path):
            shutil.copy(config_file,output_path)
            print "Creating copy of file: ",config_file
        for line in fileinput.input(output_path,inplace=1):
            tokens=line.split()
            if(len(tokens)>0):
                if (tokens[0]==name): # print "GPIO template already defined in Configuration"
                    print line
                    gpioFound=True
                    fileinput.close()
                elif (tokens[0]=='%%' and gpioFound==False):
                    break_counter += 1
                    if (break_counter==2):
                        if (IO=='Out'):
                            template='gpioout'
                        elif (IO=='In'):
                            template='gpioin'
                        elif (IO=='An'):
                            template='gpioan'
                        else:
                            template='ERROR'
                        print DEVICE_TEMPLATE.format(name=name,
                                    template=template,
                                    loc=loc,
                                    pin=pin)
                    print line
                else:
				print line #continue printing rest of the file
        fileinput.close()

    def get_gpio_info(self,platform,board_pin,IO,ty,head,bdata):
        gpio_pin=0
        gpio_csr=0
        index=0
        if (IO=='Out'):
            gptype='gpioout'
        elif (IO=='In'):
            gptype='gpioin'
        elif (IO=='An'):
            gptype='gpioan'
        if (platform=='BBB'):
            for gpios in bdata['gpio']:
                if gpios.keys()[0]==gptype:
                    for pins in bdata['gpio'][index][gptype][ty]['Head_pin']:
                        if pins['Pin']==board_pin:
                            gpio_pin=pins['gpio']
                            hex_base=int(bdata['gpio'][index][gptype][ty]['BASE'],16)
                            hex_offset=int(bdata['gpio'][index][gptype][ty]['OFFSET_DATADIR'],16)
                            gpio_csr=hex(hex_base+hex_offset)
                            return gpio_csr,gpio_pin
                else:
                    index += 1
    
    def create_gpio_templates(self,file,bdata):
        getc=0
        putc=0
        ginit=0
        break_counter=0
        filename = os.path.basename(file)
        output_path=os.path.join(OUTPUT_FOLDER,filename)
        if not os.path.exists(output_path):
            shutil.copy(file,output_path)
            print "Creating copy of file: ",config_file
        for line in fileinput.input(output_path,inplace=1):
            tokens=line.split()
            if(len(tokens)>0):
                if (tokens[0]=='%%'):
                    break_counter += 1
                    if(break_counter==1):
                        platform=bdata['Platform']
                        if (platform == 'BBB'):
                            for gpios in bdata['gpio']:
                                gptype = gpios.keys()[0]
                                getc=GPIOGETC.format(gp=gptype)
                                putc=GPIOPUTC.format(gp=gptype)
                                ginit=GPIOINIT.format(gp=gptype)
                                print GPIO_TEMPLATE.format(type=gptype,init=ginit,get=getc,put=putc)
                    if(line !='\n'):
                        print line
                else:
                    if(line!='\n'):
                        print line #continue printing rest of the file
        fileinput.close()

    def build_gpio_functions(self,bdata):
        getc=0
        putc=0
        ginit=0
        gfile=0
        for gpios in bdata['gpio']:
            gptype = gpios.keys()[0]
            gfile=GPIO_FILE.format(gp=gptype)
            path=os.path.join(OUTPUT_FOLDER,gfile)
            if not os.path.isfile(path): #create .c w. function (eg gpioin.c with gpioingetc,etc
                f = file(path,'w')
                f.write("/* %s */\n\n" %gfile)
                f.write("#include <xinu.h>\n\n")
                f.write('#define OE_ADDR (void *) 0x44E070134\n')
                
                getc=GPIOGETC.format(gp=gptype)
                f.write(GPIO_FUNC_GETC.format(gp=getc))
                
                putc=GPIOPUTC.format(gp=gptype)
                f.write(GPIO_FUNC_PUTC.format(gp=putc))
                
                ginit=GPIOINIT.format(gp=gptype)
                f.write(GPIO_FUNC_INIT.format(gp=ginit))







