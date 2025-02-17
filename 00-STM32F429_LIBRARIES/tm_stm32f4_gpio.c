/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2015
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32f4_gpio.h"

/* Private */
static GPIO_InitTypeDef GPIO_InitStruct;

/* Private functions */
static void TM_GPIO_INT_EnableClock(GPIO_TypeDef* GPIOx);
void TM_GPIO_INT_DisableClock(GPIO_TypeDef* GPIOx);

void TM_GPIO_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TM_GPIO_Mode_t GPIO_Mode, TM_GPIO_OType_t GPIO_OType, TM_GPIO_PuPd_t GPIO_PuPd, TM_GPIO_Speed_t GPIO_Speed) {	
	/* Check input */
	if (GPIO_Pin == 0x00) {
		return;
	}
	
	/* Enable clock for GPIO */
	TM_GPIO_INT_EnableClock(GPIOx);
	
	/* Fill settings */
	GPIO_InitStruct.GPIO_Mode = (GPIOMode_TypeDef) GPIO_Mode;
	GPIO_InitStruct.GPIO_OType = (GPIOOType_TypeDef) GPIO_OType;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_PuPd = (GPIOPuPd_TypeDef) GPIO_PuPd;
	GPIO_InitStruct.GPIO_Speed = (GPIOSpeed_TypeDef) GPIO_Speed;
	
	/* Init */
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void TM_GPIO_InitAlternate(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TM_GPIO_OType_t GPIO_OType, TM_GPIO_PuPd_t GPIO_PuPd, TM_GPIO_Speed_t GPIO_Speed, uint8_t Alternate) {
	uint32_t pinsource = 0, pinpos, pin;

	/* Check input */
	if (GPIO_Pin == 0x00) {
		return;
	}
	
	/* Init pin as alternate function */
	TM_GPIO_Init(GPIOx, GPIO_Pin, TM_GPIO_Mode_AF, GPIO_OType, GPIO_PuPd, GPIO_Speed);
	
	/* Set alternate functions for all pins */
	for (pinpos = 0; pinpos < 0x10; pinpos++) {
		/* Check pin */
		if ((GPIO_Pin & (1 << pinpos)) == 0) {
			continue;
		}
		
		/* Calculate pinsource from GPIO pin */
		pinsource = 0;
		for (pin = 1 << pinpos; pin > 1; pin >>= 1) {
			pinsource++;
		}
		
		/* Set alternate function */
		GPIO_PinAFConfig(GPIOx, pinsource, Alternate);
	}
}

void TM_GPIO_DeInit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	/* Fill settings */
	GPIO_InitStruct.GPIO_Mode = (GPIOMode_TypeDef) GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_OType = (GPIOOType_TypeDef) GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_PuPd = (GPIOPuPd_TypeDef) GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = (GPIOSpeed_TypeDef) GPIO_Speed_2MHz;
	
	/* Init */
	GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void TM_GPIO_SetPinAsInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	uint8_t i, pos;
	/* Go through all pins */
	for (i = 0x00; i < 0x10; i++) {
		pos = 1 << i;
		/* Pin is set */
		if (GPIO_Pin & pos) {		
			/* Clear both bits */
			GPIOx->MODER &= ~(0x03 << (2 * pos));
		}
	}
}

void TM_GPIO_SetPinAsOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	uint8_t i;
	/* Go through all pins */
	for (i = 0x00; i < 0x10; i++) {
		/* Pin is set */
		if (GPIO_Pin & (1 << i)) {
			/* Set 00 bits combination for output */
			GPIOx->MODER = (GPIOx->MODER & ~(0x03 << (2 * i))) | (0x01 << (2 * i));
		}
	}
}

