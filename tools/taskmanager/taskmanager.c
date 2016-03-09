#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <jetspace/processkit.h>


typedef struct
{
  int *pids;
  int size;
}PIDContainer;

gboolean update_rows(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
  int pid;
  gtk_tree_model_get(model, iter, 0, &pid, -1);

  PIDContainer *pc = (PIDContainer *) data;

  for(int x = 0; x < pc->size; x++)
  {
    if(pc->pids[x] == pid)
    {
      pc->pids[x] = 0;
      JetSpaceProcess *p = jetspace_get_process(pid);
      char *temp;
      if(p->state == 'S')
        temp = _("Sleeping");
      else if(p->state == 'R')
        temp = _("Running");
      if(p->state == 'Z')
        temp = _("Zombie");

      gtk_list_store_set(GTK_LIST_STORE(model), iter, 0, pid,1, p->name, 2,temp, -1);
      free(p);
      return FALSE;
    }
  }
  gtk_list_store_remove(GTK_LIST_STORE(model), iter);

  return FALSE;
}


gboolean update_processes(gpointer l)
{
  GtkListStore *list = GTK_LIST_STORE(l);

  PIDContainer *data = malloc(sizeof(*data));
  data->pids = jetspace_get_pids(&data->size);

  gtk_tree_model_foreach(GTK_TREE_MODEL(list), update_rows, data);

  GtkTreeIter iter;
  for(int x = 0; x < data->size; x++)
  {
    if(data->pids[x] != 0)
    {
      JetSpaceProcess *p = jetspace_get_process(data->pids[x]);
      char *temp;
      if(p->state == 'S')
        temp = _("Sleeping");
      else if(p->state == 'R')
        temp = _("Running");
      if(p->state == 'Z')
        temp = _("Zombie");

      gtk_list_store_append(GTK_LIST_STORE(list), &iter);
      gtk_list_store_set(GTK_LIST_STORE(list), &iter, 0, data->pids[x],1, p->name, 2,temp, -1);
      free(p);
    }
  }


}


int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win),_("SiDE Taskmanager"));

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(win), box);

  GtkTreeIter iter;
  GtkListStore *list = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);


  GtkWidget *tree = gtk_tree_view_new();
  gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree), GTK_TREE_VIEW_GRID_LINES_BOTH);
  gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(tree), TRUE);
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

  g_timeout_add(1000, update_processes, list);
  g_signal_connect_swapped(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(win);

  gtk_main();
}
