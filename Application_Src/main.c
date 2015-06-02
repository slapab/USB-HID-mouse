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
#include "stm32f429i_discovery_gyroscope.h"
#include "ts_api_extends.h"


//#include "st_logo1.h"
//#include "st_logo2.h"

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

/* Private function prototypes -----------------------------------------------*/
static void LCD_Config(void); 
static void SystemClock_Config(void);
static void Error_Handler(void);
void uint16toASCII( uint16_t _val_ , uint8_t *ptr ) ;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
#define COMMENT 0


float xyz_buff[3];
char buff_disp_xyz[30];
float Xval, Yval, Zval = 0x00;
	
int main(void)
{
	uint8_t status = 0 ;
	TS_StateTypeDef touch_data ;
	uint8_t uint16_txt[6] ;
	
  //uint32_t tobuttom = 0;
  //uint32_t totop = 0;
	

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();

  /* Configure LED3 */
  BSP_LED_Init(LED3);   
  
  /*##-1- LCD Configuration ##################################################*/ 
  /* Configure 2 layers w/ Blending */
  //LCD_Config(); 

	BSP_LCD_Init();
	
	
	//BSP_LCD_SetLayerAddress( LCD_BACKGROUND_LAYER, LCD_FRAME_BUFFER );
	//BSP_LCD_SetLayerAddress( LCD_FOREGROUND_LAYER, LCD_FRAME_BUFFER+BUFFER_OFFSET );
	// Init LCD buffers for layers
	BSP_LCD_LayerDefaultInit(LCD_BACKGROUND_LAYER, LCD_FRAME_BUFFER);
	BSP_LCD_LayerDefaultInit(LCD_FOREGROUND_LAYER, LCD_FRAME_BUFFER+BUFFER_OFFSET);
	// Enable Foreground Layer
	BSP_LCD_SetLayerVisible(LCD_FOREGROUND_LAYER, ENABLE);
	BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, DISABLE);
	
	// Select Foreground Layer
	BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	/* Clear the LCD */ 
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE); 
  BSP_LCD_Clear(LCD_COLOR_LIGHTBLUE);
  
  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);  
  
  /* Display LCD messages */
	 if (BSP_GYRO_Init() != GYRO_OK) 
		 BSP_LCD_DisplayStringAt(0, 10, (uint8_t*)"GYRO_ERROR", CENTER_MODE);
	 
  //BSP_LCD_DisplayStringAt(0, 10, (uint8_t*)"STM32F429I BSP", CENTER_MODE);
	/* Read Gyro Angular data */
    BSP_GYRO_GetXYZ(xyz_buff);
    
    /* Update autoreload and capture compare registers value */
    Xval = xyz_buff[0];
    Yval = xyz_buff[1]; 
    Zval = xyz_buff[2]; 
		
		snprintf(buff_disp_xyz, 28, "x: %d y:%d z:%d", (int32_t)Xval, (int32_t)Yval, (int32_t)Zval);
		BSP_LCD_DisplayStringAt(0, 10, (uint8_t*)buff_disp_xyz, LEFT_MODE);
	
	//############# 2 warstawa - test
	
	// Select Background Layer
	BSP_LCD_SelectLayer(LCD_BACKGROUND_LAYER);
	
	/* Clear the LCD */ 
  BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGREEN); 
  BSP_LCD_Clear(LCD_COLOR_LIGHTBLUE);
	
	
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);  
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAt(0, 35, (uint8_t*)"Drivers examples", CENTER_MODE);
	
	BSP_LCD_SetBackColor( LCD_COLOR_LIGHTCYAN) ;
	Point some_poly[5] = { { 0, 100}, {10, 120}, { 15, 135} , {75, 105}, {85, 85} } ;
	BSP_LCD_DrawPolygon( &some_poly[0], 5 ) ;
	BSP_LCD_SetBackColor( LCD_COLOR_LIGHTCYAN) ;
	BSP_LCD_FillPolygon( &some_poly[0], 5 );
	
	
	// ######## TOUCH PANEL ############
  status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
	if ( status == TS_OK ) {
		
		// Additional initialization ( interrupts and fifo threshol )
		BSP_TS_Init_extends(TS_I2C_ADDRESS, 10 );
		
		/*
		BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
		BSP_LCD_DisplayStringAt(0, 40, (uint8_t*)"TS activated", CENTER_MODE);
		
		while( 1 ) {
			BSP_TS_GetState( &touch_data ) ;
			if( touch_data.TouchDetected ) {
				
				uint16toASCII( touch_data.X, &uint16_txt[0] ) ;
				//BSP_LCD_DisplayStringAt(0, 80, &uint16_txt[0], CENTER_MODE);
				BSP_LCD_ClearStringLine( 5 ) ;
				BSP_LCD_DisplayStringAtLine( 5 , &uint16_txt[0] ) ;
				uint16toASCII( touch_data.Y, &uint16_txt[0] ) ;
				//BSP_LCD_DisplayStringAt(0, 100, &uint16_txt[0], CENTER_MODE);
				BSP_LCD_ClearStringLine( 6 ) ;
				BSP_LCD_DisplayStringAtLine( 6 , &uint16_txt[0] ) ;
			}
		}
		*/
	}
	/*
	HAL_Delay(1000);
	
	BSP_LCD_SetTransparency( LCD_FOREGROUND_LAYER , 100 );
	BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, ENABLE);
	
	HAL_Delay(2000 ) ;
	
	// Turn Off Foreground Layer
	BSP_LCD_SetLayerVisible(LCD_FOREGROUND_LAYER, DISABLE);
	
	HAL_Delay(1000) ;

	BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, ENABLE);
	
	HAL_Delay(1000) ;
	
	BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, DISABLE);
	*/
	
	 
  while (1)
  {
		 /* Read Gyro Angular data */
  //  BSP_GYRO_GetXYZ(xyz_buff);
    
    /* Update autoreload and capture compare registers value */
  /*  Xval = xyz_buff[0];
    Yval = xyz_buff[1]; 
    Zval = xyz_buff[2]; 
		
		snprintf(buff_disp_xyz, 28, "x: %1.2f y:%1.2f z:%1.2f", Xval, Yval, Zval);
		BSP_LCD_DisplayStringAt(0, 50, (uint8_t*)buff_disp_xyz, CENTER_MODE);*/
		
		TS_checkEvent( TS_I2C_ADDRESS ) ;
		
		/*
		HAL_Delay(1500) ;
		BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, ENABLE);
		BSP_LCD_SetLayerVisible(LCD_FOREGROUND_LAYER, DISABLE);
		HAL_Delay(1500) ;
		BSP_LCD_SetLayerVisible(LCD_FOREGROUND_LAYER, ENABLE);
		BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, DISABLE);
		*/
		
		
		
		
		
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



void uint16toASCII( uint16_t _val_ , uint8_t *ptr ) {
	int8_t size = 0 ;
	
	if ( _val_ >= 10000 ) 
		size = 4 ;
	else if ( _val_ >= 1000 ) 
		size = 3 ;
	else if ( _val_ >= 100 )
		size = 2 ;
	else if ( _val_ >= 10 )
		size = 1 ;
	
	// set '\0' at the end
	ptr[size+1] = '\0' ;
	
	while ( size > 0 ) {
		ptr[size] = (uint8_t)(_val_%10) + 0x30 ;
		_val_ /= 10 ;
		
		--size ;
	}
	ptr[0] = (uint8_t)_val_ + 0x30 ;
}

/**
  * @brief LCD Configuration.
  * @note  This function Configure tha LTDC peripheral :
  *        1) Configure the Pixel Clock for the LCD
  *        2) Configure the LTDC Timing and Polarity
  *        3) Configure the LTDC Layer 1 :
  *           - direct color (RGB565) as pixel format  
  *           - The frame buffer is located at FLASH memory
  *           - The Layer size configuration : 240x160
  *        4) Configure the LTDC Layer 2 :
  *           - direct color (RGB565) as pixel format 
  *           - The frame buffer is located at FLASH memory
  *           - The Layer size configuration : 240x160                      
  * @retval
  *  None
  */
static void LCD_Config(void)
{  
  LTDC_LayerCfgTypeDef pLayerCfg;
  LTDC_LayerCfgTypeDef pLayerCfg1;

  /* Initializaton of ILI9341 component*/
  ili9341_Init();
  
/* LTDC Initialization -------------------------------------------------------*/
  
  /* Polarity configuration */
  /* Initialize the horizontal synchronization polarity as active low */
  LtdcHandle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  /* Initialize the vertical synchronization polarity as active low */ 
  LtdcHandle.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
  /* Initialize the data enable polarity as active low */ 
  LtdcHandle.Init.DEPolarity = LTDC_DEPOLARITY_AL; 
  /* Initialize the pixel clock polarity as input pixel clock */  
  LtdcHandle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  
  /* Timing configuration  (Typical configuration from ILI9341 datasheet)
      HSYNC=10 (9+1)
      HBP=20 (29-10+1)
      ActiveW=240 (269-20-10+1)
      HFP=10 (279-240-20-10+1)

      VSYNC=2 (1+1)
      VBP=2 (3-2+1)
      ActiveH=320 (323-2-2+1)
      VFP=4 (327-320-2-2+1)
  */  

  /* Timing configuration */
  /* Horizontal synchronization width = Hsync - 1 */  
  LtdcHandle.Init.HorizontalSync = 9;
  /* Vertical synchronization height = Vsync - 1 */
  LtdcHandle.Init.VerticalSync = 1;
  /* Accumulated horizontal back porch = Hsync + HBP - 1 */
  LtdcHandle.Init.AccumulatedHBP = 29;
  /* Accumulated vertical back porch = Vsync + VBP - 1 */
  LtdcHandle.Init.AccumulatedVBP = 3; 
  /* Accumulated active width = Hsync + HBP + Active Width - 1 */ 
  LtdcHandle.Init.AccumulatedActiveH = 323;
  /* Accumulated active height = Vsync + VBP + Active Heigh - 1 */
  LtdcHandle.Init.AccumulatedActiveW = 269;
  /* Total height = Vsync + VBP + Active Heigh + VFP - 1 */
  LtdcHandle.Init.TotalHeigh = 327;
  /* Total width = Hsync + HBP + Active Width + HFP - 1 */
  LtdcHandle.Init.TotalWidth = 279;
  
  /* Configure R,G,B component values for LCD background color */
  LtdcHandle.Init.Backcolor.Blue = 0;
  LtdcHandle.Init.Backcolor.Green = 0;
  LtdcHandle.Init.Backcolor.Red = 0;

  LtdcHandle.Instance = LTDC;
  
/* Layer1 Configuration ------------------------------------------------------*/
  
  /* Windowing configuration */ 
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 10;//240;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 10;//160;
  
  /* Pixel Format configuration*/ 
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  
  /* Start Address configuration : frame buffer is located at FLASH memory */
  //pLayerCfg.FBStartAdress = (uint32_t)&ST_LOGO_1;
  
  /* Alpha constant (255 totally opaque) */
  pLayerCfg.Alpha = 255;
  
  /* Default Color configuration (configure A,R,G,B component values) */
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  
  /* Configure blending factors */
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  
  /* Configure the number of lines and number of pixels per line */
  pLayerCfg.ImageWidth = 10; //240;
  pLayerCfg.ImageHeight = 10; //160;

/* Layer2 Configuration ------------------------------------------------------*/
  
  /* Windowing configuration */
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 10;//240;
  pLayerCfg1.WindowY0 = 160;
  pLayerCfg1.WindowY1 = 170;//320;
  
  /* Pixel Format configuration*/ 
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  
  /* Start Address configuration : frame buffer is located at FLASH memory */
  //pLayerCfg1.FBStartAdress = (uint32_t)&ST_LOGO_2;
  
  /* Alpha constant (255 totally opaque) */
  pLayerCfg1.Alpha = 200;
  
  /* Default Color configuration (configure A,R,G,B component values) */
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  
  /* Configure blending factors */
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  
  /* Configure the number of lines and number of pixels per line */
  pLayerCfg1.ImageWidth = 10;//240;
  pLayerCfg1.ImageHeight = 10;//160;  
   
  /* Configure the LTDC */  
  if(HAL_LTDC_Init(&LtdcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }

  /* Configure the Background Layer*/
  if(HAL_LTDC_ConfigLayer(&LtdcHandle, &pLayerCfg, 0) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
  }
  
  /* Configure the Foreground Layer*/
  if(HAL_LTDC_ConfigLayer(&LtdcHandle, &pLayerCfg1, 1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(); 
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
  RCC_OscInitStruct.PLL.PLLN = 360;
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
static void Error_Handler(void)
{
    /* Turn LED3 on */
    BSP_LED_On(LED3);
    while(1)
    {
    }
}

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
