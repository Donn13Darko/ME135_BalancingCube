/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"
#include "blue.h"

// 1200(1), 2400(2), 4800(3), 9600(4), 19200(5), 38400(6), 57600(7), 115200(8), 230400(9), 460800(A), 921600(B), 1382400(C)
int16_t Divider[8] = {0, 625, 312, 156, 104, 52, 26, 13};
char *Baudrate[8] = {"NO device", "9600", "19200", "38400", "57600", "115200", "230400", "460800"};
char *AT_Command[8] = {"ERROR", "AT+BAUD4", "AT+BAUD5", "AT+BAUD6", "AT+BAUD7", "AT+BAUD8", "AT+BAUD9", "AT+BAUDA"};

char *at = "AT\0";
char *ok = "OK";

uint8_t
config_blue (uint8_t baudChoice, uint16_t timeout)
{
    const uint8_t buff_len = 8;
    char buff[buff_len];
    
	UART_PutString (at);
    
	uint16_t ts = 0;
	if (!UART_GetRxBufferSize () & (ts < timeout))
    {
        ts++;
        CyDelay (1);
    }
    
	ReadRxData (buff, buff_len, 50);
    
	if (strncmp (buff, ok, 2))
	{   
		UART_PutString (AT_Command[baudChoice]);
		CyDelay (timeout);
		ReadRxData (buff, buff_len, 50);
		if (strncmp (buff, ok, 2)) 
            return baudChoice;
	}
    
    return 255;
}

uint8_t
ReadRxData (char* buff, uint8_t buff_len, uint8_t timeout)
{
    uint8_t ts = 0;
    uint8_t rxCount = 0;
    uint8_t rxSize = buff_len - 1;
    memset (buff, '\0', buff_len);
    
    while (UART_GetRxBufferSize ())
	{
		buff[rxCount] = UART_GetChar ();
		rxCount++;
        
        ts = 0;
		while (!UART_GetRxBufferSize () & (ts < timeout))  
        {
            ts++;
            CyDelay (1);
        }
        
		if (rxSize < rxCount)
		{
			memset (buff, '\0', buff_len);
			return 1;
		}
	}
	buff[rxCount] = '\0';
    
    return 0;
}

void
WriteTxData (char *buff, uint8_t buff_len)
{
    int i;
    for (i = 0; i < buff_len; i++)
    {
        UART_PutChar (buff[i]);
    }
}

/* [] END OF FILE */
