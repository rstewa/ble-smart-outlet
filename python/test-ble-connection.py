import asyncio
from bleak import BleakClient

async def run(address, loop):
    async with BleakClient(address, loop=loop) as client:
        print(f"Connected: {client.is_connected}")

        services = await client.get_services()
        for service in services:
            print(service)

address = "EFC4216F-5A11-E3FC-1726-B19E1F430A18"
loop = asyncio.get_event_loop()
loop.run_until_complete(run(address, loop))