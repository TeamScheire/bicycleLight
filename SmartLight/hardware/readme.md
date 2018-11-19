# PCB

Naast de PCB en de compententen (lijst via [BOM3.xlsx](pcb_BLELight/BOM3.xlsx)) zijn er nog enkel componenten nodig die verbonden worden met de PCB en ingebouwd in de behuizing.

## Extra componenten:

### Farnell

- 1 x 1086610 -	ISR3SAD200 -  Pushbutton Switch, Off-(On), SPST-NO, 32 V, 48 V, 400 mA, Solder	4,96 euro
- 1 x 1086616 -	ISR3SAD900 -  Pushbutton Switch, Off-(On), SPST, 125 V, 50 V, 400 mA, Solder	5,80 euro
- 1 x 1638335 -	MCPAS6B2M1CE6 -  Pushbutton Switch, Off-(On), SPST, 125 V, 50 V, 400 mA, Solder	3,46 euro
- 1x 2476052 -	690-W05-260-044 -  USB Connector, Mini USB Type B, USB 1.0, Receptacle, 5 Ways, Panel Mount	5,12 euro
- 1 x 1908444 -	726141005 -  Dust Cap / Cover, Black, Dust Cap, Mini USB Type AB and Type B Connectors		0,502 euro

### Batterij
Deze lipo batterij is gebruikt: [LIPO4400mAh](https://www.olimex.com/Products/Power/BATTERY-LIPO4400mAh/)

### Rest

 - 4 x 001-M3 Multisert Messing (e.g per 1000 piece from Beluma) - these are inserts for the M3 screws
 - 4 x M3 screws
 - [Bevestiging voor het fietsstuur](https://www.dx.com/p/GACIRON-Universal-Bicycle-Handlebar-Mount-Mountain-Bike-Phone-Holder-Bike-Accessories-for-Smart-Phones-black-916491959)


## Alternatief
Het is ook perfect mogelijk om andere componenten te gebruiken (bijvoorbeeld de ESP-WROOM-32) die via UART over BLE met de app te communiceren.

Kijk bv in het stuk van de app naar [arduino code voor de esp-wroom-32](https://github.com/TeamScheire/bicycleLight/tree/master/SmartLight/app/ESP_simulation).

