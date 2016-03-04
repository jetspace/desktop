/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include "apps.h"

int side_apps_load(void)
{
	APP_DIR = "/usr/share/applications/";
	side_apps_dir = opendir(APP_DIR);
	if(side_apps_dir == NULL)
		return -1;
	else
		return 0;
}

int side_apps_load_dir(char *path)
{
	APP_DIR = path;
	side_apps_dir = opendir(path);
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
	bool comment = false;
	bool g_name = false;
	bool icon = false;
	ret.show_in_side = true;
	ret.hidden = false;
	memset(ret.mime_types, 0, 1000);

	struct dirent *d;

	if(!side_apps_dir)
	{
		fprintf(stderr, "SiDEApps: Invalid read on not initialized librarry!");
		return ret;
	}

	d = readdir(side_apps_dir);

	if(d == NULL)
		return ret;

	while(strlen(d->d_name) < 8 || strstr(d->d_name, ".desktop") == NULL) //.desktop == 8 so if name is shoter, not a .desktop file
    {
		d = readdir(side_apps_dir);
        if(d == NULL)
            return ret; //return if dirent is NULL while scanning for valid file...
    }

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
		if(strncmp(buffer, "Icon=", 5) == 0 && icon == false)
		{//Icon name
		  icon = true;
			strtok(buffer, "=");
			char *ptr = strtok(NULL, "\n");
			strncpy(ret.icon, ptr != NULL ? ptr : "", 200);
			ret.icon_length = strlen(ret.icon);
		}
		if(strncmp(buffer, "Comment=", 8) == 0 && comment == false)
		{//Icon name
			comment = true;
			strtok(buffer, "=");
			char *ptr = strtok(NULL, "\n");
			strncpy(ret.desc, ptr != NULL ? ptr : "", 2000);
			ret.desc_length = strlen(ret.desc);
		}
		if(strncmp(buffer, "Exec=", 5) == 0 && path == false)
		{//EXEC PATH
			path = true;
			strtok(buffer, "=");
			char *ptr = strtok(NULL, "%\n");
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
		if(strncmp(buffer , "Hidden=", 7) == 0)
		{//visible
			if(strncmp(buffer, "Hidden=true", 11) == 0)
			ret.hidden = false;
		}
		if(strncmp(buffer , "OnlyShowIn=", 11) == 0)
		{//visible
			if(strstr(buffer, "SiDE") == NULL)
				ret.show_in_side = false;
			else
				ret.show_in_side = true;
		}
		if(strncmp(buffer , "MimeType=", 9) == 0)
		{//MiME
			strtok(buffer, "=");
			char *temp = strtok(NULL, "\n");
			if(temp != NULL)
				strncpy(ret.mime_types, temp , 1000);
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
	fclose(side_apps_entry_file);
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
