/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
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
#include "tm_stm32f4_i2c.h"

static uint32_t TM_I2C_Timeout;
static uint32_t TM_I2C_INT_Clocks[3] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

/* Private functions */
void TM_I2C1_INT_InitPins(TM_I2C_PinsPack_t pinspack);
void TM_I2C2_INT_InitPins(TM_I2C_PinsPack_t pinspack);
void TM_I2C3_INT_InitPins(TM_I2C_PinsPack_t pinspack);

void TM_I2C_Init(I2C_TypeDef* I2Cx, TM_I2C_PinsPack_t pinspack, uint32_t clockSpeed) {
	I2C_InitTypeDef I2C_InitStruct;
	
	if (I2Cx == I2C1) {
		/* Enable clock */
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
		
		/* Enable pins */
		TM_I2C1_INT_InitPins(pinspack);
		
		/* Check clock */
		if (clockSpeed < TM_I2C_INT_Clocks[0]) {
			TM_I2C_INT_Clocks[0] = clockSpeed;
		}
		
		/* Set values */
		I2C_InitStruct.I2C_ClockSpeed = TM_I2C_INT_Clocks[0];
		I2C_InitStruct.I2C_AcknowledgedAddress = TM_I2C1_ACKNOWLEDGED_ADDRESS;
		I2C_InitStruct.I2C_Mode = TM_I2C1_MODE;
		I2C_InitStruct.I2C_OwnAddress1 = TM_I2C1_OWN_ADDRESS;
		I2C_InitStruct.I2C_Ack = TM_I2C1_ACK;
		I2C_InitStruct.I2C_DutyCycle = TM_I2C1_DUTY_CYCLE;
	} else if (I2Cx == I2C2) {
		/* Enable clock */
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		
		/* Enable pins */
		TM_I2C2_INT_InitPins(pinspack);
		
		/* Check clock */
		if (clockSpeed < TM_I2C_INT_Clocks[1]) {
			TM_I2C_INT_Clocks[1] = clockSpeed;
		}
		
		/* Set values */
		I2C_InitStruct.I2C_ClockSpeed = TM_I2C_INT_Clocks[1];
		I2C_InitStruct.I2C_AcknowledgedAddress = TM_I2C2_ACKNOWLEDGED_ADDRESS;
		I2C_InitStruct.I2C_Mode = TM_I2C2_MODE;
		I2C_InitStruct.I2C_OwnAddress1 = TM_I2C2_OWN_ADDRESS;
		I2C_InitStruct.I2C_Ack = TM_I2C2_ACK;
		I2C_InitStruct.I2C_DutyCycle = TM_I2C2_DUTY_CYCLE;
	} else if (I2Cx == I2C3) {
		/* Enable clock */
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
		
		/* Enable pins */
		TM_I2C3_INT_InitPins(pinspack);
		
		/* Check clock */
		if (clockSpeed < TM_I2C_INT_Clocks[2]) {
			TM_I2C_INT_Clocks[2] = clockSpeed;
		}
		
		/* Set values */
		I2C_InitStruct.I2C_ClockSpeed = TM_I2C_INT_Clocks[2];
		I2C_InitStruct.I2C_AcknowledgedAddress = TM_I2C3_ACKNOWLEDGED_ADDRESS;
		I2C_InitStruct.I2C_Mode = TM_I2C3_MODE;
		I2C_InitStruct.I2C_OwnAddress1 = TM_I2C3_OWN_ADDRESS;
		I2C_InitStruct.I2C_Ack = TM_I2C3_ACK;
		I2C_InitStruct.I2C_DutyCycle = TM_I2C3_DUTY_CYCLE;
	}
	
	/* Disable I2C first */
	I2C_Cmd(I2Cx, DISABLE);
	
	/* Initialize I2C */
	I2C_Init(I2Cx, &I2C_InitStruct);
	
	/* Enable I2C */
	I2C_Cmd(I2Cx, ENABLE);
}

