#ifndef APPLICATION_H
#define APPLICATION_H

#include "potentiometer.h"
#include "pwm_led.h"

namespace app
{
    // Uses a Potentiometer to set the brightness of a PwmLed.
    class Application
    {
    public:
        Application(const hw::Potentiometer& pot, hw::PwmLed& led);

        void run();  // never returns

    private:
        // references: Application uses these, it does not own them
        const hw::Potentiometer& pot_;
        hw::PwmLed& led_;
    };
}

#endif  // APPLICATION_H
