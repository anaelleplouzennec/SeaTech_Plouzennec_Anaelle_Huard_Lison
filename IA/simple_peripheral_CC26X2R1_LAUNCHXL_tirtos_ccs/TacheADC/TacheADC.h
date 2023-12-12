#ifndef TacheADC_H
#define TacheADC_H

uint32_t Sampling(uint_least8_t);
void TacheADC_CreateTask(void);
void TacheADC_taskFxn(UArg, UArg);
void myClockSwiFxn(uintptr_t);

#endif  /*TacheADC_H*/
