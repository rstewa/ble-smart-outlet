# source code based on: https://ladvien.com/python-serial-terminal-with-arduino-and-bleak/

import os, sys
import asyncio
import platform
from datetime import datetime
from typing import Callable, Any

from bleak import BleakClient, discover

DEGREE_SIGN = u'\N{DEGREE SIGN}'

class Connection:
    client: BleakClient = None
    
    def __init__(
        self,
        loop: asyncio.AbstractEventLoop,
        address,
        read_characteristic: str,
    ):
        self.loop = loop
        self.address = address
        self.read_characteristic = read_characteristic
        self.last_packet_time = datetime.now()
        self.connected = False
        self.connected_device = None

    def on_disconnect(self, client: BleakClient):
        self.connected = False
        print(f"Disconnected from {self.address}")

    async def cleanup(self):
        if self.client:
            await self.client.stop_notify(read_characteristic)
            await self.client.disconnect()

    async def manager(self):
        print("Starting connection manager.")
        while True:
            if self.client:
                await self.connect()
            else:
                self.client = BleakClient(self.address)

    async def connect(self):
        if self.connected:
            return
        try:
            await self.client.connect()
            self.connected = self.client.is_connected
            if self.connected:
                print(F"Connected to {self.address}")
                self.client.set_disconnected_callback(self.on_disconnect)
                await self.client.start_notify(self.read_characteristic, self.notification_handler)
                while True:
                    if not self.connected:
                        break
                    await asyncio.sleep(3.0)
            else:
                print(f"Failed to connect to {self.address}")
        except Exception as e:
            print(e)

    def notification_handler(self, sender: str, data: Any):
        temperature = int.from_bytes(data, byteorder="little", signed=True)
        # print(f'temperature: {temperature}{DEGREE_SIGN}C')
        print(temperature)


#############
# Loops
#############


async def main():
    while True:
        # YOUR APP CODE WOULD GO HERE.
        await asyncio.sleep(5)


#############
# App Main
#############

# 3A7CF8EC-02F9-3DEA-0915-B12960AA39B4 RSSI: -39 AdvertisementData(local_name='TemperatureMonitor', service_uuids=['0000181a-0000-1000-8000-00805f9b34fb'])

# read_characteristic = "0000181a-0000-1000-8000-00805f9b34fb"  # temperature characteristic uuid
read_characteristic = "00002a6e-0000-1000-8000-00805f9b34fb"  # temperature characteristic uuid
ADDRESS = (
    "cc:79:98:7e:6a:ae"
    if platform.system() != "Darwin"
    else "3A7CF8EC-02F9-3DEA-0915-B12960AA39B4"
)

if __name__ == "__main__":
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    # loop = asyncio.get_event_loop()
    address = ADDRESS

    connection = Connection(
        loop, address, read_characteristic
    )
    try:
        asyncio.ensure_future(connection.manager())
        asyncio.ensure_future(main())
        loop.run_forever()
    except KeyboardInterrupt:
        print()
        print("User stopped program.")
    finally:
        loop.run_until_complete(connection.cleanup())
