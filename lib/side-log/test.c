//Public Domain (CC0) tool to test the side-log lib

#include <side/log.h>

int main(void)
{
    side_log_set_log_level_from_enviroment();

    printf("Current Loglevel is :%d\n", side_log_get_log_level());
    side_log_debug("DEBUG!");
    side_log_note("NOTE!");
    side_log_message("MESSAGE!");
    side_log_warning("WARNING!");
    side_log_error("ERROR!");
    side_log_critical("CRITICAL!");
    side_log_failure("FAILURE!");
    return 0;
}
