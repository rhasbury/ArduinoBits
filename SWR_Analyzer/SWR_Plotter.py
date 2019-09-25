import matplotlib.pyplot as plt
import serial
import numpy as np
import json
import glob
import time
import traceback
import sys
#import getopt
import argparse
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

    parser = argparse.ArgumentParser(description='Pull and plot frequency response from esp32 swr analyser')
    parser.add_argument('--step_size', '-ss', type=int, default=4,  help='Sweep step size. 0 = 6.25khz, 1 = 10khz, 2 = 12.5khz, 3 = 25khz, 4 = 1Mhz ')
    parser.add_argument('--start_freq', '-sf', type=int, default=5000000, help='Sweep start frequency. 5000000 = 500Mhz. It is an odd scale, I know')
    parser.add_argument('--steps', '-s', type=int, default=1000, help='Number of sweep steps. Span is start_freq + (steps*step_size) ')
    parser.add_argument('--ifile', '-i', help='Input file that the sweep will be subtracted from (for comparing sweeps)')
    parser.add_argument('--ofile', '-o', help='Output file. Saves raw json returned from sweeper ')
    parser.add_argument('--plot', '-p', help='Display plot of this file. No sweep performed')
    args = parser.parse_args()

    inputfile = ''
    outputfile = ''
    # try:
    #   opts, args = getopt.getopt(sys.argv[1:],"hi:o:",["ifile=","ofile="])
    # except getopt.GetoptError:
    #   print('SWR_Plotter.py -i <inputfile> -o <outputfile>')
    #   sys.exit(2)
    #
    # for opt, arg in opts:
    #     if opt in ("-i", "--ifile"):
    #         inputfile = arg
    #     elif opt in ("-o", "--ofile"):
    #         outputfile = arg
    if(args.plot != None):
        try:
            with open(args.plot) as json_file:
                toplot = json.load(json_file)
                df = pd.DataFrame(toplot['sweep'])
                df.set_index('frequency', inplace=True)
                df.loc[:,['vref','mag']].plot()
                #df.plot()
                plt.show()


        except:
            print("inputting file barfed")

    else:
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
            print("No swr meter found. Exiting")
            exit()


        print(outputfile)
        print(inputfile)

        ser = serial.Serial(serialPort, 115200, bytesize=8, parity='N', stopbits=1, timeout=400, rtscts=False, dsrdtr=False)
        print("Fetching sweep")
        #ser.write(b'get_response,1000,6000000;')
        command = {
            "command" : "get_response",
            "start_freq" : args.start_freq,
            "steps" : args.steps, # number of sweep steps. Span is start_freq + (steps*step_size)
            "step_size" : args.step_size # 0 = 6.25khz, 1 = 10khz, 2 = 12.5khz, 3 = 25khz, 4 = 1Mhz
        }
        ser.write(json.dumps(command).encode("utf-8"))
        ser.write(b';')

        time.sleep(0.2)
        data = ser.readline()
        ser.close()

        try:
          j = json.loads(data)
          print(j['sID'])

          if(args.ofile != None):
              try:
                  with open(args.ofile, 'w') as outfile:
                      json.dump(j, outfile)
              except:
                print("Outputing file barfed")



          df = pd.DataFrame(j['sweep'])
          df.set_index('frequency', inplace=True)
          if(args.ifile != None):
            try:
                with open(args.ifile) as json_file:
                    correction = json.load(json_file)
                    print("doing correction")
                    correctionsDf = pd.DataFrame(correction['sweep'])
                    correctionsDf.set_index('frequency', inplace=True)
                    df = df.subtract(correctionsDf, fill_value=0)
            except:
                print("inputting file barfed")
          print(df)
          df.loc[:,['vref','mag']].plot()
          #df.plot()
          plt.show()


        except:
          print(data)
          print("broken data")
          print(traceback.format_exc())
