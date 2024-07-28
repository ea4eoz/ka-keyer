#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "Arduino.h"

class UserInterface{
  public:
    void begin();
    void start();
    void process();
    void forceRefresh();
  private:
    uint8_t currentScreen;
    uint8_t needRefresh;
    void splashScreenStart();
    void splashScreenStop();
    void splashScreenEnd();
    void setupScreen();
    uint8_t getNextScreen(uint8_t btn);
    void setCurrentScreen(uint8_t scr);
    void refreshUI();
    void updateScreen(uint8_t screen, uint8_t fullFrame);
};

#endif