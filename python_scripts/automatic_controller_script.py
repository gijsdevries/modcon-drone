import serial
import time
import sys

if len(sys.argv) < 2:
    print(f"Usage: python3 {sys.argv[0]} <serial_port>")
    sys.exit(1)

port = sys.argv[1]

try:
    with serial.Serial(port, 115200, timeout=1) as ser:

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
