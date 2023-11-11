/*
 * Dual_Shock.h
 *
 *  Created on: Mar 2, 2023
 *      Author: noita
 */

#ifndef DUALSHOCK_H_
#define DUALSHOCK_H_

typedef struct{
	uint16_t button_status;
	uint8_t Analog_L2;
	uint8_t Analog_R2;
	uint8_t Lx;
	uint8_t Ly;
	uint8_t Rx;
	uint8_t Ry;
}HID_JOYSTICK_Info_TypeDef;

typedef union{
	HID_JOYSTICK_Info_TypeDef JoyStickData;
	uint8_t bin[sizeof(HID_JOYSTICK_Info_TypeDef)];
}DualShock_TypeDef;

#endif /* DUALSHOCK_H_ */