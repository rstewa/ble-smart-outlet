import random
import asyncio

from random import randrange
from datetime import datetime
from aiorun import run

async def main():
    while True:
        print(randrange(30, 36))
        await asyncio.sleep(0.25)

if __name__ == '__main__':
    random.seed(999)
    run(main())
