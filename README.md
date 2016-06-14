sh2proxy
========

**sh2proxy is no longer under active development, I recommend you use ThirteenAG and Aero_'s Widescreen Fixes Pack instead (which includes the fixes from sh2proxy along with some others) as it's more actively supported:** https://thirteenag.github.io/wfp#sh2

A simple D3D8 wrapper and SH2 (Directors Cut) patcher, with source code included.

Thanks to Michael Koch for his open source DX8 wrapper
(http://www.codeguru.com/cpp/g-m/directx/directx8/article.php/c11453/Intercept-Calls-to-DirectX-with-a-Proxy-DLL.htm)

This repo contains code for the proxy, most users will want the download below.

DOWNLOAD FROM THE RELEASES SECTION: https://github.com/emoose/sh2proxy/releases

Experimental SweetFX version can be found at https://github.com/emoose/sh2proxy/issues/2#issuecomment-71128479

Additional fix for white textures (credits to Sonne170, I'll add this into the next sh2proxy release, whenever that happens): https://github.com/emoose/sh2proxy/issues/4#issuecomment-76407335

Install
-------

Binary download is available in the releases section (https://github.com/emoose/sh2proxy/releases), includes sh2proxy.ini, precompiled d3d8.dll and a matching sh2pc.exe for sh2proxy.

Visual C++ 2013 redistributable is required, download vcredist_x86.exe from

http://www.microsoft.com/en-gb/download/details.aspx?id=40784


1. Backup your sh2pc.exe file
2. Remove/uninstall any other SH2 modern compatibility fixes
3. Copy contents of bin folder to your SH2 install
4. Make sure the new sh2pc.exe file is 6,123,520 bytes
5. Edit sh2proxy.ini with your own settings
6. Run sh2pc.exe and enjoy!

Again, make sure any other compatibility fixes are removed to make sure they don't collide with this patcher!


If you have problems with SH2 changing your screen resolution to 640x480 when it first opens try running the following as admin

sdbinst -u -g {d2026473-6410-4150-9291-523b55495056}


If you installed any other custom SDB compatibility files you should also remove them with

sdbinst -u pathtoSDBfile


Usage
-------
Easy mode: edit the ini file to your settings and run sh2pc.exe


Hard mode: You can now add parameters to the exe

- -width=xxxx / -w=xxxx		change screen width to xxxx
- -height=xxxx / -h=xxxx		change screen height to xxxx
- -windowed / -window / -w	force game to run in window
- -fullscreen / -fs / -f		force game to run fullscreen
- -borderless / -bl		force game to run borderless (only takes effect if game is windowed)
- -borders / -b			force game to run with borders (only takes effect if game is windowed)
- -x=xxxx				set window x position
- -y=xxxx				set window y position

To use them just run sh2pc.exe with the parameters you want, e.g.

sh2pc.exe -w=2560 -h=1440 -f

To run the game in fullscreen 2560x1440


If no ini is found it'll run the game as default with all the fixes listed below enabled, parameters will still work fine though


Fixes
-------
Allows:
- Custom resolutions
- Windowed mode
- Borderless window
- Window positioning
- Executable Arguments/parameters

Fixes:
- Disables Safe Mode
- Improve transition speed (opening up inventory/map/etc)
- Automatically set processor affinity to only use a single core
- Create local.fix AA fix file if doesn't exist (thanks to angular graphics for the fix!)

All of the above can be configured in sh2proxy.ini

OS X Support
-------
OS X does not export any API for reliably controlling thread affinity. Nevertheless, there is a workaround:
1. Build and install [ThreadBinder](https://github.com/07151129/ThreadBinder) kernel extension.
2. Patch wine using `wine_osx_affinity.patch`. It was tested with version 1.9.11, but should work with any reasonable modern one.

After this, the game's threads should be bound to a single processor. You can observe the result by looking at wine's CPU usage:
it should never exceed 100%.

Example INI
-------
<pre>
[Video]
# width of screen buffer (default 1280)
Width = 2560
# height of screen buffer (default 720)
Height = 1440
# set to 1 to make the game run in a window (default 0)
Windowed = 1

[Window]
# set to 1 to make the window borderless (only takes effect if windowed is enabled)
# borderless windows can't be moved normally, so set the position below
# (default 0)
Borderless = 1

# Force borderless even on fullscreen (Windows 10 Fix)
ForceBorderless = 1

# works with bordered windows too
# (default 0)
PositionX = 0
PositionY = 0

[Patches]
# forces SH2 to use one core (default 1)
SingleCoreAffinity = 1

# disable SH2 gfx safe mode (default 1)
DisableSafeMode = 1

# improve speed of transitions (opening up inventory/map/etc) (default 1)
FastTransitions = 1

# transition width/height (only used internally)
# the game normally uses your screen res, which slows transitions a lot (3-4s transition time for no reason at all!)
# if set to 0 it'll use half your screen res
# in case you still experience transition lag try setting this to a sensible value, 1280x720 is a good choice
# (default 0)
FastTransitionWidth = 800
FastTransitionHeight = 600

[Extra]
# automatically create local.fix file with angular graphics local.fix file
# should fix AA on most systems
# (default 1)
CreateLocalFix = 0
</pre>
