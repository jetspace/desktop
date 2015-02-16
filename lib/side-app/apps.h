/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef _SIDE_APP_LIBRARY
#define _SIDE_APP_LIBRARY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#define APP_DIR "/usr/share/applications/"

#define SIDE_APP_VERSION "1" //will be changed if changes are made which are revlevant for compatability

//AppTypes
#define APP_TYPE_APPLICATION 0
#define APP_TYPE_UNKNOWN     -1

//SubTypes
#define APP_TYPE_MULTIMEDIA   0 //this lib don't make a differnce between PIC/VIDEO
#define APP_TYPE_DEVELOPMENT 1
#define APP_TYPE_EDUCATION    2
#define APP_TYPE_GAME         3
#define APP_TYPE_GRAPHICS     4
#define APP_TYPE_NETWORK      5
#define APP_TYPE_OFFICE       6
#define APP_TYPE_SCIENCE      7
#define APP_TYPE_SETTINGS     8
#define APP_TYPE_SYSTEM       9
#define APP_TYPE_UTILITY      10

typedef short AppType;
typedef short SubType;

typedef struct
{
	char *app_name;
	size_t app_name_length;
	char *exec;
	size_t exec_length;
	char *gen_name;
	size_t gen_name_length;
	bool show;
	bool terminal;
	AppType type;
	SubType sub;
}AppEntry;

DIR  *side_apps_dir;
FILE *side_apps_entry_file;


int side_apps_load(void);
AppEntry *side_apps_get_next_entry(void);
int side_apps_close(void);


#endif
