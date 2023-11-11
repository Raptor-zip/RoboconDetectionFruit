/*
 * ABU2023_GenericOutput_Header.c
 *
 *  Created on: Apr 15, 2023
 *      Author: ke_to
 */

#include"ABU2023_GenericOutput_Header.h"

uint8_t MsgTypeCheck(ABU2023_GenericOutput_Header_Typedef *pABU2023,
		uint8_t type1, uint8_t type2) {
	if ((pABU2023->type[0] == type1) & (pABU2023->type[1] == type2))
		return 1;
	return 0;
}
uint8_t MsgIDCheck(ABU2023_GenericOutput_Header_Typedef *pABU2023, uint8_t ID) {
	if (pABU2023->ID == ID)
		return 1;
	return 0;
}
uint8_t MsgcmdCheck(ABU2023_GenericOutput_Header_Typedef *pABU2023, uint8_t cmd) {
	if (pABU2023->cmd == cmd)
		return 1;
	return 0;
}
uint8_t MsgAllCheck(ABU2023_GenericOutput_Header_Typedef *pABU2023,
		uint8_t type1, uint8_t type2, uint8_t ID, uint8_t cmd) {
	if (MsgTypeCheck(pABU2023, type1, type2) && MsgIDCHeck(pABU2023, ID)
			&& MsgcmdCheck(pABU2023, cmd))
		return 1;
	return 0;
}

