# Full-Automated-PCBs-Recycling-Production-Line
**Overview**
The Full Line Control System is an integrated solution for managing and monitoring industrial machines. It combines local control via Arduino Mega with remote monitoring capabilities using ESP8266 and the Blynk platform. The system supports automatic and manual operation modes, ensures safety through IR and ultrasonic sensors, and provides real-time status updates via an OLED display and Blynk mobile app.

Key Features
Automatic Mode: Sequential operation of machines based on predefined sequences.
Manual Mode: Direct control over individual machines via a keypad interface.
Safety Sensors: IR and ultrasonic sensors for detecting motion and obstacles, ensuring workplace safety.
User Interface: Keypad and OLED display for user interaction and system status display.
Remote Monitoring: Integration with Blynk for real-time monitoring and control from anywhere.
Components
Arduino Mega: Controls relay modules for machine operation and interfaces with sensors and user inputs.
ESP8266: Facilitates communication with the Blynk platform, enabling remote monitoring and control.
IR and Ultrasonic Sensors: Ensures safety by detecting motion and obstacles within the workspace.
OLED Display: Provides real-time status updates and prompts for user interaction.
Keypad: Allows users to select operation modes (automatic/manual) and input credentials for access control.
Installation and Usage
Hardware Setup
Connect Arduino Mega:
Connect Arduino Mega to relay modules, IR and ultrasonic sensors, OLED display, and keypad.
Connect ESP8266:
Connect ESP8266 to Arduino Mega for communication and integration with Blynk.
Software Setup
Upload Arduino Mega Firmware:

Upload arduino_code.ino to control machine operations, interface with sensors, and manage user inputs.
Upload ESP8266 Firmware:

Upload esp8266_code.ino to facilitate communication with Blynk, transmit system status, and receive control commands.
Integration with Blynk
Configure Blynk App:
Obtain Blynk authentication token and Wi-Fi credentials.
Configure Blynk app with virtual pins for monitoring machine states and controlling operation modes remotely.
Documentation
Detailed Documentation: Refer to the project documentation for comprehensive details on system architecture, component connections, operational modes, safety protocols, and integration with Blynk.
Demonstration Video: Watch a demonstration video showcasing the system's capabilities, installation guide, and user interaction scenarios.
Future Enhancements
Implement predictive maintenance features using sensor data analytics.
Enhance user interface for intuitive control and monitoring.
Integrate with cloud platforms for data logging and advanced analytics.
Contributors
Your Name - Project Lead & Developer
License
This project is licensed under the MIT License.


