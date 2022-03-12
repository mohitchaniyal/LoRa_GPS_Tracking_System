import serial
import serial.tools.list_ports
import sys
import atexit
import time
import pymysql

print("=== Auto scan for Lora port===")

def findLoraPort():http://localhost/phpmyadmin/tbl_import.php?db=Lora_DB&table=GPS_data&token=b6a646aa2ff9d1b9e7b5e8dc35978a0d
   portList = list(serial.tools.list_ports.comports())
   for port in portList:
       if("VID:PID=04D8:00DF" in port[0]\
         or "VID:PID=04D8:00DF" in port[1]\
         or "VID:PID=04D8:00DF" in port[2]):
            #please note: it is not sure [0]
            #returned port[] is no particular order
            #so, may be [1], [2]
         return port[0]
         
def doAtExit():
   if ser.isOpen():
      ser.close()
      print("Close serial")
      print("serial.isOpen() = " + str(ser.isOpen()))

#atexit.register(doAtExit)
LoraPort = findLoraPort()
if not LoraPort:
   print("No Lora found")
   sys.exit("No Lora found - Exit")

print("Lora found: " + LoraPort)
try:
	ser = serial.Serial(LoraPort, 9600,timeout=1)
	connection = pymysql.connect(host="localhost",user="root",passwd="root",db="Lora_DB" )
	cursor = connection.cursor()
	
except Error as err:
	print(err)
# some other statements  with the help of cursor


while True:
	cursor = connection.cursor()
	rec_data=ser.readline().decode("utf-8")[:-2]
	datetimeWrite = (time.strftime("%Y-%m-%d ")+time.strftime("%H:%M:%S"))
	if rec_data:
		print(rec_data)
		li=rec_data.split()
		print(li)
		if li[2]=="RFID":
			sql = ("""INSERT INTO Card_scanned VALUES(%s,%s,%s,%s,%s,%s,%s)""",(datetimeWrite,li[3]+li[4],li[5],li[6],li[0],li[1],2))
			cursor.execute(*sql)
			connection.commit()
			li.clear()
		elif li[1]=="GPS" :
			sql2=("""INSERT INTO  GPS_data VALUES(%s,%s,%s,%s)""",(datetimeWrite,li[0],li[2],li[3]))
			cursor.execute(*sql2)
			connection.commit()
			li.clear()