# Abgabe Computergrafik Sommersemester 
Ein 3D-Computergrafik-Projekt in C++ und GLSL von Leon Seeger und Jarik Strelow für die Lehrveranstaltung „Computergrafik” im Sommersemester 2025 an der Hochschule Osnabrück.

## Features

- **Prozedurale Terrain-Generierung**: Dynamisches Gelände auf Basis von Heightmaps mit texturbasiertem Materialmix.
- **Kollisionssystem**: Echtzeit-Kollisionserkennung mithilfe von Axis-Aligned Bounding Boxes (AABB), Kapsel- und Dreieck-Kollisionen inklusive Korrektur der Spielerposition.
- **3D-Steuerung**: Bewegung per WASD, Sprinten, Schleichen, Springen sowie Maussteuerung für Blickrichtung.
- **Shader**:
  - Toon- & Blinn-Phong-Shader-Modelle für Terrain und Objekte
- **Post Processing**: Chromatic Abberation.
## Steuerung
- P -> "pausieren/fortfahren" (beim start ist das Spiel standardgemäß pausiert)
- W -> vorwärts
- S -> rückwärts
- A -> links
- D -> rechts
- W + L_SHIFT -> sprint
- W + L_CTRL -> schleichen
- SPACE -> springen

## Interaktionen
Man kann mit Münzen interagieren, indem man mit denen kollidiert. Mit der Interaktion sammelt man die Münze ein.

## Quellen:
[1] C. Ericson, Real-time collision detection, Nachdr. in Morgan Kaufmann series in interactive 3D technology. Amsterdam Heidelberg: Elsevier, 20.
[2] T. Möller, E. Haines, und N. Hoffman, Real-time rendering, Fourth edition. Boca Raton: CRC Press, Taylor and Francis Group, 2018.
[3] „What Is Gamma Correction? | Baeldung on Computer Science“. Zugegriffen: 7. September 2025. [Online]. Verfügbar unter: https://www.baeldung.com/cs/gamma-correction-brightness
[4] D. Lettier, „Chromatic Aberration | 3D Game Shaders For Beginners“. Zugegriffen: 6. September 2025. [Online]. Verfügbar unter: https://lettier.github.io/3d-game-shaders-for-beginners/chromatic-aberration.html
[5] Tara, „Answer to ‚How would you implement chromatic aberration?‘“, Game Development Stack Exchange. Zugegriffen: 6. September 2025. [Online]. Verfügbar unter: https://gamedev.stackexchange.com/a/58412
[6]  sam hocevar, „Answer to ‚GLSL Shader - Change Hue/Saturation/Brightness‘“, Game Development Stack Exchange. Zugegriffen: 6. September 2025. [Online]. Verfügbar unter: https://gamedev.stackexchange.com/a/59808
[7] K. Lawonn, „Computer Graphics II  - Normal Mapping“. Uni Jena. Zugegriffen: 28. August 2025. [Online]. Verfügbar unter: https://vis.uni-jena.de/Lecture/ComputerGraphics2/Lec10_b_NormalMapping.pdf
[8] Game Developers Conference, Math for Game Programmers: Building a Better Jump, (12. Dezember 2016). Zugegriffen: 1. August 2025. [Online Video]. Verfügbar unter: https://www.youtube.com/watch?v=hG9SzQxaCm8

## Assets:
[1] R. Tuytel, „Snow 02 Texture • Poly Haven“, Poly Haven. Zugegriffen: 8. September 2025. [Online]. Verfügbar unter: https://polyhaven.com/a/snow_02
[2] R. Tuytel, „Rocks Ground 06 Texture • Poly Haven“, Poly Haven. Zugegriffen: 8. September 2025. [Online]. Verfügbar unter: https://polyhaven.com/a/rocks_ground_06
[3] Y. Poizd, „Coin“. 26. Juli 2025. Zugegriffen: 29. August 2025. [Online]. Verfügbar unter: https://sketchfab.com/models/aa4e0a5b43444ff8895b36588cba0cf5/embed?autostart=1
[4] R. Tuytel, „Aerial Rocks 02 Texture • Poly Haven“, Poly Haven. Zugegriffen: 26. August 2025. [Online]. Verfügbar unter: https://polyhaven.com/a/aerial_rocks_02
