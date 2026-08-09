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
* WebSocket API for remote control with configurable bind address, port, and optional bearer-token authentication
* Uses QT [QWebEngine](https://wiki.qt.io/QtWebEngine) so it is basically build on chromium
* Option to set User-Agent and Accept-Language HTTP headers (Autodetect if not set)
* INI configuration file support with command-line overrides
* Configurable window title, application icon, size, and position
* Waits for an unavailable target web server and reloads once it becomes reachable. By default the target URL origin is tested; use `--connection-test-url=<url>` to override it.

## Documentation

For details on how to use this package, check out our [documentation](.docs).

By default qiosk loads `/etc/qiosk/qiosk.conf` when it exists. Use
`--config=/path/to/qiosk.conf` to load a different file instead. Explicitly
provided command-line options and the positional URL override file settings.
The packaged default configuration documents the available keys.

### Q/A

* Q: Why?
* A: Every new version of chromium browser was less and less configurable and usable as kiosk browser (removed CLI options, annoying dialogs, no way to ignore invalid SSL certs, kiosk features only achivable by writing chrome extension and injecting it)
