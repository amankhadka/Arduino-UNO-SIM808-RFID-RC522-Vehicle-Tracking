# Arduino-UNO-SIM808-RFID-RC522-Vehicle-Tracking

The project implements Arduino UNO as a base controller. It utilized SIM808 for GSM, GPS and GPRS connectivity. The project also implements RFID module RC522 for Mayfair card integration. The project is designed to be installed in moving vehicle with card controlled actions and for vehicle tracking.

Among the sketch file the major sketch is wisdomGPS_GPRS_RFID.ino which incorporate other there different modules as different hardware.

The project utilize SIM808 base library for all the integration and communincation. But the library lacks update for socket programming (TCP) So it uses its own style fot TCP socket connection and data transfer.
During the design and coding many SIM808 libraries were tested but all of them lacks some or other. Here we have designed some of them that lacks for proper communication.

The project is designed for moving vehicle and believed that a vehicle 12V battery connection to the module creates needed supply directly. 
The project can be further utilized for additional features like driver key access after RFID varification. 

The connectivity to Server can be done from "mySocket" program for php and python server side sockets.
