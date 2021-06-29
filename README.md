# Matrixmultiplikation mit Streaming SIMD Extensions

## Build
Zunächst alle Projektkonfigurationen in VS erstellen mit STRG + UMSCHALT + B

- DEBUG
- DEBUG (FAST)
(DEBUG nicht wichtig für Messungen, da Ergebnisse durch print verfälscht)

- RELEASE
- RELEASE (FAST)

Im DEBUG sind die Ziffern der Zufallsmatrizen auf 0-9 beschränkt. Im Release auf das Maximum von rand() ca. 32.000.

Dann sind die jeweiligen .exe Dateien (Ausführbare Dateien) im Ordner x64 erzeugt (oder jeweils x84 je nachdem was bei der Erstellung augewählt wurde).

Die Matrizen in Form von .txt Files sind noch nicht erzeugt, können nach belieben erzeugt werden, um dann mit der load-Funktion eingeladen zu werden. Mmentan wird die load-Funktion daher nicht genutzt und ist lediglich vorgerüstet für einen späteren Einsatz mit .txt Dateien.

Die Funktion multiply() unterscheidet sich nicht, mit oder ohne Parametern.

## Ausführen des Programms über Kommandozeile:

2 Random Matrizen (mit ziffern von 0-9) werden erzeugt und multipliziert. Hier wird die load-Funktion nicht genutzt:
```
\x64\Debug>Matrixmultiplikation_SIMD_Github.exe
```

Eine (vorher erstellte) Matrix wird als Parameter übergeben und mit sich selbst multipliziert:
```
\x64\Debug>Matrixmultiplikation_SIMD_Github.exe matrix1.txt
```

2 (vorher erstellte) Matrizen wird als Parameter übergeben und miteinander multipliziert:
```
\x64\Debug>Matrixmultiplikation_SIMD_Github.exe matrix1.txt matrix2.txt
```

## Ausführen des Programms über VS:

Unter Configuration Properties - Debugging können Command Arguments übergeben werden. Durch das Setzen passiert das Gleiche wie zuvor über die Kommandozeile. Es werden speziell definierte Matrizen gewählt, statt Random Matrizen.


