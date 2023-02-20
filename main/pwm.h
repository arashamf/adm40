//
// pwm.h
//

#include <stdint.h>
#include <stdbool.h>

#ifndef _PWM_H
#define _PWM_H

    // ������������� ������
    void pwm_Init(void);

    // ������ ����������
    void pwm_Set(uint16_t Value);

    // �������� time ����.
    void beep(int time);

#endif
