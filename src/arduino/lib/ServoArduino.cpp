#include "ServoArduino.h"

#include "OmniThing.h"
#include <string.h>
#include "frozen.h"

namespace omni
{
//private
    void ServoArduino::writeFloatNoRevert(float percent)
    {
        m_Servo.attach(m_nPin);
        if(percent < 0.f)
            percent = 0;
        if(percent > 100.f)
            percent = 100;

        int angle = percent*180/100.f;
        m_Servo.write(angle);
        LOG << F("Setting Servo to angle: ") << angle << Logger::endl;

        OmniThing::getInstance().addTrigger(this, 200, Cmd_Detach, false);
    }

    char* ServoArduino::Cmd_Detach = "detach";
    char* ServoArduino::Cmd_Revert = "revert";
//protected
//public
    ServoArduino::ServoArduino(unsigned short pin, float initialPercent, bool revert, unsigned long revertTime) :
        m_Servo(),
        m_nPin(pin),
        m_fInitial(initialPercent),
        m_bRevert(revert),
        m_nRevertTime(revertTime)
    {
        m_Servo.attach(pin);
        writeFloat(initialPercent);
    }

    ServoArduino::~ServoArduino()
    {

    }

    void ServoArduino::writeFloat(float percent)
    {
        writeFloatNoRevert(percent);

        if(m_bRevert) // set up revert trigger
        {
            OmniThing::getInstance().addTrigger(this, m_nRevertTime, Cmd_Revert, false);
        }
    }

    void ServoArduino::trigger(void* arg)
    {
        char* cmd = static_cast<char*>(arg);
        
        if(!strcmp(cmd, Cmd_Detach))
        {
            m_Servo.detach();
        }
        else if(!strcmp(cmd, Cmd_Revert))
        {
            writeFloatNoRevert(m_fInitial);     
        }
    }

    OutputFloat* ServoArduino::createFromJson(const char* json)
    {
        unsigned int len = strlen(json);
        unsigned short pin;
        float initialPercent = 50.f;
        bool revert = false;
        unsigned long revertTime;

        if(json_scanf(json, len, "{pin: %u}", &pin) != 1)
        {
            return nullptr;
        }
                
        if(json_scanf(json, len, "{revertTime: %lu}", &revertTime) == 1)
        {
            revert = true;
        }

        json_scanf(json, len, "{initialPercent: %f}", &initialPercent); // optional param

        return new ServoArduino(pin, initialPercent, revert, revertTime);
    }

    const char* ServoArduino::Type = "ServoArduino";
    ObjectConfig<OutputFloat> ServoArduino::OutputFloatConf(Type, createFromJson);

}