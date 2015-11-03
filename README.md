THE AS OF YET NOT OFFICIALLY NAMED LAUNCHER FOR IOQ3
====================================================

The launcher for ioquake3. Built with QT, written in C++. 

This project's goal as an updater is to get everyone on Windows, OS X, and Linux onto modern versions of ioquake3, and keep them up-to-date in a style similar to other stand-alone launcher/updater systems for games that aren't on major digital-distribution platforms.

As a launcher this project should enable easy access to ioquake3 features that are otherwise buried in wikis, forums, and README files.

Future goals could include the ability to launch an encapsulated mod for quake 3 or standalone games and keep them updated and on the latest versions of ioquake3 as well.

Building
=====
Instructions are available for compiling launch here:
http://wiki.ioquake3.org/Building_launch

ROADMAP
=======

Required features to reach:
---------------------------

Version 0.1 (alpha)

  * launch the ioquake3 program - DONE
  * launch quake3 at different resolutions - DONE
  * download/install patches for quake3 - not yet implemented
  * display EULA before downloading q3 patches - not yet implemented


0.5 (beta)

  * be able to download/install ioquake3
  * be able to copy quake3 data from retail CD
  * be able to update ioquake3
  * be able to update self
  * initial support for other operating systems - STARTED(Linux)

1.0

  * config launch options
  * config player options
  * WYSIWYG name config
  * work with steam/gog installation of quake3
  * backup/save/swap configs
  * support for Linux, Windows, and OS X

2.0

  * support for other games (Tremulous/SmokinGuns/TurtleArena/etc.)
  * automated mod switcher
  * builtin server browser
  * integrated newsfeed
  * LAN support
  * preload mods/maps/content via internet protocols
  * uri integration (q3://, trem://, ioq3://, etc.)
  * rcon interface for remote servers
