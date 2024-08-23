#ifndef DEBOUNCEDDIGITALREAD_H
#define DEBOUNCEDDIGITALREAD_H

#include "Arduino.h"
#include "BooleanInputBase.h"

class Debounced_DigitalRead{
    public:
        Debounced_DigitalRead(uint8_t _pin, uint8_t _inputType, bool _invertedOutput = false){
            pin = _pin;
            inputType = _inputType;
            invertedOutput = _invertedOutput; // not implemented
        }

        BooleanInputBase booleanBaseObject;

        enum DigitalReadState{
            DR_READING,
            DR_NEW_READ,
            DR_NOT_READING,
            DR_NEW_RELEASE
        } DR_State;
        
    private:
        uint8_t pin;
        uint8_t inputType;
        uint8_t debounceTime = 10;
        bool invertedOutput;
        uint32_t debounceTimer;
        bool debouncedRead;
        bool prevRead;
        bool debug = false;

    public:
        void begin(){
            debouncedRead = (inputType == INPUT_PULLUP) ? HIGH : LOW;
            booleanBaseObject.setInputType(inputType);
            pinMode(pin, inputType);
            uint32_t beginTimer = millis();
            while(millis() - beginTimer < (debounceTime * 2)) update();
            booleanBaseObject.setState(debouncedRead);
        }

        bool update(){
            if(DR_State == DR_NEW_READ) DR_State = DR_READING;
            else if(DR_State == DR_NEW_RELEASE) DR_State = DR_NOT_READING;
            
            bool rawRead = digitalRead(pin);
            if(debouncedRead != rawRead){
                if(millis() - debounceTimer > debounceTime){
                    debouncedRead = rawRead;
                    booleanBaseObject.setState(debouncedRead);
                    debounceTimer = millis();
                    if(debouncedRead) DR_State = DR_NEW_READ;
                    else DR_State = DR_NEW_RELEASE;
                    return true;
                }
            }
            else{
                debounceTimer = millis();
            }
            return false;
        }

        bool read(){
            return booleanBaseObject.read();
        }

        void setDebounceTime(uint8_t d){
            debounceTime = d;
        }
};

#endif