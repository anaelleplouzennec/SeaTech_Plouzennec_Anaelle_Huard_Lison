/* 
 * File:   UART_Protocol.h
 * Author: GEII Robot
 *
 * Created on 8 février 2023, 16:44
 */

#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

#ifdef	__cplusplus
extern "C" {
#endif

unsigned char UartCalculateChecksum(int msgFunction,int msgPayloadLength, unsigned char* msgPayload);
void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload);
void UartDecodeMessage(unsigned char c);
void UartProcessDecodedMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload);



#ifdef	__cplusplus
}
#endif

#endif	/* UART_PROTOCOL_H */

