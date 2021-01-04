/*
 * delay.h
 *
 *  Created on: 4 янв. 2021 г.
 *      Author: Xell
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include  "stm32h750xx.h"
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void delay_init(void);
void delay_us(uint32_t n);
void delay_ms(uint32_t n);
void delay_xs(uint32_t n);

#endif /* INC_DELAY_H_ */
