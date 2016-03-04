#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <jetspace/processkit.h>

int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win),_("SiDE Taskmanager"));

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(win), box);

  GtkTreeIter iter;
  GtkListStore *list = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT);

  int n = 0;
  int *pids = jetspace_get_pids(&n);
  for(int x = 0; x < n; x++)
  {
    gtk_list_store_append(list, &iter);
    JetSpaceProcess *p = jetspace_get_process(pids[x]);
    gtk_list_store_set(list, &iter, 0, pids[x],1, p->name, 2,p->state, -1);
    free(p);
  }

  GtkWidget *tree = gtk_tree_view_new();
  GtkWidget *scroll_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_win), 200);
  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scroll_win), 300);
  gtk_container_add(GTK_CONTAINER(scroll_win), tree);
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("PID"), renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("Process Name"), renderer, "text", 1, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("Status"), renderer, "text", 2, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(list));
  gtk_box_pack_start(GTK_BOX(box), scroll_win, TRUE, TRUE, 0);


  gtk_widget_show_all(win);

  gtk_main();
}
