# Building TypoLang

## Prerequisites

- **g++** >= 9.3
- **GNU Make** >= 4.2
- **CMake** >= 3.16
- **[MeerkatVM](https://github.com/MeerkatBoss/MeerkatVM)**

## Installation process on Linux

1. Clone TypoLang from GitHub project.
    ```
    $ git clone --recurse-submodules https://github.com/MeerkatBoss/compiled-language
    ```
2. Configure TypoLang using CMake
    ```
    $ mkdir build && cd build
    $ cmake -DCMAKE_BUILD_TYPE=Release ..
    ```
3. Build TypoLang
    ```
    $ make
    ```
4. Install TypoLang
    ```
    $ sudo make install
    ```
5. You can now run TypoLang compiler from command-line
    ```
    $ mbc_front
    $ mbc_mid
    $ mbc_back
    ```

## Uninstalling
To uninstall TypoLang run the following command from `build` directory:
```
$ sudo make uninstall
```