/**
  ******************************************************************************
  * @file    LTDC/LTDC_Display_2Layers/Src/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    26-June-2014
  * @brief   This example describes how to configure the LTDC peripheral 
  *          to display two Layers at the same time.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#include "stm32f4xx.h"
#include "main.h"

#include "ts_api_extends.h"
#include "lcd_layout.h"
#include "usb_mouse_api.h"


/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup LTDC_Display_2Layer
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LTDC_HandleTypeDef LtdcHandle;
USBD_HandleTypeDef USBD_Device;
extern PCD_HandleTypeDef hpcd;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

void test_reg_func( TS_mouseInputTypeDef _state_, const Point * const _inData_ ) ;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */


#define COMMENT 0

int main(void)
{
	uint8_t status = 0 ;

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();

	/* Init l3gd20 gyroscope */
	BSP_GYRO_Init();
	BSP_GYRO_Reset();
	
	/* Init USB Device Library */
  USBD_Init(&USBD_Device, &HID_Desc, 0);
  
  /* Add USB Supported Class */
  USBD_RegisterClass(&USBD_Device, USBD_HID_CLASS);
  
  /* Start USB Device Process */
	USBD_Start(&USBD_Device);

  /* Configure LED3 */
  BSP_LED_Init(LED3);
	BSP_LED_Init(LED4); 
	BSP_LCD_Init();
	//lcd_prepLayers() ;
		
	// ######## TOUCH PANEL ############
  status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
	if ( status == TS_OK ) {
		
		// Additional initialization ( interrupts and fifo threshold )
		BSP_TS_Init_extends(TS_I2C_ADDRESS );

		// register lcd notify function - for drawing action
		if ( 0 == TS_registerNotifyFunc( &lcd_handleMouseNotify ) )
			BSP_LED_On( LED4 ) ;
		
		// register usb notify handle function
		if ( 0 == TS_registerNotifyFunc(&usb_handleMouseNotify) ) {
			BSP_LED_On( LED4 ) ;
		}
		
		// register new notify function
		if ( 0 == TS_registerNotifyFunc( &test_reg_func ) )
			BSP_LED_On( LED4 ) ;
		
	}
	
  while (1)
  {
		
		TS_checkEvent( TS_I2C_ADDRESS ) ;
		usb_checkPoolInterval() ;
		//lcd_drawAction() ;
		

		
		
		
		#ifndef COMMENT
    for (tobuttom = 1; tobuttom < 41; tobuttom++)
    {
      /* move the picture */
      /* reconfigure the layer1 position */
      HAL_LTDC_SetWindowPosition(&LtdcHandle, 0, (tobuttom*4), 0); 
      /* reconfigure the layer2 position */
      HAL_LTDC_SetWindowPosition(&LtdcHandle, 0, (160 - (tobuttom*4)), 1); 
      HAL_Delay(50);
    }
    HAL_Delay(500);
    for (totop = 1; totop < 41; totop++)
    {
      /* move the picture */
      /* reconfigure the layer1 position */
      HAL_LTDC_SetWindowPosition(&LtdcHandle, 0, (160 - (totop*4)), 0); 
      /* reconfigure the layer2 position */
      HAL_LTDC_SetWindowPosition(&LtdcHandle, 0, (totop*4), 1); 
      HAL_Delay(50);
    }
    HAL_Delay(500);
		#endif
    
  }
}

void test_reg_func( TS_mouseInputTypeDef _state_ , const Point * const _inData_ ) {
	
	
		switch ( _state_ )
		{
			case TS_MOUSE_LEFT : BSP_LED_On( LED4 ) ; break ;
			case TS_MOUSE_RIGHT: BSP_LED_On( LED3 ) ; break ;
			case TS_MOUSE_SLIDER_UP : BSP_LED_On( LED4 ) ; HAL_Delay(10); BSP_LED_Off( LED4 );	break ;
			case TS_MOUSE_SLIDER_DOWN: BSP_LED_On( LED3 ); HAL_Delay(10); BSP_LED_Off( LED3 );  break ;
			case TS_MOUSE_NONE : BSP_LED_Off( LED3 ) ; BSP_LED_Off( LED4 ); break ;
		}
		
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  *         The LTDC Clock is configured as follow :
  *            PLLSAIN                        = 192
  *            PLLSAIR                        = 4
  *            PLLSAIDivR                     = 8
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /*##-1- System Clock Configuration #########################################*/  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Activate the Over-Drive mode */
  HAL_PWREx_ActivateOverDrive();
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  /*##-2- LTDC Clock Configuration ###########################################*/  
  /* LCD clock configuration */
  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/4 = 48 Mhz */
  /* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDIVR_8 = 48/8 = 6 Mhz */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 4;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct); 
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
