// This rp2040 example shows how to handle the HID input report messages of a mouse
// with the hid-report-parser library when it gets connected to the rp2040 USB host.
// The hid-report-parser library can handle the HID report descriptor and the input
// reports of the mouse even if they are complicated. The mouse doesn't have to
// support the boot protocol.
//
// This example is based on the following piece of code that works only with mice
// that support the simple HID mouse boot protocol:
// https://github.com/adafruit/Adafruit_TinyUSB_Arduino/blob/0ff42ce1904f3e800dc19a2ec7a4cf613a6282da/examples/DualRole/HID/hid_mouse_log_filter/hid_mouse_log_filter.ino
//
// I have a dozen gaming mice and none of them support the boot protocol.
// The only mouse I have with boot protocol support is a simple Raspberry PI
// office mouse.
//
// This modified example no longer performs filtering on the mouse input.
// It only remaps and forwards the received input to the host
// (your computer) to which the rp2040 is connected as a USB mouse.

/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2023 Bill Binko for Adafruit Industries
 Based on tremor_filter example by Thach Ha
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/* This example demonstrates use of both device and host, where
 * - Device run on native usb controller (roothub port0)
 * - Host depending on MCUs run on either:
 *   - rp2040: bit-banging 2 GPIOs with the help of Pico-PIO-USB library (roothub port1)
 *   - samd21/51, nrf52840, esp32: using MAX3421e controller (host shield)
 *
 * Requirements:
 * - For rp2040:
 *   - [Pico-PIO-USB](https://github.com/sekigon-gonnoc/Pico-PIO-USB) library
 *   - 2 consecutive GPIOs: D+ is defined by PIN_USB_HOST_DP, D- = D+ +1
 *   - Provide VBus (5v) and GND for peripheral
 *   - CPU Speed must be either 120 or 240 Mhz. Selected via "Menu -> CPU Speed"
 * - For samd21/51, nrf52840, esp32:
 *   - Additional MAX2341e USB Host shield or featherwing is required
 *   - SPI instance, CS pin, INT pin are correctly configured in usbh_helper.h
 */


// USBHost is defined in usbh_helper.h
#include "usb_helper.h"
#include "hid_report_parser.h"
#include <memory>
#include <map>

// We could have just used the builtin TUD_HID_REPORT_DESC_MOUSE() definition
// along with the hid_mouse_report_t struct instead of what we have below but
// I wanted to send 16-bit X and Y deltas just like a typical gaming mouse.
uint8_t const mouse_report_descriptor[] = {
  HID_USAGE_PAGE (HID_USAGE_PAGE_DESKTOP),
  HID_USAGE      (HID_USAGE_DESKTOP_MOUSE),
  HID_COLLECTION (HID_COLLECTION_APPLICATION),
    HID_USAGE_PAGE   (HID_USAGE_PAGE_BUTTON),
    HID_USAGE_MIN    (1),
    HID_USAGE_MAX    (5),
    HID_LOGICAL_MIN  (0),
    HID_LOGICAL_MAX  (1),
    HID_REPORT_SIZE  (1),
    HID_REPORT_COUNT (5),
    HID_INPUT        (HID_VARIABLE),

    HID_REPORT_COUNT (3),
    HID_INPUT        (HID_CONSTANT),

    // Gaming mice use 16-bit X/Y axes to support high IPS and acceleration
    // while office mice usually use 8 bits. The 8 bit values have the
    // added benefit of being compatible with boot protocol too without
    // adding any extra code for boot protocol support. The mouse device
    // that we emulate doesn't support boot protocol (which is how gaming
    // mice usually behave).
    HID_USAGE_PAGE    (HID_USAGE_PAGE_DESKTOP),
    HID_USAGE         (HID_USAGE_DESKTOP_X),
    HID_USAGE         (HID_USAGE_DESKTOP_Y),
    HID_LOGICAL_MIN_N ((uint16_t)-0x7FFF, 2),
    HID_LOGICAL_MAX_N (0x7FFF, 2),
    HID_REPORT_SIZE   (16),
    HID_REPORT_COUNT  (2),
    HID_INPUT         (HID_VARIABLE|HID_RELATIVE),

    HID_USAGE        (HID_USAGE_DESKTOP_WHEEL),  // vertical scroll
    HID_LOGICAL_MIN  ((uint8_t)-0x7F),
    HID_LOGICAL_MAX  (0x7F),
    HID_REPORT_SIZE  (8),
    HID_REPORT_COUNT (1),
    HID_INPUT        (HID_VARIABLE|HID_RELATIVE),

    HID_USAGE_PAGE (HID_USAGE_PAGE_CONSUMER),
    HID_USAGE_N    (HID_USAGE_CONSUMER_AC_PAN, 2), // horizontal scroll
    HID_INPUT      (HID_VARIABLE|HID_RELATIVE),
  HID_COLLECTION_END
};

