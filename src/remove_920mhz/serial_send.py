import serial
import time
import struct


ser = serial.Serial("/dev/usbserial", 115200, timeout = 0.1)
time.sleep(2)
i=0
temp=[0]*56
while True:
    i+=1
    if i==256:
        i=0
    
    for u in range(len(temp)):
        temp[u]=i
    temp[0]=0xcc
    temp[1]=0x66
    temp[2]=0x04
    print(temp)

    ser.write(bytes(temp))
    time.sleep(0.15)     
# ser.close()