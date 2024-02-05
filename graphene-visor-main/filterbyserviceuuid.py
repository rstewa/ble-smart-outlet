import asyncio
import sys
import bleak

from bleak import BleakScanner

async def main():
    scanner = bleak.BleakScanner(filters={"UUIDs":["1d93af38-9239-11ea-bb37-0242ac130002"], "DuplicateData":False})
    # scanner.register_detection_callback(detection_callback)
    await scanner.start()
    await asyncio.sleep(3.0)
    await scanner.stop()
    devices = await scanner.get_discovered_devices()

    for device in devices:
        print(device)


if __name__ == "__main__":
    asyncio.run(main())