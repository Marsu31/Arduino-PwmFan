#include "Arduino.h"
#include "PwmFan.h"

PwmFan::PwmFan ()
{
    _reset ();
}

void PwmFan::begin ( uint8_t pinPwm, uint8_t pinSwitch, uint8_t dutyCycleMin, uint8_t dutyCycleMax, float hysteresis )
{
    _begin ( pinPwm, pinSwitch, dutyCycleMin, dutyCycleMax, hysteresis );
}

void PwmFan::begin ( uint8_t pinPwm, uint8_t pinSwitch, uint8_t dutyCycleMin, uint8_t dutyCycleMax )
{
    _begin ( pinPwm, pinSwitch, dutyCycleMin, dutyCycleMax, DEFAULT_HYSTERESIS );
}

void PwmFan::begin ( uint8_t pinPwm, uint8_t pinSwitch, uint8_t dutyCycleMin )
{
    _begin ( pinPwm, pinSwitch, dutyCycleMin, DEFAULT_DUTY_MAX, DEFAULT_HYSTERESIS );
}

void PwmFan::begin ( uint8_t pinPwm, uint8_t pinSwitch )
{
    _begin ( pinPwm, pinSwitch, DEFAULT_DUTY_MIN, DEFAULT_DUTY_MAX, DEFAULT_HYSTERESIS );
}
void PwmFan::begin ()
{
    _begin ( DEFAULT_PWM_PIN, NO_SWITCH, DEFAULT_DUTY_MIN, DEFAULT_DUTY_MAX, DEFAULT_HYSTERESIS );
}

void PwmFan::end ()
{
    Serial.println ( F ( "PwmFan : end" ) );

    // stop fan
    _stop ();
    // reset configuration
    _reset ();

    // TODO : remove from fan list
}

void PwmFan::thresoldsWrite ( float tempMin, float tempMax )
{
    if ( _verbose ) {
        Serial.print ( F ( "PwmFan : thresoldsWrite(" ) );
        Serial.print ( tempMin );
        Serial.print ( F ( "," ) );
        Serial.print ( tempMax );
        Serial.println ( F ( ")" ) );
    }

    if ( tempMin < tempMax ) {
        _boundsSet = true;

        if ( tempMin != _tempMin || tempMax != _tempMax ) {
            if ( _verbose ) {
                Serial.println ( F ( "PwmFan : thresold change" ) );
            }

            _tempMin = tempMin;
            _tempMax = tempMax;
            _refresh ();
        }
    }
}

void PwmFan::temperatureWrite ( float temperature )
{
    if ( _verbose ) {
        Serial.print ( F ( "PwmFan : temperatureWrite(" ) );
        Serial.print ( temperature );
        Serial.println ( F ( ")" ) );
    }

    _temperatureSet = true;

    if ( temperature != _temperature ) {
        if ( _verbose ) {
            Serial.println ( F ( "PwmFan : temperature change" ) );
        }

        _temperature = temperature;
        _refresh ();
    }
}

uint16_t PwmFan::rpmRead ()
{
    _rpm = _top1sec * 30; // x 60 sec / 2 hall effect sensors (from pwm specification chap 2.1.3)
    return _rpm;
}

void PwmFan::verbose ()
{
    _verbose = true;
}

void PwmFan::silent ()
{
    _verbose = false;
}


void PwmFan::hysteresis ( float hysteresis )
{
    // TODO : test values
    _hysteresis = hysteresis;
}

void PwmFan::_reset ()
{
    _pinPwm = DEFAULT_PWM_PIN;
    _hysteresis = DEFAULT_HYSTERESIS;
    _dutyCyclePcMin = DEFAULT_DUTY_MIN;
    _dutyCyclePcMax = DEFAULT_DUTY_MAX;
    _dutyCycleMin = lround ( ( double ) DEFAULT_DUTY_MIN * 1023.0 / 100.0 );
    _dutyCycleMax = 1023;
    _tempMin = 0;
    _tempMax = 100;
    _temperature = 20;
    _dutyCycle = DUTY_OFF;
    _initialized = false;
    _boundsSet = false;
    _temperatureSet = false;
    _verbose = false;
    _started = false;
    _top = 0;
    _top1sec = 0;
    _rpm = 0;
}

void PwmFan::_begin ( uint8_t pinPwm, uint8_t pinSwitch, uint8_t dutyCycleMin, uint8_t dutyCycleMax, float hysteresis )
{
    Serial.println ( F ( "PwmFan : begin" ) );
    Serial.println ( F ( "PwmFan : version 1.1.5" ) );
    Serial.print ( F ( "PwmFan : pwm digital pin number : " ) );
    Serial.println ( pinPwm );
    Serial.print ( F ( "PwmFan : switch digital pin number (0=none) : " ) );
    Serial.println ( pinSwitch );
    Serial.print ( F ( "PwmFan : min duty cycle : " ) );
    Serial.print ( dutyCycleMin );
    Serial.println ( F ( "%" ) );
    Serial.print ( F ( "PwmFan : max duty cycle : " ) );
    Serial.print ( dutyCycleMax );
    Serial.println ( F ( "%" ) );

    _reset ();

    _pinPwm = pinPwm;
    _hysteresis = hysteresis;

    if ( dutyCycleMin < dutyCycleMax ) {
        _dutyCyclePcMin = max ( dutyCycleMin, 0 );
        _dutyCyclePcMax = min ( dutyCycleMax, 100 );
        // translate percent to integer understood by pwn function
        _dutyCycleMin = lround ( ( double ) _dutyCyclePcMin * 1023 / 100 );
        _dutyCycleMax = lround ( ( double ) _dutyCyclePcMax * 1023 / 100 );
        //        if ( _verbose ) {
        Serial.print ( F ( "PwmFan : internal duty cycle min : " ) );
        Serial.print ( _dutyCyclePcMin );
        Serial.print ( F ( "% => " ) );
        Serial.print ( _dutyCycleMin );
        Serial.print ( F ( ", internal duty cycle max : " ) );
        Serial.print ( _dutyCyclePcMax );
        Serial.print ( F ( "% => " ) );
        Serial.println ( _dutyCycleMax );
        //        }
    }

    _pinSwitch = pinSwitch;

    if ( pinSwitch != NO_SWITCH ) {
        pinMode ( pinSwitch, OUTPUT );
    }

    Timer1.initialize ( 40 );         // pwm spec says 25 kHz => 40 µs
    Timer1.pwm ( _pinPwm, DUTY_OFF ); // apply configuration

    _initialized = true;
}

