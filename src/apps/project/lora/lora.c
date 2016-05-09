/* * OSS-7 - An opensource implementation of the DASH7 Alliance Protocol for ultra
 * lowpower wireless sensor communication
 *
 * Copyright 2015 University of Antwerp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fifo.h"
#include "timer.h"
#include "IM880A_RadioInterface.h"




#define BUFFER_SIZE	256
#define BUFFER_SIZE_LORA 12
#define TX_LENGTH        12
#define TX_LENGTH2       2
#define INDENT_SPACES "  "

void lora_send(char* bytes, uint8_t length);
void init_lora();

static void CbRxIndication (uint8_t* rxMsg, uint8_t length, TRadioFlags rxFlags);
static void CbTxIndication (TRadioMsg* txMsg, TRadioFlags txFlags);

void init_lora()
{
	//UART LORA
	iM880A_Init();

	// Register callback functions for receive / send
	iM880A_RegisterRadioCallbacks(CbRxIndication, CbTxIndication);
}

void lora_send(char* bytes, uint8_t length)
{
	if (iM880A_SendRadioTelegram(bytes, length) != WiMODLR_RESULT_OK)
	{
		//handle faults TO-DO
	}
}

//------------------------------------------------------------------------------
//
//	Receive callback function
//
//------------------------------------------------------------------------------
static void CbRxIndication (uint8_t* rxMsg, uint8_t length, TRadioFlags rxFlags)
{
    if(rxFlags == trx_RXDone)
    {
        //mainEvent |= MSG_RECEIVED;  // Radio Msg received
    }
}


//------------------------------------------------------------------------------
//
//	Transmit callback function
//
//------------------------------------------------------------------------------
static void CbTxIndication (TRadioMsg* txMsg, TRadioFlags txFlags)
{
    if(txFlags == trx_TXDone)
    {
        // TX was successfull
    }
    else
    {
        //  error
    }
}






