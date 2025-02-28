Welcome to the FreakaZone:

1. To use the code you have to load this to VS Code with Arduino / Platform IO.

2. The invented dummy hardcoded WiFi credentials need to be replaced.

3. The main theme can be choosen in the platformio.ini
   - build_with=99 ;cleaner: to set the EEPROM to default. Important for errors or devices that no longer start
   - build_with=0 ;nothing / firmware
   - build_with=1 ;light: NeoPixel & Analog Out for WW & CW or simple clock function
   - build_with=2 ;IO: All In and Out's. Like Window (DI), RPM (speed),  ImpuleCounter (old E-Meters) or Weight Sensors (Hx711)
   - build_with=3 ;heating, DS18B20: Includes PID for heating / aircondition or oneline temperature DS18B20 for multiple heating supply and return
   - build_with=4 ;RFID: just testing now


4. The 'dest' in the sendToUpdateServer.py is the http update sever location folder (available through your web server)
   - filesufix = updatechannel

5. Pin assignment
   fixed, ToDo: Description

6. example images
   ToDo: Upload 'print' page demo, 'status' (json) page demo
