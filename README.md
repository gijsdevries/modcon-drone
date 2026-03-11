MAC-adr drone: uint8_t broadcastAddress[] = {0x88, 0x57, 0x21, 0x78, 0xaa, 0x80};

MAC-adr controller: uint8_t broadcastAddress[] = {0x80, 0xF3, 0xDA, 0x54, 0x18, 0x38};

The first argument of the python script is the port you want to send data to so e.g.
python3 python_scripts/automatic_controller_script.py /dev/ttyUSB0
