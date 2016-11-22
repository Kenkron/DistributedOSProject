import fileinput
import os.path
import shutil #copying files


OUTPUT_FOLDER='./xinu_gen'
PROTO_TEMP = """extern devcall {file}(struct dentry *{extra});\n"""
GPIOGETC="""{gp}getc"""
GPIOPUTC="""{gp}putc"""
GPIOINIT="""{gp}init"""
ALL_TEMP="/*Final project code gen section*/\n"
SAVE_MSG="""Saved {temp} to {file}"""

class PROTOTYPESH_gen:
    
    def __init__(self,filename,data):
        self.genPROTOTYPESH(filename,data)

    def genPROTOTYPESH(self,file,data):
        self.gen_proto_temp(file,data)

    def gen_proto_temp(self,path,bdata):
        getc=0
        putc=0
        ginit=0
        gfile=0
        filename = os.path.basename(path)
        output_path=os.path.join(OUTPUT_FOLDER,filename)
        shutil.copy(path,output_path) #make copy to dest folder and append data
        with open(output_path,"a") as f:
            for gpios in bdata['gpio']:
                gptype = gpios.keys()[0]
                
                getc=GPIOGETC.format(gp=gptype)
                f.write(PROTO_TEMP.format(file=getc,extra=''))
                
                putc=GPIOPUTC.format(gp=gptype)
                f.write(PROTO_TEMP.format(file=putc,extra=',bool8 turnon'))
                
                ginit=GPIOINIT.format(gp=gptype)
                f.write(PROTO_TEMP.format(file=ginit,extra=''))
