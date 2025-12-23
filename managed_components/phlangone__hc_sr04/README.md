# HC-SR04 Ultrasonic Sensor Component

ESP-IDF component for HC-SR04 ultrasonic distance sensor.

## Installation

Add this component to your project using IDF Component Manager:

```bash
idf.py add-dependency "espressif/hc_sr04 <version>"
```

Or manually clone this repository into your project's components directory:

```bash
mkdir -p components
cd components
git clone https://github.com/phlangone/hc-sr04-esp-idf.git hc_sr04
```

## Getting Started

Include the header file in your code:

```c
#include "hc_sr04.h"
```

Initialize the sensor with your GPIO configuration:

```c
hc_sr04_config_t config = {
    .trigger_pin = GPIO_NUM_4,
    .echo_pin = GPIO_NUM_5,
    .timeout_us = 30000
};
hc_sr04_handle_t sensor = hc_sr04_init(&config);
```

Measure distance in your application loop:

```c
float distance = hc_sr04_measure_cm(sensor);
```