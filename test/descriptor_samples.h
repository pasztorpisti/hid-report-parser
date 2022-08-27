#pragma once

#include <stdint.h>


namespace hid {
namespace descriptor_samples {


namespace mouse {

	// Zowie FK2 (wired gaming mouse with high-end sensor)
	// Model: ZOWIE FK2
	// P/N: 9H.N05BB.A2E
	extern const uint8_t ZOWIE_FK2[69];

	// Razer Viper Ultimate (wireless gaming mouse with high-end sensor)
	// Can operate through wireless dongle or USB-charging cable.
	// Model: RC30-030501
	// P/N:   RC30-03050100-0000
	// The Razer Viper Mini and Razer DeathAdder Elite have
	// the exact same descriptors as this Viper Ultimate:
	extern const uint8_t RAZER_VIPER_ULTIMATE_1[94];
	extern const uint8_t RAZER_VIPER_ULTIMATE_2[159];
	extern const uint8_t RAZER_VIPER_ULTIMATE_3[61];

	// Razer Pro Click Mini (wireless productivity mouse)
	// - The physical wheel can be switched to free-spinning mode
	// - The wheel can be tilted and clicked in left/right directions (+2 buttons)
	// - The buttons can be reprogrammed through software
	// - Can operate through wireless dongle or bluetooth
	// - Battery: 1x AA or 2x AA
	//   The user decides between lower weight and higher charge capacity.
	// Wireless Dongle Model: DGRFG7
	// Wireless Dongle P/N: 039901
	// Wireless Dongle HID report descriptors:
	extern const uint8_t RAZER_PRO_CLICK_MINI_1[103];
	extern const uint8_t RAZER_PRO_CLICK_MINI_2[314];
	extern const uint8_t RAZER_PRO_CLICK_MINI_3[61];
	extern const uint8_t RAZER_PRO_CLICK_MINI_4[348];
	extern const uint8_t RAZER_PRO_CLICK_MINI_5[325];

	// Logitech g305 (wireless gaming mouse with high-end sensor)
	// Wireless Dongle HID report descriptors:
	extern const uint8_t LOGITECH_G305_1[59];
	extern const uint8_t LOGITECH_G305_2[148];
	extern const uint8_t LOGITECH_G305_3[98];

	// Logitech g203 (wired gaming mouse with high-end sensor)
	extern const uint8_t LOGITECH_G203_1[67];
	extern const uint8_t LOGITECH_G203_2[151];

	// Raspberry PI Mouse (office mouse based on PixArt MCU and firmware).
	// Probably many other generic office mice used the same PixArt MCU and firmware.
	// The Logitech mouse under the following link has the exact same HID descriptor:
	// https://stackoverflow.com/questions/13864375/hid-report-descriptor-parser-how-to-find-click-bit-position
	//
	// Unlike my other mice this one interprets SetProtocol USB requests and can
	// switch to boot protocol. In boot protocol it sends 3 bytes long reports.
	// Its report protocol differs from the boot protocol only by adding a 4th
	// byte that holds the wheel delta. The mouse would be compatible with the
	// boot protocol even without handling the SetProtocol(boot) request and
	// reducing its report size to 3 bytes as result because the HID
	// specification allows the report to be longer than 3 bytes as long as the
	// first 3 bytes follow the boot protocol.
	// Quote from the HID specification (hid1_11.pdf):
	//
	//   The HID Subclass 1 defines two descriptors for Boot Devices. Devices
	//   may append additional data to these boot reports, but the first 8 bytes
	//   of keyboard reports and the first 3 bytes of mouse reports must conform
	//   to the format defined by the Boot Report descriptor in order for the
	//   data to be correctly interpreted by the BIOS.
	extern const uint8_t RASPBERRY_PI[52];

	// Glorious Model O- (wired gaming mouse with high-end sensor)
	// The first descriptor is identical to EASTERN_TIMES_TECHNOLOGY_T16_1.
	// The device returns "SINOWEALTH" and "Wired Gaming Mouse" in its USB
	// vendor and product string descriptors - it is probably a customised
	// white-label product. The vendor and product strings of the T16 are
	// "SINOWEALTH" and "Game Mouse" so the vendor is the same.
	extern const uint8_t GLORIOUS_MODEL_O_MINUS_WIRED_1[71];
	extern const uint8_t GLORIOUS_MODEL_O_MINUS_WIRED_2[213];

	// Eastern Times Technology model T16: gaming mouse with a lower end sensor.
	extern const uint8_t EASTERN_TIMES_TECHNOLOGY_T16_1[71];
	extern const uint8_t EASTERN_TIMES_TECHNOLOGY_T16_2[259];

	// MMG Mars: gaming mouse with a lower end sensor.
	extern const uint8_t MMG_MARS[64];

	// Havit: gaming mouse with a lower end sensor.
	extern const uint8_t HAVIT[204];

