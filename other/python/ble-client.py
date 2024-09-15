# Author: Ryan Stewart
# Date: 01/05/24

import asyncio
import sys

from bleak import BleakClient

# These values have been randomly generated - they must match between the Central and Peripheral devices
# Any changes you make here must be suitably made in the Arduino program as well

ADDRESS = 'cc:79:98:7e:6a:ae'
DEVICE_ID = 'EFC4216F-5A11-E3FC-1726-B19E1F430A18'
SERVICE_ID = '19b10000-e8f2-537e-4f6c-d104768a1214'
CHARACTERISTIC_UUID = '19b10001-e8f2-537e-4f6c-d104768a1214'

on_value = bytearray([0x01])
off_value = bytearray([0x00])

async def write_to_characteristic(client, characteristic,  value):
    await client.write_gatt_char(characteristic, value, response=False)

async def main(address):
    async with BleakClient(address) as client:
        print(f'Connected: {client.is_connected}')
        # print(f'Connected to {DEVICE_ID}')
        val = await client.read_gatt_char(CHARACTERISTIC_UUID)
        print(f'Original Value: {val}')

        # if (val == on_value):
        #     print ('RED ON')
        #     RED = True
        # else:
        #     print ('RED OFF')
        #     RED = False

        service = next(
            (s for s in client.services if s.uuid == SERVICE_ID), None)
        print(f'service uuid: {service.uuid}')

        characteristic = next(
            (c for c in service.characteristics if c.uuid == CHARACTERISTIC_UUID))
        print(f'characteristic uuid: {characteristic.uuid}')

        await client.write_gatt_char(characteristic, off_value)
        val = await client.read_gatt_char(characteristic)
        print(f'\nNew Value: {val}')

        while True:
            val = input('Enter start or stop: ')

            if ('start' in val):
                asyncio.ensure_future(write_to_characteristic(client, characteristic, on_value))
                # await client.write_gatt_char(characteristic, on_value, response=False)
            if ('stop'in val):
                asyncio.ensure_future(write_to_characteristic(client, characteristic, off_value))
                # await client.write_gatt_char(characteristic, off_value, response=False)

            val = await client.read_gatt_char(characteristic)
            print(f'Value: {val}')


if __name__ == "__main__":
    asyncio.run(main(sys.argv[1] if len(sys.argv) == 2 else ADDRESS))

# loop = asyncio.get_event_loop()
# try:
#     loop.run_until_complete(run())
# except KeyboardInterrupt:
#     print('\nReceived Keyboard Interrupt')
# finally:
#     print('Program finished')
