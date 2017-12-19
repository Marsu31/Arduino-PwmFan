# Arduino-PwmFan

Arduino library to control pwm fan with temperature.


The initial goal was to refresh a computer rack. Designed for 3 or 4 pins pwm fans, it allows you to control fan speed according to the temperature inside a range.


## Mechanics

[PWM specification](http://www.formfactors.org/developer/specs/4_Wire_PWM_Spec.pdf) says about PWN Input Control Signal :
> PWM Frequency: Target frequency 25 kHz acceptable operational range 21 kHz to 28 kHz

The easiest way to do this is to use the great [TimeOne library](https://www.pjrc.com/teensy/td_libs_TimerOne.html).


## Usage

### API (class PwmFan)
#### begin()

##### Description

Set the pin where PWM fan is plugged in and the duty cycle range.

##### Syntax

`void begin()`

`void begin(char pin)`

`void begin(char pin, unsigned int dutyCycleMin)`

`void begin(char pin, unsigned int dutyCycleMin, unsigned int dutyCycleMax)`

##### Parameters

* pin: pin to write to. If not set assumes `TIMER1_A_PIN`.
* dutyCycleMin: minimal duty cycle. If not set minimum duty cycle is 20%.
* dutyCycleMax: maximal duty cycle. If not set maximum duty cycle is 100%.



### Which pin ?

As this library uses Timer 1, available pins are limited. Check the [TimeOne library](https://www.pjrc.com/teensy/td_libs_TimerOne.html) to list them.
The [known_16bit_timers.h](https://github.com/PaulStoffregen/TimerOne/blob/master/config/known_16bit_timers.h) header file is more complete even if more unreadable ;-) Note that PwnFan use by default TIMER1_A_PIN.


### Example

For example, with Noctua NF-A14 IPPC-3000 PWM, the duty cycle comes from 25% to 100%. I want the fan to start at 15 °C and reach its maximum speed at 25 °C.

~~~ cpp
PwmFan pwmFan;

setup() {
  pwmFan.begin(9,25,100);     // pin, range of duty (min, max)
  pwmFan.thresoldsWrite(15,25); // range of temperature (min, max)
}

loop () {

  // read temperature from sensor ...
  float temperature = ...

  pwmFan.temperatureWrite(temperature);

}
~~~

## Tested with

* Arduino Uno

## Compilation

Compilation with **avr-g++**, use these options:

`-Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -flto -mmcu=atmega328p -g -DF_CPU=16000000L -DARDUINO=10805 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR`

## Incoming

- retrieve RPM value from tachymeter, use of [MsTimer2](http://playground.arduino.cc/Main/MsTimer2) library,
- control fans of type A (minimum RPM at 0%, need a switch to start/stop the fan), need feedback
- control multiple fans.
