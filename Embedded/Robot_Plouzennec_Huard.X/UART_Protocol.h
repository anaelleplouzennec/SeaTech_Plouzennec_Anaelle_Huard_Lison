/* 
 * File:   UART_Protocol.h
 * Author: GEII Robot
 *
 * Created on 8 février 2023, 16:44
 */

#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H
#define SET_ROBOT_STATE 0x0051
#define SET_ROBOT_MANUAL_CONTROL 0x0052

#ifdef	__cplusplus
extern "C" {
#endif

unsigned char UartCalculateChecksum(int msgFunction,int msgPayloadLength, unsigned char* msgPayload);
void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload);
void UartDecodeMessage(unsigned char c);
void UartProcessDecodedMessage(unsigned char function, unsigned char payload[]);



#ifdef	__cplusplus
}
#endif

#endif	/* UART_PROTOCOL_H */

