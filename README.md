## EtherTerm (c) 2014-2015 Michael Griffin <mrmisticismo@hotmail.com>
### An SDL2 Terminal Telnet/SSH program for Windows, Linux & OSX

**Alpha Stage 1**

<center>
![alt text](https://dl.dropboxusercontent.com/u/92792939/EtherTerm.png "EtherTerm")
</center>

The program is developed mainly in Windows using the (Codelite IDE) with
mingw32/64 and will compile in OSX, and Linux under GNU g++.

**This is a full graphics program just like a game engine, it's not a
console application and best runs on more recent hardware.** Unlike some 
older SDL applications that use software rendering, EtherTerm uses SDL2 
and full graphics acceleration on the GPU.

EtherTerm is a Telnet/SSH terminal with CP437 (ANSI) Graphics terminal
emulation for connecting to Telnet/SSH BBS systems and shells.

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

**Full Copy/Paste support**

```Left mouse click and drag``` to select text on the screen, release to copy selected text to the clipboard.

```Right mouse click``` to paste text from the clipboard.

## Work in progress:

Some items are still being worked on for scrolling regions with copy/paste and the screen buffer.

The Dialing directory is incomplete, Add, Edit, Delete, About etc..
Not all options are setup just yet.

To add new systems or remove, you must edit the ```dialdirectory.xml``` file
in a text editor located in the /assets folder.

If you want to connect to SSH servers, you must supply your username, and password
in the ```dialdirectory.xml``` file since there are no prompts to enter these when
you connect at the moment.

I'm also in the middle of implimenting more control sequence behavior.
I have a lot of XTERM sequences laid out so more will be implemented with time.

## Not yet implemented:

Lots of things are still in the early testing and debugging stages.
There are no transfer protocols at the moment.

SSH works with login and password authentication, Public Key is in testing.

Some keyboard keys still need to be implemented like F1 function keys, Keypad Keys.
All others should work just fine including CTRL Keys.

There is no scroll back buffer, this is also in the works.

FTP will be added too.

Terminal modes, by default EtherTerm only supports 80 x 25.  Other modes will be supported lateron.

***Keymappings are planned also***
Etherterm currently follows the default Windows telnet output keys seqeunces (used in Syncterm) for keys like arrrows, Insert, Delete, PageUp and Page Down.  Some other operating systems look for and use different escape sequence for their input keys which will be added later for more support.

##Demo for Windows:

This is the Demo 2 Preview client for Windows.
<br/>
<a href = 'https://dl.dropboxusercontent.com/u/92792939/EtherTerm%20Demo2.zip'>EtherTerm Demo2 Preview.zip</a>
<br/>
<br/>
Linux, OSX can be built easily with the following instructions below.

##To compile the following libraries must be pre-installed:

```
SDL2
SDL2_Net
Libssh (Requires OpenSSL, Zlib)
```

**Example Windows (mingw32/64) linking:**
Manual compiles will require the following:
```
Libs: mingw32 or 64, SDL2main, SDL2, SDL2_net, libssh
```

**Example (G++/Clang) linking:**
Manual compiles will require the following:
```
libs: libSDL2, libSDL2_net, libssh
```
linking ```libSDL2_main``` is optional on some distros.


Must compile with latest c++ environment flags.

At the very least and you can also replace 0x with 11 on
newer compilers. Usually g++ (4.7)+ or compatible compiler.

```
-std=gnu++0x or -std=c++0x
```

#Makefile instructions:

To build windows you must already have a compiler and the requirements.
You can use a good IDE like CodeLite, CodeBlocks, or Visual Studio (more headache).
You must install the libs and setup the include paths, like any project.

Makefile(s) are provides for g++ compiles on Windows, OSX and Linux.
With Windows you will need to update the paths, or import fresh into a new IDE project.

**The following Makefile commands will always default for Windows, or you can specify.**
```
make
make win

make clean
make clean-win
```

**For OSX**
```
make osx

make clean-osx
```

**For Linux (Ubuntu, Mint and Debian)**
```
make linux

make clean-linux
```

**For Linux (Arch Linux)**
```
make linux-arch

make clean-linux
```

##Linux Build Steps:

###Ubunutu, Mint and Debian can use APT-GET to install requirements.

**Clone the Git repository.**
```
git clone https://github.com/M-griffin/EtherTerm.git
```

**Install build-essential**
```
sudo apt-get install build-essential
```

**Install SDL2**
```
sudo apt-get install libsdl2-dev
```

**Install SDL2_Net**
```
sudo apt-get install libsdl2-net-dev
```

**Install Libssh**
```
sudo apt-get install libssh-dev
```

**To compile your executable.**
```
make linux 
```

###Arch Linux can use PACMAN to install requirements.

**Clone the Git repository.**
```
git clone https://github.com/M-griffin/EtherTerm.git
```

**Install SDL2**
```
sudo pacman -S sdl2
```

**Install SDL2_Net**
```
sudo pacman -S sdl2_net
```

**Install Libssh**
```
sudo pacman -S extra/libssh
```

**To compile your executable.**
```
make linux-arch
```


##OSX Build Steps:

**Clone the Git repository.**
```
git clone https://github.com/M-griffin/EtherTerm.git
```

**Install Brew if you don't have it already**
```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

**Install SDL2**
```
brew install sdl2
```
Installs /usr/local/Cellar/sdl2/2.0.3:

**Install SDL2_Net**
```
brew install sdl2_net
```
installs /usr/local/Cellar/sdl2_net/2.0.0

**Install Libssh**
```
brew install libssh
```
installs /usr/local/Cellar/libssh6.4

**To compile your executable.**
```
make osx
```

**Possible issues with OSX Yosemite when building libssh.**
There is a report and confirmation of a macro redefinition error when compiling on Yosemite.  A patch for this issue was created and applied just a few days ago.  Earlier OSX versions are not affected.

**You can use the following links for patch details, or get a new copy of libssh directly from the repo.**
Brew should have all dependencies for libssh already set for a clean libssh manual compile.
```
Bug Report
https://red.libssh.org/issues/164#change-488

Patch
https://red.libssh.org/attachments/download/95/patch-ConfigureChecks.cmake.diff
```

**link to libssh repo:**
```
git clone http://git.libssh.org/projects/libssh.git libssh
```

##Executing the first time:

Once these are all set, just run the make-osx to build the executable.

**EtherTerm will look for the ASSETS folder off the root executable folder**

After you compile, the executable is placed in a default DEBUG folder.
If you want to run from this folder you must copy the assets folder to the DEBUG 
folder or the root folder where the executable is located.

**For Example:**
```
./EtherTerm/DEBUG   ( Contains Executable )
./EtherTerm/assets  ( Contains Fonts, Screens and xml files )
```

**Copy the assets folder to the debug foler so it looks like this:**
```
./EtherTerm/DEBUG/assets
```

**Acknowledgement and Thanks:**
+ Dingo:  Telnet options and general terminal functionality
+ G00R00: Optimization ideas on scrolling pixels faster.
+ (SDL Game Programming) For good ideas with Singletons and State Machines.
+ Maze: Testing and bug reports
+ Caphood: Testing and bug reports
+ IceDevil: Testing and bug reports
+ Haliphax: Testing and bug reports
+ HellBeard: Testing and bug reports
