/**
 ******************************************************************************
 * @addtogroup Bootloader Bootloaders
 * @{
 * @addtogroup SPRF3 SP Racing F3
 * @{
 *
 * @file       sprf3/bl/pios_board.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2012-2013
 * @author     dRonin, http://dRonin.org/, Copyright (C) 2016
 * @brief      Board specific initialization for the bootloader
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 */

/* Pull in the board-specific static HW definitions.
 * Including .c files is a bit ugly but this allows all of
 * the HW definitions to be const and static to limit their
 * scope.
 *
 * NOTE: THIS IS THE ONLY PLACE THAT SHOULD EVER INCLUDE THIS FILE
 */


#include "board_hw_defs.c"

#include <pios_board_info.h>
#include <pios_usart_priv.h>
#include <pios_com_priv.h>
#include <pios.h>
#include "bl_board_xtra.h"

//uintptr_t pios_com_telem_serial_id;
uintptr_t pios_com_telem_usb_id;


#define PIOS_COM_MAIN_RX_BUF_LEN 256
#define PIOS_COM_MAIN_TX_BUF_LEN 256
uint8_t rx_buffer[PIOS_COM_MAIN_RX_BUF_LEN];
uint8_t tx_buffer[PIOS_COM_MAIN_TX_BUF_LEN];

static void setupCom();

bool PIOS_USB_HaveVSense(uintptr_t id)
{
 return false;
}

bool PIOS_USB_CableConnected(uintptr_t id)
{
	//Because we don't have native USB, but only UART2USB converter, assume we have already connected cable.
	return true;
}

void SOF_Callback(void)
{
	;
}

void SUSP_Callback(void)
{
	;
}

void PIOS_Board_Init()
{
	const struct pios_servo_cfg * servo_cfg = &pios_servo_cfg;
	const struct pios_tim_channel * channels = servo_cfg->channels;
	uint8_t num_channels = servo_cfg->num_channels;
	for (int i = 0; i < num_channels; i++) {
		GPIO_InitTypeDef init = {
			.GPIO_Pin = channels[i].pin.init.GPIO_Pin,
			.GPIO_Speed = GPIO_Speed_2MHz,
			.GPIO_Mode  = GPIO_Mode_OUT,
			.GPIO_OType = GPIO_OType_PP,
			.GPIO_PuPd  = GPIO_PuPd_DOWN
		};

		/* Nail all servo channels as outputs, LOW.
		 * Mostly for brushed clones of SPRF3E to ensure we don't
		 * spin up (they should provide pulldowns, but meh.)
		 */
		channels[i].pin.gpio->BSRR = channels[i].pin.init.GPIO_Pin << 16;
		GPIO_Init(channels[i].pin.gpio, &init);
		channels[i].pin.gpio->BSRR = channels[i].pin.init.GPIO_Pin << 16;
	}

	/* Delay system */
	PIOS_DELAY_Init();

#if defined(PIOS_INCLUDE_ANNUNC)
	PIOS_ANNUNC_Init(&pios_annunc_cfg);
#endif	/* PIOS_INCLUDE_ANNUNC */

#if defined(PIOS_INCLUDE_FLASH)
	/* Inititialize all flash drivers */
	PIOS_Flash_Internal_Init(&pios_internal_flash_id, &flash_internal_cfg);

	/* Register the partition table */
	PIOS_FLASH_register_partition_table(pios_flash_partition_table, NELEMENTS(pios_flash_partition_table));
#endif	/* PIOS_INCLUDE_FLASH */


FLASH_PrefetchBufferCmd(ENABLE);

#if defined(PIOS_INCLUDE_LED)
    const struct pios_gpio_cfg *led_cfg = PIOS_BOARD_HW_DEFS_GetLedCfg(bdinfo->board_rev);
    PIOS_Assert(led_cfg);
    PIOS_LED_Init(led_cfg);
#endif /* PIOS_INCLUDE_LED */
    setupCom();
}

void setupCom()
{
    uintptr_t pios_usart_generic_id;

    static struct pios_usart_params pios_usart_bl_params = {
			.init =	{
				.USART_BaudRate = 115200,
				.USART_WordLength = USART_WordLength_8b,
				.USART_Parity = USART_Parity_No,
				.USART_StopBits = USART_StopBits_1,
				.USART_HardwareFlowControl = USART_HardwareFlowControl_None,
				.USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
			},
		};

    if (PIOS_USART_Init(&pios_usart_generic_id, &pios_uart1_cfg, &pios_usart_bl_params)) {
        PIOS_Assert(0);
    }

    if (PIOS_COM_Init(&PIOS_COM_TELEM_USB, &pios_usart_com_driver, pios_usart_generic_id,
                      PIOS_COM_MAIN_RX_BUF_LEN,
                      PIOS_COM_MAIN_TX_BUF_LEN)) {
        PIOS_Assert(0);
    }

}

/**
 * @}
 * @}
 */
