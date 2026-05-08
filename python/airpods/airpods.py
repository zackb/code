import asyncio
import json
import sys
from bleak import BleakScanner

def decode_battery(val):
    if val <= 10:
        return val * 10
    else:
        # values 11-15 are used by Apple for special states (lid closed, out of ear, unknown)
        return -1

def detection_callback(device, advertisement_data):
    # apple's company id is 0x004C (76)
    if 76 in advertisement_data.manufacturer_data:
        m_data = advertisement_data.manufacturer_data[76]
        
        # check if it's an AirPods Proximity Pairing beacon (Type 0x07)
        # bleak strips the 2-byte company id, so m_data[0] is the type.
        if len(m_data) >= 8 and m_data[0] == 0x07:
            pods_byte = m_data[6] # Pods Battery (upper nibble = left, lower = right)
            case_byte = m_data[7] # Case Battery (upper nibble = case, lower = flags)

            left_val = (pods_byte >> 4) & 0xF
            right_val = pods_byte & 0xF
            case_val = (case_byte >> 4) & 0xF

            result = {
                "mac": device.address,
                "left": decode_battery(left_val),
                "right": decode_battery(right_val),
                "case": decode_battery(case_val)
            }
            
            print(json.dumps(result))
            sys.stdout.flush()

async def main():
    print("Scanning for AirPods via D-Bus...")
    scanner = BleakScanner(detection_callback)
    
    await scanner.start()
    try:
        await asyncio.sleep(3600.0)
    except KeyboardInterrupt:
        pass
    finally:
        await scanner.stop()

if __name__ == "__main__":
    asyncio.run(main())
