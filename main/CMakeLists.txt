idf_component_register(SRCS "main.cpp" "i2c.cpp" "pwm.cpp" "espnow_rec.cpp" "uart.cpp"
                       PRIV_REQUIRES "spi_flash" "esp_driver_gpio" "esp_driver_i2c" "esp_driver_mcpwm" "esp_wifi" "nvs_flash" "hc_sr04" "esp_driver_uart"
                       INCLUDE_DIRS "")
