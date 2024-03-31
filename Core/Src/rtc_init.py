import serial
import time

port_name = 'COM10'
baud_rate = 19200
# Open the serial port
ser = serial.Serial(port_name, baud_rate)

#get the current time and date then send them to the STM32 microcontroller
# Get the current time and date
current_time = time.localtime()
year = current_time.tm_year % 100
month = current_time.tm_mon
day = current_time.tm_mday
hours = current_time.tm_hour
minutes = current_time.tm_min
seconds = current_time.tm_sec


for item in [year, month, day, hours, minutes]:
    print("sending item: ", str(item).encode(), "type: ", type(str(item).encode()))
    ser.write(str(item).encode())
    data = ser.read()
    print("received data: ", data)

# Close the serial port
ser.close()
