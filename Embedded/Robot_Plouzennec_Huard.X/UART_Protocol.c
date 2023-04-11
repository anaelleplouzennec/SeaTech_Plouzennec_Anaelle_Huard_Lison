#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction prenant entree la trame et sa longueur pour calculer le checksum
    unsigned char checksum = 0;
    checksum ^= 0xFE;
    checksum ^= (char) (msgFunction >> 8);
    checksum ^= (char) (msgFunction >> 0);
    checksum ^= (char) (msgPayloadLength >> 8);
    checksum ^= (char) (msgPayloadLength >> 0);
    for (int i = 0; i < msgPayloadLength; i++) {
        checksum ^= msgPayload[i];
    }
    return checksum;
}

void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction d?encodage et d?envoi d?un message
    unsigned char charList [msgPayloadLength + 6];

    int pos = 0;

    charList[pos++] = 0xFE;
    charList[pos++] = (char) (msgFunction >> 8);
    charList[pos++] = (char) (msgFunction >> 0);
    charList[pos++] = (char) (msgPayloadLength >> 8);
    charList[pos++] = (char) (msgPayloadLength >> 0);

    for (int i = 0; i < msgPayloadLength; i++) {
        charList[pos++] = msgPayload[i];
    }
    charList[pos++] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
    SendMessage(charList, msgPayloadLength + 6);
}

enum StateReception {
    Waiting,
    FunctionMSB,
    FunctionLSB,
    PayloadLengthMSB,
    PayloadLengthLSB,
    Payload,
    CheckSum
};
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;
unsigned char rcvState;

void UartDecodeMessage(unsigned char c) {
    //Fonction prenant en entree un octet et servant a reconstituer les trames
    switch (rcvState) {
        case Waiting:
            if (c == 0xFE) {
                rcvState = FunctionMSB;
            }

            break;
        case FunctionMSB:
            msgDecodedFunction = (c << 8);
            rcvState = FunctionLSB;
            break;
        case FunctionLSB:
            msgDecodedFunction += (c << 0);
            rcvState = PayloadLengthMSB;
            break;

        case PayloadLengthMSB:
            msgDecodedPayloadLength = (c << 8);
            rcvState = PayloadLengthLSB;
            break;

        case PayloadLengthLSB:
            msgDecodedPayloadLength += (c << 0);

            if (msgDecodedPayloadLength == 0) {
                rcvState = CheckSum;
            }
            else if (msgDecodedPayloadLength > 1024) {
                rcvState = Waiting;
            }
            else {                
                msgDecodedPayloadIndex = 0;
                rcvState = Payload;
            }

            break;

        case Payload:
            msgDecodedPayload[msgDecodedPayloadIndex] = c;
            msgDecodedPayloadIndex++;
            if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                rcvState = CheckSum;
            break;

        case CheckSum:
        {
            unsigned char calculatedChecksum;
            unsigned char receivedChecksum = c;
            calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            if (calculatedChecksum == receivedChecksum) {

                //Success, on a un message valide
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            }
            rcvState = Waiting;
        }
            break;
        default:
            rcvState = Waiting;
            break;
    }
}

void UartProcessDecodedMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload) {
    //Fonction appelee apres le decodage pour executer l?action
    //correspondant au message recu
    switch (msgFunction) {
        case 0x0020:
            if (msgPayload[0] == 1) {
                if (msgPayload[1] == 1) {

                } else {

                }
            } else if (msgPayload[0] == 2) {
                if (msgPayload[1] == 1) {

                } else {

                }
            } else if (msgPayload[0] == 3) {
                if (msgPayload[1] == 1) {

                } else {

                }
            }
            break;
        case 0x0030: if (msgPayload[0] == 1) {

            } else if (msgPayload[0] == 2) {

            } else if (msgPayload[0] == 3) {

            }
        case 0x0040:
            if (msgPayload[0] == 1) {

            } else if (msgPayload[0] == 2) {

            }
            break;
    }
}