// This struct matches the above HID input report descriptor.
// This is what our Arduino as a HID device will use to send
// mouse reports to the computer.
typedef struct TU_ATTR_PACKED {
  uint8_t buttons;
  int16_t x;
  int16_t y;
  int8_t v_scroll;
  int8_t h_scroll;
} MouseReport;

Adafruit_USBD_HID usb_hid(mouse_report_descriptor, sizeof(mouse_report_descriptor), HID_ITF_PROTOCOL_MOUSE, 1);


void setup() {
  Serial.begin(115200);

  // The hid_report_parser library can handle complicated reports so there
  // is no need to dumb down the devices to HID_PROTOCOL_BOOT that would be
  // the default value without the below call.
  tuh_hid_set_default_protocol(HID_PROTOCOL_REPORT);

  usb_hid.begin();

#if defined(CFG_TUH_MAX3421) && CFG_TUH_MAX3421
  // init host stack on controller (rhport) 1
  // For rp2040: this is called in core1's setup1()
  USBHost.begin(1);
#endif

  //while ( !Serial ) delay(10);   // wait for native usb
  Serial.println("TinyUSB Mouse Example");
}


#if defined(CFG_TUH_MAX3421) && CFG_TUH_MAX3421
//--------------------------------------------------------------------+
// Using Host shield MAX3421E controller
//--------------------------------------------------------------------+
void loop() {
  USBHost.task();
  Serial.flush();
}

#elif defined(ARDUINO_ARCH_RP2040)
//--------------------------------------------------------------------+
// For RP2040 use both core0 for device stack, core1 for host stack
//--------------------------------------------------------------------+
void loop() {
  Serial.flush();
}

//------------- Core1 -------------//
void setup1() {
  // configure pio-usb: defined in usbh_helper.h
  rp2040_configure_pio_usb();

  // run host stack on controller (rhport) 1
  // Note: For rp2040 pico-pio-usb, calling USBHost.begin() on core1 will have most of the
  // host bit-banging processing works done in core1 to free up core0 for other works
  USBHost.begin(1);
}

void loop1() {
  USBHost.task();
}

#endif


struct MountedMouse {
  hid::BitField<hid::MouseConfig::NUM_BUTTONS> buttons;
  hid::Int32Array<hid::MouseConfig::NUM_AXES> axes;
  hid::SelectiveInputReportParser parser;
};

std::map<uint16_t,std::unique_ptr<MountedMouse>> mounted_mice;

uint16_t map_key(uint8_t dev_addr, uint8_t instance) {
  return (uint16_t)(((uint16_t)dev_addr << 8) | (uint16_t)instance);
}


