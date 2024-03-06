/*
 * DHT22.c
 *
 *  Created on: Mar 5, 2024
 *      Author: mn
 */
#include "DHT22.h"

TIM_HandleTypeDef *DHT22_TIM;
uint8_t RH1, RH2, TC1, TC2, SUM, CHECK;
uint32_t pMillis, cMillis;
float DHT22_Celsius = 0;
float DHT22_Fahrenheit = 0;
float DHT22_Humidity = 0;

// 	HAL_TIM_Base_Start(&htim1);
void DHT22_Init(TIM_HandleTypeDef *htim){
	DHT22_TIM = htim;
}

void DHT22_MicroDelay(uint16_t delay) {
	__HAL_TIM_SET_COUNTER(DHT22_TIM, 0);
	while (__HAL_TIM_GET_COUNTER(DHT22_TIM) < delay)
		;
}

uint8_t DHT22_Start(void) {
	uint8_t Response = 0;
	GPIO_InitTypeDef GPIO_InitStructPrivate = { 0 };
	GPIO_InitStructPrivate.Pin = DHT22_PIN;
	GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStructPrivate); // set the pin as output
	HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, 0);   // pull the pin low
	DHT22_MicroDelay(1300);   // wait for 1300us
	HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, 1);   // pull the pin high
	DHT22_MicroDelay(30);   // wait for 30us
	GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DHT22_PORT, &GPIO_InitStructPrivate); // set the pin as input
	DHT22_MicroDelay(40);
	if (!(HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN))) {
		DHT22_MicroDelay(80);
		if ((HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN)))
			Response = 1;
	}
	pMillis = HAL_GetTick();
	cMillis = HAL_GetTick();
	while ((HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN)) && pMillis + 2 > cMillis) {
		cMillis = HAL_GetTick();
	}
	return Response;
}

uint8_t DHT22_Read(void) {
	uint8_t a, b;
	for (a = 0; a < 8; a++) {
		pMillis = HAL_GetTick();
		cMillis = HAL_GetTick();
		while (!(HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN))
				&& pMillis + 2 > cMillis) {  // wait for the pin to go high
			cMillis = HAL_GetTick();
		}
		DHT22_MicroDelay(40);   // wait for 40 us
		if (!(HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN)))   // if the pin is low
			b &= ~(1 << (7 - a));
		else
			b |= (1 << (7 - a));
		pMillis = HAL_GetTick();
		cMillis = HAL_GetTick();
		while ((HAL_GPIO_ReadPin(DHT22_PORT, DHT22_PIN))
				&& pMillis + 2 > cMillis) {  // wait for the pin to go low
			cMillis = HAL_GetTick();
		}
	}
	return b;
}

void DHT22_Loop(){
	if (DHT22_Start()) {
		RH1 = DHT22_Read(); // First 8bits of humidity
		RH2 = DHT22_Read(); // Second 8bits of Relative humidity
		TC1 = DHT22_Read(); // First 8bits of Celsius
		TC2 = DHT22_Read(); // Second 8bits of Celsius
		SUM = DHT22_Read(); // Check sum
		CHECK = RH1 + RH2 + TC1 + TC2;
		if (CHECK == SUM) {
			if (TC1 > 127) // If TC1=10000000, negative temperature
					{
				DHT22_Celsius = (float) TC2 / 10 * (-1);
			} else {
				DHT22_Celsius = (float) ((TC1 << 8) | TC2) / 10;
			}
			DHT22_Fahrenheit = DHT22_Celsius * 9 / 5 + 32;
			DHT22_Humidity = (float) ((RH1 << 8) | RH2) / 10;
		}
	}
	HAL_Delay(1000);
}
