/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include "apps.h"

int side_apps_load(void)
{
	side_apps_dir = opendir(APP_DIR);
	if(side_apps_dir == NULL)
		return -1;
	else
		return 0;
}

AppEntry *side_apps_get_next_entry(void)
{
	AppEntry *ret = malloc(sizeof(AppEntry));

	if(ret == NULL)
		return NULL;

	struct dirent *d;

	d = readdir(side_apps_dir);

	if(d == NULL)
		return NULL;

	while(strlen(d->d_name) < 8) //.desktop == 8 so if name is shoter, not a .desktop file
		d = readdir(side_apps_dir);

	char *filename = malloc(sizeof(d->d_name) + sizeof(APP_DIR));
	strcat(filename, APP_DIR);
	strcat(filename, d->d_name);

	side_apps_entry_file = fopen(filename, "r");
	if(side_apps_entry_file == NULL)
		return NULL;

	char buffer[2000];
	while(fgets(buffer, 2000, side_apps_entry_file) != NULL)
	{
		if(strncmp(buffer, "Name=", 5) == 0)
		{//we found the name(!!!)
			ret->app_name = malloc(strlen(buffer) - 5);
			strtok(buffer, "=");
			char *ptr = strtok(NULL, "\n");
			strcpy(ret->app_name, ptr != NULL ? ptr : "");
		}
		if(strncmp(buffer, "Exec=", 5) == 0)
		{//EXEC PATH
			ret->exec = malloc(strlen(buffer) -5);
			strtok(buffer, "=");
			char *ptr = strtok(NULL, "\n");
			strcpy(ret->exec, ptr != NULL ? ptr : "");
		}
		if(strncmp(buffer , "Type=", 5) == 0)
		{//type
			if(strcmp(buffer, "Type=Application") == 0)
			ret->type = APP_TYPE_APPLICATION;
		}
		if(strncmp(buffer, "Categories=", 11) == 0)
		{

			if(strlen(buffer) == 11)
				ret->sub = APP_TYPE_UNKNOWN; //does not contain categories...
			else
			{
				char *b = malloc(sizeof(buffer) -11);
				strtok(buffer, "=");
				strcpy(b, strtok(NULL, "\n"));


				//Now phrase a category

				char *ptr = strtok(b, ";\n");

				ret->sub = APP_TYPE_UNKNOWN;

				while(ptr != NULL)
				{
					if(strcmp(ptr, "AudioVideo") == 0 || strcmp(ptr, "Audio") == 0 || strcmp(ptr, "Video") == 0)
					{
						ret->sub = APP_TYPE_MULTIMEDIA;
					}
					if(strcmp(ptr, "Development") == 0)
					{
						ret->sub = APP_TYPE_DEVELOPMENT;
					}
					if(strcmp(ptr, "Education") == 0)
					{
						ret->sub = APP_TYPE_EDUCATION;
					}
					if(strcmp(ptr, "Game") == 0)
					{
						ret->sub = APP_TYPE_GAME;
					}
					if(strcmp(ptr, "Graphics") == 0)
					{
						ret->sub = APP_TYPE_GRAPHICS;
					}
					if(strcmp(ptr, "Network") == 0)
					{
						ret->sub = APP_TYPE_NETWORK;
					}
					if(strcmp(ptr, "Office") == 0)
					{
						ret->sub = APP_TYPE_OFFICE;
					}
					if(strcmp(ptr, "Science") == 0)
					{
						ret->sub = APP_TYPE_SCIENCE;
					}
					if(strcmp(ptr, "Settings") == 0)
					{
						ret->sub = APP_TYPE_SETTINGS;
					}
					if(strcmp(ptr, "System") == 0)
					{
						ret->sub = APP_TYPE_SYSTEM;
					}
					if(strcmp(ptr, "Utility") == 0)
					{
						ret->sub = APP_TYPE_UTILITY;
					}

					for(int x = 0; x < strlen(ptr); x++)
						{
							if(ptr[x] != ';')
								continue;
							else
								puts("FOUND");
						}
					ptr = strtok(NULL, ";\n");
				}
			}
		}

	}


	return ret;
}

int side_apps_close(void)
{
	closedir(side_apps_dir);

	if(side_apps_dir != NULL)
		return -1;
	else
		return 0;

}
