#pragma once

#include "usings.h"
#include <Preferences.h>
#include "SerialLineReader.hpp"

class Configurator{
private:
    Preferences prefs;
    uint8_t pinOut;
    uint8_t pinIn;
    bool _enabled = false;
    SerialLineReader line_reader;
    bool check_status(){
        if(digitalRead(pinIn) != LOW) return false;
        digitalWrite(pinOut, HIGH);
        if(digitalRead(pinIn) != HIGH) return false;
        digitalWrite(pinOut, LOW);
        if(digitalRead(pinIn) != LOW) return false;
        return true;
    }
    void processSetCmd(String &line){
        int eqPos = line.indexOf('=');
        if(eqPos == -1){
            Serial.printf("Cannot parse cmd '%s'\n", line);
        }
        String key = line.substring(4, eqPos);
        String val = line.substring(eqPos+1);
        String old = prefs.getString(key.c_str());
        Serial.printf("Setting '%s': '%s' -> '%s'\n", key.c_str(), old.c_str(), val.c_str());
        prefs.putString(key.c_str(), val.c_str());
    }
public:
    Configurator(Preferences &prefs, uint8_t pinOut, uint8_t pinIn): prefs(prefs), pinOut(pinOut), pinIn(pinIn), line_reader(Serial){
        pinMode(pinOut, OUTPUT);
        pinMode(pinIn, INPUT);
    }
    bool enabled(){
        return _enabled;
    }
    void begin(){
        this->_enabled = this->check_status();
        if(this->_enabled){
            prefs.begin("app", RW_MODE);
        }
        end();
    }
    void end(){
        pinMode(pinOut, INPUT);
    }
    void update(){
        if(line_reader.available()){
             String line = line_reader.readLine();
             line.trim();
             if(!line.isEmpty()){
               if(line.startsWith("set ")){
                    processSetCmd(line);
               }
             }
           }
    }
};