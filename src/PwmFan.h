#ifndef PWMFAN_H
#define PWMFAN_H

#define DEFAULT_PWM_PIN TIMER1_A_PIN
#define DUTY_OFF 0
#define DEFAULT_DUTY_MIN 20  // pwm spec : nothing ! designers seems to choose around 20%
#define DEFAULT_DUTY_MAX 100 // pwm spec : max is 100%

#include <TimerOne.h>

class PwmFan
{
  public:
    PwmFan ();
    ~PwmFan ();
    /**
     * @brief
     * @param pin pin to write to
     * @param dutyCycleMin minimal duty cycle
     * @param dutyCycleMax maximal duty cycle
     */
    void begin ( char pin, unsigned int dutyCycleMin, unsigned int dutyCycleMax );
    /**
     * @brief maximum duty cycle is 100%
     * @param pin pin to write to
     * @param dutyCycleMin minimal duty cycle
     */
    void begin ( char pin, unsigned int dutyCycleMin );
    /**
     * @brief minimum and maximum duty cycles are 0% and 100%
     * @param pin pin to write to
     */
    void begin ( char pin );
    /**
      * @brief assumes pin is TIMER1_A_PIN and minimum and maximum duty cycles are 0% and 100%
      */
    void begin ();
    void end ();
    void thresoldsWrite ( float tempMin, float tempMax );
    void temperatureWrite ( float temperature );
    /**
     * @brief not yet implemented
     */
    unsigned int rpmRead ();
    void verbose ();
    void silent ();

  private:
    // properties
    char _pin;                      // pin where fan control wire (pwm) is connected
    unsigned int _dutyCyclePcMin;   // minimum duty cycle in %
    unsigned int _dutyCyclePcMax;   // maximum duty cycle in %
    unsigned int _dutyCycleMin;     // minimum duty cycle inside [0;1023] range
    unsigned int _dutyCycleMax;     // maximum duty cycle inside [0;1023] range
    float _tempMin;                 // minimum thresold of temperature
    float _tempMax;                 // maximum thresold of temperature
    float _temperature;             // current temperature
    unsigned int _dutyCycle;        // current duty cycle
    bool _initialized;              // is the class initialized ?
    bool _boundsSet;                // are bounds set ?
    bool _temperatureSet;           // is temperature set ?
    bool _verbose;                  // verbose mode ?
    bool _started;                  // switch on ? (fan of type A)
    volatile unsigned int _millis;  // when counter started
    volatile unsigned int _counter; // nb tops of hall effect sensor
    // methods
    void _reset (); // reset configuration
    void _begin ( char pin, unsigned int dutyCycleMin, unsigned int dutyCycleMax );
    void _start ();
    void _stop ();
    void _refresh ();
    unsigned int _mapfloat ( float value, float in_min, float in_max, unsigned int out_min, unsigned int out_max );
};

#endif // PWMFAN_H
