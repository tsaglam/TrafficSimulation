# Mikroskopische Verkehrssimulation

PdMKP 2018 Programmierprojekt

## Anmerkungen zum Build-System

Zum automatischen Testen wird das mitgelieferte Makefile verwendet. Im Projekt kann ein weiteres beliebiges Build-System genutzt werden, allerdings muss die Makefile aktuell und funktionsfähig gehalten werden.

## Testdateien

Die mitgelieferten Testfälle decken einzelne Bereiche der Implementierung ab. Die Musterlösungen sind als JSON-Dump in den zugehörigen .sol Dateien zu finden.

### 00-zero_timestep
Komplexe Eingabe. Es werden null Zeitschritte simuliert. Positionen in der Eingabe entsprechen Positionen in der Ausgabe.

### 05-1car_1step
Ein Automobil mit freier Fahrt nach einem Zeitschritt.

### 10-1car_10steps
Ein Automobil mit freier Fahrt nach zehn Zeitschritten.

### 13-speed_limit
Ein Automobil mit freier Fahrt limitiert durch ein Geschwindigkeitslimit.

### 15-1car_uturn
Ein Automobil mit freier Fahrt das zwischen zwei Kreuzungen pendelt.

### 20-3cars_1lane
Test des IDM-Verfolgermodell, drei Automobile in einer Spur. Absteigend nach maximal Geschwindigkeit angeordnet.

### 25-2two_cars_before_lane_change
Test des Spurwechsel nach MOBIL. Zwei Automobile in der selben Spur. Simulation endet einen Zeitschritt vor Spurwechsel des hinteren Automobils.

### 27-2cars_lane_change
Test des Spurwechsel nach MOBIL. Zwei Automobile in der selben Spur. Simulation endet im Zeitschritt des Spurwechsels.


