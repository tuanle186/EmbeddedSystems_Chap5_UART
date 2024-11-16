/*
 * alarm.c
 *
 *  Created on: Nov 15, 2024
 *      Author: dinhq
 */

#include "lab4.h"
#include "lcd.h"
#include "ds3231.h"
#include "button.h"

uint8_t alarm_hours = 0;
uint8_t alarm_min = 0;
uint8_t alarm_sec = 0;
uint8_t alarm_year = 0;
uint8_t alarm_month = 0;
uint8_t alarm_date = 0;
uint8_t alarm_day= 0;
uint8_t counter_alarm = 0;
uint8_t alarm_flag = 0;

void display_time_alarm(){
	lcd_ShowIntNum(70, 100, alarm_hours, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(110, 100, alarm_min, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 100, alarm_sec, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(20, 130, alarm_day+1, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(70, 130, alarm_date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(110, 130, alarm_month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(150, 130, alarm_year, 2, YELLOW, BLACK, 24);
}

void take_current_time_for_alarm(){
	alarm_hours = ds3231_hours;
	alarm_min = ds3231_min;
	alarm_sec = ds3231_sec;
	alarm_year = ds3231_year;
	alarm_month = ds3231_month;
	alarm_date = ds3231_date;
	alarm_day= ds3231_day;
}
void alarm_clock(){
	counter_modify_clock = (counter_modify_clock+1)%10;
	switch(state_modify_clock){
	case MODIFY_DATE:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(70, 130, alarm_date, 2, YELLOW, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(70, 130, alarm_date, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			if (alarm_month == 1 || alarm_month == 3 || alarm_month == 5 || alarm_month == 7 || alarm_month == 8 || alarm_month == 10 || alarm_month == 12){
				alarm_date = alarm_date%31+1;
			}
			else if (alarm_month == 4 || alarm_month == 6 || alarm_month == 9 || alarm_month == 11){
				alarm_date = alarm_date%30+1;
			}
			else{
				alarm_date = alarm_date%28+1;
			}
			lcd_ShowIntNum(70, 130, alarm_date, 2, YELLOW, BLACK, 24);
		}

		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				if (alarm_month == 1 || alarm_month == 3 || alarm_month == 5 || alarm_month == 7 || alarm_month == 8 || alarm_month == 10 || alarm_month == 12){
					alarm_date = alarm_date%31+1;
				}
				else if (alarm_month == 4 || alarm_month == 6 || alarm_month == 9 || alarm_month == 11){
					alarm_date = alarm_date%30+1;
				}
				else{
					alarm_date = alarm_date%28+1;
				}
				lcd_ShowIntNum(70, 130, alarm_date, 2, YELLOW, BLACK, 24);
			}
		}
		break;
	case MODIFY_HOUR:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(70, 100, alarm_hours, 2, GREEN, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(70, 100, alarm_hours, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			alarm_hours = (alarm_hours+1)%24;
			lcd_ShowIntNum(70, 100, alarm_hours, 2, GREEN, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				alarm_hours = (alarm_hours+1)%24;
				lcd_ShowIntNum(70, 100, alarm_hours, 2, GREEN, BLACK, 24);
			}
		}
		break;
	case MODIFY_MINUTE:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(110, 100, alarm_min, 2, GREEN, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(110, 100, alarm_min, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			alarm_min = (alarm_min+1)%60;
			lcd_ShowIntNum(110, 100, alarm_min, 2, GREEN, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				alarm_min = (alarm_min+1)%60;
				lcd_ShowIntNum(110, 100, alarm_min, 2, GREEN, BLACK, 24);
			}
		}
		break;
	case MODIFY_SECOND:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(150, 100, alarm_sec, 2, GREEN, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(150, 100, alarm_sec, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			alarm_sec = (alarm_sec+1)%60;
			lcd_ShowIntNum(150, 100, alarm_sec, 2, GREEN, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				alarm_sec = (alarm_sec+1)%60;
				lcd_ShowIntNum(150, 100, alarm_sec, 2, GREEN, BLACK, 24);
			}
		}
		break;
	case MODIFY_DAY:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(20, 130, alarm_day+1, 2, YELLOW, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(20, 130, alarm_day+1, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			alarm_day += 1;
			lcd_ShowIntNum(20, 130, alarm_day+1, 2, YELLOW, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				alarm_day += 1;
				lcd_ShowIntNum(20, 130, alarm_day+1, 2, YELLOW, BLACK, 24);
			}
		}
		break;
	case MODIFY_MONTH:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(110, 130, alarm_month, 2, YELLOW, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(110, 130, alarm_month, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			alarm_month = alarm_month%12+1;
			lcd_ShowIntNum(110, 130, alarm_month, 2, YELLOW, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				alarm_month = alarm_month%12+1;
				lcd_ShowIntNum(110, 130, alarm_month, 2, YELLOW, BLACK, 24);
			}
		}
		break;
	case MODIFY_YEAR:
		if (counter_modify_clock == 0){
			lcd_ShowIntNum(150, 130, alarm_year, 2, YELLOW, BLACK, 24);
		}
		else if (counter_modify_clock == 5){
			lcd_ShowIntNum(150, 130, alarm_year, 2, BLACK, BLACK, 24);
		}

		if (button_count[3] == 1){
			alarm_year += 1;
			lcd_ShowIntNum(150, 130, alarm_year, 2, YELLOW, BLACK, 24);
		}
		if (button_count[3] >= 40){
			counter_200ms_clock = (counter_200ms_clock+1)%4;
			if (counter_200ms_clock == 0){
				alarm_year += 1;
				lcd_ShowIntNum(150, 130, alarm_year, 2, YELLOW, BLACK, 24);
			}
		}
		break;
	default:
		break;
	}

}

void check_alarm(){
	if (alarm_hours == ds3231_hours && alarm_min == ds3231_min && alarm_sec == ds3231_sec && alarm_year == ds3231_year && alarm_month == ds3231_month && alarm_date == ds3231_date && alarm_day == ds3231_day){
		alarm_flag = 1;
	}
	if (alarm_flag){
		counter_alarm += 1;
		lcd_ShowStr(70, 200, "ALARM", WHITE, BLACK, 24, 0);
		if (counter_alarm == 200){
			counter_alarm = 0;
			alarm_flag = 0;
			lcd_ShowStr(70, 200, "ALARM", BLACK, BLACK, 24, 0);
		}
	}
}
