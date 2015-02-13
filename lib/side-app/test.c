/*
This file is Public Domain (CC0)
You could do whatever you want
*/

#include <stdio.h>
#include <side/apps.h>

int main(int argc, char **argv)
{
	side_apps_load();
	AppEntry *e;
	while((e = side_apps_get_next_entry()))
	{
		printf("entry: %30s \t\t %20s : %d\n", e->exec, e->app_name, e->sub);
	}
	side_apps_close();

	return 0;
}
