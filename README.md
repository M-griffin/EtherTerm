## EtherTerm (c) 2014-2015 Michael Griffin <mrmisticismo@hotmail.com>
### An SDL2 Terminal Telnet/SSH program for Windows, Linux & OSX

**Alpha Stage 1**

<center>
![alt text](https://dl.dropboxusercontent.com/u/92792939/EtherTerm.png "EtherTerm")
</center>

The program is developed mainly in Windows using the (Codelite IDE) with
mingw32/64 and will compile in OSX, and Linux under GNU g++.

**This is a full graphics program just like a game engine, it's not a
console application and best runs on more recent hardware.**

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

Text selection is almost finished. You can left click and drag the mouse around.
to highlight text on the screen. Copying selected text will be completed soon.

Right click will paste text from the clipboard.

```ALT + ENTER``` toggles between different window screen sizes.
This cycles two different window resolutions then goes FULL SCREEN.

## Work in progress:

The Dialing directory is incomplete, Add, Edit, Delete, About etc..
Not all options are setup just yet.

To add new systems or remove, you must edit the dialingdirectory.xml file
in a text editor located in the /assets folder.

If you want to connect to SSH servers, you must supply your username, and password
in the ```dialdirectory.XML``` file since there are no prompts to enter these when
you connect at the moment.

I'm also in the middle of implimenting more control sequence behavior.
I have a lot of XTERM sequences laid out so more will be implemented with time.

## Not yet implemented:

Lots of things are still in the early testing and debugging stages.
There are no transfer protocols at the moment.

Some keyboard keys still need to be implemented like F1 function keys.
All others should work just fine including CTRL Keys.

There is no scroll back buffer, this is also in the works.

FTP will be added too.

Terminal modes, by default EtherTerm only supports 80 x 25.  Other modes will be supported lateron.

***Keymappings are planned also***
Etherterm currently follows the default Windows telnet output keys seqeunces (used in Syncterm) for keys like arrrows, Insert, Delete, PageUp and Page Down.  Some other operating systems look for and use different escape sequence for their input keys which will be added later for more support.

##To compile the following libraries must be pre-installed:

```
SDL2
SDL2_Net
Libssh (Requires OpenSSL, Zlib)
```

**Example Windows (mingw32/64) linking:**
```
Libs: mingw32 or 64, SDL2main, SDL2, SDL2_net, libssh
```

**Example (G++) linking:**
```
libs: libSDL2, libSDL2_net, libssh
```

Must compile with latest c++ environment flags.

At the very least and you can also replace 0x with 11 on
newer compilers. Usually g++ (4.7)+

```
-std=gnu++0x or -std=c++0x
```

#Makefile instructions:

To build windows you must already have a compiler and the requirements.
You can use a good IDE like CodeLite, CodeBlocks, or Visual Studio (more headache).
You then must installs the libs and setup the include paths.

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

**For Linux**
```
make linux

make clean-linux
```

##Linux Build Steps:

**For Ubunutu, Mint and Debian you can use APT-GET to install all requirements.**
Notes for Arch Linux will follow shortly once steps are completed.

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
make-linux 
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
installs libssh /usr/local/Cellar/libssh6.4

**To compile your executable.**
```
make-osx
```

**Possible issues with OSX Yosemite when building libssh.**
The default compiler for OSX seems to be LLVM clang.  I use GNU g++ in most of my development and there is and
error that has been corrected when compiling with GNU g++ below. 

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
+ IceDevil: Testing and bug reports
