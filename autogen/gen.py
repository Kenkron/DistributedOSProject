import json
import sys

from src.XINUH_gen import XINUH_gen
from src.PROTOTYPESH_gen import PROTOTYPESH_gen
from src.CONFIGURATION_gen import CONFIGURATION_gen
from src.CH_gen import CH_gen
from src.IOT_gen import IOT_gen

#COMMAND LINE INPUT (eg. 'python gen.py iot.json bbb.json'
iot_json = sys.argv[1]
bbb_json = sys.argv[2]

#FILE CONSTANTS (copy these /xinu_raw/* files from xinu src)
XINU_FILE='./xinu_raw/xinu.h'#'../include/xinu.h'
PROTO_FILE='./xinu_raw/prototypes.h'#'../include/prototypes.h'
CONFIG_FILE='./xinu_raw/Configuration'#'../config/Configuration'
IOT_FILE='./xinu_gen/iot.h'#'../include/bbbgpio.h'


#JSON
iot = open(iot_json)
bbb = open(bbb_json)
data = json.load(iot)
bdata = json.load(bbb)
bbb.close()
iot.close()

def main():
    print '-------------------------------------------------------'
    print '***NOTE: THIS MAY PRODUCE DUPLICATE ENTRIES/FILES!!!***'
    print '***START: IOT Code generator v1.0'
    print '-------------------------------------------------------'

    
    #GEN CODE
    print '***INFO***'
    print '-------------------------------------------------------'
    gen_code(data)

    print '***END: Complete!'
    print '-------------------------------------------------------'


def gen_code(data):
        #XINUH_gen(XINU_FILE,data,bdata) #DO NOT UNCOMMENT. Only gen '#include iot.h' to xinu.h. No add led.h, etc.
        PROTOTYPESH_gen(PROTO_FILE,bdata)
        CONFIGURATION_gen(CONFIG_FILE,data,bdata)
        CH_gen(data)
        IOT_gen(IOT_FILE,XINU_FILE,data)

#MAIN START
main()




	






			