uint16_t TM_GPIO_GetPortSource(GPIO_TypeDef* GPIOx) {
	uint8_t portsource = 0;
#ifdef USE_GPIOA
	if (GPIOx == GPIOA) {
		portsource = 0x00;
	}
#endif
#ifdef USE_GPIOB
	if (GPIOx == GPIOB) {
		portsource = 0x01;
	}
#endif
#ifdef USE_GPIOC
	if (GPIOx == GPIOC) {
		portsource = 0x02;
	}
#endif
#ifdef USE_GPIOD
	if (GPIOx == GPIOD) {
		portsource = 0x03;
	}
#endif
#ifdef USE_GPIOE
	if (GPIOx == GPIOE) {
		portsource = 0x04;
	}
#endif
#ifdef USE_GPIOF
	if (GPIOx == GPIOF) {
		portsource = 0x05;
	}
#endif
#ifdef USE_GPIOG
	if (GPIOx == GPIOG) {
		portsource = 0x06;
	}
#endif
#ifdef USE_GPIOH
	if (GPIOx == GPIOH) {
		portsource = 0x07;
	}
#endif
#ifdef USE_GPIOI
	if (GPIOx == GPIOI) {
		portsource = 0x08;
	}
#endif
#ifdef USE_GPIOJ
	if (GPIOx == GPIOJ) {
		portsource = 0x09;
	}
#endif
#ifdef USE_GPIOK
	if (GPIOx == GPIOK) {
		portsource = 0x0A;
	}
#endif
	
	/* Return portsource */
	return portsource;
}

uint16_t TM_GPIO_GetPinSource(uint16_t GPIO_Pin) {
	uint16_t pinsource = 0;
	
	/* Get pinsource */
	pinsource = 0;
	while (GPIO_Pin > 1) {
		pinsource++;
		GPIO_Pin >>= 1;
	}
	
	/* Return source */
	return pinsource;
}

/* Private functions */
static void TM_GPIO_INT_EnableClock(GPIO_TypeDef* GPIOx) {
#ifdef USE_GPIOA
	if (GPIOx == GPIOA) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	}
#endif
#ifdef USE_GPIOB
	if (GPIOx == GPIOB) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	}
#endif
#ifdef USE_GPIOC
	if (GPIOx == GPIOC) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	}
#endif
#ifdef USE_GPIOD
	if (GPIOx == GPIOD) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	}
#endif
#ifdef USE_GPIOE
	if (GPIOx == GPIOE) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	}
#endif
#ifdef USE_GPIOF
	if (GPIOx == GPIOF) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
	}
#endif
#ifdef USE_GPIOG
	if (GPIOx == GPIOG) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;
	}
#endif
#ifdef USE_GPIOH
	if (GPIOx == GPIOH) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOHEN;
	}
#endif
#ifdef USE_GPIOI
	if (GPIOx == GPIOI) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOIEN;
	}
#endif
#ifdef USE_GPIOJ
	if (GPIOx == GPIOJ) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;
	}
#endif
#ifdef USE_GPIOK
	if (GPIOx == GPIOK) {
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOKEN;
	}
#endif
}

void TM_GPIO_INT_DisableClock(GPIO_TypeDef* GPIOx) {
#ifdef USE_GPIOA
	if (GPIOx == GPIOA) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOAEN;
	}
#endif
#ifdef USE_GPIOB
	if (GPIOx == GPIOB) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOBEN;
	}
#endif
#ifdef USE_GPIOC
	if (GPIOx == GPIOC) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN;
	}
#endif
#ifdef USE_GPIOD
	if (GPIOx == GPIOD) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIODEN;
	}
#endif
#ifdef USE_GPIOE
	if (GPIOx == GPIOE) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOEEN;
	}
#endif
#ifdef USE_GPIOF
	if (GPIOx == GPIOF) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOFEN;
	}
#endif
#ifdef USE_GPIOG
	if (GPIOx == GPIOG) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOGEN;
	}
#endif
#ifdef USE_GPIOH
	if (GPIOx == GPIOH) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOHEN;
	}
#endif
#ifdef USE_GPIOI
	if (GPIOx == GPIOI) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOIEN;
	}
#endif
#ifdef USE_GPIOJ
	if (GPIOx == GPIOJ) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOJEN;
	}
#endif
#ifdef USE_GPIOK
	if (GPIOx == GPIOK) {
		RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOKEN;
	}
#endif
}

