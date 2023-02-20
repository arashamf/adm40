//
// pwm.h
//

#include <stdint.h>
#include <stdbool.h>

#ifndef _PWM_H
#define _PWM_H

    // Инициализация модуля
    void pwm_Init(void);

    // Задать скважность
    void pwm_Set(uint16_t Value);

    // Попищать time мсек.
    void beep(int time);

#endif
