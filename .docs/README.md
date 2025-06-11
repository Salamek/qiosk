## Content
- [Installation](#installation)
  - [Debian](#debian)
  - [Archlinux](#archlinux)
  - [Raspberry Pi](#raspberry-pi)
- [Usage](#usage)
- [Tips and tricks](#tips-and-tricks)
  - [Remote control](#remote-control)
  - [Virtual keyboard](#virtual-keyboard)
  - [Passing arguments to underlying chromium](#passing-arguments-to-underlying-chromium)


## Installation

Qiosk can be installed on multiple Linux based distributions, officially supported are Debian 12 and Archlinux other distributions based on Debian may work too.

- **Debian**
- **Archlinux**
- **Raspberry Pi**

### Debian

For Debian there are prepared DEB packages in my repository:

1) Make sure that your system is up-to-date

```bash
apt update && apt upgrade
```

2) Install required dependencies

```bash
apt install wget
```

3) Add GPG key for my repository

```bash
wget -O- https://repository.salamek.cz/deb/salamek.gpg | tee /usr/share/keyrings/salamek-archive-keyring.gpg > /dev/null
```

4) Add repository

```bash
echo "deb     [signed-by=/usr/share/keyrings/salamek-archive-keyring.gpg] https://repository.salamek.cz/deb/pub all main" | tee /etc/apt/sources.list.d/salamek.cz.list
```

5) Update packages info

```bash
apt update
```

6) Install qiosk

```bash
apt install qiosk
```

WARNING: Debian 12 armhf `qt6-webengine` used by QT6 `qiosk` is crashing with `SIGBUS` code but QT5 version works fine, use `qt5-qiosk` (QT5 port of qiosk):

2) Install QT5 version of qiosk
```
apt install qt5-qiosk
```

### Archlinux

For Archlinux there are prepared packages in my repository:

1) Make sure that your system is up-to-date

```bash
pacman -Syu
```

2) Add repository by adding this into your `/etc/pacman.conf`

```ini
[salamek]
Server = https://repository.salamek.cz/arch/pub/$arch
SigLevel = Optional
```

5) Install qiosk

```bash
pacman -Sy qiosk
```

# Usage


Just call qiosk from CLI like this:

```
qiosk https://github.com
```

More options:

```
Usage: qiosk [options] url
Kiosk browser written in QT

Options:
  -h, --help                                         Displays help on
                                                     commandline options.
  --help-all                                         Displays help, including
                                                     generic Qt options.
  -v, --version                                      Displays version
                                                     information.
  -m  --window-mode                                  Set window mode.
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
  --display-scroll-bars                              Display scroll bars.
  --underlay-navbar                                  Display navbar under the
                                                     webview.
  --display-addressbar                               Display address bar.
  --profile-name <Profile name.>                     Profile name, if not
                                                     specified or default,
                                                     default off-record profile
                                                     is used.
  --hide-cursor                                      Hide cursor.
  --navbar-enable-buttons <back,forward,reload,home> Navbar enabled button

Arguments:
  url                                                URL to display.
                                        URL to display.
```


# Tips and tricks

## Remote control

Since 2.4 qiosk can be remote controled via websocket connection on port 1791 binded to localhost


### Commands

#### SetUrl

Sets currently displayed url

```json
{
    "command": "setUrl",
    "data": {
        "url": "https://github.com"
    }
}
```

#### SetWindowMode

Sets window mode, one of "hidden", "automaticvisibility", "windowed", "minimized", "maximized", "fullscreen"

```json
{ 
    "command": "setWindowMode",
    "data": { 
        "windowMode": "fullscreen"
    }
} 
```

#### SetIdleTime

Sets idle time to reload to homePage, idle time is in seconds

```json
{
    "command": "setIdleTime",
    "data": {
        "idleTime": 60
    }
}  
```

#### SetWhiteList

Sets and enables white list, set list of allowed urls or empty to disable

```json
{
    "command": "setWhiteList",
    "data": {
        "whitelist": ["https://github.com/*"]
    }
}    
```

#### SetPermissions(

Sets enabled permissions for browser like access to camera/mic/gps etc...

Permissions are

* `geolocation` Enable geolocation
* `notifications` Enable notifications
* `invalid-certificate` Accept invalid certificates
* `media-audio-capture` Allow audio capture
* `media-video-capture` Allow video capture
* `media-audio-video-capture` Allow audio and video capture
* `mouse-lock` Allows mouse lock in window
* `desktop-video-capture` Allows capturing desktop video (not really usefull for kiosk without one right? :-D)
* `desktop-audio-video-capture` Allows capturing desktop audio and video ^


```json
{
    "command": "setPermissions",
    "data": {
        "permissions": ["geolocation"]
    }
}
```

#### SetNavbarVerticalPosition

Sets navbar veritcal positon:

* `top` Navbar is on top of screen
* `bottom` Navbar is on bottom of screen

```json
{  
    "command": "setNavbarVerticalPosition",
    "data": {
        "navbarVerticalPosition": "bottom"
    }
}  
```

#### SetNavbarHorizontalPosition

Set navbar horizontal position:

* `left` Navbar is on left of screen
* `right` Navbar is on right of screen
* `center` Navbar is on center of screen

```json
{
    "command": "setNavbarHorizontalPosition",
    "data": {
        "navbarHorizontalPosition": "center"
    }
}    
```

#### SetNavbarWidth

Sets nawbar width in %

```json
{
    "command": "setNavbarWidth",
    "data": {
        "navbarWidth": 100
    }  
}    
```

#### SetNavbarHeight

Sets nawbar height in %

```json
{
    "command": "setNavbarHeight",
    "data": {
        "navbarHeight": 5
    }
}      
```  

#### SetDisplayAddressBar

Sets if addess bar is displayed or not

```json
{
    "command": "setDisplayAddressBar",
    "data": {
        "displayAddressBar": true
    }  
}      
``` 

#### SetDisplayNavBar

Sets if navbar is displayed or not

```json
{
    "command": "setDisplayNavBar",
    "data": {
        "displayNavBar": true
    }
}
``` 

#### SetUnderlayNavBar

Sets if navbar is displayed under or over the web content

```json
{
    "command": "setUnderlayNavBar",
    "data": {
        "underlayNavBar": false
    }
}
``` 

#### SetHomePage

Sets home page


```json
{
    "command": "setHomePage",
    "data": {
        "homePageUrl": "https://gitlab.com"
    }
}
``` 


### Events

#### Active event
Triggers when browser becomes active

```json
{
   "event": "active",
   "message": "Browser went to active state"
}
```

#### Idle

Triggers when browser becomes idle

```json
{
    "event": "idle",
    "message": "Browser went to idle state"
}
```

## Virtual keyboard
To enable virtual keyboard you have to have QT virtual keyboard (qt6-virtualkeyboard-plugin) installed and specify envirement variable:
```bash
QT_IM_MODULE=qtvirtualkeyboard qiosk ...
```
or
```bash
set QT_IM_MODULE=qtvirtualkeyboard
qiosk ...
```


## Passing arguments to underlying Chromium
Sometimes there is a need to pass arguments to underlying chromium for options that are not supported by qiosk directly, this can be achieved by:
```
# Disables pinch zoom
QTWEBENGINE_CHROMIUM_FLAGS="--disable-pinch" qiosk <url>
````
(Mostly)All possible options are listed at https://gist.github.com/dodying/34ea4760a699b47825a766051f47d43b or https://peter.sh/experiments/chromium-command-line-switches/ 















