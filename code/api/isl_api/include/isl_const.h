/*
 *     Name: isl_const.h
 *
 *     Description: ISL API constants.
 *
 *     Modification date: 04/03/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _ISL_CONST_H_
#define _ISL_CONST_H_

/*
 *     Constants definition
 */

// Default log file name
#ifndef LOG_FILE_NAME
#define LOG_FILE_NAME				"openisl"
#endif

// Default maximum of FIFO depth
#define DEFAULT_MAX_FIFO_DEPTH		1024

// Default maximum number of simulations running
#define DEFAULT_MAX_NB_RUN_SIMS		256

// Default maximum of number of readers
#define DEFAULT_MAX_NB_READERS		16

// Default maximum size of a string (SHM)
#define DEFAULT_MAX_SHM_STRING_SIZE	1024

// Time value for event based exchange
#define EVENT_DEF_TIME_VAL			-1.0

// Default Step Tolerance
#define DEFAULT_STEP_TOLERANCE	1e-6

#endif // _ISL_CONST_H_
