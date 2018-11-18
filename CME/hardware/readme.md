In deze folder vind je het schema van de opstelling. Dit is gewoon gesoldeerd op een prototype Eurocard PCB.

Voor de behuzing is uit een stuk hout de juiste gleuf uitgefreezed waarin de LCD strip past. Deze is met Resion Epoxy gevuld zodat het waterdicht is. Onderaan is een rechthoek uitgefreezed waar de elektronica in past.


# Hardware components

- Arduino pro mini 3.3V 8 Mhz: https://www.antratek.nl/arduino-pro-mini-328-3-3v-8mhz
- Handlebar: https://www.aliexpress.com/item/Motorcycle-E-Bike-Headlight-Horn-Signal-Handlebar-ON-OFF-Switch-Button-ja-6/32787161687.html
- 2.5W Solar panel
- LiPo Rider V1.3: http://wiki.seeedstudio.com/Lipo_Rider_V1.3/
- on/off switch
- 2 mini USB ports (1 for solar panel, 1 for optional usb charging) - Farnell: 690-W05-260-044
- Hornit Fietsbel: https://www.bol.com/nl/p/hornit-db140-fietsbel-luidste-fietstoeter-ter-wereld-zwart/9200000057530050
- see schematic for other parts



# Hardware setup
The LiPo Rider is used to connect the solar pannel to the battery, an additional  USB charging is possible.
From the pins of the battery 2 wires are soldered to be connected to the PCB (or prototyping board).
Measure with a voltmeter the correct combinations of the handle bar buttons to connect 3 of them to the GND and the others the the corresponding pins on the connector.


From the Hornit bell, remove the button and connect the two wires which were connected to the button the CME. Measure the voltage to connect them accordingly.
