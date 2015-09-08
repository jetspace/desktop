//Public Domain (CC0) tool to test the side-config lib

#include <side/config.h>

int main(void)
{
  side_dump_config_file("test.conf");
  side_lookup_value("test.conf", "a");
  side_set_value("test.conf", "ABC", "123", true);
  return 0;
}
