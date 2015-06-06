/**
  ******************************************************************************
  * @file    LTDC/LTDC_Display_2Layers/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    26-June-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "main.h"
#include "stm32f4xx_it.h"

/////////////////////////////////// LCD /////////////////////////////////
// From ts_api_extends.c
extern volatile sig_atomic_t ts_int_catched ;

// From ts_api_extends.c
extern volatile sig_atomic_t ts_buttons_delay ;
extern volatile sig_atomic_t ts_mouse_last_signal_delay ;
extern volatile sig_atomic_t ts_slider_hop_time ;
////////////////////////////////////////////////////////////////////////

/////////////////////////////////// USB ///////////////////////////////
extern PCD_HandleTypeDef hpcd;
uint8_t HID_Buffer[4];
extern USBD_HandleTypeDef USBD_Device;
static void GetPointerData(uint8_t *pbuf);
static float xyz_buff[3];
//char buff_disp_xyz[30];
TS_mouseInputTypeDef mouse_state = TS_MOUSE_NONE;
#define CURSOR_STEP     1
#define ABS(x) (((x) > 0) ? (x) : (-x))
//////////////////////////////////////////////////////////////////////
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
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler (void)
{
  static __IO uint32_t counter=0;
  HAL_IncTick();
  
	// For touch driver purposes
	++ts_buttons_delay ;
	++ts_mouse_last_signal_delay ;
	if (ts_slider_hop_time > 0 )
		++ts_slider_hop_time ;
	
  /* check Joystick state every polling interval (1ms) */
  if (counter++ == USBD_HID_GetPollingInterval(&USBD_Device))
  {  
    GetPointerData(HID_Buffer);
    
    /* send data though IN endpoint*/
    if((HID_Buffer[1] != 0) || (HID_Buffer[1] != 0) || HID_Buffer[2] != 0 || HID_Buffer[3] != 0 )
    {
      USBD_HID_SendReport(&USBD_Device, HID_Buffer, 4);
    }
    counter =0;
  }

}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles USB-On-The-Go FS or HS global interrupt request.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  HAL_PCD_IRQHandler(&hpcd);
}

/**
  * @brief  This function handles USB OTG FS or HS Wakeup IRQ Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_FS  
void OTG_FS_WKUP_IRQHandler(void)
#else  
void OTG_HS_WKUP_IRQHandler(void)
#endif
{
  if((&hpcd)->Init.low_power_enable)
  {
    /* Reset SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));  
    
    /* Configures system clock after wake-up from STOP: enable HSE, PLL and select 
    PLL as system clock source (HSE and PLL are disabled in STOP mode) */
    
    __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);
    
    /* Wait till HSE is ready */  
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
    {}
    
    /* Enable the main PLL. */
    __HAL_RCC_PLL_ENABLE();
    
    /* Wait till PLL is ready */  
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
    {}
    
    /* Select PLL as SYSCLK */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_SYSCLKSOURCE_PLLCLK);
    
    while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL)
    {}
    
    /* ungate PHY clock */
     __HAL_PCD_UNGATE_PHYCLOCK((&hpcd)); 
  }
#ifdef USE_USB_FS
  /* Clear EXTI pending Bit*/
  __HAL_USB_FS_EXTI_CLEAR_FLAG();
#else
    /* Clear EXTI pending Bit*/
  __HAL_USB_HS_EXTI_CLEAR_FLAG();
#endif
  
}


/**
  * @brief  Gets Pointer Data.
  * @param  pbuf: Pointer to report
  * @retval None
  */
static void GetPointerData(uint8_t *pbuf)
{
  int8_t x = 0, y = 0;
	BSP_GYRO_GetXYZ(xyz_buff);
		if (xyz_buff[0] > 10000.0f)
			y += CURSOR_STEP;
		if (xyz_buff[0] < -10000.0f)
			y -= CURSOR_STEP;
		if (xyz_buff[1] > 10000.0f)
			x -= CURSOR_STEP;
		if (xyz_buff[1] < -10000.0f)
			x += CURSOR_STEP;
	mouse_state = TS_getCurrMouseState();
	if (mouse_state == TS_MOUSE_LEFT)
		pbuf[0] = 0x01;
	else if (mouse_state == TS_MOUSE_RIGHT)
		pbuf[0] = 0x02;
	else
		pbuf[0] = 0;
  pbuf[1] = x;
  pbuf[2] = y;
	if (mouse_state == TS_MOUSE_SLIDER_UP)
    pbuf[3] = 1;
	else if (mouse_state == TS_MOUSE_SLIDER_DOWN)
		pbuf[3] = -1;
	else
		pbuf[3] = 0;

	
}

/** 
	* @brief This function is EXTI15_10_IRQHandler. 
	*	And STMPE811 driver is handling there.
*/

void STMPE811_INT_EXTIHandler(void) {
	
		ts_int_catched = 1 ;
	
		// Clear pending
		EXTI->PR |= STMPE811_INT_PIN ;
}

/**
  * @}
  */

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
