"""
Disconnect callback
-------------------

An example showing how the `set_disconnected_callback` can be used on BlueZ backend.

Updated on 2019-09-07 by hbldh <henrik.blidh@gmail.com>

"""

import asyncio
import platform
import logging
import coloredlogs
import verboselogs

from bleak import BleakClient, BleakScanner, discover
from bleak.exc import BleakError


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

    device = await BleakScanner.find_device_by_address(ADDRESS, timeout=20.0)
    if not device:
        raise BleakError(f"A device with address {ADDRESS} could not be found.")

    async with BleakClient(
        device, disconnected_callback=disconnected_callback
    ) as client:
        logger.info(f'Subscribing to notifications for characteristic with uuid: {CHARACTERISTIC_UUID}')
        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)

        print("Sleeping until device disconnects...")
        await disconnected_event.wait()
        print("Connected:", client.is_connected)


if __name__ == "__main__":
    asyncio.run(main())