	// Ludos Flamma: gaming mouse with a lower end sensor.
	// Sensor: PMW3325DB-TWV1
	// MCU: Holtek HT82F553
	// The first descriptor is almost the same as the EASTERN_TIMES_TECHNOLOGY_T16_1.
	// The only difference is that the Ludos declares the logical minimums and
	// maximums before the usages and the negative logical minimums mirror the
	// the values of the positive maximums.
	extern const uint8_t LUDOS_FLAMMA_1[71];
	extern const uint8_t LUDOS_FLAMMA_2[117];
	extern const uint8_t LUDOS_FLAMMA_3[32];

	// Office mouse with 3 buttons without a scroll wheel.
	// source: https://stackoverflow.com/questions/59201479/how-to-parse-hid-report-descriptor
	extern const uint8_t UNKNOWN_1[50];

	// This was copied from a HID report descriptor tutorial (see the link).
	// It is a valid descriptor but declares only a single report ID so something
	// like this would be part of a larger descriptor with multiple report IDs
	// in a real-world device.
	// source: https://who-t.blogspot.com/2018/12/understanding-hid-report-descriptors.html
	extern const uint8_t UNKNOWN_2[79];

	// The Arduino Mouse library running on a SparkFun Pro Micro clone.
	// It doesn't make sense to restrict the number of buttons to 3 because today
	// all major desktop operating systems and many of the related applications
	// can already utilise at least 5 mouse buttons through standard HID drivers.
	// It was more than 2 decades ago when most mice had only 2 or 3 buttons.
	extern const uint8_t ARDUINO_MOUSE_LIBRARY[54];

	// A small and unusual descriptor that works in practice (at least on Linux).
	// - It doesn't have a Collection(Physical) block and its usual Usage(Pointer) prefix.
	//   According to the HID specification the top level Collection(Application)
	//   is the only one that is mandatory so this should work everywhere.
	// - Defining x/y/wheel before the buttons to avoid a
	//   "Usage Page (Generic Desktop Ctrls)" statement before Usage(x/y/wheel).
	// - It isn't compatible with the boot protocol that expects the buttons in
	//   the first byte of the report and X/Y deltas in the second and third bytes.
	extern const uint8_t HANDCRAFTED_1[45];

	// Almost the same as HANDCRAFTED_1 but without the 3 padding bits after the
	// 5 buttons. Reports that aren't byte-aligned are not playing by the rules
	// but Linux and Win10 accept them.
	// According to a HID specification document (https://www.usb.org/sites/default/files/hidpar.pdf)
	// byte alignment is a "Microsoft Constraint". Windows 10 accepts reports
	// that aren't byte aligned but an old Windows XP laptop seems to reject
	// devices with reports that aren't byte aligned. The hid device is marked
	// with a yellow warning/exclamation-mark in the device manager.
	extern const uint8_t HANDCRAFTED_2[39];

	// Almost the same as HANDCRAFTED_2 but this one defines 8 buttons instead
	// of 5 to make the report byte-aligned without const padding bits.
	extern const uint8_t HANDCRAFTED_3[39];

	// This example defines the buttons inside a mouse COLLECTION(APPLICATION)
	// but the axes (x/y/wheel) are top level items outside of collections.
	// This of course isn't permitted by the HID specification but Linux accepts
	// this pathological example and everything works, windows 10 rejects it.
	extern const uint8_t HANDCRAFTED_4[39];

} // namespace mouse


namespace keyboard {

	// CoolerMaster MasterKeys S PBT
	// Model: SGK-4005-KKCM1-UK
	// A sturdy TKL (tenkeyless) gaming keyboard without the stupid gamer-y looks.
	//
	// COOLERMASTER_MASTERKEYS_S_1: a standard boot protocol descriptor.
	// An N-KRO gaming keyboard shouldn't use the 6-KRO boot protocol as its
	// main/default report protocol because this way the N-KRO capability of the
	// hardware can't be utilised through generic HID drivers.
	//
	// A properly implemented gaming keyboard should start in N-KRO compatible
	// report mode by default with a report descriptor that can handle all keys
	// simultaneously with a large bitfield. That way N-KRO works on all major
	// desktop operating systems (including Linux) out of the box without
	// hardware-specific drivers.
	// If the BIOS asks for boot protocol with a SetProtocol(boot) request then
	// the keyboard can start sending reports using the predefined boot protocol.
	// A BIOS like that doesn't even try to parse report descriptors so the
	// keyboard doesn't have to provide a report descriptor to the BIOS.
	// This is the recommended implementation even in the HID specification.
	// Quote from the HID specification:
	//
	//   These descriptors describe reports that the BIOS expects to see.
	//   However, since the BIOS does not actually read the Report descriptors,
	//   these descriptors do not have to be hard-coded into the device if an
	//   alternative report	descriptor is provided. Instead, descriptors that
	//   describe the device reports in a USB-aware operating system should be
	//   included (these may or may not be the same). When the HID class driver
	//   is loaded, it will issue a Change Protocol, changing from the boot
	//   protocol to the report protocol after reading the boot interface's
	//   Report descriptor.
	//
	// Another quote:
	//
	//   Do a Set_Protocol to ensure the device is in boot mode. By default,
	//   the device comes up in non-boot mode (must read the Report descriptor
	//   to know the protocol), so this step allows the system to put the device
	//   into the predefined boot protocol mode.
	//
	// COOLERMASTER_MASTERKEYS_S_2: vendor specific input and output.
	//
	// COOLERMASTER_MASTERKEYS_S_3: this should be the main (and perhaps the
	// only) descriptor of the keyboard: it handles consumer/media keys and all
	// other keys in N-KRO mode by sending the key states in large bitfields.
	// Unfortunately the bitfield that holds the regular keys has been declared
	// as an array input (instead of var input) so it isn't recognised as a
	// bitfield by HID drivers. The bitfield of the less important
	// consumer/media keys has been declared correctly (as var input) but all
	// other keys are doomed so a generic HID driver can access them only
	// through the boot protocol of the first HID interface but that of course
	// comes with an inherent 6-KRO limitation.
	// Perhaps they declared the bitfield of the regular keys as a useless array
	// intentionally as a HACK/placeholder to avoid a conflict between the first
	// and the third HID interfaces by preventing generic HID drivers from
	// recognising the keys in the third HID interface.
	extern const uint8_t COOLERMASTER_MASTERKEYS_S_1[64];
	extern const uint8_t COOLERMASTER_MASTERKEYS_S_2[34];
	extern const uint8_t COOLERMASTER_MASTERKEYS_S_3[134];