//--------------------------------------------------------------------+
// TinyUSB Host callbacks
//--------------------------------------------------------------------+
extern "C"
{

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use.
// tuh_hid_parse_report_descriptor() can be used to parse common/simple enough
// descriptor. Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE,
// it will be skipped therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len) {
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);
  Serial.printf("HID Mount VID:PID=%04x:%04x dev_addr=%d instance=%d\r\n",
    vid, pid, dev_addr, instance);

  std::unique_ptr<MountedMouse> p(new MountedMouse);

  auto buttons_ref = p->buttons.Ref();
  auto axes_ref = p->axes.Ref();
  hid::MouseConfig cfg;
  auto cfg_root = cfg.Init(&buttons_ref, &axes_ref);

  // SelectiveInputReportParser::Init creates a mapping between the fields of
  // the input report and our variables (m->buttons and m->axes) and stores
  // that mapping in the SelectiveInputReportParser instance for later use.
  int res = p->parser.Init(cfg_root, desc_report, desc_len);
  // Returns hid::ERR_COULD_NOT_MAP_ANY_USAGES if the descriptor doesn't
  // have an application container tagged with Usage(Mouse).
  if (res) {
    Serial.printf("SelectiveInputReportParser::Init failed: result=%s[%d] descriptor_size=%u\r\n",
      hid::str_error(res, "UNKNOWN"), res, desc_len);
    return;
  }
  mounted_mice[map_key(dev_addr, instance)] = std::move(p);

  // The cfg object is no longer needed after the parser.Init call but
  // it contains potentially useful information in its member variables:

  Serial.printf("mapped fields: buttons[0-4]=%d%d%d%d%d x/y/vscroll/hscroll=%d/%d/%d/%d\r\n",
    cfg.buttons.mapped[hid::MouseConfig::BTN_LEFT]    ? 1 : 0,
    cfg.buttons.mapped[hid::MouseConfig::BTN_RIGHT]   ? 1 : 0,
    cfg.buttons.mapped[hid::MouseConfig::BTN_MIDDLE]  ? 1 : 0,
    cfg.buttons.mapped[hid::MouseConfig::BTN_BACK]    ? 1 : 0,
    cfg.buttons.mapped[hid::MouseConfig::BTN_FORWARD] ? 1 : 0,
    cfg.axes.mapped[hid::MouseConfig::X]              ? 1 : 0,
    cfg.axes.mapped[hid::MouseConfig::Y]              ? 1 : 0,
    cfg.axes.mapped[hid::MouseConfig::V_SCROLL]       ? 1 : 0,
    cfg.axes.mapped[hid::MouseConfig::H_SCROLL]       ? 1 : 0);

  Serial.printf("properties: x_min/max=%d/%d, vscroll_min/max=%d/%d\n",
    cfg.axes.properties[hid::MouseConfig::X].logical_min,
    cfg.axes.properties[hid::MouseConfig::X].logical_max,
    cfg.axes.properties[hid::MouseConfig::V_SCROLL].logical_min,
    cfg.axes.properties[hid::MouseConfig::V_SCROLL].logical_max);

  if (!tuh_hid_receive_report(dev_addr, instance))
    Serial.printf("Error: cannot request to receive report\r\n");
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);
  Serial.printf("HID Unmount VID:PID=%04x:%04x dev_addr=%d instance=%d\r\n",
    vid, pid, dev_addr, instance);
  mounted_mice.erase(map_key(dev_addr, instance));
}

int16_t clamp_int16(int32_t v) {
  if (v < -0x7fff)
    return (int16_t)-0x7fff;
  if (v > 0x7fff)
    return (int16_t)0x7fff;
  return (int16_t)v;
}

int8_t clamp_int8(int32_t v) {
  if (v < -0x7f)
    return (int8_t)-0x7f;
  if (v > 0x7f)
    return (int8_t)0x7f;
  return (int8_t)v;
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
  struct Finally {
    uint8_t dev_addr;
    uint8_t instance;
    ~Finally() {
      if (!tuh_hid_receive_report(dev_addr, instance))
        Serial.printf("Error: cannot request to receive report\r\n");
    }
  } finally = { dev_addr, instance };

  auto it = mounted_mice.find(map_key(dev_addr, instance));
  if (it == mounted_mice.end())
    return;
  MountedMouse& m = *it->second.get();

  // Acting as a USB host: parsing the input report received from the physical
  // mouse and storing the field values into the variables of our application:
  // m->buttons and m->axes.

  int res = m.parser.Parse(report, len);
  if (res) {
    if (res == hid::ERR_NOTHING_CHANGED)
      return;
    // Running into errors every now and then isn't a serious
    // issue if your mouse otherwise seems to work properly.
    Serial.printf("SelectiveInputReportParser::Parse failed: result=%s[%d] report_size=%u\r\n",
      hid::str_error(res, "UNKNOWN"), res, len);
    return;
  }

  // Acting as a USB device:
  // Converting the received mouse input into the format we defined in our
  // HID descriptor and sending it to the host.

  MouseReport r;
  r.buttons = m.buttons.Flags<uint8_t>(0);
  r.x = clamp_int16(m.axes[hid::MouseConfig::X]);
  r.y = clamp_int16(m.axes[hid::MouseConfig::Y]);
  r.v_scroll = clamp_int8(m.axes[hid::MouseConfig::V_SCROLL]);
  r.h_scroll = clamp_int8(m.axes[hid::MouseConfig::H_SCROLL]);

  usb_hid.sendReport(0, &r, sizeof(r));
}

} // extern C
