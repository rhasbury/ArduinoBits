import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
import serial

serialPort = None

style.use('fivethirtyeight')


fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)

def animate(i):
    ser = serial.Serial(serialPort, 115200, bytesize=8, parity='N', stopbits=1, timeout=1, rtscts=False, dsrdtr=False)
    
    #graph_data = open('example.txt','r').read()
    #lines = graph_data.split('\n')
    xs = []
    ys = []
##    for line in lines:
##        if len(line) > 1:
##            x, y = line.split(',')
##            xs.append(float(x))
##            ys.append(float(y))
    ax1.clear()
    ax1.plot(xs, ys)


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
            ser = serial.Serial(port, 115200, bytesize=8, parity='N', stopbits=1, timeout=1, rtscts=False, dsrdtr=False)
            time.sleep(2) # for Arduino Nano this needs to be 2 seconds, for Arduino micro it can be almost zero. (nano resets on serial connection) 
            ser.flushInput()               
            ser.write(b'whatis;')       
            time.sleep(2)
            result = ser.readline()
            print("Testing port {}".format(port))            
            if(result.find(b'swr') >= 0):
                serialPort = port
                print("Found swr analyzer on serial port {}".format(serialPort))
                ser.close()
                break
            
            print("closind read result {}".format(result))
            ser.close()
        except:
            raise
        

    if(serialPort == None):
        print("No monitor found. Exiting")
        exit()

    


            
    ani = animation.FuncAnimation(fig, animate, interval=1000)
    plt.show()
