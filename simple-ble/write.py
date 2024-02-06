import simplepyble

ADDRESS = 'cc:79:98:7e:6a:ae'
SERVICE_UUID = '19b10000-e8f2-537e-4f6c-d104768a1214'
CHARACTERISTIC_UUID = '19b10001-e8f2-537e-4f6c-d104768a1214'

on_value = bytes([int('0')])
off_value = bytes([int('1')])

def main():
    adapters = simplepyble.Adapter.get_adapters()

    if len(adapters) == 0:
        print("No adapters found")

    adapter = adapters[0]

    print(f"Selected adapter: {adapter.identifier()} [{adapter.address()}]")

    adapter.set_callback_on_scan_start(lambda: print("Scan started."))
    adapter.set_callback_on_scan_stop(lambda: print("Scan complete."))
    adapter.set_callback_on_scan_found(None) # lambda peripheral: print(
        # f"Found {peripheral.identifier()} [{peripheral.address()}]"))

    # Scan for 5 seconds
    adapter.scan_for(5000)
    peripherals = adapter.scan_get_results()

    peripheral = next((p for p in peripherals if p.address() == ADDRESS), None)

    print(f"Connecting to: {peripheral.identifier()} [{peripheral.address()}]")
    peripheral.connect()

    print("Successfully connected")
    service = next((s for s in peripheral.services() if s.uuid() == SERVICE_UUID), None)
    characteristic = next((c for c in service.characteristics() if c.uuid() == CHARACTERISTIC_UUID), None)

    for c in service.characteristics():
        print(f"Characteristic: {c.uuid()}")

    while True:
        # Query the user for content to write
        content = input("Enter content to write: ")

        if 'q' in content:
            break

        # Write the content to the characteristic
        # Note: `write_request` required the payload to be presented as a bytes object.
        write_value = bytes([int(content)])
        peripheral.write_request(SERVICE_UUID, CHARACTERISTIC_UUID, write_value)

    peripheral.disconnect()

if __name__ == "__main__":
    main()