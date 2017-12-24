#include "Arduino.h"
#include "PwmFan.h"

PwmFan::PwmFan ()
{
    _reset ();
}

void PwmFan::begin ( char pin, unsigned int dutyCycleMin, unsigned int dutyCycleMax )
{
    _begin ( pin, dutyCycleMin, dutyCycleMax );
}

void PwmFan::begin ( char pin, unsigned int dutyCycleMin )
{
    _begin ( pin, dutyCycleMin, DEFAULT_DUTY_MAX );
}

void PwmFan::begin ( char pin )
{
    _begin ( pin, DEFAULT_DUTY_MIN, DEFAULT_DUTY_MAX );
}
void PwmFan::begin ()
{
    _begin ( DEFAULT_PWM_PIN, DEFAULT_DUTY_MIN, DEFAULT_DUTY_MAX );
}

void PwmFan::end ()
{
    // stop fan
    _stop ();
    // reset configuration
    _reset ();

    // TODO : remove from fan list
}

void PwmFan::thresoldsWrite ( float tempMin, float tempMax )
{
    if ( tempMin < tempMax ) {
        _boundsSet= true;
        if ( tempMin != _tempMin || tempMax != _tempMax ) {
            Serial.println ( F ( "thresold change" ) );
            _tempMin= tempMin;
            _tempMax= tempMax;
            _refresh ();
        }
    }
}

void PwmFan::temperatureWrite ( float temperature )
{
    _temperatureSet= true;
    if ( temperature != _temperature ) {
        Serial.println ( F ( "temperature change" ) );
        _temperature= temperature;
        _refresh ();
    }
}

unsigned int PwmFan::rpmRead ()
{
    return -1;
}

void PwmFan::verbose ()
{
    _verbose= true;
}

void PwmFan::silent ()
{
    _verbose= false;
}

PwmFan::~PwmFan ()
{
}

void PwmFan::_reset ()
{
    _pin= DEFAULT_PWM_PIN;
    _dutyCyclePcMin= DEFAULT_DUTY_MIN;
    _dutyCyclePcMax= DEFAULT_DUTY_MAX;
    _dutyCycleMin= 0;
    _dutyCycleMax= 1023;
    _tempMin= 0;
    _tempMax= 100;
    _temperature= 20;
    _dutyCycle= 0;
    _initialized= false;
    _boundsSet= false;
    _temperatureSet= false;
    _verbose= false;
    _started= false;
    _millis= 0;
    _counter= 0;
}

void PwmFan::_begin ( char pin, unsigned int dutyCycleMin, unsigned int dutyCycleMax )
{
    _pin= pin;
    if ( dutyCycleMin < dutyCycleMax ) {
        _dutyCyclePcMin= max ( dutyCycleMin, 0 );
        _dutyCyclePcMax= min ( dutyCycleMax, 100 );
        // translate percent to integer understood by pwn function
        _dutyCycleMin= roundf ( _dutyCyclePcMin * 1023.0 / 100.0 );
        _dutyCycleMax= roundf ( _dutyCyclePcMax * 1023.0 / 100.0 );
    }

    Timer1.initialize ( 40 );      // pwm spec says 25 kHz => 40 µs
    Timer1.pwm ( _pin, DUTY_OFF ); // apply configuration

    _initialized= true;
}

void PwmFan::_start ()
{
    if ( !_started ) {
        // switch on
        // TODO
        _started= true;
    }
}

void PwmFan::_stop ()
{
    if ( _started ) {
        // duty 0
        Timer1.setPwmDuty ( _pin, DUTY_OFF );
        // switch off
        // TODO
        _started= false;
    }
}

void PwmFan::_refresh ()
{

    // do something if class is initialized (call of begin()) and all parameters are set (bounds and temperature)
    if ( _initialized && _boundsSet && _temperatureSet ) {
        unsigned int dutyCycle;

        // make fan rotate if minimal thresold reached
        if ( _temperature >= _tempMin ) {
            if ( _temperature >= _tempMax ) {
                dutyCycle= _dutyCycleMax;
            } else {
                dutyCycle= _mapfloat ( _temperature, _tempMin, _tempMax, _dutyCycleMin, _dutyCycleMax );
            }

        } else {
            // TODO : don't stop fan if started less than 15min ago and t° ~ t°min (time for t° to increase and manage
            // sensor's accuracy)
            dutyCycle= 0;
        }

        if ( dutyCycle != _dutyCycle ) {
            _dutyCycle= dutyCycle;
            // duty cycle change : maj of fan speed
            Timer1.setPwmDuty ( _pin, _dutyCycle );
            if ( _verbose ) {
                Serial.print ( F ( "t° thresold min : " ) );
                Serial.print ( _tempMin );
                Serial.print ( F ( ", t° thresold max : " ) );
                Serial.print ( _tempMax );
                Serial.print ( F ( ", t° : " ) );
                Serial.print ( _temperature );
                Serial.print ( F ( ", RPM : N/A" ) );
                Serial.print ( F ( ", duty cycle min : " ) );
                Serial.print ( _dutyCyclePcMin );
                Serial.print ( F ( "%, duty cycle max : " ) );
                Serial.print ( _dutyCyclePcMax );
                Serial.print ( F ( "%, duty cycle : " ) );
                Serial.print ( roundf ( _dutyCycle * 100.0 / 1023.0 ) );
                Serial.println ( F ( "%" ) );
            }
        }
    }
}

unsigned int PwmFan::_mapfloat ( float value, float fromLow, float fromHigh, unsigned int toLow, unsigned int toHigh )
{
    return (unsigned int)roundf ( value - fromLow ) * ( toHigh - toLow ) / ( fromHigh - fromLow ) + toLow;
}
