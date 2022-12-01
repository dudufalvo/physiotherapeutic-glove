import serial

ser = serial.Serial("COM15", 9600)
ser.reset_input_buffer()
command = ['I', 1, ';']
for letter in command:
    ser.write(bytes(str(letter), 'utf-8'))

