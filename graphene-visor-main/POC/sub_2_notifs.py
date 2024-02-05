import sys
import asyncio
import platform

from bleak import BleakClient
from aiorun import run


DEGREE_SIGN = u'\N{DEGREE SIGN}'
CHARACTERISTIC_UUID = "00002a6e-0000-1000-8000-00805f9b34fb"
ADDRESS = (
    "cc:79:98:7e:6a:ae"
    if platform.system() != "Darwin"
    else "3A7CF8EC-02F9-3DEA-0915-B12960AA39B4"
)


def notification_handler(sender, data):
    temperature = int.from_bytes(data, byteorder="little", signed=True)
    print(f'temperature: {temperature}{DEGREE_SIGN}C')
    # print(temperature)


# async def main(address, char_uuid):
async def main():
    try:
        print(f'Attempting to connect to address: {ADDRESS}')
        async with BleakClient(ADDRESS) as client:
            print(f"Connected: {client.is_connected}")
            await client.start_notify(CHARACTERISTIC_UUID, notification_handler) # subscribe to temperature characteristic
            while True:
                if not client.is_connected: # break the loop if we diconnect
                    break
                await asyncio.sleep(1.0)

    except asyncio.CancelledError:
        print(f'disconnected...')
        await client.stop_notify(CHARACTERISTIC_UUID)


if __name__ == "__main__":
    run(main())
