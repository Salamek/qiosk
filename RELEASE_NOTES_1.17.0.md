# qiosk 1.17.0

This release collects all changes since qiosk 1.13.0.

## Highlights

### Configuration file support

- Added INI configuration support using `/etc/qiosk/qiosk.conf`.
- Added `--config=<file>` for loading a custom configuration file.
- Explicit command-line options override values from the configuration file.
- Added a documented default configuration containing all available settings.

### Window customization

- Added configurable window title and application icon.
- Added configurable initial window size and position.
- New options:
  - `--window-title`
  - `--window-icon`
  - `--window-size`
  - `--window-position`
- Window geometry applies in `windowed` and `automaticvisibility` modes.
- Positioning is best-effort on X11 and may be ignored by Wayland compositors.
- Closes #14 and #21.

### WebSocket control

- Added optional bearer-token authentication.
- Added configurable WebSocket bind address and port.
- New options:
  - `--websocket-bind`
  - `--websocket-port`
  - `--websocket-token`
- Authenticated clients must provide:

  ```http
  Authorization: Bearer <token>
  ```

- Authentication remains disabled when no token is configured.
- The default listener remains `127.0.0.1:1791`.
- WebSocket listener startup failures are now logged.
- Closes #7.

### Connection recovery

- Connection checks now use the target URL origin by default instead of the complete path and query string.
- Added `--connection-test-url=<url>` for selecting a custom probe target.
- Fixed stale-reply races and overlapping connection checks.
- Fixed unexpected periodic page reloads caused by repeating timers.
- Fixed infinite reload loops on HTTP errors such as rate-limit pages.
- Recovery now distinguishes connection, DNS and Chromium network-change errors from HTTP, certificate and other loading failures.
- Chromium network error pages are reloaded once the target becomes reachable.
- Added detailed page-load error logging.

### Chromium renderer recovery

- Renderer crashes now trigger a controlled delayed reload.
- Repeated renderer termination signals are suppressed during recovery.
- Added a temporary red notification bar showing the termination reason and exit code.
- Removed the blocking recovery dialog, keeping qiosk suitable for unattended operation.
- Added a warning when renderer recovery does not finish successfully.

### URL handling

- URLs without a protocol now default to `https://`.
- Host-and-port values such as `localhost:8080` are recognized correctly.
- Plain HTTP URLs must now explicitly include `http://`.

### Idle timer

- Replaced periodic idle polling with deadline-based scheduling.
- Reduced unnecessary timer wakeups.
- User activity now directly reschedules the idle deadline.

### Virtual keyboard

- Fixed virtual keyboard visibility and layout issues.
- Anchored the keyboard panel to the bottom of its container.
- Removed conflicting manual visibility handling.
- Improved keyboard resizing and focused-element scrolling.

### Qt compatibility

- Updated permission handling for Qt 6.8 and newer.
- Retained compatibility with Qt 5 and older Qt 6 releases.
- Resolved related Qt deprecation warnings.

## Other changes

- Disabled the navigation bar by default in the packaged configuration.
- Updated documentation with configuration, window customization and authenticated WebSocket examples.

## Upgrade notes

- WebSocket behavior remains backward-compatible unless a token is configured.
- URLs without a scheme now use HTTPS.
- Window positioning may be ignored under Wayland.
- WebSocket bearer tokens sent over plain `ws://` are not encrypted; use a trusted network, VPN or encrypted tunnel for remote access.
