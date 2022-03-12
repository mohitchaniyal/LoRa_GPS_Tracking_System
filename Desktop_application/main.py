from time import strftime
import serial
import serial.tools.list_ports
import sys
import atexit
import MySQLdb
from time import strftime
import mysql.connector
from mysql.connector import Error
print("=== Auto scan for Lora port===")

def findLoraPort():
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
ser = serial.Serial(LoraPort, 9600,timeout=1)
db = MySQLdb.connect(host="localhost",user="root",passwd="root",db="Lora")

cur = db.cursor()
while True:
   rec_data=ser.readline().decode("utf-8")[:-2]
   if rec_data:
      print(rec_data)