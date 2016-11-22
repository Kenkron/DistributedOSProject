import fileinput
import os.path
import shutil

XINUH_TEMP = """#include <{head}.h>"""
ALL_TEMP="/*Final project code gen section*/\n"
SAVE_MSG="""Saved {temp} to {file}"""
OUTPUT_FOLDER='./xinu_gen'

class XINUH_gen:
    
    def __init__(self,filename,data,bdata):
        self.genXINUH(filename,data,bdata)

    def genXINUH(self,file,data,bdata):
        self.save_inline(file,ALL_TEMP) #project changes header
        self.gen_device_header(file,data)
        self.gen_gpio_header(file,bdata)

    def gen_device_header(self,file,data):
        if (data['Language']=='C'):
            for info in data:
                platform=data['Platform']
                if (info=='Devices' and platform == 'BBB'):
                    for ty in data[info]:
                        if (ty=='Type'):
                            for styles in data[info][ty]:
                                for sens in styles['Sensor']:
                                    filename=sens['Filename']
                                    head = self.pretty_system_header(filename)
                                    self.save_inline(file,head)

    def gen_gpio_header(self,file,bdata):
        platform=bdata['Platform']
        if (platform == 'BBB'):
            for gpios in bdata['gpio']:
                gp = gpios.keys()[0]
                head = self.pretty_system_header(gp)
                self.save_inline(file,head)

    def pretty_system_header(self,filename):
        return XINUH_TEMP.format(head=filename)

    def save_inline(self,file,head):
        inserted = False
        filename = os.path.basename(file)
        output_path=os.path.join(OUTPUT_FOLDER,filename)
        shutil.copy(file,output_path) #make copy to dest folder and append data

        with open(output_path, "a") as myfile:
            myfile.write(head)
            myfile.write('\n')
        myfile.close()
        print SAVE_MSG.format(temp=head,file=file)

    def save_error(self,file):
        print "We have already saved new header information to %s" %file


