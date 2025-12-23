idf_component_register(SRCS "main.cpp" "i2c.cpp" "espnow_rec.cpp"
                       PRIV_REQUIRES "spi_flash" "esp_driver_gpio" "esp_driver_i2c" "esp_driver_mcpwm" "esp_wifi" "nvs_flash"
                       INCLUDE_DIRS "")
