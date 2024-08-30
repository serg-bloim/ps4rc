#pragma once
#include <Arduino.h>
#include "List.hpp"

namespace BlinkLibrary{
    /*
    These patterns define intervals how long LED stays on and off.
    First number defines the first OFF duration, 2nd - the first ON duration, 3rd - the second OFF, 4th - the second ON and so on.
    */
    const int NORMAL_DISCONNECTED[] = {1500,50,100, 50};
    const int NORMAL_CONNECTED[]    = {300,300};
    const int CONFIGURATION[]       = {100,100,100,100,100,800};
}