## EtherTerm (c) 2014-2015 Michael Griffin <mrmisticismo@hotmail.com>
### An SDL2 Terminal Telnet/SSH program for Windows, Linux & OSX

**Alpha Stage 1**

<center>
![alt text](https://dl.dropboxusercontent.com/u/92792939/EtherTerm.png "EtherTerm")
</center>

EtherTerm is a Telnet/SSH terminal with CP437 (ANSI) Graphics terminal
emulation for connecting to Telnet/SSH BBS systems and shells.

The program is developed mainly in Windows using the (Codelite IDE) with
mingw32/64 and will cross-compile under OSX, and Linux with GNU g++ or clang.

**This is a full graphics program just like a game engine, it's not a
console application and best runs on more recent hardware.** Unlike some
older SDL applications that use software rendering, EtherTerm uses SDL2
and full OpenGL/DirectX graphics acceleration on the GPU.

Each system can be setup with a default font.  Any systems using Syncterm
escape sequences for font switching will override the default font.

## Fonts currently supported are:

+ VGA-8x16             (CP437)
+ MicroKnightPlus-8x16 (AMIGA)
+ Mo'soul-8x16         (AMIGA)
+ Pot Noodle-8x16      (AMIGA)
+ Topaz Plus-8x16      (AMIGA)

**UTF-8 Output encoding with TTF fonts are in the works!**

**Window and Full Screen Modes**

```ALT + ENTER``` toggles between different window screen sizes.
This cycles two different window resolutions then goes Fullscreen.

***Keymappings are are now available***

Etherterm currently follows the default Windows telnet output keys seqeunces (used in Syncterm) 
as the ANSI default. VT100, LINUX and SCO key mappings are supported for terminals sessions.

**Copy/Paste support**

```Left mouse click and drag``` to select text on the screen, release to copy selected text to the clipboard.

```Right mouse click``` to paste text from the clipboard.

Some items with copy/paste are still a work in progress.

## Work in progress:

Some items are still being worked on for scrolling regions with copy/paste and the screen buffer.

The Dialing directory is incomplete, Add, Edit, Delete, About etc..
Not all options are setup just yet.

To add new systems or remove, you must edit the ```dialdirectory.xml``` file
in a text editor located in the /assets folder.

I'm also in the middle of implimenting more control sequence behavior.
I have a lot of XTERM sequences laid out so more will be implemented with time.

##Some recent additions

```ALT + H``` added for quick disconnections from systems.

```SSH login/passowrd prompts``` have been added when they are not specified in the ```dialdirectory.xml```

```Keymappings``` keymappings for terminal support.

## Not yet implemented:

Lots of things are still in the early testing and debugging stages.
There are no transfer protocols at the moment.

SSH works with login and password authentication, Public Key is in testing.

There is no scroll back buffer, this is also in the works.

FTP will be added too.

Terminal size is locked at 80 x 25 for development.
Other modes will be configurable later on.

## Demo for Windows:

This is the Demo 2.3 Preview for Windows.
<br/>
<a href = 'https://dl.dropboxusercontent.com/u/92792939/EtherTerm%20Demo2.3.zip'>EtherTerm Demo 2.3</a>
<br/>
<br/>
Linux, OSX can be built easily with the following instructions below.

## Build Instructions

Generally, the following libraries must be installed:

- SDL2
- SDL2_Net
- Libssh (Requires OpenSSL, Zlib)

As well as a recent version of gnu/gcc providing g++, or clang.

Linking ```libSDL2_main``` is optional on some distributions.

For windows, mingw32 or 64 is required.

### Windows

Must compile with latest c++ environment flags.

At the very least and you can also replace 0x with 11 on
newer compilers. Usually g++ (4.7)+ or compatible compiler.

```
-std=gnu++0x or -std=c++0x
```

To build windows you must already have a compiler and the requirements.
You can use a good IDE like CodeLite, CodeBlocks, or Visual Studio.
You must install the libs and setup the include paths, like any project.

Makefile(s) are provided for g++ compiles on Windows.

With Windows you will need to setup a command line environment appropriate
for compilation of this projet, or create a build target with the necessry
settings using a project file of your preferred IDE.

```
make clean-win
make win
```

### OSX

- install http://brew.sh/
- ``brew install sdl2 sdl2_net libssh``
- ``make clean-osx; osx``

### Ubuntu, Mint, and Debian Linux

- ``sudo apt-get install build-essential libsdl2-dev libsdl2-net-dev libssh-dev``
- ``make clean-linux; make linux``

### Arch Linux

- ``sudo pacman -S sdl2 sdl2_net extra/libssh``
- ``make clean-linux; make linux-arch``

## Executing the first time:

**EtherTerm will look for the ASSETS folder off the root executable folder**

The Makefiles are now updated to copy the executable down from the DEBUG folder to the root folder.

```./EtherTerm to execute the program```

**Just in case, although I haven't seen any issues**

If you have any issues with files not loading, check or reset permissions while inside of the root EtherTerm folder.

``chmod -R u+r .``

**Acknowledgement and Thanks:**
+ Dingo:  Telnet options and general terminal functionality
+ G00R00: Optimization ideas on scrolling pixels faster.
+ (SDL Game Programming) For good ideas with Singletons and State Machines.
+ Maze: Testing and bug reports
+ Caphood: Testing and bug reports
+ IceDevil: Testing and bug reports
+ Haliphax: Testing and bug reports
+ HellBeard: Testing and bug reports
