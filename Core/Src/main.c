/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
int16_t Buffor_a[4000];
uint16_t Buffor_b[400];
uint16_t Buffor_usrednianie[20];
uint16_t Buffor[20],i = 0,n = 0,t = 0,count, a=0, y=0, l_p = 0, licznik_PID_p = 0, z =0, b= 0;
uint32_t pomiar =0, x=0;
uint8_t Received[10];
int32_t roznica_polozen, polozenie_nowe_32, zmiana_polozenia_32, suma_zmian_polozen_1ms;


float odczyt_z_Received;
//odczyt dla regulacji pradu silnika
float Prad,Prad_zadany;
float Kp_prad  = 0,Ti_prad  = 0,Td_prad = 0;
//odczyt dla regulacji polozenia silnika
float polozenie_z,Polozenie_zadane,Polozenie_mierzone;
float Kp_polozenie = 0 ,Ti_polozenie  = 0,Td_polozenie  = 0;
//odczyt dla regulacji predkosc silnika
float predkosc_zadana,predkosc_z,predkosc_mierzona,aktualna_roznica_polozenia;
float Kp_predkosc = 0,Ti_predkosc = 0,Td_predkosc = 0;
//odbior wartosci od ADC
uint16_t adc_wartosc[1];
uint8_t wysyl[100], start, stop;
float prad_mierzony,wartosc_ADC,pomiar_mV,napiecie_ref_czujnika = 2417;
float srednia,srednia_100;
uint32_t wynik,suma,test, suma_polozenia_buffor,suma_zmian_polozenia;
uint16_t  zmiana_polozenia;
static uint32_t polozenie_stare, polozenie_nowe;
static int16_t polozenie_n, polozenie_s;
static uint16_t polozenie;
static float predkosc_w_obr;

//czasy probkowania
float czas_probkowania_prad = 0.00005;
float czas_probkowania_pol = 0.001;
float czas_probkowania_pr = 0.001;

// wartosci maksymalne wielkosci
float max_napiecie = 9;
float min_napiecie = -9;
float max_prad = 3;
float min_prad = -3;
float max_predkosc = 14;
float min_predkosc = -14;

//odczyt predkosci
uint16_t polozenie_do_predkosci[2];

// PID prad
float suma_uchybow_prad = 0;
float uchyb_aktualny_prad = 0, uchyb_wczescniejszy_prad = 0;
float calka_prad, rozniczka_prad;
float wyjscie_z_PID_prad;

// PID polozenie silnik
float suma_uchybow_pol = 0;
float uchyb_aktualny_pol = 0, uchyb_wczescniejszy_pol = 0;
float calka_pol, rozniczka_pol;
float wyjscie_z_PID_pol, Predkosc_zadana_reg_polozenia,Zadana_predkosc_reg_polozenia;

// PID predkosc silnik
float suma_uchybow_pr = 0;
float uchyb_aktualny_pr = 0, uchyb_wczescniejszy_pr = 0;
float calka_pr, rozniczka_pr;
float wyjscie_z_PID_pr,Zadany_prad_reg_predkosci;


//scalowanie wartosci do inncyh regulatorow
float PWM_z_PID_prad, Prad_z_PID_predkosc;
float prad_z_PID_predkosc;
float predkosci_z_PID_polozenie;

uint8_t licznik_KASKADA_PID;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM17_Init(void);
static void MX_TIM15_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	Odbior_z_UART();

	HAL_UART_Receive_IT(&huart2, &Received, 10);
}

