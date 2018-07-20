import os
import logging
logging.basicConfig(format='%(asctime)s %(levelname)s %(message)s', filename='./jvcdevice.log', level=logging.DEBUG)
import datetime
import time
from time import mktime
import sys
import serial
import glob

serialPort = None
baud = 9600

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result 
 
if __name__ == "__main__":
    # Create logger and set options
    logging.getLogger().setLevel(logging.INFO)
    logging.info("Logging started")


    availableports = serial_ports()
    
    print(availableports)
    
    for port in availableports:
        try:
            ser = serial.Serial(port, baud, bytesize=8, parity='N', stopbits=1, timeout=1, rtscts=False, dsrdtr=False)
            time.sleep(4)
            ser.flushInput()               
            ser.write(b'isjvc\n')       
            time.sleep(1.5)
            result = ser.readline()
            
            print("Testing port {}".format(port))
            if(int(result) >= 0):
                serialPort = port
                print("Found JVCIR interface on serial port {}".format(serialPort))
                ser.close()
                break
            
            ser.close()
        except ValueError:
            ser.close()
            pass
        except:
            raise
    if(serialPort == None):
        logging.error("JVC IR interface device not found")
        exit()
    ser = serial.Serial(serialPort, baud, bytesize=8, parity='N', stopbits=1, timeout=1, rtscts=False, dsrdtr=False)
    #ser.setRTS(0) 
    time.sleep(3) # creating connection will reset arduino, need to wait for reset complete. 	
    ser.write(b'turn_off')       
    time.sleep(2.1)
    print(ser.readline())
    print(ser.readline())
    print(ser.readline())
    ser.close
  