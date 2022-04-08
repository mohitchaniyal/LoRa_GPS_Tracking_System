from time import sleep
from SX127x.LoRa import *
from SX127x.board_config import BOARD

BOARD.setup()
local_address="FF"
Nodes=["A1","A2","A3","A4"]
dtype=["RFID","GPS","GACK","RACK"]
destination=' '
class LoRaRcvCont(LoRa):
    def __init__(self, verbose=False):
        super(LoRaRcvCont, self).__init__(verbose)
        self.set_mode(MODE.SLEEP)
        self.set_dio_mapping([0] * 6)

    def start(self):
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT)
        while True:
            sleep(.5)
            rssi_value = self.get_rssi_value()
            status = self.get_modem_status()
            sys.stdout.flush()
            

    def on_rx_done(self):
        self.clear_irq_flags(RxDone=1)
        payload = self.read_payload(nocheck=True)
        data=bytes(payload).decode("utf-8",'ignore')
        print(data)
        data_list=data.split()
        self.set_mode(MODE.SLEEP)
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT)
        if data_list[0]==local_address:
            
            destination=data_list[1]
            if data_list[2]=="GPS":
                self.send_data("{} {} {}".format(destination,local_address,dtype[2]))
            elif data_list[2]=="RFID":
                if data_list[3]=="54" and data_list[4]=="227" and data_list[5]=="157" and data_list[6]=="43":
                    self.send_data("{} {} {} {}".format(destination,local_address,dtype[0],"Sonesh"))
                elif data_list[3]=="98" and data_list[4]=="150" and data_list[5]=="40" and data_list[6]=="27":
                    self.send_data("{} {} {} {}".format(destination,local_address,dtype[0],"Karan"))
    def send_data(self,text):
        print(text)
        ascii_data=[]
        for char in text :
            ascii_data.append(ord(char))
        self.write_payload(ascii_data)
        self.set_mode(MODE.TX)
        sleep(.5)
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT)

lora = LoRaRcvCont(verbose=False)
lora.set_mode(MODE.STDBY)

#  Medium Range  Defaults after init are 434.0MHz, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on 13 dBm

lora.set_pa_config(pa_select=1)

try:
    lora.start()
except KeyboardInterrupt:
    sys.stdout.flush()
    print("")
    sys.stderr.write("KeyboardInterrupt\n")
finally:
    sys.stdout.flush()
    print("")
    lora.set_mode(MODE.SLEEP)
    BOARD.teardown()