void Odbior_z_UART(void)
{
	uint8_t Data[100];
	uint8_t Data_2[100];
	uint16_t size = 0;
	switch (Received[0]){
	case 'a':
		odczyt_z_Received = ((Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1);
		Prad = odczyt_z_Received/1000;
		suma_uchybow_prad = 0;
		HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, SET);
		__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1,0);
		pomiar = 0;
		x = 0;
		y = 0;
		z= 0;
		l_p = 0;
		start = 1;
		break;
	case 'b':
		odczyt_z_Received = ((Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1);
		Kp_prad = odczyt_z_Received/100;
		break;
	case 'c':
		odczyt_z_Received = ((Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1);
		Ti_prad = odczyt_z_Received/100;
		break;
	case 'd':
		odczyt_z_Received = ((Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1);
		Td_prad = odczyt_z_Received/100;
		break;
	case 'e':
		if (y <=4000){
			size = sprintf(Data_2, "%d\n\r", Buffor_a[y]);
			HAL_UART_Transmit_IT(&huart2, Data_2, size);
			y++;
		}
		break;
	case 'f':
		odczyt_z_Received = (Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		polozenie_z = odczyt_z_Received;
		__HAL_TIM_SET_COUNTER(&htim2,0);
		l_p = 0;
		x = 0;
		y = 0;
		z = 0;
		start = 2;
		break;
	case 'g':
		odczyt_z_Received = (Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		Kp_polozenie = odczyt_z_Received/1000;
		break;
	case 'h':
		odczyt_z_Received = (Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		Ti_polozenie = odczyt_z_Received/1000;
		break;
	case 'i':
		odczyt_z_Received = (Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		Td_polozenie = odczyt_z_Received/1000;
		break;
	case 'j':
		odczyt_z_Received = (Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		predkosc_z = odczyt_z_Received;
		__HAL_TIM_SET_COUNTER(&htim2,0);
		HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, SET);
		__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1,0);
		l_p = 0;
		polozenie_n = 0;
		polozenie_s = 0;
		predkosc_w_obr = 0;
		suma_uchybow_pr = 0;
		x = 0;
		y = 0;
		z= 0;
		start = 3;
		break;
	case 'k':
		odczyt_z_Received = (Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		Kp_predkosc = odczyt_z_Received/1000;
		break;
	case 'l':
		odczyt_z_Received = (Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		Ti_predkosc = odczyt_z_Received/1000;
		break;
	case 'm':
		odczyt_z_Received = (Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		Td_predkosc = odczyt_z_Received/1000;
		break;
	case 'n':
		odczyt_z_Received = (Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		polozenie_z = odczyt_z_Received;
		__HAL_TIM_SET_COUNTER(&htim1,0);
		HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, SET);
		__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1,0);
		l_p = 0;
		x = 0;
		y = 0;
		z= 0;
		start = 4;
		break;
	case 'o':
		odczyt_z_Received = (Received[3]-48)*1000000+(Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		predkosc_z = odczyt_z_Received;
		__HAL_TIM_SET_COUNTER(&htim1,0);
		HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, SET);
		__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1,0);
		l_p = 0;
		polozenie_n = 0;
		polozenie_s = 0;
		x = 0;
		y = 0;
		z= 0;
		start = 5;
		break;
	case 'w':
		suma_uchybow_prad = 0;
		Prad_zadany = 0;
		prad_mierzony = 0;
		suma_uchybow_pr = 0;
		predkosc_zadana = 0;
		predkosc_mierzona = 0;
		suma_uchybow_pol = 0;
		Polozenie_zadane = 0;
		Polozenie_mierzone = 0;
		__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1,0);
		__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1,0);
		break;
	case 'y':
		__HAL_TIM_SET_COUNTER(&htim2,0);
		__HAL_TIM_SET_COUNTER(&htim1,0);
		break;
	case 'x':
		odczyt_z_Received = (Received[4]-48)*100000+(Received[5]-48)*10000+(Received[6]-48)*1000+(Received[7]-48)*100+(Received[8]-48)*10+(Received[9]-48)*1;
		napiecie_ref_czujnika = odczyt_z_Received/100;
		break;
	case 'z':
		if (y <=3000){
			size = sprintf(Data_2, "%d\n\r", Buffor_a[y]);
			HAL_UART_Transmit_IT(&huart2, Data_2, size);
			y++;
		}
		break;
	}
}

void PID_prad(void)
{
			uchyb_wczescniejszy_prad = uchyb_aktualny_prad;
			uchyb_aktualny_prad = Prad_zadany-prad_mierzony;
			suma_uchybow_prad = suma_uchybow_prad + uchyb_aktualny_prad;
			calka_prad = czas_probkowania_prad*Ti_prad*suma_uchybow_prad;
			rozniczka_prad = Td_prad*(uchyb_aktualny_prad-uchyb_wczescniejszy_prad)/czas_probkowania_prad;
			wyjscie_z_PID_prad = Kp_prad*(uchyb_aktualny_prad + calka_prad + rozniczka_prad);
			if(wyjscie_z_PID_prad >= 9)
			{
				wyjscie_z_PID_prad = 9;
			}
			if(wyjscie_z_PID_prad <= -9)
			{
				wyjscie_z_PID_prad = -9;
			}
			PWM_z_PID_prad = (wyjscie_z_PID_prad/30)*100;
			if(Prad_zadany == 0)
			{
				PWM_z_PID_prad = 0;
			}
			if(PWM_z_PID_prad>0)
			{
				HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, RESET);
				__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1,0);

				__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1,PWM_z_PID_prad);
				HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, SET);
			}
			if(PWM_z_PID_prad==0)
			{
				HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, RESET);
				__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1,0);

				__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1,0);
				HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, SET);
			}
			if(PWM_z_PID_prad<0)
			{
				PWM_z_PID_prad = PWM_z_PID_prad*(-1);
				HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, RESET);
				__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1,0);

				__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1,PWM_z_PID_prad);
				HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, SET);
			}
}



