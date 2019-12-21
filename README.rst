

.. image:: http://htc.zapto.org:8090/job/EtherTerm/badge/icon?png
    :target: https://htc.zapto.org:8090/job/EtherTerm/

4.0 Demo Branch is now the latest branch work in progress with a rewrite and reintegration of SSH.
===============
You can follow the latest updates and work in the following branches:

- ``master`` - current release branch at 0.4.11-alpha


.. contents:: Table of Contents
   :depth: 3

Introduction
============

.. image:: http://i.imgur.com/0BaGSSq.png
   :alt: Screenshot

**EtherTerm** is a portable terminal emulator and telnet/ssh client for Windows
Linux, BSD, Arm and OSX.  It differs from standard terminal emulators such as xterm and PuTTY
by providing a common telnet and ssh protocol across platforms, with particular
focus on font sets appropriate for connecting to BBS servers using cp437, and
Amiga art.  UTF-8 Font glyphs are planned for the future.

EtherTerm is comparable to programs such as syncterm_ and netrunner_.  Notably:
It is an **SDL2 graphics program** and makes use of full OpenGL/DirectX graphics
acceleration.  This is not a console application.

.. _syncterm: http://syncterm.bbsdev.net/
.. _netrunner: http://www.mysticbbs.com/downloads.html

Technical Notes
============
The removal of Boost libs will be replaced with standard **C++11** functionally instead.  The build and project files are currently in the process of being rewritten and separated in folders by environment/os.

The previous 3.0 Demo branch is a fork and rewrite of the internals for EtherTerm.  This version differs a bit as asynchronous sockets were added along with the ability to open several connections in their own windows.  Other telnet option fixes and updates to the terminal have also been added along with some pre-testing of file upload/downloads.  Still very raw (Dialing Directory is still unfinished.  This more of a POC (Proof of concept) rewrite to see what can be done differently.   Also some rendering changes were made and are in testing such as recolor blocks using alpha channels for a slight speed up.  ``Note`` the 3.x branch had some experimental stuff I was testing,  right now I've had to roll back several commits and some things are disabled until I get time to fix and re-implement them.  Mainly input fields for SSH Connections.

**Please keep in mind this is Alpha Software and is very much a work in progress.**

Project details
===============

EtherTerm is Open Source Software, (c) 2014-2019 Michael Griffin <mrmisticismo@hotmail.com>

.. image:: https://imgur.com/H3auoEE.png
   :alt: Screenshot


Currently, no *auto-configure* environment is provided, and build targets are
limited to **Windows**, **Linux**, **ARM**, **FreeBSD** and **OSX**.

EtherTerm is **Alpha** software.  Please
report any issues or feature requests as a `github issue`_.  Pull requests and contributions are always welcome.

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

Key Mapping
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


Download Windows (Demo Previews)
=========

- `Windows 0.4.11 (Latest) Preview <https://drive.google.com/file/d/1PA09I1TuVIFL5fCh_RBY15Z_gn8_yTZg/view?usp=sharing>`_

- `Windows 0.4.10 (Latest) Preview <https://drive.google.com/file/d/1_YJXR0TOjlJi6tm_pOq2DmgwxScC7hI6/view?usp=sharing>`_

- `Windows 0.4.6 Preview <https://drive.google.com/file/d/1FT94kn7UY1EZ_qy5ICbTqCj2-Dwm290N/view?usp=sharing>`_

- `Windows 0.3.1 Preview <https://drive.google.com/file/d/0ByPPRlSSb7N-dGpGXzV0a3VIN2c/view?usp=sharing>`_

- `Windows 0.2.8 Preview <https://drive.google.com/file/d/0ByPPRlSSb7N-V2c2bzFUbnRScDQ/view?usp=sharing>`_



For all other systems, you must build from the project folder.

Building (Windows, Linux, BSD, OSX)
========

Generally, the following libraries must be installed:

- sdl2 
- libssh (Requires OpenSSL, Zlib)
- yaml-cpp

As well as a recent version of gnu/gcc providing g++, or clang.
``GCC/G++ version 5.x and above are required along with -std=c++11``

- I highly reccomend and IDE such as - `CodeLite <https://www.codelite.org>`_
- Makefiles generated from Codelite are available in the project folders for building on the commandline.
- **FreeBSD 11 and OSX High Sierra have been tested with latest version of clang LLVM**
- For Windows, mingw32 or 64 is required.

**NOTE**: EtherTerm will seek an ``./assets`` folder relative to the executable.  In this way,
  it is not (yet) a very "portable" executable.  At this stage, a build folder has been created with quick scripts that will copy from the DEBUG folder to the build folder where the assets is now located by default.


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

License
=======

EtherTerm is available under the zlib license :
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
