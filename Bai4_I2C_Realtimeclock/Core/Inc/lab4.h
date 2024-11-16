/*
 * lab4.h
 *
 *  Created on: Nov 15, 2024
 *      Author: dinhq
 */

#ifndef INC_LAB4_H_
#define INC_LAB4_H_

#include "main.h"

#define MODIFY_HOUR 	0
#define MODIFY_MINUTE 	1
#define MODIFY_SECOND 	2
#define MODIFY_YEAR 	3
#define MODIFY_MONTH 	4
#define MODIFY_DATE 	5
#define MODIFY_DAY 		6

#define NORMAL_MODE		7
#define MODIFY_MODE		8
#define ALARM_MODE		9

extern uint16_t counter_modify_clock;
extern uint16_t counter_200ms_clock;
extern uint16_t state_modify_clock;
extern uint16_t state_main_clock;

extern uint8_t alarm_hours;
extern uint8_t alarm_min;
extern uint8_t alarm_sec;
extern uint8_t alarm_year;
extern uint8_t alarm_month;
extern uint8_t alarm_date;
extern uint8_t alarm_day;
extern uint8_t counter_alarm;
extern uint8_t alarm_flag;

void update_state();
void modify_clock();
void alarm_clock();
void take_current_time_for_alarm();
void check_alarm();
void display_time_alarm();

#endif /* INC_LAB4_H_ */