void PID_predkosc(void)
{
	uchyb_wczescniejszy_pr = uchyb_aktualny_pr;
	uchyb_aktualny_pr = predkosc_zadana - predkosc_mierzona;
	suma_uchybow_pr = suma_uchybow_pr + uchyb_aktualny_pr;
	calka_pr = Ti_predkosc*czas_probkowania_pr*suma_uchybow_pr;
	rozniczka_pr = Td_predkosc*(uchyb_aktualny_pr - uchyb_wczescniejszy_pr)/czas_probkowania_pr;
	wyjscie_z_PID_pr = Kp_predkosc*uchyb_aktualny_pr + calka_pr + rozniczka_pr;
	if(predkosc_zadana == 0)
	{
		wyjscie_z_PID_pr = 0;
	}
	if(wyjscie_z_PID_pr >= 3.5)
	{
		wyjscie_z_PID_pr = 3.5;
	}
	if(wyjscie_z_PID_pr <= -3.5)
	{
		wyjscie_z_PID_pr = -3.5;
	}
	Zadany_prad_reg_predkosci = wyjscie_z_PID_pr;
}

void PID_polozenie(void)
{
	uchyb_wczescniejszy_pol = uchyb_aktualny_pol;
	uchyb_aktualny_pol = Polozenie_zadane - Polozenie_mierzone;
	suma_uchybow_pol = suma_uchybow_pol + uchyb_aktualny_pol;
	calka_pol = Ti_polozenie*czas_probkowania_pol*suma_uchybow_pol;
	rozniczka_pol = Td_polozenie*((uchyb_aktualny_pol - uchyb_wczescniejszy_pol)/czas_probkowania_pol);
	wyjscie_z_PID_pol = Kp_polozenie*uchyb_aktualny_pol + calka_pol +rozniczka_pol;
	if(Polozenie_zadane == 0)
	{
		Zadana_predkosc_reg_polozenia = 0;
	}
		if(wyjscie_z_PID_pol >= 14)
		{
			wyjscie_z_PID_pol = 14;
		}
		if(wyjscie_z_PID_pol <= -14)
		{
			wyjscie_z_PID_pol = -14;
		}
		if(uchyb_aktualny_pol < 0)
		{
			wyjscie_z_PID_pol = 0;
		}
		Zadana_predkosc_reg_polozenia = wyjscie_z_PID_pol;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM17_Init();
  MX_TIM15_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_TIM16_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  	  HAL_GPIO_WritePin(EN_B_GPIO_Port, EN_B_Pin, RESET);
  	  HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
  	  HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
  	  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  	  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
	 HAL_UART_Receive_IT(&huart2, &Received, 10);
	 HAL_TIM_Base_Start_IT(&htim6);
	 HAL_TIM_Base_Start_IT(&htim15);
	 HAL_ADC_Start_DMA(&hadc1, adc_wartosc, 1);
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1|RCC_PERIPHCLK_TIM15
                              |RCC_PERIPHCLK_TIM16|RCC_PERIPHCLK_TIM17
                              |RCC_PERIPHCLK_ADC1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Tim15ClockSelection = RCC_TIM15CLK_HCLK;
  PeriphClkInit.Tim16ClockSelection = RCC_TIM16CLK_HCLK;
  PeriphClkInit.Tim17ClockSelection = RCC_TIM17CLK_HCLK;
  PeriphClkInit.Adc1ClockSelection = RCC_ADC1PLLCLK_DIV6;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_181CYCLES_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 27;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 99;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 27;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 99;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 27;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 99;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
  HAL_TIM_MspPostInit(&htim16);

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 27;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 99;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim17, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */
  HAL_TIM_MspPostInit(&htim17);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 57600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EN_B_GPIO_Port, EN_B_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EN_A_GPIO_Port, EN_A_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EN_B_Pin */
  GPIO_InitStruct.Pin = EN_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EN_B_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EN_A_Pin */
  GPIO_InitStruct.Pin = EN_A_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EN_A_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{


		switch(start){
		// prad plynacy w uzwojeniach silnika
		case 1:
			if (x <= 4000)
				{
					pomiar_mV = (adc_wartosc[0] * 3300 / 4095);
					Buffor_a[x] = ((pomiar_mV - napiecie_ref_czujnika)/185)*1000;
					if(x>=1000)
				{
					prad_mierzony = ((pomiar_mV - napiecie_ref_czujnika)/185);
					pomiar++;
					Prad_zadany = Prad;
				}
				x++;
				}
			if(x >=4001)
			{
				suma_uchybow_prad = 0;
				Prad_zadany = 0;
				prad_mierzony = 0;
			}
		break;

		//polozenie walu silnika
		case 2:

			if (x <= 50000)
			{
				polozenie_nowe = __HAL_TIM_GET_COUNTER(&htim2);
				polozenie_n = (int16_t)polozenie_nowe;
				zmiana_polozenia_32 = polozenie_n - polozenie_s;
				if(zmiana_polozenia_32 <= -1000)
				{
					zmiana_polozenia_32 = zmiana_polozenia_32 + 65535;
				}
				if(zmiana_polozenia_32 >= 1000)
				{
					zmiana_polozenia_32 = zmiana_polozenia_32 - 65535;
				}
				suma_zmian_polozen_1ms = suma_zmian_polozen_1ms + zmiana_polozenia_32;
				polozenie_s = polozenie_n;
				if(l_p >= 20)
				{
					polozenie_nowe = __HAL_TIM_GET_COUNTER(&htim2);
					polozenie_n = (int16_t)polozenie_nowe;
					aktualna_roznica_polozenia = suma_zmian_polozen_1ms;
					predkosc_w_obr= (float)aktualna_roznica_polozenia/4.0;
					Buffor_a[z] = polozenie_n;
					polozenie_s = polozenie_n;
					z++;
					suma_zmian_polozen_1ms = 0;
					l_p = 0;
				}
				if(x>=10000)
				{
					Polozenie_zadane = polozenie_z;
					Polozenie_mierzone = polozenie_n*0.00025;
					predkosc_zadana = Zadana_predkosc_reg_polozenia;
					predkosc_mierzona = predkosc_w_obr;
					Prad_zadany = Zadany_prad_reg_predkosci;
					pomiar_mV = (adc_wartosc[0] * 3300 / 4095);
					prad_mierzony = ((pomiar_mV - napiecie_ref_czujnika)/185);
				}

				l_p++;
				x++;
			}

			if(x >=50001)
			{
				suma_uchybow_prad = 0;
				Prad_zadany = 0;
				prad_mierzony = 0;
				suma_uchybow_pr = 0;
				predkosc_zadana = 0;
				predkosc_mierzona = 0;
				suma_uchybow_pol = 0;
				Polozenie_zadane = 0;
				Polozenie_mierzone = 0;
				__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1,0);
				__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1,0);
			}

		break;

		// predkosc silnika
		case 3:
			if (x <= 80000)
			{
				polozenie_nowe = __HAL_TIM_GET_COUNTER(&htim2);
				polozenie_n = (int16_t)polozenie_nowe;
				zmiana_polozenia_32 = polozenie_n - polozenie_s;
				if(zmiana_polozenia_32 <= -1000)
				{
					zmiana_polozenia_32 = zmiana_polozenia_32 + 65535;
				}
				if(zmiana_polozenia_32 >= 1000)
				{
					zmiana_polozenia_32 = zmiana_polozenia_32 - 65535;
				}
				suma_zmian_polozen_1ms = suma_zmian_polozen_1ms + zmiana_polozenia_32;
				polozenie_s = polozenie_n;
				if(l_p >= 20)
				{
					aktualna_roznica_polozenia = suma_zmian_polozen_1ms;
					predkosc_w_obr= (float)aktualna_roznica_polozenia/4.0;
					Buffor_a[z] = predkosc_w_obr*1000;

					z++;
					suma_zmian_polozen_1ms = 0;
					l_p = 0;

				}
				if(x>=4000)
				{
					predkosc_zadana = predkosc_z;
					predkosc_mierzona = predkosc_w_obr;
					Prad_zadany = Zadany_prad_reg_predkosci;
					pomiar_mV = (adc_wartosc[0] * 3300 / 4095);
					prad_mierzony = ((pomiar_mV - napiecie_ref_czujnika)/185);
				}
				l_p++;
				x++;
			}
			if(x >=80001)
			{
				suma_uchybow_prad = 0;
				Prad_zadany = 0;
				prad_mierzony = 0;
				suma_uchybow_pr = 0;
				predkosc_zadana = 0;
				predkosc_mierzona = 0;
				predkosc_w_obr= 0;
				__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1,0);
				__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1,0);
			}

		break;

		//polozenie masy
		case 4:
			if (x <= 50000)
			{
				polozenie_nowe = __HAL_TIM_GET_COUNTER(&htim1);
				polozenie_n = (int16_t)polozenie_nowe;
				zmiana_polozenia_32 = polozenie_n - polozenie_s;
				if(zmiana_polozenia_32 <= -1000)
				{
					zmiana_polozenia_32 = zmiana_polozenia_32 + 65535;
				}
				if(zmiana_polozenia_32 >= 1000)
				{
					zmiana_polozenia_32 = zmiana_polozenia_32 - 65535;
				}
				suma_zmian_polozen_1ms = suma_zmian_polozen_1ms + zmiana_polozenia_32;

				polozenie_s = polozenie_n;
				if(l_p >= 20)
				{
					polozenie_nowe = __HAL_TIM_GET_COUNTER(&htim1);
					polozenie_n = (int16_t)polozenie_nowe;
					aktualna_roznica_polozenia = suma_zmian_polozen_1ms;
					predkosc_w_obr= (float)aktualna_roznica_polozenia/4.0;
					Buffor_a[z] = polozenie_n;
					z++;
					suma_zmian_polozen_1ms = 0;
					l_p = 0;
				}
				if(x>=10000)
				{
					Polozenie_zadane = polozenie_z;
					Polozenie_mierzone = polozenie_n*0.00025;
					predkosc_zadana = Zadana_predkosc_reg_polozenia;
					predkosc_mierzona = predkosc_w_obr;
					Prad_zadany = Zadany_prad_reg_predkosci;
					pomiar_mV = (adc_wartosc[0] * 3300 / 4095);
					prad_mierzony = ((pomiar_mV - napiecie_ref_czujnika)/185);


				}
				l_p++;
				x++;
			}
			if(x >=50001)
			{
				suma_uchybow_prad = 0;
				Prad_zadany = 0;
				prad_mierzony = 0;
				suma_uchybow_pr = 0;
				predkosc_zadana = 0;
				predkosc_mierzona = 0;
				suma_uchybow_pol = 0;
				Polozenie_zadane = 0;
				Polozenie_mierzone = 0;
				__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1,0);
				__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1,0);
			}
			break;

		//predkosc masy
		case 5:
			if (x <= 80000)
			{
				polozenie_nowe = __HAL_TIM_GET_COUNTER(&htim1);
				polozenie_n = (int16_t)polozenie_nowe;
				zmiana_polozenia_32 = polozenie_n - polozenie_s;
				if(zmiana_polozenia_32 <= -1000)
				{
					zmiana_polozenia_32 = zmiana_polozenia_32 + 65535;
				}
				if(zmiana_polozenia_32 >= 1000)
				{
					zmiana_polozenia_32 = zmiana_polozenia_32 - 65535;
				}
				suma_zmian_polozen_1ms = suma_zmian_polozen_1ms + zmiana_polozenia_32;
				polozenie_s = polozenie_n;
				if(l_p >= 20)
					{
						aktualna_roznica_polozenia = suma_zmian_polozen_1ms;
						predkosc_w_obr= (float)aktualna_roznica_polozenia/4.0;
						Buffor_a[z] = predkosc_w_obr*1000;
						z++;
						suma_zmian_polozen_1ms = 0;
						l_p = 0;
					}
				if(x>=4000)
					{
						predkosc_zadana = predkosc_z;
						predkosc_mierzona = predkosc_w_obr;
						Prad_zadany = Zadany_prad_reg_predkosci;
						pomiar_mV = (adc_wartosc[0] * 3300 / 4095);
						prad_mierzony = ((pomiar_mV - napiecie_ref_czujnika)/185);
					}
					l_p++;

					x++;
				}
				if(x >=80001)
				{
					suma_uchybow_prad = 0;
					Prad_zadany = 0;
					prad_mierzony = 0;
					suma_uchybow_pr = 0;
					predkosc_zadana = 0;
					predkosc_mierzona = 0;
					__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1,0);
					__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1,0);
				}
			break;
	}

}

	if(htim->Instance == TIM15)
	{

		PID_prad();

		if(licznik_KASKADA_PID>=20)
			{
				PID_predkosc();

				PID_polozenie();

				licznik_KASKADA_PID = 0;
			}
		licznik_KASKADA_PID++;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
