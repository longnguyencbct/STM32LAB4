/*
 * software_timer.h
 *
 *  Created on: Oct 18, 2023
 *      Author: clong
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

extern int timer_flag[];

void setTimer(int timer_index,int duration);
void timerRun();

#endif /* INC_SOFTWARE_TIMER_H_ */
