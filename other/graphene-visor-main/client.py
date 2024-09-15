# references:
#   https://github.com/hbldh/bleak/blob/master/examples/enable_notifications.py
#   https://ladvien.com/arduino-nano-33-bluetooth-low-energy-setup/

import asyncio
import platform
import logging
from tkinter import W
import coloredlogs
import verboselogs

from bleak import BleakClient
from aiorun import run


verboselogs.install()
logger = logging.getLogger(__name__)
coloredlogs.install(logger=logger, fmt='%(asctime)s %(name)s[%(process)d] %(levelname)s %(message)s')


DEGREE_SIGN = u'\N{DEGREE SIGN}'
CHARACTERISTIC_UUID = "00002a6e-0000-1000-8000-00805f9b34fb"
ADDRESS = (
    "cc:79:98:7e:6a:ae"
    if platform.system() != "Darwin"
    else "3A7CF8EC-02F9-3DEA-0915-B12960AA39B4"
)


def notification_handler(sender, data):
    temperature = int.from_bytes(data, byteorder="little", signed=True)
    # print(f'temperature: {temperature}{DEGREE_SIGN}C')
    print(temperature)


async def main():
    disconnected_event = asyncio.Event()

    def disconnected_callback(client):
        print("Disconnected callback called!")
        disconnected_event.set()
        asyncio.get_event_loop().stop()

    try:
        logger.info(f'Attempting to connect to address: {ADDRESS}')
        async with BleakClient(ADDRESS, disconnected_callback=disconnected_callback) as client:
            logger.success(f'Connected: {client.is_connected}')
            logger.info(f'Subscribing to notifications for characteristic with uuid: {CHARACTERISTIC_UUID}')
            await client.start_notify(CHARACTERISTIC_UUID, notification_handler) # subscribe to temperature characteristic

            await disconnected_event.wait()
            print("Connected:", client.is_connected)

    except asyncio.CancelledError:
        logger.info(f'Disconnecting from address: {ADDRESS}')
        await client.stop_notify(CHARACTERISTIC_UUID)
        await client.disconnect()
    


if __name__ == "__main__":
    # TODO: add user abillty to connect & disconnect from characterstic and service -> await client.stop_notify(CHARACTERISTIC_UUID)
    run(main())
