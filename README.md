# Matrixmultiplikation mit Streaming SIMD Extensions

In der einfachsten Form wird das Projekt mit `cl /std:c17 main.c` kompiliert.
Die ausfuehrbare Datei heisst dann `main.exe`.

Zusaetzlich kann folgende (vor dem Dateinamen zu platzierende) Option verwendet werden:

* `/FAs`: Ausgabe des generierten Assembler-Codes in die Datei `main.asm`

Zudem erlauben folgende paarweise kombinierbare Optionen die Beeinflussung des Verhaltens des erzeugten Programms:

* `/DNDEBUG`: Definition der Preprocessor-Variable `NDEBUG`, um ausschliesslich zum Debuggen benoetigten Code zu entfernen
* `/DFAST`: Definition der Preprocessor-Variable `FAST`, um die Matrixmultiplikation mit SIMD-Operationen durchzufuehren

So bietet sich beispielsweise folgender Befehl an, um ein zur Vermessung geeignetes Programm zu erzeugen: 

```shell
cl /std:c17 /DNDEBUG /DFAST main.c
```
