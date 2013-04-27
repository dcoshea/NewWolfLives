NewWolfLives
============

A fork resurrecting NewWolf (Wolfenstein 3D for OpenGL on Windows)

NewWolf was last updated in 2006 (version 0.73).  As I have not heard back from the author and have some fixes and ideas for improvements, I'm creating this fork for those who want a more modern (although perhaps less "authentic") Wolfenstein 3D experience.  With the code on GitHub, it will be easy for anyone else to fork in case I also disappear!

Improvements over NewWolf
-------------------------
* Added 1680x1050 video mode, suggestions on other modes are welcome.
* Enable use of "fov" cvar values greater than 75 without some parts
  of the sides of the view being left black (not drawn).
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
  * Fix automap bug where objects (most likely actors) were not drawn
    if they were on top of a door.
* Other debugging tools/cheats:
  * Pass "+set whereforeartthouromero 1" on the command line to cause
    all actors to be spawned with only 1 hit point.
