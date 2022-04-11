import mysql.connector

connection = mysql.connector.connect(host="localhost",user="phpmyadmin",passwd="chaniyal",db="LoRa_DB" )
cursor = connection.cursor()


def query(command):
    cursor.execute(command,data)
    results=cursor.fetchall()
    connection.commit()
    return results
    
q="""show databases;"""
UserRecord="""CREATE TABLE user(
    Id int(11) NOT NULL,
    User_name varchar(20) NOT NULL ,
    RFID varchar(20) NOT NULL ,
    Balance float(10),
    PRIMARY KEY(Id),
    UNIQUE (RFID)
)"""
query(UserRecord)
while True:
    i=input('con')
    ide=1
    uname=input('uname')
    rfid=input('rfid')
    bal=int(input('bal'))
    data=(ide,uname,rfid,bal)
    insert_UR="""INSERT INTO user VALUES(%s,%s,%s,%s)"""
    res=query(insert_UR,data)
    ide=ide+1
    data=()
    print(res)
    if i=='n':
        break

connection.close()
