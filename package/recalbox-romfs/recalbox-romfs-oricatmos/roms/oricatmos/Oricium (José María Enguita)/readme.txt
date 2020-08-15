=======================================
	      Oricium
    	   The Oric Game
=======================================

Website: oricium.defence-force.org.
Forum: forum.defence-force.org - "Games" forum.

Version 1.2
August 2014

José María (Chema) Enguita
Email: enguita@gmail.com

This file should be distributed alongside with the game (in whatever format,
tap, ort or dsk) *and including* the instructions contained in Oricium.pdf.


INTRODUCTION
============

IMPORTANT: Read the instructions in the included pdf!

Oricium is a fast, lateral scrolling shoot-em-up with a touch of puzzle
game. Its difficulty curve is smooth, so it is easy to go through the
first levels. However it quickly becomes challenging, so don't judge it just
after playing 5 minutes.

Oricium started as an experiment to see if it was possible to produce a fast 
scrolling shoot-em-up game, including details such as masked sprites, animations, 
pseudo-paralax scrolling, many enemies, fast action, etc. 

Those specs seem at a first glance, something impossible for an Oric computer. 
At least they seemed so to me! Of course the first inspiration came from the 
well-known Andrew Braybrook’s game Uridium (en.wikipedia.org/wiki/Uridium), 
but soon the gameplay departed from the original.

Oricium is quite a complex game, including (for the first time) synchronization
with the screen refresh, and procedural generation for the dreadnaughts and other
scenes. That is why it has the amazing number of 42 different levels. 

I also made a huge effort in many small details (graphics, music, sfx, animations…) 
to make the game look professional and also make the player smile from time to 
time if possible.

I also wanted this game to be tape-based, so everybody could enjoy it in their real
machines. This is, as you may know, quite a big restriction! Fortunately, with the 
appearance of Cumulus and all the Microdisc controller clones, more and more people 
are able to use disks, so this might be the last time I develop a tape-based game.

Any thoughts, comments, questions, bugs or whatever, are more than welcome. 
Post them in the Oric Forums (forum.defence-force.org) (there is a forum for games)
or email me at enguita@gmail.com. I will try to solve them as soon as possible.


LOADING ORICIUM
===============

Oricium will work on any 48K Oric-1, Atmos and Pravetz 8D. It should also work 
in a Telestrat, though this has not been fully tested.

As Oricium runs synchronized with the vertical screen retrace, you'll get
the best experience by playing it on a real machine and a TV. 

If you use an emulator please be sure to use one with support of full raster 
emulation in the video code, such as Oricutron.

Oricium also supports the mod that takes the VSync signal from the RGB port and 
feeds it through the Tape-in pin in the tape connector (this mod is also emulated
in Oricutron). 

This idea first appeared at Fabrice Frances’ site:
http://oric.free.fr/hardware.html.

Again, read the instructions for information about how to use this mod, or 
manually calibrate the vertical retrace timing.

If you want to load Oricium on a real Oric, just convert it to wav
first, using tap2wav or tap2cd (Check for those on Fabrice France's site
and read the instructions). A pregenerated ultrafast loading wav file is
included (works only on ATMOS).

If you have a disk system, use the included disk image file (dsk).


INSTRUCTIONS
============

Read the manual. It is not *that* big.



REPORTING BUGS & COMMENTS
=========================

Any thoughts, comments, questions, bugs or whatever, are more than welcome. 
Post them in the Oric Forums (forum.defence-force.org) or email me at 
enguita@gmail.com


CREDITS
=======

Thanks a lot to Dbug for creating the astonishing loading screen in just a couple of days!

This game would have never seen the light without the help of the Oric community.

Thanks to Dbug, Symoon, Godzil, Greymagick and Fabrice for the tests of the Alpha 
versions and for providing insights and ideas, as well as always giving me energy 
to complete this game.

Thanks too to the people in RetroWiki (www.retrowiki.es) for their help, support 
and nice comments. Specially to Silicebit for his counter routine (and also for this 
great Microdisc controller clone!), and dancresp and ron for their feedback and 
courage during the beta testing stage.

And also thanks to Twilighte, as he was always a source of inspiration. 
Rest in peace; we’ll always remember you.


CHANGELOG
=========

Version 1.2
------------
Included the patches from version 1.1. Also added a "PRESS A KEY" message after loading and
while the presentation screen is visible, giving time to the user to stop the player. This
solved an issue where some players were creating sound artifacts after loading the game 
in ultra fast mode which were, in turn, confused with the pressence of the VSync hack. 


Version 1.1
------------
Symoon's work: 
WIP version hacked so it loads on Oric-1. Also version 1.0 tap file had some different
headers in the tap files which, though valid, were creating problems in Euphoric and 
similar emulators. This has also been solved. 



Version 1.0
------------
Initial version (pre-release). 

We are aware of an issue when loading from tape on 
an Oric-1. After loading the presentation screen the Oric return to the prompt 
(though it is not visible, as ink and paper are both black). 

There may be some workarounds, but the easiest one is: stop the tape player,
type (blindly) CLOAD"" <RETURN> and restart the tape so it continues loading.

Will be solved in the next release.


Please report any further bug that you encounter (in the forums or directly 
emailing me at enguita@gmail.com). I will try to fix them as soon as possible

Happy playing!!!

-- Chema


