"""
Service Explorer
----------------

An example showing how to access and print out the services, characteristics and
descriptors of a connected GATT server.

Created on 2019-03-25 by hbldh <henrik.blidh@nedomkull.com>

"""

import sys
import platform
import asyncio
import logging

from bleak import BleakClient

logger = logging.getLogger(__name__)

ADDRESS = (
  "24:71:89:cc:09:05"
  if platform.system() != "Darwin"
  else "3A7CF8EC-02F9-3DEA-0915-B12960AA39B4"
)
TEMPERATURE_UUID = "00002a6e-0000-1000-8000-00805f9b34fb"

async def main(address):
  async with BleakClient(address) as client:
    logger.info(f"Connected: {client.is_connected}")

    # temperature characteristic: 00002a6e-0000-1000-8000-00805f9b34fb
    temperature = await client.read_gatt_char(TEMPERATURE_UUID)


if __name__ == "__main__":
  logging.basicConfig(level=logging.INFO)
  asyncio.run(main(sys.argv[1] if len(sys.argv) == 2 else ADDRESS))
