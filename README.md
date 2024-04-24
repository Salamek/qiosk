# qiosk
Qiosk is a "kiosk first" web browser written in QT as a core part of [chromium-kiosk](https://github.com/Salamek/chromium-kiosk) project.

## Features

* Installed from repository
* Tested on Archlinux, Archlinux ARM, Debian, Raspbian
* Integrated native virtual keyboard
* Integrated whitelist
* Resets to default settings when idle for specified amount of time
* Configurable button bar (Reload, Back, Forward, Home) with options to specify height/width, position
* Configurable address bar (display or not)
* Option to set permissions (camera, geolocation, screen-recording, ignore-ssl, etc.)
* Option to enable/disable scrollbars
* Configurable profile (default=icognito, or different name for normal profile)
* Websocket API for ~remote control
* Uses QT [QWebEngine](https://wiki.qt.io/QtWebEngine) so it is basically build on chromium


## Documentation

For details on how to use this package, check out our [documentation](.docs).

### Q/A

* Q: Why?
* A: Every new version of chromium browser was less and less configurable and usable as kiosk browser (removed CLI options, annoying dialogs, no way to ignore invalid SSL certs, kiosk features only achivable by writing chrome extension and injecting it)



