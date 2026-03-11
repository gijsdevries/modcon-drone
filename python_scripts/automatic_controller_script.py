import serial
import time

try:
    with serial.Serial('/dev/ttyUSB2', 115200, timeout=1) as ser:

        ser.write(b"0")
        time.sleep(1)

        ser.write(b"c")
        time.sleep(5)

        ser.write(b"30")
        time.sleep(5)

        ser.write(b"50")
        time.sleep(5)

        ser.write(b"80")
        time.sleep(5)

        ser.write(b"50")
        time.sleep(5)

        ser.write(b"30")
        time.sleep(5)

        ser.write(b"0")
        time.sleep(5)

        ser.write(b"z")
        time.sleep(1)

        if ser.in_waiting:
            data = ser.read(ser.in_waiting)
            print(data.decode(errors="ignore"))

except serial.SerialException as e:
    print(f"Fout: {e}")
