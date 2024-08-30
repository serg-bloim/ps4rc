#pragma once
#include <Arduino.h>
#include "List.hpp"

class Blinker
{
private:
  static const size_t pattern_size = 100;
  size_t pin;
  StaticList<int, pattern_size> pattern;
  int* pattern_iter = pattern.begin();

  size_t next_ts = millis();
  bool state = false;

public:
  Blinker(size_t pin) : pin(pin) {}
  void begin(){
    pinMode(pin, OUTPUT);
  }

  template <size_t N>
  void set_pattern(const int (&p_pattern)[N]){
    int n = min(N, pattern_size);
    this->pattern.clear();
    for(int i = 0; i < n; i++){
       this->pattern.add(p_pattern[i]);
    }
    this->pattern_iter = pattern.begin();
    this->state = false;
    this->next_ts = millis() + *(this->pattern_iter);
    digitalWrite(pin, state);
  }

  void update(){
    if(millis() > next_ts){
      this->pattern_iter++;
      state = !state;
      if (this->pattern_iter == this->pattern.end()){
        this->pattern_iter = this->pattern.begin();
        state = false;
      }
      this->next_ts += *(this->pattern_iter);
      digitalWrite(pin, state);
    }
  }
};