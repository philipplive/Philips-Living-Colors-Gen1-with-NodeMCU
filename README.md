# Philips Living Colors (Gen 1) mit NodeMCU + CC2500 steuern

### Protokol
Die gesendeten Blöcke sind immer 17 Byte lang, wobei die ersten 15 notwendig sind

| Byte  | Beschreibung |
| ------ | --------- |
| 1  | Immer 0x0E / 14 (nachfolgende Paketgrösse?) |
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

Befehle: 
| Wert  | Befehl |
| ------ | --------- |
| 0x01 | Unbekannt |
| 0x03 | HSV Wert setzen |
| 0x05 | Lampe einschalten |
| 0x07 | Lampe ausschalten |