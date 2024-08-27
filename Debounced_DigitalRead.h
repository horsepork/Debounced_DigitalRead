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
        };
        
    private:
        DigitalReadState DR_State = DR_NOT_READING;
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

            bool newReadState = debouncedRead;
            if(invertedOutput) newReadState = !debouncedRead;
            booleanBaseObject.setState(newReadState);
        }

        bool update(){
            if(DR_State == DR_NEW_READ) DR_State = DR_READING;
            else if(DR_State == DR_NEW_RELEASE) DR_State = DR_NOT_READING;
            
            bool rawRead = digitalRead(pin);
            if(debouncedRead != rawRead){
                if(millis() - debounceTimer > debounceTime){
                    debouncedRead = rawRead;
                    bool newReadState = debouncedRead;
                    if(invertedOutput) newReadState = !debouncedRead;
                    booleanBaseObject.setState(newReadState);
                    debounceTimer = millis();
                    if(debouncedRead){
                        if(invertedOutput) DR_State = DR_NEW_RELEASE;
                        else DR_State = DR_NEW_READ;
                    }
                    else{
                        if(invertedOutput) DR_State = DR_NEW_READ;
                        else DR_State = DR_NEW_RELEASE;
                    } 
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

        DigitalReadState GetDigitalReadState(){
            return DR_State;
        }

        void setInverted(bool inverted){
            invertedOutput = inverted;
        }
};

#endif