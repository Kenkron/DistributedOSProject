import fileinput
import os.path

OUTPUT_FOLDER='./xinu_gen'
PROTO_TEMPLATE = """extern {call} {func}({para})\n"""
FUNC_TEMPLATE="""
{ret} {fname}({para}){{
	{code}
}};\n
"""
FILE_HEADER_TEMPLATE="""/*{name}{ext}*/\n\n"""
XINU_HEADER='#include <xinu.h>\n'
SAVE_MSG="""Saved {temp} to {file}"""

#COULD WRITE PROTO_TEMPLATE TO PROTOTYPES.H AS WELL!!!

class CH_gen:
    
    def __init__(self,data):
        self.genCH(data)

    def genCH(self,data):
        self.gen_proto_temp(data)
        print SAVE_MSG.format(temp='.c/.h',file='.c/.h')
    
    def gen_proto_temp(self,data):
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
                                funcs=sens['functions']
                                self.create_device_header(filename,funcs)
                                self.create_device_imple(filename,funcs,name)

    def create_device_header(self,filename,funcs):
        suffix='.h'
        path = os.path.join(OUTPUT_FOLDER,filename+suffix)
        if (not os.path.isfile(path)):
            header = file(path,'w')
            header.write(FILE_HEADER_TEMPLATE.format(name=filename,ext=suffix))
            for func in funcs:
                name = func.keys()[0]
                header.write(PROTO_TEMPLATE.format(call='syscall',func=name,para='did32'))

    def create_device_imple(self,filename,funcs,driver_id):
        suffix='.c'
        path = os.path.join(OUTPUT_FOLDER,filename+suffix)
        if (not os.path.isfile(path)):
            header = file(path,'w')
            header.write(FILE_HEADER_TEMPLATE.format(name=filename,ext=suffix))
            header.write(XINU_HEADER)
            for func in funcs:
                name = func.keys()[0]
                getput = func.values()[0]
                if getput=='get':
                    code="return gpioin/an/outgetc(&devtab[dev_id]);"
                elif getput=='put':
                    code="bool8 turnon = 0; \n return gpioin/an/outputc(&devtab[dev_id],turnon);"
                header.write(FUNC_TEMPLATE.format(ret='syscall',fname=name,para='did32 dev_id',code=code))

