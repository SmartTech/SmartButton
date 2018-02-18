#include "SmartButton.h"

static int attachedButtons = 0;

// Constructors ////////////////////////////////////////////////////////////////
SmartButton::SmartButton()
{
}

// Public Methods //////////////////////////////////////////////////////////////
void SmartButton::begin(int _pin, int _hold, int _debounce) {
  pin      = _pin;
  holder   = _hold;
  debounce = _debounce;
  begined  = true;
  num      = attachedButtons;
  setActive(active);
  attachedButtons++;
  handleMicros = micros();
}

void SmartButton::begin(byte* btn_ptr, int _hold, int _debounce) {
	btn = btn_ptr;
	begin(0, _hold, _debounce);
}

void SmartButton::setHold(uint16_t _hold) { holder = _hold; }
void SmartButton::setDebounce(uint8_t _debounce) { debounce = _debounce; }
void SmartButton::setActive(bool _active) { 
  active = _active;
  if(!btn) {
	if(active) pinMode(pin, INPUT);
	else pinMode(pin, INPUT_PULLUP);
	digitalWrite(pin, !active);
  }
}

bool SmartButton::onClick() {
  bool state = onClickFlag;
  onClickFlag = false;
  return state;
}
bool SmartButton::onHold() {
  bool state = onHoldFlag;
  onHoldFlag = false;
  return state;
}
bool SmartButton::onPressed() {
  bool state = onPressedFlag;
  onPressedFlag = false;
  return state;
}
bool SmartButton::onReleased() {
  bool state = onReleasedFlag;
  onReleasedFlag = false;
  return state;
}

bool SmartButton::isHold() { return (holdCounter>=holder); }
bool SmartButton::isPressed() { return buttonState; }
bool SmartButton::isReleased() { return !buttonState; }
void SmartButton::setMultiClicked(EventCallback2 function) { user_MultiClicked = function; }
void SmartButton::setClicked(EventCallback1 function) { user_onClick = function; }
void SmartButton::setHolded(EventCallback1 function) { user_onHold = function; }
void SmartButton::setPressed(EventCallback1 function) { user_onPressed = function; }
void SmartButton::setReleased(EventCallback1 function) { user_onReleased = function; }
void SmartButton::setBeep(EventCallback function) { user_onBeep = function; }

void SmartButton::click(uint8_t _count) {
	if(!_count) return;
	if(_count==1) {
		if(!user_onClick) return;
		#if defined(DEBUG_BTN)
			DEBUG_BTN.println("--> Virtual onClick");
		#endif
		user_onClick(num);
	} else {
		if(!user_MultiClicked) return;
		#if defined(DEBUG_BTN)
		  DEBUG_BTN.println("--> Virtual MultiClick");
		#endif
		user_MultiClicked(num, _count);
	}
}

void SmartButton::hold() {
	if(!user_onHold) return;
	#if defined(DEBUG_BTN)
	  DEBUG_BTN.println("--> Virtual onHold");
	#endif
	user_onHold(num);
}

bool SmartButton::isActive() {
	if(btn) return (*btn == active);
	else    return (digitalRead(pin) == active);
}

void SmartButton::handleLoop() { if(begined) handleButtons(); }
void SmartButton::handle() { 
	if(begined) {
		if(micros() - handleMicros >= 1000) {
			handleButtons();
			handleMicros = micros();
		}
	}
}

void SmartButton::handleCallbacks() {
	//--------------------------------------------------
	if(multiCallbackFlag) {
		if(user_MultiClicked) {
			#if defined(DEBUG_BTN)
			  DEBUG_BTN.println("--> MultiClick");
			#endif
			user_MultiClicked(num, multiCallbackFlag);
		}
		multiCallbackFlag = false;
	}
	//--------------------------------------------------
	if(holdCallbackFlag) {
		if(user_onHold) {
			#if defined(DEBUG_BTN)
			  DEBUG_BTN.println("--> onHold");
			#endif
			user_onHold(num);
		}
		holdCallbackFlag = false;
	}
	//--------------------------------------------------
	if(pressedCallbackFlag) {
		if(user_onPressed) {
			#if defined(DEBUG_BTN)
				DEBUG_BTN.println("--> onPressed");
		    #endif
			user_onPressed(num);
		}
		pressedCallbackFlag = false;
	}
	//--------------------------------------------------
	if(releasedCallbackFlag) {		
		if(user_onReleased) {
			#if defined(DEBUG_BTN)
				DEBUG_BTN.println("--> onReleased");
			#endif
			user_onReleased(num);
		}
		releasedCallbackFlag = false;
	}
	//--------------------------------------------------
	if(clickCallbackFlag) {
		if(user_onClick) user_onClick(num);
		clickCallbackFlag = false;
	}
}

// Private Methods /////////////////////////////////////////////////////////////
void SmartButton::handleButtons() {
	resetButtons();
	if(pressed_num) {
      if(pressed_timeout < 500) {
		  pressed_timeout++;
	  }
      else {
		if(pressed_num>1) {
			if(user_MultiClicked) {
				multiCallbackFlag = pressed_num;
			}
		}
        pressed_timeout = 0;
        pressed_num = 0;
      }
    }
    if(buttonState) {
      if(holdCounter < holder) holdCounter++;
      else if(holdCounter == holder) {
        holdCounter++;
        pressed_num = 0;
		onHoldFlag = true;
		if(user_onHold) holdCallbackFlag = true;
		if(user_onBeep) user_onBeep();
      }
    }
    if(isActive()) {
      if(!buttonState) {
        if(debounceCounter < debounce) debounceCounter++;
        else {
          debounceCounter = 0;
          buttonState = true;
		  onPressedFlag = true;
          if(user_onPressed) pressedCallbackFlag = true;
        }
      } else {
        debounceCounter = 0;
      }
    } else {
      if(buttonState) {
        if(debounceCounter < debounce) debounceCounter++;
        else {
          debounceCounter = 0;
          buttonState = false;
          pressed_num++;
          pressed_timeout = 0;
		  onReleasedFlag = true;
		  if(user_onReleased) releasedCallbackFlag = true;
		  if(holdCounter < holder) {
			onClickFlag = true;
		    if(user_onClick) clickCallbackFlag = true;
			if(user_onBeep)  user_onBeep();
		  }
          holdCounter = 0;
        }
      } else {
        debounceCounter = 0;
      }
    }
	// ToDo: добавить функцию для активации колбеков из главного цикла
	//handleCallbacks();
}

void SmartButton::resetButtons() {
    onClickFlag    = false;
    onHoldFlag     = false;
    onPressedFlag  = false;
    onReleasedFlag = false;
}