	// Raspberry PI branded keyboard.
	//
	// The first descriptor is a standard HID boot protocol descriptor with
	// 3 output bits for LEDs (caps/num/scroll lock) followed by 5 const
	// bits for byte alignment. Boot keyboards that support COMPOSE and
	// KANA have 5 LED output bits followed by 3 const bits for padding.
	// An example to that can be found in the HID specification (hid1_11.pdf)
	// under section "B.1 Protocol 1 (Keyboard)":
	//
	// A regular office keyboard like this benefits from using a report protocol
	// that is identical to the predefined boot protocol. This is the simplest
	// way to satisfy both the BIOS and the generic HID driver of a desktop
	// operating system - killing two birds with one stone. Unlike an N-KRO
	// gaming keyboard, a typical office keyboard with low-cost 6-KRO hardware
	// doesn't sacrifice functionality by doing this.
	//
	// The second descriptor adds support for consumer/media keys
	// (play/pause/volume/etc...). The two HID interfaces operate in parallel.
	//
	// The Logitech Deluxe 250 keyboard (a comfortable low-cost office keyboard
	// with rubber dome switches) uses the exact same descriptor (byte-by-byte)
	// as the first descriptor of the Raspberry PI keyboard despite the fact
	// that the Deluxe 250 was designed and manufactured about 2 decades earlier.
	extern const uint8_t RASPBERRY_PI_1[65];
	extern const uint8_t RASPBERRY_PI_2[59];

} // namespace keyboard


namespace gamepad {

	// Genuine Sony PlayStation 4 controller (aka DualShock 4 or DS4)
	// Model:    CUH-ZCT2U
	// Model no: 4-698-771-02 G
	extern const uint8_t PS4_1[507];

	// Another DS4 HID report descriptor.
	// source: https://github.com/jfedor2/hid-remapper/blob/59f82eb1e362c9daa7fbbbf764a976e8356412c2/firmware/src/our_descriptor.cc
	extern const uint8_t PS4_2[171];

	// X52 Pro (I have one but I was lazy to unpack it from storage to add a 3rd source)
	// source: https://www.consoletuner.com/forum/viewtopic.php?f=25&t=16511&sid=b666cee6b44fc2876260d4e5f8b1a584&start=10
	// source: https://github.com/hathach/tinyusb/issues/1883#issuecomment-1429075421
	extern const uint8_t X52_PRO_HOTAS[125];

	// X52 non-Pro
	// source: https://www.consoletuner.com/forum/viewtopic.php?t=11404
	extern const uint8_t X52_HOTAS[119];

	// Speedlink Phantom Hawk
	// source: https://stackoverflow.com/questions/74783919/problem-with-joystick-hid-descriptor-readout
	//
	// The descriptor seems to have incorrect logical maximum and physical maximum
	// values (-1 instead of 255) because the vendor used 8-bit signed integers
	// instead of 16-bit signed integers so sign-extending those values to int32
	// fills the most significant bits with ones instead of zeros. Some HID
	// descriptor parsers (like this one and the Linux HID driver) can detect
	// that mistake (by checking if the maximum is lower than the minimum) and
	// correct it by treating the maximum as an unsigned integer by converting
	// it to an int32 through zero-extension.
	extern const uint8_t SPEEDLINK_PHANTOM_HAWK[106];

} // namespace gamepad


} // namespace descriptor_samples
} // namespace hid
