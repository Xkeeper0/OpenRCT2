#OpenRCT2 Ex PLUS

This is a fork of OpenRCT2 with some junk. It will probably not be updated
regularly, or at all, and is just here for my amusement.

If you somehow got here by mistake, you should probably go to 
[the original repository](https://github.com/OpenRCT2/OpenRCT2).

#Added junk

* In-game console shows chat history, even if offline
* Signs show what text they have when hovered over
* Park price view shows "suggested guests" and approx. entry value
* Ride price view shows "ride value" for rides (and breaks weird for others)
* Numeric version of peep happy/energy/etc. bars
* Mulitplayer changes:
	- New toolbar button icon based on the main menu
	- Quick reconnect menu option
	- Badge showing sync/desync status
	- Badge showing current player count
	- Sync status window (shows random value and sprite hashes)

#Downloading
Usually available on [AppVeyor](https://ci.appveyor.com/project/Xkeeper0/openrct2).
Look for the latest "ex-plus" build.

#Various notes

`cmake -DCMAKE_BUILD_TYPE=Debug ..`, `DESTDIR=. make install -j 12`

`Imaging.cpp@136`, bad assert
