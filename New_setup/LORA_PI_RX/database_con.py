import mysql.connector

connection = mysql.connector.connect(host="localhost",user="phpmyadmin",passwd="chaniyal",db="LoRa" )
cursor = connection.cursor()


def query(command):
    cursor.execute(command)
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
GPSRecord="""CREATE TABLE gps_record(
    Timestamp timestamp NOT NULL,
    Latitude float(10) NOT NULL ,
    Longitude float(10) NOT NULL 
);"""
Card_Scanned="""CREATE TABLE card_scanned(
    Transaction_ID int(20) AUTO_INCREMENT ,
    RFID varchar(20) NOT NULL ,
    Latitude float(10) NOT NULL ,
    Longitude float(10) NOT NULL ,
    node_id varchar(5) NOT NULL,
    Timestamp timestamp NOT NULL,
    PRIMARY KEY(Transaction_ID))"""
    
live_custmer="""Create Table live_custmer(
        RFID varchar(20) NOT NULL ,
        Latitude float(10) NOT NULL ,
        Longitude float(10) NOT NULL ,
        PRIMARY KEY(RFID)
        )"""
res=query(Card_Scanned)
print(res)

connection.close()
