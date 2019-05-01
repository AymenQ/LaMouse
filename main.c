#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <os.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>
#include "Descriptors.h"

/** Buffer to hold the previously generated Mouse HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevMouseHIDReportBuffer[sizeof(USB_MouseReport_Data_t)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
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
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	os_init();

	GlobalInterruptEnable();

	USB_Init();
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


/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
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

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	// Unused (but mandatory for the HID class driver) in this demo, since there are no Host->Device reports
}
