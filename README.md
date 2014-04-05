sh2proxy
========

A simple D3D8 wrapper and SH2 patcher, with source code included.

Thanks to Michael Koch for his open source DX8 wrapper
(http://www.codeguru.com/cpp/g-m/directx/directx8/article.php/c11453/Intercept-Calls-to-DirectX-with-a-Proxy-DLL.htm)

-------
Install
-------
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

-------
Usage
-------
Easy mode: edit the ini file to your settings and run sh2pc.exe

Hard mode: You can now add parameters to the exe

-width=xxxx / -w=xxxx		change screen width to xxxx
-height=xxxx / -h=xxxx		change screen height to xxxx
-windowed / -window / -w	force game to run in window
-fullscreen / -fs / -f		force game to run fullscreen
-borderless / -bl		force game to run borderless (only takes effect if game is windowed)
-borders / -b			force game to run with borders (only takes effect if game is windowed)
-x=xxxx				set window x position
-y=xxxx				set window y position

To use them just run sh2pc.exe with the parameters you want, e.g.
sh2pc.exe -w=2560 -h=1440 -f
To run the game in fullscreen 2560x1440

If no ini is found it'll run the game as default with all the fixes listed below enabled, parameters will still work fine though

-------
Fixes
-------
Allows:
- Custom resolutions
- Windowed mode
- Borderless window
- Window positioning
- Executable Arguments/Parameters

Fixes:
- Disables Safe Mode
- Improve transition speed (opening up inventory/map/etc)
- Automatically set processor affinity to only use a single core
- Create local.fix AA fix file if doesn't exist (thanks to angular graphics for the fix!)

All of the above can be configured in sh2proxy.ini