#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include "../../shared/strdup.h"
#include "../../shared/run.h"

int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);
  run_dialog("APPLET");
  gtk_main();
  return 0;
}
