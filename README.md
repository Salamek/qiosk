# qiosk
Qiosk is a "kiosk first" web browser written in QT as a core part of [chromium-kiosk](https://github.com/Salamek/chromium-kiosk) project.

## Features

* Installed from repository
* Tested on Archlinux, Archlinux ARM, Debian, Raspbian
* Integrated native virtual keyboard
* Integrated whitelist
* Resets to default settings when idle for specified amount of time
* Configurable button bar with options to specify height/width, position
* Uses QT [QWebEngine](https://wiki.qt.io/QtWebEngine) so it is basically build on chromium

## Installation

### Archlinux
(Use Archlinux ARM for Raspberry install)

Add repository by adding this at end of file /etc/pacman.conf

```
[salamek]
Server = https://repository.salamek.cz/arch/pub/$arch
SigLevel = Optional
```

and then install by running

```bash
$ pacman -Sy qiosk
```

## Debian and derivates
(For Raspbian i suggest to use Lite relase)

Add repository by running these commands

```bash
$ wget -O- https://repository.salamek.cz/deb/salamek.gpg | sudo tee /usr/share/keyrings/salamek-archive-keyring.gpg
$ echo "deb     [signed-by=/usr/share/keyrings/salamek-archive-keyring.gpg] https://repository.salamek.cz/deb/pub all main" | sudo tee /etc/apt/sources.list.d/salamek.cz.list
```

And then you can install a package `qiosk`

```bash
$ apt update && apt install qiosk
```

## Usage

Just call qiosk from CLI like this:

```
qiosk https://github.com
```

More options:

```
Kiosk browser written in QT

Options:
  -h, --help                                         Displays help on
                                                     commandline options.
  --help-all                                         Displays help including Qt
                                                     specific options.
  -v, --version                                      Displays version
                                                     information.
  -f, --fullscreen                                   Run browser in fullscreen
                                                     mode.
  -i, --idle <seconds>                               Timeout to reset
  -w, --white-list <string>                          Whitelist with glob syntax
  --navbar-vertical-position <top|bottom>            Navbar vertical position
  --navbar-horizontal-position <left|right|center>   Navbar horizontal position
  --navbar-width <%>                                 Navbar width
  --navbar-height <%>                                Navbar height
  -a, --allow-feature <desktop-audio-video-capture|d Allow feature
  esktop-video-capture|geolocation|invalid-certifica
  te|media-audio-capture|media-audio-video-capture|m
  edia-video-capture|mouse-lock|notifications>
  --display-navbar                                   Display navbar with
                                                     navigation buttons.
  --underlay-navbar                                  Display navbar under the
                                                     webview.
  --display-addressbar                               Display address bar.

Arguments:
  url                                                URL to display.
```

### Q/A

* Q: Why?
* A: Every new version of chromium browser was less and less configurable and usable as kiosk browser (removed CLI options, annoying dialogs, no way to ignore invalid SSL certs, kiosk features only achivable by writing chrome extension and injecting it)



