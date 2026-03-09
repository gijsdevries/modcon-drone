import serial
import time

#TODO test if this reads out csv data okay
try:
    with serial.Serial('/dev/ttyUSB0', 115200, timeout=1) as ser:

        while True:
            ser.write(b"z")
            time.sleep(5)

            ser.write(b"x")
            time.sleep(5)

            ser.write(b"c")
            time.sleep(5)

            if ser.in_waiting:
                data = ser.read(ser.in_waiting)
                print(data.decode(errors="ignore"))

except serial.SerialException as e:
    print(f"Fout: {e}")
