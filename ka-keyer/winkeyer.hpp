#ifndef WINKEYER_HPP
#define WINKEYER_HPP

#include <Arduino.h>

void winkeyerBegin();
void winkeyerProcess();
void winkeyerSendPotentiometer();
bool winkeyerGetHostOpened();
bool winkeyerGetSerialEchoback();
bool winkeyerGetPaddleEchoback();

#endif