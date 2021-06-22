# Matrixmultiplikation mit Streaming SIMD Extensions

In der einfachsten Form wird das Projekt mit `cl /std:c17 main.c` kompiliert.
Die ausfuehrbare Datei heisst dann `main.exe`.

Zusaetzlich kann folgende (vor dem Dateinamen zu platzierende) Option verwendet werden:

* `/FAs`: Ausgabe des generierten Assembler-Codes in die Datei `main.asm`

Zudem erlauben folgende paarweise kombinierbare Optionen die Beeinflussung des Verhaltens des erzeugten Programms:

* `/DNDEBUG`: Definition der Preprocessor-Variable `NDEBUG`, um ausschliesslich zum Debuggen benoetigten Code zu entfernen
* `/DFAST`: Definition der Preprocessor-Variable `FAST`, um die Matrixmultiplikation mit SIMD-Operationen durchzufuehren

So bietet es sich beispielsweise an, die durch die folgenden Befehle erzeugten Programme zu vermessen und zu vergleichen: 

```shell
cl /std:c17 /DNDEBUG main.c
cl /std:c17 /DNDEBUG /DFAST main.c
```

## Build
Zunächst alle Projektmappen in VS erstellen mit STRG + UMSCHALT + B
- DEBUG
- DEBUG (FAST)
- RELEASE
- RELEASE (FAST)

Dann sind die jeweiligen Ordner mit den .exe Dateien (Ausführbare Dateien) erzeugt im Ordner x64 (oder jeweils x84 je nachdem was bei der Erstellung augewählt wurde)

## Ausführen des Programms über Kommandozeile:

2 Random Matrizen (mit ziffern von 0-9) werden erzeugt und multipliziert. Hier wird die load-Funktion nicht genutzt. :
```
\x64\Debug>Matrixmultiplikation_SIMD_GITHUB.exe
```


Eine (vorher erstellte) Matrix wird als Parameter übergeben und mit sich selbst multipliziert:
```
\x64\Debug>Matrixmultiplikation_SIMD_GITHUB.exe matrix1.txt
```


2 (vorher erstellte) Matrizen wird als Parameter übergeben und miteinander multipliziert:
```
\x64\Debug>Matrixmultiplikation_SIMD_GITHUB.exe matrix1.txt matrix2.txt
```

Die .txt Files sind noch nicht erzeugt, können nach belieben erzeugt werden, um dann mit der load-Funktion eingeladen zu werden.
Die Dunktion multiply() unterscheidet sich nicht, mit oder ohne Parametern.
