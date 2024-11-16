/*
 * lab4.c
 *
 *  Created on: Nov 15, 2024
 *      Author: dinhq
 */

#include "lab4.h"
#include "lcd.h"
#include "ds3231.h"
#include "button.h"

uint16_t counter_modify_clock = 0;
uint16_t counter_200ms_clock = 0;
uint16_t state_modify_clock = MODIFY_DATE;
uint16_t state_main_clock = NORMAL_MODE;



void update_state(){
	if (button_count[0] == 1){
		if (state_main_clock == ALARM_MODE){
			ds3231_ReadTime();
			displayTime();
			state_main_clock = NORMAL_MODE;
			lcd_Fill(0, 250, 240, 320, BLACK);
			lcd_ShowStr(70, 250, "NORMAL MODE", WHITE, BLACK, 24, 0);
		}
		else if (state_main_clock == NORMAL_MODE){
			ds3231_ReadTime();
			displayTime();
			state_main_clock = MODIFY_MODE;
			state_modify_clock = MODIFY_HOUR;
			lcd_Fill(0, 250, 240, 320, BLACK);
			lcd_ShowStr(70, 250, "MODIFY MODE", WHITE, BLACK, 24, 0);
		}
		else{
			ds3231_ReadTime();
			displayTime();
			take_current_time_for_alarm();
			state_main_clock = ALARM_MODE;
			state_modify_clock = MODIFY_HOUR;
			lcd_Fill(0, 250, 240, 320, BLACK);
			lcd_ShowStr(70, 250, "ALARM_MODE", WHITE, BLACK, 24, 0);
		}
	}
	if (button_count[12] == 1){
		if (state_main_clock == MODIFY_MODE){
			displayTime();
			state_modify_clock = (state_modify_clock+1)%7;
		}
		else if (state_main_clock == ALARM_MODE){
			display_time_alarm();
			state_modify_clock = (state_modify_clock+1)%7;
		}
	}
}

void clock_run(){
	switch(state_main_clock){
	case NORMAL_MODE:
		ds3231_ReadTime();
		displayTime();
		check_alarm();
		break;
	case MODIFY_MODE:
		modify_clock();
		break;
	case ALARM_MODE:
		alarm_clock();
		break;
	default:
		break;
	}
}

void modify_clock(){
	counter_modify_clock = (counter_modify_clock+1)%10;
	switch(state_modify_clock){
	case MODIFY_DATE:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(70, 130, ds3231_date, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			if (ds3231_month == 1 || ds3231_month == 3 || ds3231_month == 5 || ds3231_month == 7 || ds3231_month == 8 || ds3231_month == 10 || ds3231_month == 12){
				ds3231_date = ds3231_date%31+1;
			}
			else if (ds3231_month == 4 || ds3231_month == 6 || ds3231_month == 9 || ds3231_month == 11){
				ds3231_date = ds3231_date%30+1;
			}
			else{
				ds3231_date = ds3231_date%28+1;
			}
			ds3231_Write(ADDRESS_DATE, ds3231_date);
			ds3231_ReadTime();
			lcd_ShowIntNum(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
		}

		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				if (ds3231_month == 1 || ds3231_month == 3 || ds3231_month == 5 || ds3231_month == 7 || ds3231_month == 8 || ds3231_month == 10 || ds3231_month == 12){
					ds3231_date = ds3231_date%31+1;
				}
				else if (ds3231_month == 4 || ds3231_month == 6 || ds3231_month == 9 || ds3231_month == 11){
					ds3231_date = ds3231_date%30+1;
				}
				else{
					ds3231_date = ds3231_date%28+1;
				}
				ds3231_Write(ADDRESS_DATE, ds3231_date);
				ds3231_ReadTime();
				lcd_ShowIntNum(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
			}
		}
		break;
	case MODIFY_HOUR:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(70, 100, ds3231_hours, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			ds3231_hours = (ds3231_hours+1)%24;
			ds3231_Write(ADDRESS_HOUR, ds3231_hours);
			ds3231_ReadTime();
			lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				ds3231_hours = (ds3231_hours+1)%24;
				ds3231_Write(ADDRESS_HOUR, ds3231_hours);
				ds3231_ReadTime();
				lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
			}
		}
		break;
	case MODIFY_MINUTE:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(110, 100, ds3231_min, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			ds3231_min = (ds3231_min+1)%60;
			ds3231_Write(ADDRESS_MIN, ds3231_min);
			ds3231_ReadTime();
			lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				ds3231_min = (ds3231_min+1)%60;
				ds3231_Write(ADDRESS_MIN, ds3231_min);
				ds3231_ReadTime();
				lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24);
			}
		}
		break;
	case MODIFY_SECOND:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(150, 100, ds3231_sec, 2, GREEN, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(150, 100, ds3231_sec, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			ds3231_sec = (ds3231_sec+1)%60;
			ds3231_Write(ADDRESS_SEC, ds3231_sec);
			ds3231_ReadTime();
			lcd_ShowIntNum(150, 100, ds3231_sec, 2, GREEN, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				ds3231_sec = (ds3231_sec+1)%60;
				ds3231_Write(ADDRESS_SEC, ds3231_sec);
				ds3231_ReadTime();
				lcd_ShowIntNum(150, 100, ds3231_sec, 2, GREEN, BLACK, 24);
			}
		}
		break;
	case MODIFY_DAY:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(20, 130, ds3231_day+1, 2, YELLOW, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(20, 130, ds3231_day+1, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			ds3231_day += 1;
			ds3231_Write(ADDRESS_DAY, ds3231_day);
			ds3231_ReadTime();
			lcd_ShowIntNum(20, 130, ds3231_day+1, 2, YELLOW, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				ds3231_day += 1;
				ds3231_Write(ADDRESS_DAY, ds3231_day);
				ds3231_ReadTime();
				lcd_ShowIntNum(20, 130, ds3231_day+1, 2, YELLOW, BLACK, 24);
			}
		}
		break;
	case MODIFY_MONTH:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(110, 130, ds3231_month, 2, YELLOW, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(110, 130, ds3231_month, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			ds3231_month = ds3231_month%12+1;
			ds3231_Write(ADDRESS_MONTH, ds3231_month);
			ds3231_ReadTime();
			lcd_ShowIntNum(110, 130, ds3231_month, 2, YELLOW, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				ds3231_month = ds3231_month%12+1;
				ds3231_Write(ADDRESS_MONTH, ds3231_month);
				ds3231_ReadTime();
				lcd_ShowIntNum(110, 130, ds3231_month, 2, YELLOW, BLACK, 24);
			}
		}
		break;
	case MODIFY_YEAR:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(150, 130, ds3231_year, 2, YELLOW, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(150, 130, ds3231_year, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			ds3231_year += 1;
			ds3231_Write(ADDRESS_YEAR, ds3231_year);
			ds3231_ReadTime();
			lcd_ShowIntNum(150, 130, ds3231_year, 2, YELLOW, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				ds3231_year += 1;
				ds3231_Write(ADDRESS_YEAR, ds3231_year);
				ds3231_ReadTime();
				lcd_ShowIntNum(150, 130, ds3231_year, 2, YELLOW, BLACK, 24);
			}
		}
		break;
	default:
		break;
	}

}

