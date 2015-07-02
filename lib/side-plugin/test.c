#include <side/plugin.h>

#include <stdio.h>

int main(int argc, char **argv)
{
  if(check_version(COMPATIBLE_SINCE, 0, 11))
    printf("COMPATIBLE_SINCE has worked\n");

  if(check_version(COMPATIBLE_UNTIL, 0, 65))
    printf("COMPATIBLE_UNTIL has worked\n");

  if(check_version(ONLY_FOR_VERSION, 0, 62))
    printf("ONLY_FOR_VERSION has worked\n");

}
