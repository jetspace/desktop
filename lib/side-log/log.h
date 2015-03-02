/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef _SIDE_LOG_LIBRARY
#define _SIDE_LOG_LIBRARY

#include <stdio.h>  //printing
#include <stdlib.h> //memory
#include <string.h> //creating output

enum
{//print all with log level >= log_level
  SIDE_LOG_LEVEL_DEBUG   = 0,
  SIDE_LOG_LEVEL_NOTE,
  SIDE_LOG_LEVEL_MESSAGE,
  SIDE_LOG_LEVEL_WARNING,
  SIDE_LOG_LEVEL_ERROR,
  SIDE_LOG_LEVEL_CRITICAL,
  SIDE_LOG_LEVEL_FAILURE
};

//set to a default, usefull level
int SIDE_LOGLEVEL = SIDE_LOG_LEVEL_MESSAGE;


//Get current Log Level
#define side_log_get_log_level() SIDE_LOGLEVEL

//Set Log Level
#define side_log_set_log_level(x) SIDE_LOGLEVEL = x

//log macros
#define side_log_debug(x) side_log(SIDE_LOG_LEVEL_DEBUG, x)
#define side_log_note(x) side_log(SIDE_LOG_LEVEL_NOTE, x)
#define side_log_message(x) side_log(SIDE_LOG_LEVEL_MESSAGE, x)
#define side_log_warning(x) side_log(SIDE_LOG_LEVEL_WARNING, x)
#define side_log_error(x) side_log(SIDE_LOG_LEVEL_ERROR, x)
#define side_log_critical(x) side_log(SIDE_LOG_LEVEL_CRITICAL, x)
#define side_log_failure(x) side_log(SIDE_LOG_LEVEL_FAILURE, x)

//log_function
short side_log(short type, char *err);

//set log_level from env
void side_log_set_log_level_from_enviroment(void);



#endif
