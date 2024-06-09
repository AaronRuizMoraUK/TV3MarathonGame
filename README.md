# TV3 Marathon Game

Game developed for the 2008 charity event "La Marató de TV3" focused on mental health, organized by the Barcelona TV3 channel.

This game was an initiative of the Master in Videogame Development of Pompeu Fabra University and built on top of the 2007 project [Galaxy Scraper](https://github.com/AaronRuizMoraUK/GalaxyScraper).

## Game Synopsis

Experience Doc's groundbreaking invention: a suit that shrinks you to navigate and treat mental illnesses inside a patient's brain.

Tackle depression, OCD, and hyperactivity through educational mini-games and puzzles, boosting neurotransmitter levels and restoring brain function in an immersive and scientific adventure.

## Build and Run

Requirements
- Microsoft Windows 
- Microsoft Visual Studio 2022
- CMake 3.28
- Graphics card with DirectX 9.0c support

Steps
- Clone repo
  ````
  git clone https://github.com/AaronRuizMoraUK/TV3MarathonGame.git
  ````
- Generate Visual Studio solution using CMake in `build` folder
  ```` 
  cd TV3MarathonGame
  mkdir build
  cd build
  cmake .. -G "Visual Studio 17 2022"
  ````
- Open `TV3MarathonGame.sln` with Visual Studio
- Build and run `TV3MarathonGame` project

The game runs by default in window mode at `1280x720` resolution. To run the game in full screen set the `fullscreen` property to `1` in  `TV3MarathonGame\data\config.xml` file. Resolution can be changed in the `Options` menu inside the game.

## Controls

A gamepad with analog stick, such as the Xbox Controller, is highly recommended:

- Use left analog stick to move
- `A` button to kick.
- `Start` button to pause the game.

Controls using keyboard:

- Use `WSAD` keys to run.
- Left mouse button to kick.
- Use `Enter` to pause the game.

## Improvements

I made the following enhancements while adapting the project for Github.

- Translated game to English. Some text within textures remains in Catalan.
- Built the project using CMake.
- Added support for modern screen resolutions and refresh rates.
- Integrated DXSDK-D3DX to continue using the deprecated DirectX 9 utilities, which are no longer included in the DirectX components of the Windows SDK.
- Improved separation between game code and third-party libraries.

## Third-party Libraries

- **[DXSDK-D3DX](https://www.nuget.org/packages/Microsoft.DXSDK.D3DX)**: Headers, import libraries and runtime DLLs for the deprecated D3DX9 utilities libraries. 
- **[Bass](https://www.un4seen.com/)**: Audio library.
- **[Cal3D](https://github.com/mp3butcher/Cal3D)**: 3D character animation library.
- **[Expat](https://github.com/libexpat/libexpat)**: C library for parsing XML.
- **[Lua](https://github.com/lua/lua)**: Powerful, efficient, lightweight, embeddable scripting language.
- **[ParticleAPI](https://github.com/davemc0/Particle)**: Particle system library.
- **[SOLID](https://github.com/dtecta/solid3)**: Library for performing intersection tests and proximity queries for collision detection.

## Credits

Produced by La Fundació La Marató de TV3, in collaboration with TVC Editions.

This game was an initiative of the Master in Videogame Development of Pompeu Fabra University.

Executive Producer

- Daniel Sánchez-Crespo (UPF)

Developed by

- Aaron Ruiz
- Carlos Guzmán
- Xavi Lamarca
- Roger Solé
- Josep Soto

Music by

- Nomar (sinplanb)
