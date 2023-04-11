/* 
 * File:   main.c
 * Author: GEII Robot
 *
 * Created on 26 septembre 2022, 14:38
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"
#include "main.h"
#include "UART.h"
#include "CB_TX1.h"
#include <libpic30.h>
#include "CB_RX1.h"
#include "UART_Protocol.h"

unsigned int ADCValue0, ADCValue1, ADCValue2, ADCValue3, ADCValue4;
unsigned char sensorCode;
unsigned char payload[] = {'B', 'o', 'n', 'j', 'o', 'u', 'r'};

int main(void) {
    /***************************************************************************************************/
    //Initialisation de l'oscillateur
    /****************************************************************************************************/
    InitOscillator();

    /****************************************************************************************************/
    // Configuration des entr�es sorties
    /****************************************************************************************************/
    InitIO();

    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE = 1;

    //InitTimer23();
    InitTimer1();
    InitTimer4();

    InitPWM();
    PWMSetSpeedConsigne(20, MOTEUR_DROIT);
    InitADC1();
    InitUART();
    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [0])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit2 = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result [2])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [3])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche2 = 34 / volts - 5;
            volts = ((float) result [4])*3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34 / volts - 5;

            sensorCode = 0b00000;
            if (robotState.distanceTelemetreGauche2 < 30)
                sensorCode = sensorCode | 0b10000;
            if (robotState.distanceTelemetreGauche < 35)
                sensorCode = sensorCode | 0b01000;
            if (robotState.distanceTelemetreCentre < 40)
                sensorCode = sensorCode | 0b00100;
            if (robotState.distanceTelemetreDroit < 35)
                sensorCode = sensorCode | 0b00010;
            if (robotState.distanceTelemetreDroit2 < 30)
                sensorCode = sensorCode | 0b00001;

            //                    ADCValue0 = result[0];
            if (robotState.distanceTelemetreDroit2 < 30 || robotState.distanceTelemetreDroit < 30) {
                LED_ORANGE = 1;
            } else {
                LED_ORANGE = 0;
            }
            //                    ADCValue1 = result[1];
            if (robotState.distanceTelemetreCentre < 30) {
                LED_BLEUE = 1;
            } else {
                LED_BLEUE = 0;
            }
            //                    ADCValue2 = result[2];
            if (robotState.distanceTelemetreGauche2 < 30 || robotState.distanceTelemetreGauche < 30) {
                LED_BLANCHE = 1;
            } else {
                LED_BLANCHE = 0;
            }
            
            unsigned char payload[3] = { (unsigned char)robotState.distanceTelemetreGauche, (unsigned char)robotState.distanceTelemetreCentre, (unsigned char)robotState.distanceTelemetreDroit};
            UartEncodeAndSendMessage(0x0030, 3, payload);
            //UartEncodeAndSendMessage(0x0080,7,payload);
            //            SendMessage((unsigned char*) "Bonjour",7);
        }

        int i;
        for (i = 0; i < CB_RX1_GetDataSize(); i++) {
            unsigned char c = CB_RX1_Get();
            SendMessage(&c, 1);
        }
        
        //LED_BLANCHE = !LED_BLANCHE;
        //LED_BLEUE = !LED_BLEUE;
        //LED_ORANGE = !LED_ORANGE;

         __delay32(1000);
        //SendMessageDirect((unsigned char*) "Bonjour", 7);
        //__delay32(40000000);
    } // fin main
}


unsigned char stateRobot;

void OperatingSystemLoop(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;

        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(12.5, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-12.5, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-12.5, MOTEUR_DROIT);
            PWMSetSpeedConsigne(12.5, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_LEGER_DROITE:
            PWMSetSpeedConsigne(-10, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_LEGER_DROITE_EN_COURS;

        case STATE_TOURNE_LEGER_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_LEGER_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-10, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_LEGER_GAUCHE_EN_COURS;

        case STATE_TOURNE_LEGER_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode(void) {

    switch (sensorCode) {
        case 0b00000:
            nextStateRobot = STATE_AVANCE;
            break;
        case 0b00001:
            nextStateRobot = STATE_TOURNE_LEGER_GAUCHE;
            break;
        case 0b00010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b00111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01000:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b01001:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b01010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b01101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b01111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b10000:
            nextStateRobot = STATE_TOURNE_LEGER_DROITE;
            break;
        case 0b10001:
            nextStateRobot = STATE_AVANCE;
            break;
        case 0b10010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b10011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b10100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b10101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b10110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b10111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b11000:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11001:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11010:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11011:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;
        case 0b11100:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11101:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11110:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
            break;
        case 0b11111:
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
            break;



    }
    //    unsigned char positionObstacle = PAS_D_OBSTACLE;
    //
    //    //D�termination de la position des obstacles en fonction des t�l�m�tres
    //    if (robotState.distanceTelemetreDroit < 30 &&
    //            robotState.distanceTelemetreCentre > 20 &&
    //            robotState.distanceTelemetreGauche > 30) //Obstacle � droite
    //        positionObstacle = OBSTACLE_A_DROITE;
    //    else if (robotState.distanceTelemetreDroit > 30 &&
    //            robotState.distanceTelemetreCentre > 20 &&
    //            robotState.distanceTelemetreGauche < 30) //Obstacle � gauche
    //        positionObstacle = OBSTACLE_A_GAUCHE;
    //    else if (robotState.distanceTelemetreCentre < 20) //Obstacle en face
    //        positionObstacle = OBSTACLE_EN_FACE;
    //    else if (robotState.distanceTelemetreDroit > 30 &&
    //            robotState.distanceTelemetreCentre > 20 &&
    //            robotState.distanceTelemetreGauche > 30) //pas d?obstacle
    //        positionObstacle = PAS_D_OBSTACLE;



    //    //D�termination de l?�tat � venir du robot
    //    if (positionObstacle == PAS_D_OBSTACLE)
    //        nextStateRobot = STATE_AVANCE;
    //    else if (positionObstacle == OBSTACLE_A_DROITE)
    //        nextStateRobot = STATE_TOURNE_GAUCHE;
    //    else if (positionObstacle == OBSTACLE_A_GAUCHE)
    //        nextStateRobot = STATE_TOURNE_DROITE;
    //    else if (positionObstacle == OBSTACLE_EN_FACE)
    //        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;

    //Si l?on n?est pas dans la transition de l?�tape en cours
    if (nextStateRobot != stateRobot - 1)
    {
        //Appel� si on doit changer d'�tat 
        stateRobot = nextStateRobot;
        unsigned char payload[] = {stateRobot, (unsigned char)(timestamp>>24), (unsigned char)(timestamp>>16), (unsigned char)(timestamp>>8), (unsigned char)(timestamp>>0)};
        UartEncodeAndSendMessage(0x0050, 5, payload);
                
    }
}


