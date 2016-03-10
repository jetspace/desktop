#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <glib/gi18n.h>
#include <sys/sysinfo.h>
#include <jetspace/processkit.h>


typedef struct
{
  int *pids;
  int size;
}PIDContainer;

typedef struct
{
  GtkWidget *window;
  GtkListStore *store;
  GtkWidget *statusbar;
  GtkTreeModel *sort;
  int sort_col;
  GtkWidget *mem_use;
  GtkWidget *swap_use;
}SiDETaskmanagerProto;

enum
{
  SIDE_TASKMANAGER_CONTEXT_PROCESSES = 0
};

enum
{
  COL_PID = 0,
  COL_NAME,
  COL_STATUS
};

gboolean update_rows(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
  int pid;
  gtk_tree_model_get(model, iter, COL_PID, &pid, -1);

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
      else if(p->state == 'Z')
        temp = _("Zombie");
      else
        temp = _("Unknown");

      gtk_list_store_set(GTK_LIST_STORE(model), iter, COL_PID, pid,COL_NAME, p->name, COL_STATUS,temp, -1);
      free(p);
      return FALSE;
    }
  }
  gtk_list_store_remove(GTK_LIST_STORE(model), iter);
  return FALSE;
}


gboolean update_processes(gpointer l)
{
  SiDETaskmanagerProto *taskmanager = (SiDETaskmanagerProto *) l;
  GtkListStore *list = GTK_LIST_STORE(taskmanager->store);

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
      else
        temp = _("Unknown");


      gtk_list_store_append(GTK_LIST_STORE(list), &iter);
      gtk_list_store_set(GTK_LIST_STORE(list), &iter, COL_PID, data->pids[x],COL_NAME, p->name, COL_STATUS,temp, -1);
      free(p);
    }
  }

  GtkStatusbar *sb = GTK_STATUSBAR(taskmanager->statusbar);
  gchar *temp = g_strdup_printf("%d processes", data->size);
  gtk_statusbar_remove_all(sb, SIDE_TASKMANAGER_CONTEXT_PROCESSES);
  gtk_statusbar_push(sb, SIDE_TASKMANAGER_CONTEXT_PROCESSES , temp);
  g_free(temp);

  free(data->pids);
  free(data);

  //UPDATE RESOURCES
  struct sysinfo info;
  sysinfo(&info);

  double memusage = (double) (info.totalram - info.freeram) / (double) info.totalram;
  gtk_level_bar_set_value(GTK_LEVEL_BAR(taskmanager->mem_use), memusage);

  double swapusage = 0;
  if(info.totalswap > 0)
    swapusage = (double) (info.totalswap - info.freeswap) / (double) info.totalswap;

  gtk_level_bar_set_value(GTK_LEVEL_BAR(taskmanager->swap_use), swapusage);




}

gint task_sort_func(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer data)
{
  SiDETaskmanagerProto *taskmanager = (SiDETaskmanagerProto *) data;
  if(taskmanager->sort_col == COL_NAME || taskmanager->sort_col == COL_STATUS)
  {
    char *nA, *nB;
    int pA, pB;

    int arg = taskmanager->sort_col;

    gtk_tree_model_get(model, a, COL_PID, &pA, arg, &nA, -1);
    gtk_tree_model_get(model, b, COL_PID, &pB, arg, &nB, -1);

    int compare = strcasecmp(nA, nB);
    if(compare == 0)
    {
      return pA - pB;
    }
    else
      return compare;
  }
  else
    {
      int pA, pB;

      gtk_tree_model_get(model, a, COL_PID, &pA, -1);
      gtk_tree_model_get(model, b, COL_PID, &pB, -1);

      return pA - pB;

    }

    return 0;

}

void change_sort(GtkTreeViewColumn *col, gpointer data)
{
  SiDETaskmanagerProto *taskmanager = (SiDETaskmanagerProto *) data;
  const gchar *title = gtk_tree_view_column_get_title(col);
  if(strcmp(title, _("PID")) == 0)
    taskmanager->sort_col = COL_PID;
  else if(strcmp(title, _("Process Name")) == 0)
    taskmanager->sort_col = COL_NAME;
  else if(strcmp(title, _("Status")) == 0)
    taskmanager->sort_col = COL_STATUS;

  int direction;
  g_object_get(col, "sort-order", &direction, NULL);
  if(direction != GTK_SORT_ASCENDING)
    direction = GTK_SORT_ASCENDING;
  else
    direction = GTK_SORT_DESCENDING;
  g_object_set(col, "sort-order", direction, NULL);

  gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(taskmanager->sort), taskmanager->sort_col, direction);
  gtk_tree_sortable_sort_column_changed(GTK_TREE_SORTABLE(taskmanager->sort));

}


int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  SiDETaskmanagerProto *taskmanager = malloc(sizeof(*taskmanager));
  taskmanager->sort_col = COL_NAME;

  taskmanager->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(taskmanager->window),_("SiDE Taskmanager"));

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(taskmanager->window), box);


  GtkWidget *use_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(use_box), gtk_label_new(_("Memory:")), FALSE, FALSE, 0);
  taskmanager->mem_use = gtk_level_bar_new_for_interval(0,1);
  gtk_box_pack_end(GTK_BOX(use_box), taskmanager->mem_use, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), use_box, FALSE, FALSE, 0);

  use_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(use_box), gtk_label_new(_("Swap:")), FALSE, FALSE, 0);
  taskmanager->swap_use = gtk_level_bar_new_for_interval(0,1);
  gtk_box_pack_end(GTK_BOX(use_box), taskmanager->swap_use, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box), use_box, FALSE, FALSE, 0);



  GtkTreeIter iter;
  taskmanager->store = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);


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
  g_signal_connect(column, "clicked", G_CALLBACK(change_sort), taskmanager);
  gtk_tree_view_column_set_clickable(column, TRUE);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("Process Name"), renderer, "text", 1, NULL);
  g_signal_connect(column, "clicked", G_CALLBACK(change_sort), taskmanager);
  gtk_tree_view_column_set_clickable(column, TRUE);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("Status"), renderer, "text", 2, NULL);
  g_signal_connect(column, "clicked", G_CALLBACK(change_sort), taskmanager);
  gtk_tree_view_column_set_clickable(column, TRUE);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);


  taskmanager->sort = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(taskmanager->store));
  gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(taskmanager->sort), taskmanager->sort_col, task_sort_func, taskmanager, NULL);



  gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(taskmanager->sort));
  gtk_box_pack_start(GTK_BOX(box), scroll_win, TRUE, TRUE, 0);


  taskmanager->statusbar = gtk_statusbar_new();
  gtk_box_pack_end(GTK_BOX(box), taskmanager->statusbar, FALSE, FALSE, 0);

  g_timeout_add(1000, update_processes, taskmanager);
  g_signal_connect_swapped(taskmanager->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(taskmanager->window);

  gtk_main();
}