uint8_t TM_I2C_Read(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg) {
	uint8_t received_data;
	TM_I2C_Start(I2Cx, address, I2C_Direction_Transmitter, 0);
	TM_I2C_WriteData(I2Cx, reg);
	TM_I2C_Stop(I2Cx);
	TM_I2C_Start(I2Cx, address, I2C_Direction_Receiver, 0);
	received_data = TM_I2C_ReadNack(I2Cx);
	return received_data;
}

void TM_I2C_Write(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data) {
	TM_I2C_Start(I2Cx, address, I2C_Direction_Transmitter, 0);
	TM_I2C_WriteData(I2Cx, reg);
	TM_I2C_WriteData(I2Cx, data);
	TM_I2C_Stop(I2Cx);
}

void TM_I2C_ReadMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	uint8_t i;
	TM_I2C_Start(I2Cx, address, I2C_Direction_Transmitter, 1);
	TM_I2C_WriteData(I2Cx, reg);
	TM_I2C_Stop(I2Cx);
	TM_I2C_Start(I2Cx, address, I2C_Direction_Receiver, 1);
	for (i = 0; i < count; i++) {
		if (i == (count - 1)) {
			/* Last byte */
			data[i] = TM_I2C_ReadNack(I2Cx);
		} else {
			data[i] = TM_I2C_ReadAck(I2Cx);
		}
	}
}

void TM_I2C_ReadMultiNoRegister(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data, uint16_t count) {
	uint8_t i;
	TM_I2C_Start(I2Cx, address, I2C_Direction_Receiver, 1);
	for (i = 0; i < count; i++) {
		if (i == (count - 1)) {
			/* Last byte */
			data[i] = TM_I2C_ReadNack(I2Cx);
		} else {
			data[i] = TM_I2C_ReadAck(I2Cx);
		}
	}
}

void TM_I2C_WriteMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	uint8_t i;
	TM_I2C_Start(I2Cx, address, I2C_Direction_Transmitter, 0);
	TM_I2C_WriteData(I2Cx, reg);
	for (i = 0; i < count; i++) {
		TM_I2C_WriteData(I2Cx, data[i]);
	}
	TM_I2C_Stop(I2Cx);
}

int16_t TM_I2C_Start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction, uint8_t ack) {
	/* Generate I2C start pulse */
	I2C_GenerateSTART(I2Cx, ENABLE);
	
	/* Wait till I2C is busy */
	TM_I2C_Timeout = TM_I2C_TIMEOUT;
	while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB) && TM_I2C_Timeout) {
		if (--TM_I2C_Timeout == 0x00) {
			return 1;
		}
	}

	/* Enable ack if we select it */
	if (ack) {
		I2C_AcknowledgeConfig(I2C1, ENABLE);
	}
	
	/* Send address */
	I2C_Send7bitAddress(I2Cx, address, direction);

	/* Send write/read bit */
	if (direction == I2C_Direction_Transmitter) {
		TM_I2C_Timeout = TM_I2C_TIMEOUT;
		while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_ADDR) && TM_I2C_Timeout) {
			if (--TM_I2C_Timeout == 0x00) {
				return 1;
			}
		}
	} else if (direction == I2C_Direction_Receiver) {
		TM_I2C_Timeout = TM_I2C_TIMEOUT;
		while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) && TM_I2C_Timeout) {
			if (--TM_I2C_Timeout == 0x00) {
				return 1;
			}
		}
	}
	I2Cx->SR2;
	
	/* Return 0, everything ok */
	return 0;
}

void TM_I2C_WriteData(I2C_TypeDef* I2Cx, uint8_t data) {
	/* Wait till I2C is not busy anymore */
	TM_I2C_Timeout = TM_I2C_TIMEOUT;
	while (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE) && TM_I2C_Timeout) {
		TM_I2C_Timeout--;
	}
	
	/* Send I2C data */
	I2Cx->DR = data;
}

uint8_t TM_I2C_ReadAck(I2C_TypeDef* I2Cx) {
	uint8_t data;
	
	/* Enable ACK */
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	
	/* Wait till not received */
	TM_I2C_Timeout = TM_I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) && TM_I2C_Timeout) {
		TM_I2C_Timeout--;
	}

	/* Read data */
	data = I2Cx->DR;
	
	/* Return data */
	return data;
}

