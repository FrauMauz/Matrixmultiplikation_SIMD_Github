# Matrixmultiplikation mit Streaming SIMD Extensions

In der einfachsten Form wird das Projekt mit `cl /std:c17 main.c` kompiliert.
Die ausfuehrbare Datei heisst dann `main.exe`.

Zusaetzliche (vor dem Dateinamen zu platzierende) paarweise kombinierbare Optionen sind:

* `/O2`: Optimierung der Performance
* `/Zc:inline`: Entfernung von Symbolen unreferenzierter Funktionen
* `/FAs`: Ausgabe des generierten Assembler-Codes in die Datei `main.asm`

Zudem erlaubt folgende Option die Beeinflussung des Verhaltens des erzeugten Programms:

* `/DNDEBUG`: Definition der Preprocessor-Variable `NDEBUG`, um ausschliesslich zum Debuggen benoetigten Code zu entfernen

So bietet sich beispielsweise folgender Befehl an, um ein zur Vermessung geeignetes Programm zu erzeugen: 

```shell
cl /std:c17 /O2 /Zc:inline /DNDEBUG main.c
```
