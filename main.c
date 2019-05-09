#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <os.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>
#include "Descriptors.h"

// Previous HID report - so the driver can compare between this and the current
static uint8_t PrevMouseHIDReportBuffer[sizeof(USB_MouseReport_Data_t)];

// Driver interface config - includes what ttype of device it is, the report
// size, and the previous report buffer
USB_ClassInfo_HID_Device_t Mouse_HID_Interface =
{
	.Config =
	{
		.InterfaceNumber              = INTERFACE_ID_Mouse,
		.ReportINEndpoint             =
		{
			.Address              = MOUSE_EPADDR,
			.Size                 = MOUSE_EPSIZE,
			.Banks                = 1,
		},
		.PrevReportINBuffer           = PrevMouseHIDReportBuffer,
		.PrevReportINBufferSize       = sizeof(PrevMouseHIDReportBuffer),
	},
};


int main(void) {
	// Disable watchdog timer (not handled in this application)
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	// Clock prescaling /1
	clock_prescale_set(clock_div_1);

	// Initialise FortunaOS
	os_init();

	// Enable interrupts (same as sei())
	GlobalInterruptEnable();

	// Initialise USB hardware
	USB_Init();
	display_string("Please use the direction keys to move the mouse.\nCentre button to click.");
	for(;;) {
		HID_Device_USBTask(&Mouse_HID_Interface);
		USB_USBTask();
	}
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	HID_Device_ConfigureEndpoints(&Mouse_HID_Interface);
	USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Mouse_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Mouse_HID_Interface);
}

/*
 * Create HID report callback function
 * HIDInterfaceInfo is the interface info (same as initialised at top of file) 
 * ReportID is the ID requested by the host computer
 * ReportData is a buffer which holds the report that should be created
 * ReportSize is an integer pointer which holds the size of the created report
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
					 uint8_t* const ReportID,
					 const uint8_t ReportType,
					 void* ReportData,
					 uint16_t* const ReportSize)
{
	USB_MouseReport_Data_t* MouseReport = (USB_MouseReport_Data_t*)ReportData;

	if (get_switch_state(_BV(SWN)) > 0) {
		MouseReport->Y =  -1;
	}

	if (get_switch_state(_BV(SWS)) > 0) {
		MouseReport->Y =  1;
	}

	if (get_switch_state(_BV(SWW)) > 0) {
		MouseReport->X =  -1;
	}

	if (get_switch_state(_BV(SWE)) > 0) {
		MouseReport->X =  1;
	}

	if(get_switch_state(_BV(SWC)) > 0) {
		MouseReport->Button |= (1 << 0);
	}


	*ReportSize = sizeof(USB_MouseReport_Data_t);
	return true;
}

// Process received report callback function - not used as this application
// acts as a device, not a host
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{ }
