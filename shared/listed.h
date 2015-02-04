/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef _LISTED_H_
#define _LISTED_H_

#include <glib.h>
#include <string.h>

//Tests [N] elements of [BLACKLIST] for [QUERRY]
// MATCH    :TRUE
// NO_MATCH :FALSE
gboolean is_listed(char **blacklist, int n, char *querry)
{
  while(n > 0)
    {
        if(strcmp(blacklist[n-1], querry) == 0)
          return TRUE;
        n--;
    }

  //IF WE REACH THIS POINT = NO MATCH
  return FALSE;
}



#endif
