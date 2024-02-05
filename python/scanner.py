import asyncio
from bleak import BleakScanner

async def main():
    devices = await BleakScanner.discover(timeout=30)
    for device in devices:
        print(device)

asyncio.run(main())