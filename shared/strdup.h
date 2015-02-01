/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

/*
Sample Implementation of strdup because it is not in the C99 standard, and out project should follow
the standards...
*/

//Thanks to https://gist.github.com/reagent/3758387 for help! ;-)
#ifndef _STRDUP_
#define _STRDUP_
#include <string.h>
char *strdup(const char *src)
{
  int len = strlen(src) + 1;
  void *new = malloc(len);

  if (new == NULL)
    return NULL;

  return (char *) memcpy(new, src, len);
}
#endif
