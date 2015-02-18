/*
This file is Public Domain (CC0)
You could do whatever you want
*/

#include <stdio.h>
#include <side/apps.h>

int main(int argc, char **argv)
{
	side_apps_load();
	AppEntry e;
	do
	{
		e = (AppEntry) side_apps_get_next_entry();
		printf("entry: %30s \t\t %10s : %d : %s\n", e.exec, e.app_name, e.sub, e.show == true ? "YES" : "NO");
	}while(e.valid == true);
	side_apps_close();

	return 0;
}
