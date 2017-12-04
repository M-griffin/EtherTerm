
4.0 Demo Branch is now the latest branch work in progress with a rewrite and reintegration of SSH.
===============
The removal of Boost libs will be replaced with standard c++ functionaly instead.  The build and project files are currently in the process of rewritten and seperated in folders by environment/os. 


The previous 3.0 Demo branch is a fork and rewrite of the internals for EtherTerm.  This version differs a bit as asynchronous sockets were added along with the ability to open several connections at the same each in their own window.  Other telnet option fixes and updated to the terminal have also been added along with some pre-testing of file upload/downloads.  Still very raw (Dialing Directory is still unfinished, and SSH support is disconnected at the moment).  This more of a POC (Proof of concept) rewrite to see what can be done differently.   Also some rendering changes were made and are in testing such as recolor blocks using alpha channels for a slight speed up.  Although some kinks need to be worked out on some background / foreground color swaps, this isn't 100% yet.  Blinking cursor code has also been improved however some issues the color of the cursor following the color of the current x/y position on the screen due to using Alpha channels for quick color changes.


.. contents:: Table of Contents
   :depth: 3

Introduction
============

.. image:: http://i.imgur.com/0BaGSSq.png
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

EtherTerm is Open Source Software, (c) 2014-2017 Michael Griffin <mrmisticismo@hotmail.com>

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
as used in Syncterm as **ANSI**, **VT100**, **LINUX** and **SCO** key mappings
are also available.

Copy/Paste
----------

Copy and paste is comparable to X11: ``Left mouse click and drag`` to
select text on the screen, release to copy selected text to the clipboard.
``Right mouse click`` to paste text from the clipboard.

Dialing directory
-----------------

To add new systems or remove, you must edit the ``assets/dialdirectory.xml`` file.


Downloads (Demo Previews)
=========

- `Windows 3.1 Preview <https://drive.google.com/file/d/0ByPPRlSSb7N-dGpGXzV0a3VIN2c/view?usp=sharing>`_

- `Windows 2.8 Preview <https://drive.google.com/file/d/0ByPPRlSSb7N-V2c2bzFUbnRScDQ/view?usp=sharing>`_



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
- ftp client support.
- utf-8 encoding.

Acknowledgments and Thanks
==========================

- `dingo <https://github.com/jquast>`_: Telnet options and general terminal functionality.
- `g00r00 <http://www.mysticbbs.com/>`_: Optimization ideas on scrolling pixels faster.
- `SDL Game Development <http://www.amazon.com/SDL-Game-Development-Shaun-Mitchell/dp/1849696829>`_.
- `maze <https://github.com/tehmaze>`_: Testing and bug reports.
- `Caphood <http://www.reddit.com/user/Caphood>`_: Testing and bug reports.
- IceDevil: Testing and bug reports.
- `haliphax <https://github.com/haliphax>`_: Testing and bug reports.
- `hellbeard <https://github.com/ericolito>`_: Testing and bug reports.
