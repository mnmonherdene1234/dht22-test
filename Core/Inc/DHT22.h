/*
 * DHT22.h
 *
 *  Created on: Mar 5, 2024
 *      Author: mn
 */

#ifndef INC_DHT22_H_
#define INC_DHT22_H_

#include "stm32f1xx_hal.h"

#define DHT22_PORT GPIOB
#define DHT22_PIN GPIO_PIN_9

extern float DHT22_Celsius;
extern float DHT22_Fahrenheit;
extern float DHT22_Humidity;

void DHT22_Init(TIM_HandleTypeDef *htim);
uint8_t DHT22_Start(void);
uint8_t DHT22_Read(void);
void DHT22_Loop();
void DHT22_MicroDelay(uint16_t delay);

#endif /* INC_DHT22_H_ */
