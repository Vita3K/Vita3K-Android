# Build Vita3K

Vita3K uses CMake for its project configuration and generation. In theory, it should be compatible with any project generator supported by CMake, C++17 compatible compiler and an IDE with CMake support.

The project provides [CMake presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) to allow configuring and building Vita3K without having to deal with adding the needed arguments through a command-line interface or using the user interface of your IDE. As long as your IDE or code editor supports CMake, the software should immediately detect the presets and let you choose which configuration settings you want to use to generate the program. Reference on how to use CMake presets with various IDEs and code editors can be found here:

- [Visual Studio](https://docs.microsoft.com/en-us/cpp/build/cmake-presets-vs)
- [Visual Studio Code](https://github.com/microsoft/vscode-cmake-tools/blob/main/docs/cmake-presets.md)
- [CLion](https://www.jetbrains.com/help/clion/cmake-presets.html): CMake presets in CLion are imported as CLion's [CMake profiles](https://www.jetbrains.com/help/clion/cmake-profile.html).

All presets are named after `<target_os>-<project_generator>-<compiler>`, are automatically hidden and shown depending on your host OS and generate a binary folder of path `<source_directory>/build/<preset_name>`. For command-line users, run `cmake --list-presets` on the top directory of the repository to see which presets are available to you. For presets without `<project_generator>` and/or `<compiler>`, the project generator and/or the compiler haven't been explicitly specified in the preset to let CMake fallback to the platform defaults.

If you still want to use presets but none of them works for your setup, you can make new ones by creating a `CMakeUserPresets.json` file and you can check the specification [here](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html). Git will ignore this file.

**Note: Vita3K doesn't support compilation for 32-bit/x86/i386 platforms.**

For convenience, the following building instructions are given as examples:

## Windows, Linux and MacOS

see [building.md]([https://developer.android.com/ndk/downloads](https://github.com/Vita3K/Vita3K/blob/master/building.md)) for pc version, this fork only for android

## Android

- Building the Android version requires both the [Android SDK](https://developer.android.com/ndk/downloads) and [Android NDK](https://developer.android.com/ndk/downloads), both can be installed from Android Studio. You will need to set the environment variable ANDROID_NDK_HOME (and ANDROID_SDK_HOME when not using Android Studio) to their proper location.

- The Android version of Vita3K relies on [vcpkg](https://vcpkg.io/en/) to build some of its dependencies.
  ```sh
  vcpkg install boost-system:arm64-android boost-filesystem:arm64-android boost-program-options:arm64-android boost-icl:arm64-android boost-variant:arm64-android openssl:arm64-android zlib:arm64-android
  ```
  You will also need to set the environment variable VCPKG_ROOT to its proper location.

- Building can be done with Android studio: select the Vita3K Android folder and click on the build icon or by command line:
  ```sh
  ./gradlew --stacktrace --configuration-cache --build-cache --parallel --configure-on-demand assembleRelease
  ```

  or if you dont have "keystore.kjs", you can build using debug version (but file become large than release version) use this command line:
  ```sh
  ./gradlew --stacktrace --configuration-cache --build-cache --parallel --configure-on-demand assembleReldebug
  ```

   or if you want build using github action and you don't have "keystore.kjs", you need replace some command at android.yml in ".github/workflows/" then find this command:
  ```sh
  ./gradlew --stacktrace --info --configuration-cache --build-cache --parallel --configure-on-demand assembleRelease
  ```
  and then replace to this command:
  ```sh
  ./gradlew --info --configuration-cache --build-cache --parallel --configure-on-demand assembleReldebug
  ```

  if you have keystore.kjs, you need put "secret code" in your github fork settings > secret and variable > actions > new repository secret

  and add following name:
  - KEYSTORE (fill secret with converted text using base64 tool that contain file your keystore.kjs)
  - SIGNING_KEY_ALIAS (fill secret with your alias name that used in keystore.kjs)
  - SIGNING_KEY_PASSWORD (fill secret with your password that used in keystore.kjs)

### Building SDL

You can use prebuild libsdl from Macdu build or build from libsdl.org without any patch since this version of Vita3K already support libadrenotools but you just need merge "android/src/main/java/org/libsdl/app" from libsdl.org source code if you build from libsdl.org

## Note

- After cloning or checking out a branch, you can check update submodules (if update not break your config).
  ```sh
  git submodule update --init --recursive
  ```

- If Boost failed to build, you can use the system Boost package (Linux and macOS only).

  ```sh
  brew install boost # for macOS
  sudo apt install libboost-filesystem-dev libboost-program-options-dev libboost-system-dev # for Ubuntu/Debian
  ```

  If needed, CMake options `VITA3K_FORCE_CUSTOM_BOOST` and `VITA3K_FORCE_SYSTEM_BOOST` can be set to change the way the CMake project looks for Boost.
