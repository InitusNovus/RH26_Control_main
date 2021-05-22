/* 
 * Gpio_Debounce.h
 * Created on: 2019. 10. 25.
 * Author: Dua
 */

#ifndef GPIO_DEBOUNCE
#define GPIO_DEBOUNCE
/* Includes */
#include "IfxPort.h"
#include "Ifx_Types.h"


/* Enumeration */
typedef enum
{
	Gpio_Debounce_BufferLength_noDebounce = 0,
	Gpio_Debounce_BufferLength_1 = 1,
	Gpio_Debounce_BufferLength_2 = 2,
	Gpio_Debounce_BufferLength_3 = 3,
	Gpio_Debounce_BufferLength_4 = 4,
	Gpio_Debounce_BufferLength_5 = 5 ,
	Gpio_Debounce_BufferLength_6 = 6,
	Gpio_Debounce_BufferLength_7 = 7,
	Gpio_Debounce_BufferLength_8 = 8,
	Gpio_Debounce_BufferLength_9 = 9,
	Gpio_Debounce_BufferLength_10 = 10,
	Gpio_Debounce_BufferLength_11 = 11,
	Gpio_Debounce_BufferLength_12 = 12,
	Gpio_Debounce_BufferLength_13 = 13,
	Gpio_Debounce_BufferLength_14 = 14,
	Gpio_Debounce_BufferLength_15 = 15,
	Gpio_Debounce_BufferLength_16 = 16,
	Gpio_Debounce_BufferLength_17 = 17,
	Gpio_Debounce_BufferLength_18 = 18,
	Gpio_Debounce_BufferLength_19 = 19,
	Gpio_Debounce_BufferLength_20 = 20,
	Gpio_Debounce_BufferLength_21 = 21,
	Gpio_Debounce_BufferLength_22 = 22,
	Gpio_Debounce_BufferLength_23 = 23,
	Gpio_Debounce_BufferLength_24 = 24,
	Gpio_Debounce_BufferLength_25 = 25,
	Gpio_Debounce_BufferLength_26 = 26,
	Gpio_Debounce_BufferLength_27 = 27,
	Gpio_Debounce_BufferLength_28 = 28,
	Gpio_Debounce_BufferLength_29 = 29,
	Gpio_Debounce_BufferLength_30 = 30,
	Gpio_Debounce_BufferLength_31 = 31,
	Gpio_Debounce_BufferLength_32 = 32,
}Gpio_Debounce_BufferLength;


/* Data Structures */
typedef uint32 Gpio_Debounce_Buffer;

typedef struct 
{
    IfxPort_Pin* port;			//input pin
    boolean on;
	boolean flag;
	Gpio_Debounce_BufferLength bufferLen;
	Gpio_Debounce_Buffer buffer;
	Gpio_Debounce_Buffer bufferMask;
}Gpio_Debounce_input;

typedef struct
{
	IfxPort_Pin* port;
	IfxPort_InputMode inputMode;
	Gpio_Debounce_BufferLength bufferLen;
}Gpio_Debounce_inputConfig;


/* Function Prototypes */
IFX_EXTERN void Gpio_Debounce_initInput(Gpio_Debounce_input* input, Gpio_Debounce_inputConfig* config);
IFX_EXTERN boolean Gpio_Debounce_pollInput(Gpio_Debounce_input* input);
IFX_EXTERN void Gpio_Debounce_initInputConfig(Gpio_Debounce_inputConfig* config);

#endif