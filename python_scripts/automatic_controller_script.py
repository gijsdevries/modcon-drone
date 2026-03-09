import serial
import time

try:
    with serial.Serial('/dev/ttyUSB0', 115200, timeout=1) as ser:

        while 1:
            ser.write(b"z")
            print("send command: Z")
            time.sleep(5)

            ser.write(b"x")
            print("send command: x")
            time.sleep(5)

            ser.write(b"c")
            print("send command: c")
            time.sleep(5)

except serial.SerialException as e:
    print(f"Fout: {e}")
