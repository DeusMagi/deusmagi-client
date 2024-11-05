Deus Magi
=========

Deus Magi is a MMO role playing game similar to Ultima Online.

Players can explore a fantasy world filled with magic and quests! A place where you can encounter strange creatures, craft recipes and roleplay with your friends. Please follow the in-game tutorial that will introduce you to the world of Incuna and beyond ...

![Screenshot](https://raw.githubusercontent.com/DeusMagi/deusmagi-client/refs/heads/main/build/AppDir/textures/screenshot.png)

Features
--------

- 4 Races
- 25+ Spells
- 15+ Trainable Skills
- PvP Arenas
- Player Shops
- Player Housing
- Guilds / Factions / Gods
- 300+ Items / Weapons / Clothing
- 150+ Mobs / Creatures
- 7 Islands / 19 Worlds

Client Requirements
-------------------

- Any Linux x64 distribution that supports [AppImage](https://appimage.org)
  - Install the dependencies (e.g. Ubuntu) ...

    ```
    sudo apt-get update
    sudo apt-get -y install \
        libsdl2-dev \
        libsdl2-image-dev \
        libsdl2-mixer-dev \
        libsdl2-ttf-dev \
        libxml2-dev \
        libxmu-dev \
        mesa-utils \
        timidity
    ```
    
  - Make the AppImage executable ...
  
    ```
    chmod a+x DeusMagi.AppImage
    ```
  
  - Run `./DeusMagi.AppImage`

- Windows 10+ with [WSL v2 Ubuntu](https://learn.microsoft.com/en-us/windows/wsl/install) installed
  - Install the Windows Subsystem for Linux v2
    - Open a Command Prompt and type `wsl --install`
    - It should install WSL v2 with Ubuntu by default
    - Restart your computer and let WSL finish installing
      - It can take 5 minutes or more for Windows to continue the installation after the restart
      - In some cases, you may need to run `wsl --install` a second time
    - Setup a user account / password in WSL
    - Install the dependencies in WSL ...

      ```
      sudo apt-get update
      sudo apt-get -y install \
          libsdl2-dev \
          libsdl2-image-dev \
          libsdl2-mixer-dev \
          libsdl2-ttf-dev \
          libxml2-dev \
          libxmu-dev \
          mesa-utils \
          timidity
      ```

    - Close the Command Prompt
    
  - Run `DeusMagi.bat`

Issues / Features
---------------------

Let's build this together! Please start a [discussion](https://github.com/DeusMagi/deusmagi-client/discussions) to report issues or request new features.

History
-------------------------

This [game](https://github.com/atrinik/atrinik/) was originally created by Alex Tokar in 2009. It was forked and updated to work on modern systems.
