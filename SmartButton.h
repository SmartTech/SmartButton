#ifndef SMARTBUTTON_H
#define SMARTBUTTON_H

#include <Arduino.h>

//#define DEBUG_BTN Serial

class SmartButton {

  private:
  
  typedef void (*EventCallback)();
  typedef void (*EventCallback1)(int);
  typedef void (*EventCallback2)(int, int);
  
  unsigned int  holdCounter      = false;
  unsigned int  debounceCounter  = false;
  unsigned int  pressed_timeout  = false;
  unsigned long handleMicros     = false;

  bool begined        = false;
  bool buttonState    = false;
  bool holdedState    = false;
  bool onClickFlag    = false;
  bool onHoldFlag     = false;
  bool onPressedFlag  = false;
  bool onReleasedFlag = false;
  
  uint8_t multiCallbackFlag    = 0;
  uint8_t holdCallbackFlag     = 0;
  uint8_t clickCallbackFlag    = 0;
  uint8_t pressedCallbackFlag  = 0;
  uint8_t releasedCallbackFlag = 0;

  void handleButtons();
  void resetButtons();
  
  int  pin         = false;
  int  holder      = false;
  int  debounce    = false;
  int  num         = false;
  byte pressed_num = false;
  bool active      = LOW;
  byte* btn        = NULL;    // указатель виртуальной кнопки
  int  multiPeriod = 500;
  
  EventCallback2 user_MultiClicked = NULL;
  EventCallback1 user_onClick      = NULL;
  EventCallback1 user_onHold       = NULL;
  EventCallback1 user_onPressed    = NULL;
  EventCallback1 user_onReleased   = NULL;
  EventCallback  user_onBeep       = NULL;
  
  public:
  
  SmartButton();
  
  void begin(int _pin, int _hold = 1000, int _debounce = 50);
  void begin(uint8_t* btn_ptr, int _hold = 1000, int _debounce = 50);
  
  void setActive(bool _active);
  void setDebounce(uint8_t _debounce);
  void setHold(uint16_t _hold);
  void setMultiPeriod(uint16_t _period);
  
  void handle();
  void handleLoop();
  void handleCallbacks();
  
  bool onClick();
  bool onHold();
  bool onPressed();
  bool onReleased();
  
  bool isHold();
  bool isActive();
  bool isPressed();
  bool isReleased();
  
  void click(uint8_t _count = 1);
  void hold();
  
  void setMultiClicked(EventCallback2);
  void setClicked(EventCallback1);
  void setHolded(EventCallback1);
  void setPressed(EventCallback1);
  void setReleased(EventCallback1);
  void setBeep(EventCallback);

};

#endif // SMARTBUTTON_H
