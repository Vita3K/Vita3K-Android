# THIS IS NOT MAIN REPO VITA3K!

# Minimum system requirement
- Vulkan 1.1 (All SoC) / OpenGLES 3.2 (Adreno only)
- Android 9, 64 bit CPU for Snapdragon variant
- Android 11, 64 bit CPU for Mediatek / Exynos / Rockchip (kirin still not supported?, idk why and since i dont have that phone variant there is no way to find out)
- **PowerVR GPU NOT SUPPORTED** because it's have limited Vulkan feature (not yet find workaround since i don't have PowerVR phones)
- At least 4GB of RAM
- 6GB Free storage

# Known Issue
Adreno GPU Driver only supported v11.5 (old sdl2 version) and [v12_build10](https://github.com/ikhoeyZX/Vita3K-Android/releases/tag/v12_build10) or newer (new sdl2 version)
- Vulkan 1.0 can run but it's too buggy, use OpenGL(ES) instead (adreno only, other SoC didn't support openGLES in this emu for now, because most mobile devices other than snapdragon doesn't support Vertex Shader SSBO) 

# Vita3K

![C/C++ CI](https://github.com/Vita3K/Vita3K/workflows/C/C++%20CI/badge.svg)
[![Vita3K discord server](https://img.shields.io/discord/408916678911459329?color=5865F2&label=Vita3K%20&logo=discord&logoColor=white)](https://discord.gg/6aGwQzh)
[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/vita3k)

## Introduction

Vita3K is an experimental PlayStation Vita emulator for Windows, Linux, macOS and Android.

* [Website](https://vita3k.org/) (information for users)
* [Wiki](https://github.com/Vita3K/Vita3K/wiki) (information for developers)
* [Discord server](https://discord.gg/MaWhJVH) (recommended)

## Android

This repository contains the source code for the Android version before it gets merged to the main repository.
Pull requests are welcomed and accepted if they target some Android-specific part of the code. Otherwise please do your pull requests directly to the [main repository](https://github.com/Vita3K/Vita3K). Please be aware that this repository can be force pushed.

Build instruction for the Android version are specified in [`building.md`](./building.md).

## Compatibility

The emulator currently runs most homebrew programs. It is also able to load some commercial games.

- [Homebrew compatibility page](https://vita3k.org/compatibility-homebrew.html)
- [Commercial compatibility page](https://vita3k.org/compatibility.html)

## Gallery

|               **Persona 4 Golden** by Atlus                   |                     **A Rose in the Twilight** by Nippon Ichi Software                         |
| :-----------------------------------------------------------: | :--------------------------------------------------------------------------------------------: |
| ![Persona 4 Golden screenshot](./_readme/screenshots/P4G.png) | ![A Rose in the Twilight screenshot](./_readme/screenshots/A%20Rose%20in%20the%20Twilight.png) |

|                  **Alone with You** by Benjamin Rivers                     |                 **VA-11 HALL-A** by Sukeban Games                    |
| :------------------------------------------------------------------------: | :------------------------------------------------------------------: |
| ![Alone with You screenshot](./_readme/screenshots/Alone%20With%20You.png) | ![VA-11 HALL-A screenshot](./_readme/screenshots/VA-11%20HALL-A.png) |

|              **Fruit Ninja** by Halfbrick Studios                  |                **Jetpack Joyride** by Halfbrick Studios                    |
| :----------------------------------------------------------------: | :------------------------------------------------------------------------: |
| ![Fruit Ninja Screenshot](./_readme/screenshots/Fruit%20Ninja.png) | ![Jetpack Joyride Screenshot](./_readme/screenshots/Jetpack%20Joyride.png) |

## License

Vita3K is licensed under the **GPLv2** license. This is largely dictated by external dependencies, most notably Unicorn.

## Downloads
* Windows
  * Requirements:
    * [Microsoft Visual C++ 2015-2022 Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe)
* Linux
  * Arch based:
    * [vita3k-bin](https://aur.archlinux.org/packages/vita3k-bin)<sup><small>AUR</small></sup>
    * [vita3k-git](https://aur.archlinux.org/packages/vita3k-git)<sup><small>AUR</small></sup>
  * Requirements:
    * xdg-desktop-portal
* Android
    * [Official Vita3k APK](https://github.com/Vita3K/Vita3K-Android/releases/) or [this custom fork APK](https://github.com/ikhoeyZX/Vita3K-Android/releases)
    * [Adreno drivers](https://github.com/K11MCH1/AdrenoToolsDrivers/releases/)

## Building

Please see [`building.md`](./building.md).

## Running
Look through the app list and click on the app you would like to run and click the start button.

For more detailed instructions on running/installing apps on Vita3K, please read the **#info-faq** channel on our [Discord Server](https://discord.gg/MaWhJVH).

## Bugs and issues
The project is in an early stage, so please be mindful when opening new issues. Expect crashes, glitches, low compatibility and poor performance.

## Thanks
Thanks go out to the developer team and [everyone who has contributed](https://github.com/Vita3K/Vita3K/graphs/contributors). 
* These are people like **petmac, frangarcj, VelocityRa, 1whatleytay, EXtremeExploit, HolyMcDiver, HorrorTroll, IllusionMan1212, KorewaWatchful, scribam, sunho, wfscans, Macdu, bookmist, pent0 and Zangetsu38**.
* Thanks to **UnearthlyGoose** for designing the Android overlay.

## Supporters
Thank you to the supporters and to all those who support us on our [ko-fi](https://ko-fi.com/vita3K).
* Among them, those who subscribed to the Nibble Tier and upper: **j0hnnybrav0, Mored4u, TacoOblivion, Undeadbob and uplush**

## Note
The purpose of this emulator is not to enable illegal activity. You can dump games from a Vita by using [NoNpDrm](https://github.com/TheOfficialFloW/NoNpDrm) or [FAGDec](https://github.com/CelesteBlue-dev/PSVita-RE-tools/tree/master/FAGDec/build). You can get homebrew programs from [VitaDB](https://vitadb.rinnegatamante.it/).

PlayStation, PlayStation Vita and PlayStation Network are all registered trademarks of Sony Interactive Entertainment Inc. This emulator is not related to or endorsed by Sony, or derived from confidential materials belonging to Sony.
