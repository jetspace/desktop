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

AppEntry side_apps_get_next_entry(void)
{
	AppEntry ret;
	ret.valid = false;
	bool name = false;
	bool path = false;
	bool g_name = false;


	struct dirent *d;

	d = readdir(side_apps_dir);

	if(d == NULL)
		return ret;

	while(strlen(d->d_name) < 8) //.desktop == 8 so if name is shoter, not a .desktop file
		d = readdir(side_apps_dir);

	char *filename = malloc(sizeof(d->d_name) + sizeof(APP_DIR));
	filename[0] = '\0';
	strcpy(filename, APP_DIR);
	strcat(filename, d->d_name);

	side_apps_entry_file = fopen(filename, "r");
	if(side_apps_entry_file == NULL)
		return ret;

	ret.show = true;
	ret.terminal = false;

	char buffer[2000];
	while(fgets(buffer, 2000, side_apps_entry_file) != NULL)
	{
		if(strncmp(buffer, "Name=", 5) == 0 && name == false)
		{//we found the name(!!!)
			name = true;
			strtok(buffer, "=");
			char *ptr = strtok(NULL, "\n");
			strncpy(ret.app_name, ptr != NULL ? ptr : "", 200);
			ret.app_name_length = strlen(ret.app_name);
		}
		if(strncmp(buffer, "GenericName=", 12) == 0 && g_name == false)
		{//we found the gen name(!!!)
			g_name = true;
			strtok(buffer, "=");
			char *ptr = strtok(NULL, "\n");
			strncpy(ret.gen_name, ptr != NULL ? ptr : "", 200);
			ret.gen_name_length = strlen(ret.gen_name);
		}
		if(strncmp(buffer, "Exec=", 5) == 0 && path == false)
		{//EXEC PATH
			path = true;
			strtok(buffer, "=");
			char *ptr = strtok(NULL, "\n");
			strncpy(ret.exec, ptr != NULL ? ptr : "", 2000);
			ret.exec_length = strlen(ret.exec);
			ret.exec[strlen(ret.exec)] = '\0';
		}
		if(strncmp(buffer , "Terminal=", 9) == 0)
		{//type
			if(strncmp(buffer, "Terminal=true", 13) == 0)
			ret.terminal = true;
		}
		if(strncmp(buffer , "Type=", 5) == 0)
		{//type
			if(strncmp(buffer, "Type=Application", 16) == 0)
			ret.type = APP_TYPE_APPLICATION;
		}
		if(strncmp(buffer , "NoDisplay=", 10) == 0)
		{//visible
			if(strncmp(buffer, "NoDisplay=true", 14) == 0)
			ret.show = false;
		}
		if(strncmp(buffer, "Categories=", 11) == 0)
		{

			if(strlen(buffer) == 11)
				ret.sub = APP_TYPE_UNKNOWN; //does not contain categories...
			else
			{
				ret.sub = APP_TYPE_UNKNOWN;
				if(strtok(buffer, "=") == NULL)
					continue;

				char *b = strtok(NULL, "\n");

				if(b == NULL)
					continue;


				//Now phrase a category

				char *ptr = strtok(b, ";\n");



				while(ptr != NULL)
				{
					if(strcmp(ptr, "AudioVideo") == 0 || strcmp(ptr, "Audio") == 0 || strcmp(ptr, "Video") == 0)
					{
						ret.sub = APP_TYPE_MULTIMEDIA;
						break;
					}
					if(strcmp(ptr, "Development") == 0)
					{
						ret.sub = APP_TYPE_DEVELOPMENT;
						break;
					}
					if(strcmp(ptr, "Education") == 0)
					{
						ret.sub = APP_TYPE_EDUCATION;
						break;
					}
					if(strcmp(ptr, "Game") == 0)
					{
						ret.sub = APP_TYPE_GAME;
						break;
					}
					if(strcmp(ptr, "Graphics") == 0)
					{
						ret.sub = APP_TYPE_GRAPHICS;
						break;
					}
					if(strcmp(ptr, "Network") == 0)
					{
						ret.sub = APP_TYPE_NETWORK;
						break;
					}
					if(strcmp(ptr, "Office") == 0)
					{
						ret.sub = APP_TYPE_OFFICE;
						break;
					}
					if(strcmp(ptr, "Science") == 0)
					{
						ret.sub = APP_TYPE_SCIENCE;
						break;
					}
					if(strcmp(ptr, "Settings") == 0)
					{
						ret.sub = APP_TYPE_SETTINGS;
						break;
					}
					if(strcmp(ptr, "System") == 0)
					{
						ret.sub = APP_TYPE_SYSTEM;
						break;
					}
					if(strcmp(ptr, "Utility") == 0)
					{
						ret.sub = APP_TYPE_UTILITY;
						break;
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

	ret.valid = true;
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
