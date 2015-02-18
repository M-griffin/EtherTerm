.. contents:: Table of Contents
   :depth: 3

Introduction
============

.. image:: https://dl.dropboxusercontent.com/u/92792939/EtherTerm.png
   :alt: Screenshot

EtherTerm is a portable terminal emulator and telnet/ssh client for Windows
and Unix.  It differs from standard terminal emulators such as xterm and PuTTY
by providing a common telnet and ssh protocol across platforms, with particular
focus on font sets appropriate for connecting to BBS servers using cp437, and
amiga art.

EtherTerm is comparable to programs such as syncterm_ and netrunner_.  Notably:
It is an **SDL graphics program** and makes use of full OpenGL/DirectX graphics
acceleration.

.. _syncterm: http://syncterm.bbsdev.net/
.. _netrunner: http://www.mysticbbs.com/downloads.html

Project details
===============

EtherTerm is Public Domain Software, ``(c) 2014-2015 Michael Griffin <mrmisticismo@hotmail.com>``

Currently, no *auto-configure* environment is provided, and build targets are
limited to Windows, Linux, and OSX.  EtherTerm is **Alpha** software.  Please
report any issues or feature requests as a `github issue`_.  Pull requests welcome.

.. _github issue: https://github.com/M-griffin/EtherTerm/issues

Fonts
-----

Each system can be setup with a default font.  Any systems using
`Syncterm escape sequences`_ for font switching will override the
default font.

- VGA-8x16 (CP437)
- MicroKnightPlus-8x16 (AMIGA)
- Mo'soul-8x16 (AMIGA)
- Pot Noodle-8x16 (AMIGA)
- Topaz Plus-8x16 (AMIGA)

.. _Syncterm escape sequences: http://cvs.synchro.net/cgi-bin/viewcvs.cgi/*checkout*/src/conio/cterm.txt

Fullscreen
----------

``ALT + ENTER`` cycles between:

- 1x unscaled window
- 2x scaled window
- scaled fullscreen

Keymapping
----------

Etherterm currently follows the default Windows telnet output keys sequences
as used in Syncterm as ``ANSI``. ``VT100``, ``LINUX`` and ``SCO`` key mappings
are also available.

Copy/Paste
----------

Copy and paste is comparable to X11: ``Left mouse click and drag`` to
select text on the screen, release to copy selected text to the clipboard. 
``Right mouse click`` to paste text from the clipboard.

Dialing directory
-----------------

To add new systems or remove, you must edit the ``assets/dialdirectory.xml`` file.

Downloads
=========

- `Windows 2.3 Preview <https://dl.dropboxusercontent.com/u/92792939/EtherTerm%20Demo2.3.zip>`_

For all other systems, you must build from the project folder.

Building
========

Generally, the following libraries must be installed:

- sdl2 and sdl2_net
- libssh (Requires OpenSSL, Zlib)

As well as a recent version of gnu/gcc providing g++, or clang.

For Windows, mingw32 or 64 is required.

- **NOTE**: EtherTerm will seek an ``./assets`` folder relative to the executable.  In this way,
  it is not (yet) a very "portable" executable.  At this stage, please execute only from the
  project folder.

- ``./EtherTerm`` to run on unix systems, or just ``EtherTerm`` on Windows.

Windows
-------

Must compile with latest c++ environment flags:  At the very least and you can
also replace 0x with 11 on newer compilers. Usually g++ (4.7)+ or compatible
compiler:

```
-std=gnu++0x or -std=c++0x
```

To build windows you must already have a compiler and the requirements.
You can use a good IDE like CodeLite, CodeBlocks, or Visual Studio.
You must install the libs and setup the include paths, like any project.

Makefile(s) are provided for g++ compiles on Windows.

With Windows you will need to setup a command line environment appropriate
for compilation of this projet, or create a build target with the necessry
settings using a project file of your preferred IDE::

    make clean-win
    make win

OSX
---

- install http://brew.sh/
- ``brew install sdl2 sdl2_net libssh``
- ``make clean-osx``
- ``make osx``

Ubuntu, Mint, and Debian Linux
------------------------------

- ``sudo apt-get install build-essential libsdl2-dev libsdl2-net-dev libssh-dev``
- ``make clean-linux``
- ``make linux``

Arch Linux
----------

- ``sudo pacman -S sdl2 sdl2_net extra/libssh``
- ``make clean-linux``
- ``make linux-arch``

Changelog
=========

**v2.3**

- ``ALT + H`` for quick disconnection ("hangup")
- SSH login/password prompts when not specified in ``assets/dialdirectory.xml``
- selection of various input key mappings (VT100, ANSI, etc.)

TODO
====

- "change_scroll_region": csr terminal sequence.
- "alternate screen" buffer: smcup, rmcup terminal sequence.
- scrollback buffer.
- interactively edit the dialing directory.
- other runtime configuration/setup options.
- alternative window sizes, currently limited to only 80x25.
- attempting to reach compliance with terminal type 'xterm-256color'.
- transfer protocols (x, y, and z-modem).
- ssh pubkey authentication.
- ftp client support.
- utf-8 encoding.

Acknowledgments and Thanks
==========================

- `dingo <https://github.com/jquast>`_: Telnet options and general terminal functionality.
- `g00r00 <http://www.mysticbbs.com/>`_: Optimization ideas on scrolling pixels faster.
- `SDL Game Development <http://www.amazon.com/SDL-Game-Development-Shaun-Mitchell/dp/1849696829>`_.
- `maze <https://github.com/tehmaze>`_: Testing and bug reports.
- `Caphood <http://www.reddit.com/user/Caphood>`_: Testing and bug reports.
- `IceDevil: Testing and bug reports.
- `haliphax <https://github.com/haliphax>`_: Testing and bug reports.
- `hellbeard <https://github.com/ericolito>`_: Testing and bug reports.
