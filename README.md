NewWolfLives
============

A fork resurrecting NewWolf (Wolfenstein 3D for OpenGL on Windows)

NewWolf was last updated in 2006 (version 0.73).  As I have not heard
back from the author and have some fixes and ideas for improvements,
I'm creating this fork for those who want a more modern (although
perhaps less "authentic") Wolfenstein 3D experience.  With the code on
GitHub, it will be easy for anyone else to fork in case I also
disappear!

Improvements over NewWolf
-------------------------
* Added 1680x1050 video mode, suggestions on other modes are welcome.
* Enable use of "fov" cvar values greater than 75 without some parts
  of the sides of the view being left black (not drawn).
* Add a crosshair, enabled by default, to address the complaint that
  it is hard to aim when using the 3D weapon models.  Set the cvar
  "crosshair_enabled" to 0 to disable it.  The cvars
  "crosshair_outer_radius", "crosshair_inner_radius",
  "crosshair_thickness", "crosshair_alpha" and "crosshair_bonus_scale"
  can be used to customise its appearance.
* Automap enhancements:
  * Whereas the automap was previously either visible or hidden, there
    is now a third possible state: visible but partially transparent.
    The new 'cyclemap' command cycles between these three states,
    whilst the existing 'togglemap' command continues to toggle
    between visible and hidden.  The default binding for the TAB key
    continues to be 'togglemap'.  Change this in the console by typing
    'bind tab cyclemap'.
  * The transparency/alpha level for both the visible and visible but
    partially transparent states can be set individually using the
    "automap_alpha_normal" (default 1.0) and
    "automap_alpha_transparent" (default 0.4) cvars respectively, so
    if you want to toggle between invisible and transparent, without
    the map ever being completely opaque, you can leave TAB bound to
    'togglemap' and set "automap_alpha_normal" to 0.4.
  * To improve transparency of the map, the grey background is no
    longer drawn.  Set the cvar "automap_draw_background" to 1 to
    restore the previous behavior.
  * The automap now aligns the player's location with the center of
    the screen and rotates the automap so that the direction the
    player is facing always corresponds to the top of the screen.  Set
    the "automap_center_player" or "automap_rotate" cvars to 0 to
    disable either of these, restoring the old behavior.
  * The automap now does not show secrets in yellow in the automap.
    If you really want to cheat, set the cvar "automap_secrets" to 1
    to restore the previous behavior.
  * Add a new cvar "automap_actors", which can be set to 1 to show
    actors in the map.  Previously this could be done by hacking the
    code.
* Other debugging tools/cheats:
  * Pass "+set whereforeartthouromero 1" on the command line to cause
    all actors to be spawned with only 1 hit point.
  * The "min_ms_between_frames" cvar can be set to control the minimum
    time in milliseconds between frames, and therefore the maximum
    frame rate/FPS.

Fixes for bugs in NewWolf
-------------------------
* Fix Spear of Destiny final boss levels (maps 18 and 21).
* Fix slow Fake Hitler projectiles (an issue in the original
  Wolfenstein 3D).
* Fix automap bug where objects (most likely actors) were not drawn if
  they were on top of a door.
* Fix problem where player keeps moving forward after releasing the
  forward key if they moved the mouse a lot while moving forward.
* Fix secret push walls to move no more than 2 blocks, not 3, making
  for example the secret area in Wolfenstein 3D E6M2 accessible.

Installation
------------
1. Go to http://newwolf.sourceforge.net/ and install the latest
   NewWolf version (0.73), plus any model and texture packs you'd
   like.  Quick link to NewWolf itself:
   http://sourceforge.net/projects/newwolf/files/NewWolf%20GL/0.73/nwgl_73.zip/download
   That .zip file contains a readme.txt explaining how to run NewWolf.
   Get this working first.  Note that the faq.txt suggests setting the
   screen resolution on the command line, but you can set it in game
   via the "Video Options" menu.  You'll need to do it this way for
   the new video mode(s) added in NewWolfLives as I haven't told you
   what the mode numbers are!
2. Download NewWolfLives from
   https://sourceforge.net/projects/newwolflives/files/releases/,
   unzip the .exe into the same directory as NewWolf.exe, and run it
   exactly as you would run NewWolf.exe.

How to build the sources
------------------------
NewWolfLives uses CMake to generate its Microsoft Visual Studio
solution/project files, which should make it easier to use different
versions of Visual Studio and/or other compilers in the future.  This
means that, compared to NewWolf, there is an extra CMake generation
step required before launching Visual Studio.  This has been tested
with CMake version 2.6:
1. Create a directory in which to perform the build.  This does not
   have to be inside the working copy/source tree.  For example, for
   consistency with NewWolf, create "proj_vs80" at the top level of
   the source tree.
2. Open a Command Prompt in that directory.
3. Run `cmake.exe -G "Visual Studio 8 2005" <path to source
   directory>`.  To follow on from the example above, `cmake.exe -G
   "Visual Studio 8 2005" ..\src`.  Sample output:
```
-- Check for working C compiler: cl
-- Check for working C compiler: cl -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler: cl
-- Check for working CXX compiler: cl -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Configuring done
-- Generating done
-- Build files have been written to: C:/[...]/NewWolfLives/proj_vs80
```

In Visual Studio, you may wish to right-click on "NewWolfLives" in the
**Solution Explorer** and select **Set as StartUp Project** so that
the **Start Debugging** toolbar button works without requiring you to
select an executable file name.

When you perform a build, output appears in the following
sub-directories of the build directory, where CONFIGURATION is
e.g. Debug:
* CONFIGURATION/: .exe file
* NewWolfLives.dir/CONFIGURATION/: .obj files

Links
-----
* http://newwolf.sourceforge.net/ - Original NewWolf, still required.
* https://sourceforge.net/projects/newwolflives/files/releases/ -
  NewWolfLives downloads.
* https://github.com/dcoshea/NewWolfLives - NewWolfLives source code,
  issue tracker, etc.
