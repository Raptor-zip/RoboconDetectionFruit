/*
 * ABU2023_GenericOutput_Header.h
 *
 *  Created on: Apr 15, 2023
 *      Author: ke_to
 */

#ifndef ABU2023_GENERICOUTPUT_HEADER_H_
#define ABU2023_GENERICOUTPUT_HEADER_H_

#include<stdio.h>

typedef union {
	struct {
		uint8_t type[2]; // Field to represent a 2-byte data type
		uint8_t ID; // Field to represent ID
		uint8_t cmd; // Field to represent command
		float M; // Field to represent the amount of operation
	};
	uint8_t bin[8]; // Field to represent 8-byte binary data
} ABU2023_GenericOutput_Header_Typedef;

uint8_t MsgTypeCheck(ABU2023_GenericOutput_Header_Typedef *pABU2023,
		uint8_t type1, uint8_t type2);
uint8_t MsgIDCheck(ABU2023_GenericOutput_Header_Typedef *pABU2023, uint8_t ID);
uint8_t MsgcmdCheck(ABU2023_GenericOutput_Header_Typedef *pABU2023, uint8_t cmd);
uint8_t MsgAllCheck(ABU2023_GenericOutput_Header_Typedef *pABU2023,
		uint8_t type1, uint8_t type2, uint8_t ID, uint8_t cmd);

#endif /* ABU2023_GENERICOUTPUT_HEADER_H_ */