uint8_t TM_I2C_ReadNack(I2C_TypeDef* I2Cx) {
	uint8_t data;
	
	/* Disable ACK */
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	
	/* Generate stop */
	I2C_GenerateSTOP(I2Cx, ENABLE);
	
	/* Wait till received */
	TM_I2C_Timeout = TM_I2C_TIMEOUT;
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) && TM_I2C_Timeout) {
		TM_I2C_Timeout--;
	}

	/* Read data */
	data = I2Cx->DR;
	
	/* Return data */
	return data;
}

uint8_t TM_I2C_Stop(I2C_TypeDef* I2Cx) {
	/* Wait till transmitter not empty */
	TM_I2C_Timeout = TM_I2C_TIMEOUT;
	while (((!I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXE)) || (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BTF))) && TM_I2C_Timeout) {
		if (--TM_I2C_Timeout == 0x00) {
			return 1;
		}
	}
	
	/* Generate stop */
	I2C_GenerateSTOP(I2Cx, ENABLE);
	
	/* Return 0, everything ok */
	return 0;
}

uint8_t TM_I2C_IsDeviceConnected(I2C_TypeDef* I2Cx, uint8_t address) {
	uint8_t connected = 0;
	/* Try to start, function will return 0 in case device will send ACK */
	if (!TM_I2C_Start(I2Cx, address, I2C_Direction_Transmitter, 1)) {
		connected = 1;
	}
	
	/* STOP I2C */
	TM_I2C_Stop(I2Cx);
	
	/* Return status */
	return connected;
}

/* Private functions */
void TM_I2C1_INT_InitPins(TM_I2C_PinsPack_t pinspack) {
	/* Init pins */
	if (pinspack == TM_I2C_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_Pin_6 | GPIO_Pin_7, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF_I2C1);
	} else if (pinspack == TM_I2C_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_Pin_8 | GPIO_Pin_9, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF_I2C1);
	} else if (pinspack == TM_I2C_PinsPack_3) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_Pin_6 | GPIO_Pin_9, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF_I2C1);
	} else if (pinspack == TM_I2C_PinsPack_Custom) {
		/* Init custom pins, callback function */
		TM_I2C_InitCustomPins(I2C1);
	}
}

void TM_I2C2_INT_InitPins(TM_I2C_PinsPack_t pinspack) {
	/* Init pins */
	if (pinspack == TM_I2C_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOB, GPIO_Pin_10 | GPIO_Pin_11, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF_I2C2);
	} else if (pinspack == TM_I2C_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOF, GPIO_Pin_0 | GPIO_Pin_1, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF_I2C2);
	} else if (pinspack == TM_I2C_PinsPack_3) {
		TM_GPIO_InitAlternate(GPIOH, GPIO_Pin_4 | GPIO_Pin_5, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF_I2C2);
	} else if (pinspack == TM_I2C_PinsPack_Custom) {
		/* Init custom pins, callback function */
		TM_I2C_InitCustomPins(I2C2);
	}
}

void TM_I2C3_INT_InitPins(TM_I2C_PinsPack_t pinspack) {
	/* Init pins */
	if (pinspack == TM_I2C_PinsPack_1) {
		TM_GPIO_InitAlternate(GPIOA, GPIO_Pin_8, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF_I2C3);
		TM_GPIO_InitAlternate(GPIOC, GPIO_Pin_9, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF_I2C3);
	} else if (pinspack == TM_I2C_PinsPack_2) {
		TM_GPIO_InitAlternate(GPIOH, GPIO_Pin_7 | GPIO_Pin_8, TM_GPIO_OType_OD, TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium, GPIO_AF_I2C3);
	} else if (pinspack == TM_I2C_PinsPack_Custom) {
		/* Init custom pins, callback function */
		TM_I2C_InitCustomPins(I2C3);
	}
}
