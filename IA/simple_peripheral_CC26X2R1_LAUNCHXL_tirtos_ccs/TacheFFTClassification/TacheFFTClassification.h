/*
 * TacheFFTClassification.h
 *
 *  Created on: 12 déc. 2023
 *      Author: TP-EO-5
 */

#ifndef TACHEFFTCLASSIFICATION_TACHEFFTCLASSIFICATION_H_
#define TACHEFFTCLASSIFICATION_TACHEFFTCLASSIFICATION_H_

void TacheFFTClassification_CreateTask(void);
static void TacheFFTClassification_taskFxn(UArg a0, UArg a1);
void FFTClassificationTrigger(float serie[]);



#endif /* TACHEFFTCLASSIFICATION_TACHEFFTCLASSIFICATION_H_ */
