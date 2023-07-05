#include "Arduino.h"

class Debounced_DigitalRead{
    public:
        Debounced_DigitalRead(uint8_t _pin, uint8_t _inputType, bool _invertedOutput = false){
            pin = _pin;
            inputType = _inputType;
            invertedOutput = _invertedOutput; // not implemented
        }
        
    private:
        uint8_t pin;
        uint8_t inputType;
        uint8_t debounceTime = 10;
        bool invertedOutput;
        uint32_t debounceTimer;
        bool debouncedRead;
        bool rawRead;
        bool prevRead;
        bool debug = false;

    public:
        void begin(){
            pinMode(pin, inputType);
            uint32_t beginTimer = millis();
            while(millis() - beginTimer < debounceTime * 2) update();
        }

        void update(){
            rawRead = digitalRead(pin);
            if(debouncedRead != rawRead){
                if(millis() - debounceTimer > debounceTime){
                    debouncedRead = rawRead;
                    debounceTimer = millis();
                }
            }
            else{
                debounceTimer = millis();
            }
        }

        bool read(){
            if(inputType == INPUT_PULLUP) return !debouncedRead;
            return debouncedRead;
        }

        void setDebounceTime(uint8_t d){
            debounceTime = d;
        }
};