idf_component_register(SRCS "main.cpp" "i2c.cpp"
                       PRIV_REQUIRES "spi_flash" "esp_driver_gpio" "esp_driver_i2c" "esp_driver_mcpwm"
                       INCLUDE_DIRS "")
