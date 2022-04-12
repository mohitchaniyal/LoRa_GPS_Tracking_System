from time import sleep
from SX127x.LoRa import *
from SX127x.board_config import BOARD
import mysql.connector 
import time
import sys
from math import radians, cos, sin, asin, sqrt

BOARD.setup()
local_address="FF"
Nodes=["A1","A2","A3","A4"]
dtype=["RFID","GPS","GACK","RACK"]
destination=' '
charge=10
class LoRaRcvCont(LoRa):
    def __init__(self, verbose=False):
        super(LoRaRcvCont, self).__init__(verbose)
        self.set_mode(MODE.SLEEP)
        self.set_dio_mapping([0] * 6)
        
        self.connection = mysql.connector.connect(host="localhost",user="phpmyadmin",passwd="chaniyal",db="LoRa" )
        self.cursor = self.connection.cursor()
            
        

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
                datetimeWrite = time.strftime("%Y-%m-%d ")+time.strftime("%H:%M:%S")
                lat=float(data_list[3])/100
                lon=float(data_list[4])/100
                g_data=(datetimeWrite,lat,lon,destination)
                g_q="""insert into gps_record values(%s,%s,%s,%s)"""
                try :
                    res=self.query(g_q,g_data)
                    print(res)
                    self.send_data("{} {} {}".format(destination,local_address,dtype[2]))
                except:
                    pass
                
            elif data_list[2]=="RFID":
                rfid_no=data_list[3]+' '+  data_list[4]+' '+ data_list[5]+' '+  data_list[6]
                datetimeWrite = time.strftime("%Y-%m-%d ")+time.strftime("%H:%M:%S")
                lat=float(data_list[7])/100
                lon=float(data_list[8])/100
                q="""select User_name,RFID,Balance from user where RFID=%s"""
                data=self.query(q,(rfid_no,))
                print(data)
                CS_data=(rfid_no,lat,lon,destination,datetimeWrite)
                LC_data=(rfid_no,lat,lon)
                insert_to_CS="""insert into card_scanned(RFID,Latitude,Longitude,node_id,Timestamp) values(%s,%s,%s,%s,%s)"""
                qq=self.query(insert_to_CS,CS_data)
                print(qq)
                if data==[]:
                    self.send_data("{} {} {} {} {}".format(destination,local_address,dtype[0],'Unknown User',"N/A"))
                else :
                    t_q="""select RFID,Latitude,Longitude from live_custmer where RFID=%s"""
                    answer=self.query(t_q,(rfid_no,))
                    print(answer)
                    if answer==[]:
                        insert_to_LC="""insert into live_custmer(RFID,Latitude,Longitude) values(%s,%s,%s)"""
                        LC_res=self.query(insert_to_LC,LC_data)
                        self.send_data("{} {} {} {} {}".format(destination,local_address,dtype[0],data[0][0],data[0][2]))
                    else :
                        lon_lat_fetch="""select RFID,Latitude,Longitude from live_custmer where RFID=%s"""
                        lon_lat_data=self.query(lon_lat_fetch,(rfid_no,))
                        lat1=lon_lat_data[0][1]
                        lon1=lon_lat_data[0][2]
                        distance=self.haversine(lon1,lat1,lon,lat)
                        print(distance)
                        del_raw="""DELETE FROM live_custmer WHERE RFID=%s"""
                        self.query(del_raw,(rfid_no,))
                        deduct_q="""UPDATE user SET Balance=%s WHERE RFID= %s"""
                        deduct_data=(int(data[0][2])-charge,rfid_no)
                        self.query(deduct_q,deduct_data)
                        q="""select User_name,RFID,Balance from user where RFID=%s"""
                        data=self.query(q,(rfid_no,))
                        self.send_data("{} {} {} {} {}".format(destination,local_address,dtype[0],data[0][0],data[0][2]))
                        print('hello')
                        
                
                
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
    
    def query(self,command,data):
        
        self.cursor.execute(command,data)
        results=self.cursor.fetchall()
        self.connection.commit()
        return results
    def haversine(self,lon1, lat1, lon2, lat2):
    # convert decimal degrees to radians 
        lon1, lat1, lon2, lat2 = map(radians, [lon1, lat1, lon2, lat2])

    # haversine formula 
        dlon = lon2 - lon1 
        dlat = lat2 - lat1 
        a = sin(dlat/2)**2 + cos(lat1) * cos(lat2) * sin(dlon/2)**2
        c = 2 * asin(sqrt(a)) 
        r = 6371 # Radius of earth in kilometers. Use 3956 for miles
        return c * r
    
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
    lora.connection.close()
    
