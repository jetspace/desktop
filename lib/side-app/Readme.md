#SIDE Apps library

This library phrases the .desktop files on your system

##Usage:

when complilling with GCC use '-lside-apps' and include <side/apps.h>

##Functions:

###side_apps_load(void)

You have to call this function first

	-> It will init all needed things of the lib

###side_apps_close(void)

Call this at the end, will do a basic cleanup

###side_apps_get_next_entry(void)

Return the next .desktop file contents

##Data Types

struct AppEntry
{
	char *app_name //Name of the Application
	char *exec     //Executeable of the App
	short type     //Type (Unknown/Application)
	short sub      //SubType (Graphics/Game/Network)
}

