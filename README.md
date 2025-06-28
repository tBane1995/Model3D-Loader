# Model3D Loader

## Spis treści
- [Opis](#opis)
- [Zrzuty ekranu](#zrzuty-ekranu)
- [Instalacja](#instalacja)
- [Technologie](#technologie)
- [Licencja](#licencja)

## Opis
**Model3D Loader** to program, który ma renderować modele trójwymiarowe.

## Zrzuty ekranu
![Zrzut ekranu](screenshots/01.png)

## Instalacja
## Instalacja
1. Pobierz i zainstaluje Visual Studio 2022
2. Pobierz i zainstaluje CMake
3. Pobierz bibliotekę GLFW 3.4 (https://www.glfw.org/) i umieść ją w katalogu `C:\glfw-3.4.bin.WIN64`.

4. Otwórz **Wiersz poleceń (cmd)**.
5. Przejdź do katalogu projektu:
`
cd ..\..\Model3D Loader
`
6. Utwórz folder **build**:
`
mkdir build
`
7. Wejdź do folderu **build**:
`
cd build
`
8. Uruchom konfigurację projektu za pomocą **CMake**:
`
cmake ..
`
9. Zbuduj projekt:
`
cmake --build .
`

10. Plik wykonywalny znajdziesz w **build/Debug** lub **build/Release**
## Technologie
Program stworzono w języku C++ z wykorzystaniem bibliotek OpenGL 3.3.0, GLAD oraz GLFW 3.4.
  
## Licencja
Licencja Otwarta – Uznanie autorstwa  
  
Ten program może być:  
-Pobierany  
-Kopiowany  
-Modyfikowany  
-Wykorzystywany w projektach prywatnych i komercyjnych  
  
Pod warunkiem, że:  
-Zachowana zostanie informacja o autorze oryginalnego programu  
-Podane zostanie źródło (link do repozytorium)  
-W przypadku modyfikacji, należy wyraźnie zaznaczyć, że program został zmodyfikowany oraz przez kogo.  
