import asyncio
import websockets
import json
import logging
import sys
from docopt import docopt

logging.basicConfig()
log = logging.getLogger(__name__)
log.setLevel(logging.DEBUG)

docopt_config = """
Qiosk command console.

License: GPL-3.0
Website: https://github.com/Salamek/chromium-kiosk

Command details:
    setUrl                 Set displayed url.
    setHomePage            Set homePageUrl.
    setWindowMode          Sets window mode.
    setIdleTime            Set idle time.
    setWhiteList           Set white list items.
    setPermissions         Set permissions.
    setNavbarVerticalPosition  Set navbar vertical position.
    setNavbarHorizontalPosition Set navbar horizontal position.
    setNavbarWidth         Set navbar width.
    setNavbarHeight        Set navbar height.
    setDisplayAddressBar   Set display address bar.
    setDisplayNavBar       Set display navbar.
    setUnderlayNavBar      Set underlay navbar.
    getConfiguration       Returns configuration.
    exit                   Exit the interface.
Usage:
    q setUrl <url>
    q setHomePage <homePageUrl>
    q setWindowMode <windowMode>
    q setIdleTime <idleTime>
    q setWhiteList [<whitelist>...]
    q setPermissions [<permissions>...]
    q setNavbarVerticalPosition <navbarVerticalPosition>
    q setNavbarHorizontalPosition <navbarHorizontalPosition>
    q setNavbarWidth <navbarWidth>
    q setNavbarHeight <navbarHeight>
    q setDisplayAddressBar <displayAddressBar>
    q setDisplayNavBar <displayNavBar>
    q setUnderlayNavBar <underlayNavBar>
    q getConfiguration
    q exit
"""

allowed_commands = [
    'setUrl',
    'setHomePage',
    'setWindowMode',
    'setIdleTime',
    'setWhiteList',
    'setPermissions',
    'setNavbarVerticalPosition',
    'setNavbarHorizontalPosition',
    'setNavbarWidth',
    'setNavbarHeight',
    'setDisplayAddressBar',
    'setDisplayNavBar',
    'setUnderlayNavBar',
    'getConfiguration',
    'exit'
]


async def send_message(websocket: websockets.ClientConnection):
    is_running = True
    while is_running:
        command_parts = input('Enter command: ').split(' ')
        command_options = docopt(docopt_config, command_parts)

        command_name = None
        for allowed_command in allowed_commands:
            if command_options.get(allowed_command, False):
                command_name = allowed_command
                break

        if not command_name:
            raise Exception('Unknown command')

        if command_name == 'exit':
            is_running = False


        data = {}
        for option_name, option_value in command_options.items():
            if option_name.startswith('<') and option_name.endswith('>'):
                if option_value == 'true':
                    option_value = True
                elif option_value == 'false':
                    option_value = False
                elif isinstance(option_value, str):
                    try:
                        option_value = int(option_value)
                    except (TypeError, ValueError):
                        pass

                data[option_name.replace('<', '').replace('>', '')] = option_value

        command = {
            'command': command_name,
            'data': data
        }

        log.debug('Sending')
        log.debug(command)

        await websocket.send(json.dumps(command))
        await asyncio.sleep(1)
    sys.exit(0)


async def receive_message(websocket: websockets.ClientConnection):
    log.info('Receiving')
    while True:
        response = await websocket.recv()
        log.info(response)


async def main():
    uri = "ws://localhost:1791"
    async with websockets.connect(uri) as websocket:
        asyncio.create_task(send_message(websocket))
        asyncio.create_task(receive_message(websocket))
        await asyncio.Future()

asyncio.run(main())
