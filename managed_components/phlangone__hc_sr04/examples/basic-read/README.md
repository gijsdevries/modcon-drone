# Basic Read Example for HC-SR04 with ESP-IDF

This example demonstrates how to interface the HC-SR04 ultrasonic distance sensor with an ESP32 using ESP-IDF. It reads distance measurements and prints them to the serial console.

## Features

- Initializes HC-SR04 sensor
- Periodically reads distance
- Outputs results via UART

## Requirements

- ESP32 development board
- HC-SR04 ultrasonic sensor
- Jumper wires
- ESP-IDF (v4.x or later)

## Wiring

| HC-SR04 Pin | ESP32 Pin |
|-------------|-----------|
| VCC         | 5V        |
| GND         | GND       |
| TRIG        | GPIOxx    |
| ECHO        | GPIOyy    |

Replace `GPIOxx` and `GPIOyy` with your chosen pins. Update the code accordingly.

## Build and Flash

```sh
idf.py set-target esp32
idf.py build
idf.py flash
idf.py monitor
```

## Output

Distance readings will be displayed in centimeters on the serial monitor.

## License

MIT