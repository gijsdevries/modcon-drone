idf_component_register(SRCS "main.cpp"
                       PRIV_REQUIRES spi_flash "esp_driver_gpio" "esp_wifi" "nvs_flash"
                       INCLUDE_DIRS "")
