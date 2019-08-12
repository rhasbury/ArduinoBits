import matplotlib.pyplot as plt
import serial
import numpy as np
import json
import glob
import time
import traceback
import sys
import pandas as pd
from matplotlib.widgets import Button

serialPort = None


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

# start here
if __name__ == "__main__":
    availableports = serial_ports()
    

    print(availableports)

    for port in availableports:
        try:
            ser = serial.Serial(port, 115200, bytesize=8, parity='N', stopbits=1, timeout=1, rtscts=True, dsrdtr=True)
            time.sleep(0.1) # for Arduino Nano this needs to be 2 seconds, for Arduino micro it can be almost zero. (nano resets on serial connection) 
            ser.flushInput()               
            ser.write(b'whatis\n')       
            time.sleep(0.2)
            result = ser.readline()
            print("Testing port {}".format(port))            
            if(result.find(b'swrmeter4g') >= 0):
                serialPort = port
                print("Found swr analyzer on serial port {}".format(serialPort))
                ser.close()
                break
            
            print("Closing. Read result {}".format(result))
            ser.close()
        except:
            raise
        

    if(serialPort == None):
        print("No swrmeter found. Exiting")
        exit()
    


	
    ser = serial.Serial(serialPort, 115200, bytesize=8, parity='N', stopbits=1, timeout=400, rtscts=False, dsrdtr=False)
    print("Fetching sweep")
    ser.write(b'get_response,1000,5000000;')       
    
    time.sleep(0.2)
    data = ser.readline()
    ser.close()

    try:
      j = json.loads(data)            
      print(j['sID'])
      df = pd.DataFrame(j['sweep'])
      print(df)
      df.set_index('frequency', inplace=True)
      df.plot()
      plt.show()
    
    
    except:   
      print(data)
      print("broken data")    
      print(traceback.format_exc())
      
        
            
    
    
