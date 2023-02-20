//Для регулярных

#include "stm32f4xx.h"
uint32_t LED_STATE = 0xF000;
uint32_t res;
int main ()
{
RCC-> AHB1ENR | = RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN;
RCC-> APB2ENR | = RCC_APB2ENR_ADC1EN | RCC_APB2ENR_SYSCFGEN;
GPIOA-> MODER | = 0x0000000C;
GPIOD-> MODER | = 0x55000000;
GPIOD-> OTYPER | = 0;
GPIOD-> OSPEEDR | = 0;
ADC1-> SMPR2 | = (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0); // Задаем время выборки                                                                                                                                                                                
        ADC1-> CR2 | = ADC_CR2_EXTSEL; // Преобразование инжектированной группы                                                                                                          
        ADC1-> CR2 | = ADC_CR2_EXTEN; // Разрешаем внешний запуск инжектированной группы
        ADC1-> CR2 | = ADC_CR2_CONT; // Преобразования запускаются одно за другими
        ADC1-> SQR3 | = ADC_SQR3_SQ1_0; // Задаем номер канала - ADC1
        ADC1-> CR2 | = ADC_CR2_ADON; // Теперь включаем АЦП
        в то время как (1)
{
        ADC1-> CR2 | = ADC_CR2_SWSTART; // Запуск преобразований
        while (! (ADC1-> SR & ADC_SR_EOC)); // ждем пока первое преобразование завершится
        ADC1-> SR = 0; 
        res = ADC1-> DR;
        if (res> 1024)
        {
          GPIOD-> ODR = LED_STATE;
        }
        еще
        {
          GPIOD-> BSRRH | = (1 << 15) | (1 << 14) | (1 << 13) | (1 << 12);
        }
}
}

#include "stm32f4xx.h"
uint32_t LED_STATE = 0xF000;
uint32_t res;
int main ()
{
RCC-> AHB1ENR | = RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN;
RCC-> APB2ENR | = RCC_APB2ENR_ADC1EN | RCC_APB2ENR_SYSCFGEN;
GPIOA-> MODER | = 0x0000000C;
GPIOD-> MODER | = 0x55000000;
GPIOD-> OTYPER | = 0;
GPIOD-> OSPEEDR | = 0;
ADC1-> SMPR2 | = (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0); // Задаем время выборки                                                                                                                                                                                
        ADC1-> CR2 | = ADC_CR2_EXTSEL; // Преобразование инжектированной группы                                                                                                          
        ADC1-> CR2 | = ADC_CR2_EXTEN; // Разрешаем внешний запуск инжектированной группы
        ADC1-> CR2 | = ADC_CR2_CONT; // Преобразования запускаются одно за другими
        ADC1-> SQR3 | = ADC_SQR3_SQ1_0; // Задаем номер канала - ADC1
        ADC1-> CR2 | = ADC_CR2_ADON; // Теперь включаем АЦП
        в то время как (1)
{
        ADC1-> CR2 | = ADC_CR2_SWSTART; // Запуск преобразований
        while (! (ADC1-> SR & ADC_SR_EOC)); // ждем пока первое преобразование завершится
        ADC1-> SR = 0; 
        res = ADC1-> DR;
        if (res> 1024)
        {
          GPIOD-> ODR = LED_STATE;
        }
        еще
        {
          GPIOD-> BSRRH | = (1 << 15) | (1 << 14) | (1 << 13) | (1 << 12);
        }
}
}

Для инжектированных:

#include "stm32f4xx.h"
uint32_t LED_STATE = 0xF000;
uint32_t res;
int main ()
{
RCC-> AHB1ENR | = RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN;
RCC-> APB2ENR | = RCC_APB2ENR_ADC1EN | RCC_APB2ENR_SYSCFGEN;
GPIOA-> MODER | = 0x0000000C;
GPIOD-> MODER | = 0x55000000;
GPIOD-> OTYPER | = 0;
GPIOD-> OSPEEDR | = 0;
ADC1-> SMPR2 | = (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0); // Задаем время выборки                                                                                                                                                                                
        ADC1-> CR2 | = ADC_CR2_JEXTSEL; // Преобразование инжектированной группы                                                                                                          
        ADC1-> CR2 | = ADC_CR2_JEXTEN; // Разрешаем внешний запуск инжектированной группы
        ADC1-> CR2 | = ADC_CR2_CONT; // Преобразования запускаются одно за другими
        ADC1-> CR1 | = ADC_CR1_JAUTO;
        // ADC1-> CR1 | = 0x00000020;
        ADC1-> JSQR | = ADC_JSQR_JSQ4_0; // Задаем номер канала - ADC1
        ADC1-> CR2 | = ADC_CR2_ADON; // Теперь включаем АЦП
        в то время как (1)
{
        ADC1-> CR2 | = ADC_CR2_JSWSTART; // Запуск преобразований
        while (! (ADC1-> SR & ADC_SR_JEOC)); // ждем пока первое преобразование завершится
        ADC1-> SR = 0; 
        res = ADC1-> JDR1;
        if (res> 1024)
        {
          GPIOD-> ODR = LED_STATE;
        }
        еще
        {
          GPIOD-> BSRRH | = (1 << 15) | (1 << 14) | (1 << 13) | (1 << 12);
        }
}
}