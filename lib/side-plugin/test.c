#include <side/plugin.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  if(!check_version(COMPATIBLE_SINCE, "0.11"))
    return 1;

  if(check_version(COMPATIBLE_UNTIL, "0.65"))
    return 1;

  if(check_version(ONLY_FOR_VERSION, "0.62"))
    return 1;

  return 0;
}