void PwmFan::_start ()
{
    if ( !_started ) {
        if ( _verbose ) {
            Serial.println ( F ( "PwmFan : start" ) );
        }

        // switch on
        if ( _pinSwitch != NO_SWITCH ) {
            digitalWrite ( _pinSwitch, HIGH );
        }

        _started = true;
    }
}

void PwmFan::_stop ()
{
    if ( _started ) {
        if ( _verbose ) {
            Serial.println ( F ( "PwmFan : stop" ) );
        }

        // duty 0
        Timer1.setPwmDuty ( _pinPwm, DUTY_OFF );

        // switch off
        if ( _pinSwitch != NO_SWITCH ) {
            digitalWrite ( _pinSwitch, LOW );
        }

        _started = false;
    }
}

void PwmFan::_refresh ()
{

    if ( _verbose ) {
        Serial.println ( F ( "PwmFan : refresh" ) );
    }

    // do something if class is initialized (call of begin()) and all parameters are set (bounds and temperature)
    if ( _initialized && _boundsSet && _temperatureSet ) {
        uint16_t dutyCycle;

        // make fan rotate if minimal thresold reached
        if ( _temperature >= _tempMin ) {
            _start ();

            if ( _temperature >= _tempMax ) {
                dutyCycle = _dutyCycleMax;

                if ( _verbose ) {
                    Serial.println ( F ( "PwmFan : temperature > max" ) );
                    Serial.print ( F ( "PwmFan : internal duty cycle min : " ) );
                    Serial.print ( _dutyCycleMin );
                    Serial.print ( F ( ", internal duty cycle max : " ) );
                    Serial.print ( _dutyCycleMax );
                    Serial.print ( F ( ", internal duty : " ) );
                    Serial.println ( dutyCycle );
                }

            } else {
                if ( _verbose ) {
                    Serial.println ( F ( "PwmFan : calc duty" ) );
                }

                dutyCycle = _mapfloat ( _temperature, _tempMin, _tempMax, _dutyCycleMin, _dutyCycleMax );
            }


        } else {
            // hysteresis : let temperature decrease from 'hysteresis' degrees
            if ( _started && _temperature >= ( _tempMin - _hysteresis ) ) {
                dutyCycle = _dutyCycleMin;

            } else {
                // TODO : don't stop fan if started less than 15min ago and t° ~ t°min (time for t° to increase and manage
                // sensor's accuracy)
                _stop();
            }
        }

        if ( _started ) {
            // maj of fan speed
            //            Serial.print ( F ( "PwmFan : " ) );
            //            Serial.print ( F ( "duty cycle min : " ) );
            //            Serial.print ( _dutyCycleMin );
            //            Serial.print ( F ( ", duty cycle max : " ) );
            //            Serial.print ( _dutyCycleMax );
            //            Serial.print ( F ( ", apply duty : " ) );
            //            Serial.println ( dutyCycle );
            Timer1.setPwmDuty ( _pinPwm, dutyCycle );
        }

        // duty cycle has changed
        if ( dutyCycle != _dutyCycle ) {
            _dutyCycle = dutyCycle;

            if ( _verbose ) {
                Serial.print ( F ( "PwmFan : " ) );
                Serial.print ( F ( "t° thresold min : " ) );
                Serial.print ( _tempMin );
                Serial.print ( F ( ", t° thresold max : " ) );
                Serial.print ( _tempMax );
                Serial.print ( F ( ", t° : " ) );
                Serial.print ( _temperature );
                Serial.print ( F ( ", RPM : " ) );
                Serial.print ( _rpm );
                Serial.print ( F ( ", duty cycle min : " ) );
                Serial.print ( _dutyCyclePcMin );
                Serial.print ( F ( "%, duty cycle max : " ) );
                Serial.print ( _dutyCyclePcMax );
                Serial.print ( F ( "%, duty cycle : " ) );
                Serial.print ( lround ( ( double ) _dutyCycle * 100.0 / 1023.0 ) );
                Serial.println ( F ( "%" ) );
            }
        }
    }
}

void PwmFan::rpmTop ()
{
    _top++;
}

void PwmFan::rpmCount ()
{
    noInterrupts ();
    _top1sec = _top;
    _top = 0;
    interrupts ();
}

uint16_t PwmFan::_mapfloat ( float value, float fromLow, float fromHigh, uint16_t toLow, uint16_t toHigh )
{
    if ( _verbose ) {
        Serial.println ( F ( "PwmFan : _mapfloat" ) );
    }

    return ( uint16_t ) lround ( ( double ) ( value - fromLow ) * ( toHigh - toLow ) / ( fromHigh - fromLow ) + toLow );
}
