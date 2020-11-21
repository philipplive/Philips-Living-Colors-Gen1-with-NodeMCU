# Philips Living Colors (Gen 1) mit NodeMCU + CC2500 steuern
<img src="./doc/header.jpg"/>

## Programmablauf
Das Programm dient dazu, die Lampen, welche an einer geschalteten Steckdose angeschlossen sind, nach einem stromlosen Zustand, automatisch einzuschalten (diese müssen ansonsten via Fernbedienung Manuell eingeschaltet werden).
### Start
In den ersten X Sekunden wird nach Sendersignalen gelausch und falls empfangen, diese als neue Lampen im EEPROM abgelegt. Falls nichts gefunden, werden alte Lampen aus dem EEPROM geladen.
### Loop
Grundsätzlich passiert hier nichts mehr. Wird jedoch ein Sendersignal empfangen, so wird dies als neue Default-Farbe gespeichert welche beim Start geladen wird.
Ein Taster ermöglicht zudem ein manuelles setzen der Farbe (pro Tastendruck wird der H-Wert um jeweils ca. 28° erhöht).

## Materialliste
* NodeMCU (v0.9)
* CC2500

## Protokol
Die gesendeten Blöcke sind immer 17 Byte lang, wobei die ersten 15 notwendig scheinen.

| Byte  | Beschreibung |
| ------ | --------- |
| 1  | Immer 0x0E / 14  |
| 2  | Adresse  |
| 3  | Adresse  |
| 4  | Adresse  |
| 5  | Adresse  |
| 6  | Adresse  |
| 7  | Adresse  |
| 8  | Adresse  |
| 9  | Adresse  |
| 10  | Immer 0x11 / 17  |
| 11  | Befehl   |
| 12  | Vorlaufender Zähler  |
| 13  | Wert H  |
| 14  | Wert S  |
| 15  | Wert V  |

* Die Adresse ist offenbar 8 Byte lang (da die letzten 4 Byte immer identisch sind, nehme ich an, dies ist die Absenderadresse, also die Adresse der Fernbedienung)

Befehle: 
| Wert  | Befehl |
| ------ | --------- |
| 0x01 | Unbekannt |
| 0x03 | HSV Wert setzen |
| 0x05 | Lampe einschalten (als Parameter können ebenfalls die HSV Werte gesetzt werden) |
| 0x07 | Lampe ausschalten |

## Dokumente
* CC2500 Dokumentation [> hier](https://www.ti.com/lit/ds/swrs040c/swrs040c.pdf?ts=1604949563747)
* HSV Farbraum [> hier](https://de.wikipedia.org/wiki/HSV-Farbraum)