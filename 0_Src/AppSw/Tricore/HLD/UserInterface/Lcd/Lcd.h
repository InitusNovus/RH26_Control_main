/*
 * Lcd.h
 * 2019. 7. 5.
 */
#ifndef HLD_USERINTERFACE_LCD_H_
#define HLD_USERINTERFACE_LCD_H_

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include <Ifx_Types.h>
#include "Configuration.h"
#include "ConfigurationIsr.h"
#include "IfxPort.h"
/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/
#define LCD_ROW0	LcdLine0
#define LCD_ROW1	LcdLine1
#define LCD_ROW2	LcdLine2
#define LCD_ROW3	LcdLine3

/******************************************************************************/
/*------------------------------Type Definitions------------------------------*/
/******************************************************************************/


/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

typedef enum
{
	LcdLine0 = 0x00,
	LcdLine1 = 0x40,
	LcdLine2 = 0x14,
	LcdLine3 = 0x54,
}HLD_Lcd_line;


/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/


/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/


/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_EXTERN void HLD_Lcd_init(void);

IFX_EXTERN void HLD_Lcd_clear(void);
//IFX_EXTERN void HLD_Lcd_dispLine(HLD_Lcd_line line, char* text);
IFX_EXTERN void HLD_Lcd_dispLine(HLD_Lcd_line line,  char const *fmt, ...);
IFX_EXTERN void HLD_Lcd_dispRowCol(HLD_Lcd_line row, uint8 col, char* text);
IFX_EXTERN void HLD_Lcd_dispCharRowCol(HLD_Lcd_line row, uint8 col, char text);

/******************************************************************************/
/*---------------------Inline Function Implementations------------------------*/
/******************************************************************************/



#endif /* 0_SRC_APPSW_TRICORE_HLD_USERINTERFACE_LCD_H_ */
