#include <Arduino.h>

#define WELCOME_MSG_EN "Hello!. Google node is initialized and ready to use."

const char *Temperature_EN[] PROGMEM =
    {
        "The current temperature is %d",
        "It is %d degrees now",
        "Currently, it is %d degrees"};

const char *HUMIDITY_EN[] PROGMEM =
    {
        "The current temperature level is %d ",
        "It is %d %% humide now",
        "Currently, it is %d %% humide"};
