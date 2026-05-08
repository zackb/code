#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define EIR_FLAGS 0x01
#define EIR_MANUFACTURE_SPECIFIC 0xFF

// Helper to decode battery nibble. 15 (0xF) means disconnected/unknown.
int decode_battery(int val) {
    if (val == 15)
        return -1;
    if (val == 10)
        return 100;
    return val * 10;
}

int main() {
    int device_id = hci_get_route(NULL);
    if (device_id < 0) {
        perror("No Bluetooth adapter found");
        return 1;
    }

    int sock = hci_open_dev(device_id);
    if (sock < 0) {
        perror("Failed to open HCI socket (try running as root)");
        return 1;
    }

    // Set BLE scan parameters
    uint8_t scan_type = 0x01; // Active scanning
    uint16_t interval = htobs(0x0010);
    uint16_t window = htobs(0x0010);
    uint8_t own_type = 0x00;
    uint8_t filter_policy = 0x00;

    if (hci_le_set_scan_parameters(sock, scan_type, interval, window, own_type, filter_policy, 1000) < 0) {
        perror("Failed to set LE scan parameters");
        close(sock);
        return 1;
    }

    if (hci_le_set_scan_enable(sock, 0x01, 0, 1000) < 0) {
        perror("Failed to enable LE scan");
        close(sock);
        return 1;
    }

    printf("Scanning for Apple Proximity Pairing (AirPods) beacons...\n");

    // Filter for LE events
    struct hci_filter flt;
    hci_filter_clear(&flt);
    hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
    hci_filter_set_event(EVT_LE_META_EVENT, &flt);
    if (setsockopt(sock, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
        perror("Could not set socket filter");
        return 1;
    }

    unsigned char buf[HCI_MAX_EVENT_SIZE];
    while (1) {
        int len = read(sock, buf, sizeof(buf));
        if (len < 0)
            continue;

        // Skip to the LE event data
        evt_le_meta_event* meta = (void*)(buf + (1 + HCI_EVENT_HDR_SIZE));
        if (meta->subevent != EVT_LE_ADVERTISING_REPORT)
            continue;

        le_advertising_info* info = (le_advertising_info*)(meta->data + 1);

        // Parse the advertising data length and payload
        int offset = 0;
        while (offset < info->length) {
            uint8_t length = info->data[offset];
            if (length == 0)
                break;

            uint8_t type = info->data[offset + 1];
            if (type == EIR_MANUFACTURE_SPECIFIC) {
                uint8_t* m_data = &info->data[offset + 2];
                int m_len = length - 1;

                // Check for Apple Company ID (0x004C)
                if (m_len >= 2 && m_data[0] == 0x4C && m_data[1] == 0x00) {
                    // Check for Proximity Pairing Type (0x07)
                    if (m_len >= 3 && m_data[2] == 0x07) {
                        // We found an AirPods packet!
                        // Based on known reverse engineering offsets:
                        // Byte 2 = 0x07, Byte 3 = length
                        // Byte 8 = Pods Battery (Left/Right nibbles)
                        // Byte 9 = Flags & Case Battery

                        if (m_len >= 10) {
                            int battery_byte = m_data[8]; // Typically Left/Right
                            int case_byte = m_data[9];    // Upper nibble is usually Case

                            int left_val = (battery_byte >> 4) & 0xF;
                            int right_val = battery_byte & 0xF;
                            int case_val = (case_byte >> 4) & 0xF;

                            int left_pct = decode_battery(left_val);
                            int right_pct = decode_battery(right_val);
                            int case_pct = decode_battery(case_val);

                            printf("AirPods Beacon Detected! MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
                                   info->bdaddr.b[5],
                                   info->bdaddr.b[4],
                                   info->bdaddr.b[3],
                                   info->bdaddr.b[2],
                                   info->bdaddr.b[1],
                                   info->bdaddr.b[0]);

                            printf("  Left:  %d%%\n", left_pct);
                            printf("  Right: %d%%\n", right_pct);
                            printf("  Case:  %d%%\n\n", case_pct);
                        }
                    }
                }
            }
            offset += length + 1;
        }
    }

    // Cleanup (though we loop forever above)
    hci_le_set_scan_enable(sock, 0x00, 0, 1000);
    close(sock);
    return 0;
}
