#ifndef PWMFAN_H
#define PWMFAN_H

#define DEFAULT_PWM_PIN TIMER1_A_PIN // default pin for pwm
#define DUTY_OFF 0                   // duty 0%
#define DEFAULT_DUTY_MIN 20          // pwm spec : nothing ! arbitrary 20%
#define DEFAULT_DUTY_MAX 100         // pwm spec : max is 100%
#define NO_SWITCH 0                  // if no switch present (mosfet)
#define DEBOUNCING_INTERVAL 6        // interval to debounce interrupts
#define DEFAULT_HYSTERESIS 1.0

// Timer 1 controls pwm on pins 9 and 10 (Arduino Uno).
#include <TimerOne.h>

class PwmFan
{
    public:
        PwmFan ();
        /**
         * @brief
         * @param pinPwm pin to write to
         * @param dutyCycleMin minimal duty cycle
         * @param dutyCycleMax maximal duty cycle
         * @param hysteresis temperature difference between start and stop
         */
        void begin ( uint8_t pinPwm, uint8_t pinSwitch, uint8_t dutyCycleMin, uint8_t dutyCycleMax, float hysteresis );
        /**
         * @brief
         * @param pinPwm pin to write to
         * @param dutyCycleMin minimal duty cycle
         * @param dutyCycleMax maximal duty cycle
         */
        void begin ( uint8_t pinPwm, uint8_t pinSwitch, uint8_t dutyCycleMin, uint8_t dutyCycleMax );
        /**
         * @brief maximum duty cycle is 100%
         * @param pinPwm pin to write to
         * @param dutyCycleMin minimal duty cycle
         */
        void begin ( uint8_t pinPwm, uint8_t pinSwitch, uint8_t dutyCycleMin );
        /**
         * @brief minimum and maximum duty cycles are 0% and 100%
         * @param pinPwm pin to write to
         */
        void begin ( uint8_t pinPwm, uint8_t pinSwitch );
        /**
          * @brief assumes pin is TIMER1_A_PIN and minimum and maximum duty cycles are 0% and 100%
          */
        void begin ();
        void end ();
        void thresoldsWrite ( float tempMin, float tempMax );
        void temperatureWrite ( float temperature );
        void verbose ();
        void silent ();
        void hysteresis ( float hysteresis );
        void rpmCount ();
        void rpmTop ();
        /**
         * @brief not yet implemented
         */
        uint16_t rpmRead ();

    private:
        // properties
        uint8_t _pinPwm;            // pin where fan control wire (pwm) is connected
        uint8_t _pinTach;           // tachometer pin
        uint8_t _pinSwitch;         // pin connected to mosfet
        uint8_t _dutyCyclePcMin;    // minimum duty cycle in %
        uint8_t _dutyCyclePcMax;    // maximum duty cycle in %
        uint16_t _dutyCycleMin;     // minimum duty cycle inside [0;1023] range
        uint16_t _dutyCycleMax;     // maximum duty cycle inside [0;1023] range
        float _tempMin;             // minimum thresold of temperature
        float _tempMax;             // maximum thresold of temperature
        float _temperature;         // current temperature
        float _hysteresis;          // hysteresis difference
        uint16_t _dutyCycle;        // current duty cycle
        bool _initialized;          // is the class initialized ?
        bool _boundsSet;            // are bounds set ?
        bool _temperatureSet;       // is temperature set ?
        bool _verbose;              // verbose mode ?
        bool _started;              // switch on ? (fan of type A)
        volatile uint16_t _top;     // nb tops of hall effect sensor
        volatile uint16_t _top1sec; // nb tops of hall effect sensor in 1 second
        volatile uint16_t _rpm;     // RPM
        // volatile uint32_t _last_top_interrupt_time= 0;
        // methods
        void _reset (); // reset configuration
        void _begin ( uint8_t pinPwm, uint8_t pinSwitch, uint8_t dutyCycleMin, uint8_t dutyCycleMax, float hysteresis );
        void _start ();
        void _stop ();
        void _refresh ();
        uint16_t _mapfloat ( float value, float in_min, float in_max, uint16_t out_min, uint16_t out_max );
};

#endif // PWMFAN_H
