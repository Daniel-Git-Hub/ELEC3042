import serial #allows python to interface with the serial ports

ser = serial.Serial('COM7') #for linux might will look like '/dev/ttyAMA0'

ser.baudrate = 115200 #these are our settings that we defined in serial_init() on the ATMEGA chip
ser.parity = serial.PARITY_NONE
ser.stopbits = serial.STOPBITS_TWO
ser.butesize = serial.EIGHTBITS

reading = 0 #used to keep track of how many messages of come in
while True:
  char = ser.read(1) #read one incoming message
  
  #if you are using python 2 
  #print(str(reading) + " : "  + str(char) + " : " + str(ord(char)) + " : " + str(hex(ord(char)))  + " : " + str(bin(ord(char))))
  
  #if you are using python 3
  print(str(reading) + " : "  + str(char, 'utf-8') + " : " + str(ord(char)) + " : " + str(hex(ord(char)))  + " : " + str(bin(ord(char))))
  reading += 1