#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <glib/gi18n.h>
#include <jetspace/processkit.h>

#include "../../shared/info.h"

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
  GtkWidget *cpu_load;
  long long cpu_1;
  long long cpu_2;
  long long cpu_1_prev;
  long long cpu_2_prev;
  GtkWidget *mem_percent;
  GtkWidget *swap_percent;
  GtkWidget *cpu_percent;
}SiDETaskmanagerProto;

enum
{
  SIDE_TASKMANAGER_CONTEXT_PROCESSES = 0
};

enum
{
  COL_PID = 0,
  COL_NAME,
  COL_STATUS,
  COL_PRIORITY
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
      if(p == NULL)
        continue;
      char *temp;
      if(p->state == 'S')
        temp = _("Sleeping");
      else if(p->state == 'R')
        temp = _("Running");
      else if(p->state == 'Z')
        temp = _("Zombie");
      else
        temp = _("Unknown");

      gtk_list_store_set(GTK_LIST_STORE(model), iter, COL_PID, pid,COL_NAME, p->name, COL_STATUS,temp,COL_PRIORITY,p->priority, -1);
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
      if(p == NULL)
        continue;
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
      gtk_list_store_set(GTK_LIST_STORE(list), &iter, COL_PID, data->pids[x],COL_NAME, p->name, COL_STATUS,temp,COL_PRIORITY,p->priority, -1);
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
  FILE *meminfo = fopen("/proc/meminfo", "r");
  if(meminfo == NULL)
  {
    g_warning("Could not open /proc/meminfo, this should normaly work on Linux systems");
    return TRUE;
  }

  char buffer[1000];
  unsigned long long total_mem = 0;
  unsigned long long free_mem = 0;
  unsigned long long used_mem = 0;
  unsigned long long free_swap = 0;
  unsigned long long total_swap = 0;
  unsigned long long buff;
  while(fgets(buffer, 1000, meminfo) != NULL)
  {
    if(sscanf(buffer, "MemTotal:\t%llu kB", &total_mem) == 1)
      used_mem += total_mem;
    else if(sscanf(buffer, "MemFree:\t%llu kB", &free_mem) == 1)
      used_mem -= free_mem;
    else if(sscanf(buffer, "Cached:\t%llu kB", &buff) == 1)
      used_mem -= buff;
    else if(sscanf(buffer, "Buffers:\t%llu kB", &buff) == 1)
      used_mem -= buff;

    sscanf(buffer, "SwapFree:\t%llu kB", &free_swap);
    sscanf(buffer, "SwapTotal:\t%llu kB", &total_swap);
  }

  double memusage = (double) used_mem / total_mem;
  char *bufferstr;
  gtk_level_bar_set_value(GTK_LEVEL_BAR(taskmanager->mem_use), memusage);

  bufferstr = g_strdup_printf("%.2f %%", memusage * 100);
  gtk_label_set_text(GTK_LABEL(taskmanager->mem_percent), bufferstr);
  g_free(bufferstr);

  double swapusage = 0;
  if(total_swap > 0) // SWAP could be disabled == 0 ; division by zero
  {
    swapusage = (double) (total_swap - free_swap) / total_swap;
    bufferstr = g_strdup_printf("%.2f %%", swapusage * 100);
    gtk_label_set_text(GTK_LABEL(taskmanager->swap_percent), bufferstr);
    g_free(bufferstr);
  }
  else
    gtk_label_set_text(GTK_LABEL(taskmanager->swap_percent), "0 %");

  gtk_level_bar_set_value(GTK_LEVEL_BAR(taskmanager->swap_use), swapusage);

  fclose(meminfo);

  FILE *stat = fopen("/proc/stat", "r");
  if(stat == NULL)
  {
    g_warning("Failed to access /proc/stat. This is not normal on Linux systems");
    return TRUE;
  }

  long long user, nice, sys, idle, iowait, irq, sirq,steal;
  fscanf(stat, "%s  %lld %lld %lld %lld %lld %lld %lld %lld", buffer, &user, &nice, &sys, &idle, &iowait, &irq, &sirq, &steal);
  taskmanager->cpu_1 = user + nice + sys + irq + sirq + steal;
  taskmanager->cpu_2 = idle + iowait;

  if(taskmanager->cpu_1_prev > 0 && taskmanager->cpu_2_prev > 0)
  {
    long long idle_total = taskmanager->cpu_2 - taskmanager->cpu_2_prev;
    long long total = (taskmanager->cpu_1 + taskmanager->cpu_2) - (taskmanager->cpu_1_prev + taskmanager->cpu_2_prev);

    double load = (double) (total - idle_total) / total;
    gtk_level_bar_set_value(GTK_LEVEL_BAR(taskmanager->cpu_load), load);
    bufferstr = g_strdup_printf("%.2f %%", load * 100);
    gtk_label_set_text(GTK_LABEL(taskmanager->cpu_percent), bufferstr);
    g_free(bufferstr);
  }

  taskmanager->cpu_1_prev = taskmanager->cpu_1;
  taskmanager->cpu_2_prev = taskmanager->cpu_2;


  return TRUE;

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
  int temp;
  g_object_get(col, "x-offset", &temp, NULL);
  g_warning("%d CLICKED", temp);
  const gchar *title = gtk_tree_view_column_get_title(col);
  if(strcmp(title, _("PID")) == 0)
    taskmanager->sort_col = COL_PID;
  else if(strcmp(title, _("Process Name")) == 0)
    taskmanager->sort_col = COL_NAME;
  else if(strcmp(title, _("Status")) == 0)
    taskmanager->sort_col = COL_STATUS;
  else if(strcmp(title, _("Priority")) == 0)
    taskmanager->sort_col = COL_PRIORITY;

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

void show_system_infos(void)
{
  system("side-settings-explorer --open settings.sysinfo &");
}

void show_help(void)
{
  gtk_show_uri(gdk_screen_get_default (), "http://side.rtfd.org", GDK_CURRENT_TIME, NULL);
}

void show_about(void)
{
    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), _("SiDE Taskmanager"));
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "http://jetspace.github.io");
    gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(dialog), ARTISTS);
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), COPYRIGHT);
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), TASKMANGER_DESCRIPTION);
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_MIT_X11);
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), AUTHORS);
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("/usr/share/icons/jetspace/JetSpace.png", NULL), 100, 100, GDK_INTERP_BILINEAR));
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}


int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  SiDETaskmanagerProto *taskmanager = malloc(sizeof(*taskmanager));
  taskmanager->sort_col = COL_NAME;

  taskmanager->cpu_1_prev = -1;
  taskmanager->cpu_2_prev = -1;

  taskmanager->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(taskmanager->window),_("SiDE Taskmanager"));

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_set_border_width(GTK_CONTAINER(box), 8);
  gtk_container_add(GTK_CONTAINER(taskmanager->window), box);


  GtkWidget *menubar = gtk_menu_bar_new();
  //FILE
  GtkWidget *filemenu   = gtk_menu_new();
  GtkWidget *file       = gtk_menu_item_new_with_label(_("File"));
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
     GtkWidget *info   = gtk_menu_item_new_with_label(_("System Information"));
     gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), info);
     g_signal_connect_swapped(info, "activate", G_CALLBACK(show_system_infos), NULL);
     GtkWidget *quit   = gtk_menu_item_new_with_label(_("Quit"));
     gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
     g_signal_connect_swapped(quit, "activate", G_CALLBACK(gtk_main_quit), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
  //ABOUT
  GtkWidget *aboutmenu   = gtk_menu_new();
  GtkWidget *about       = gtk_menu_item_new_with_label(_("About"));
      gtk_menu_item_set_submenu(GTK_MENU_ITEM(about), aboutmenu);
      GtkWidget *ab     = gtk_menu_item_new_with_label(_("About"));
      gtk_menu_shell_append(GTK_MENU_SHELL(aboutmenu), ab);
      g_signal_connect_swapped(ab, "activate", G_CALLBACK(show_about), NULL);
      GtkWidget *help   = gtk_menu_item_new_with_label(_("Help"));
      gtk_menu_shell_append(GTK_MENU_SHELL(aboutmenu), help);
      g_signal_connect_swapped(help, "activate", G_CALLBACK(show_help), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), about);

  gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 0);

  GtkWidget *use_grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(use_grid), 5);
  gtk_box_pack_start(GTK_BOX(box), use_grid, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);

  GtkWidget *label = gtk_label_new(_("Memory:"));
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_grid_attach(GTK_GRID(use_grid), label, 0, 0, 1, 1);
  taskmanager->mem_use = gtk_level_bar_new_for_interval(0,1);
  gtk_level_bar_add_offset_value (GTK_LEVEL_BAR(taskmanager->mem_use), GTK_LEVEL_BAR_OFFSET_HIGH, 0.20);
  gtk_level_bar_add_offset_value (GTK_LEVEL_BAR(taskmanager->mem_use), GTK_LEVEL_BAR_OFFSET_LOW, 0.80);
  gtk_grid_attach(GTK_GRID(use_grid), taskmanager->mem_use, 1, 0, 1, 1);
  gtk_widget_set_hexpand(taskmanager->mem_use, TRUE);
  taskmanager->mem_percent = gtk_label_new("");
  gtk_widget_set_halign(taskmanager->mem_percent, GTK_ALIGN_END);
  gtk_grid_attach(GTK_GRID(use_grid), taskmanager->mem_percent, 2,0,1,1);


  label = gtk_label_new(_("Swap:"));
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_grid_attach(GTK_GRID(use_grid), label, 0, 1, 1, 1);
  taskmanager->swap_use = gtk_level_bar_new_for_interval(0,1);
  gtk_level_bar_add_offset_value (GTK_LEVEL_BAR(taskmanager->swap_use), GTK_LEVEL_BAR_OFFSET_HIGH, 0.20);
  gtk_level_bar_add_offset_value (GTK_LEVEL_BAR(taskmanager->swap_use), GTK_LEVEL_BAR_OFFSET_LOW, 0.80);
  gtk_grid_attach(GTK_GRID(use_grid), taskmanager->swap_use, 1, 1, 1, 1);
  gtk_widget_set_hexpand(taskmanager->swap_use, TRUE);
  taskmanager->swap_percent = gtk_label_new("");
  gtk_widget_set_halign(taskmanager->swap_percent, GTK_ALIGN_END);
  gtk_grid_attach(GTK_GRID(use_grid), taskmanager->swap_percent, 2,1,1,1);


  label = gtk_label_new(_("CPU:"));
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_grid_attach(GTK_GRID(use_grid), label, 0, 2, 1, 1);
  taskmanager->cpu_load = gtk_level_bar_new_for_interval(0,1);
  gtk_level_bar_add_offset_value (GTK_LEVEL_BAR(taskmanager->cpu_load), GTK_LEVEL_BAR_OFFSET_HIGH, 0.20);
  gtk_level_bar_add_offset_value (GTK_LEVEL_BAR(taskmanager->cpu_load), GTK_LEVEL_BAR_OFFSET_LOW, 0.80);
  gtk_grid_attach(GTK_GRID(use_grid), taskmanager->cpu_load, 1, 2, 1, 1);
  gtk_widget_set_hexpand(taskmanager->cpu_load, TRUE);
  taskmanager->cpu_percent = gtk_label_new("");
  gtk_widget_set_halign(taskmanager->cpu_percent, GTK_ALIGN_END);
  gtk_grid_attach(GTK_GRID(use_grid), taskmanager->cpu_percent, 2,2,1,1);



  GtkTreeIter iter;
  taskmanager->store = gtk_list_store_new(4, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);


  GtkWidget *tree = gtk_tree_view_new();
  gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(tree), GTK_TREE_VIEW_GRID_LINES_BOTH);
  gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(tree), TRUE);
  GtkWidget *scroll_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_win), 200);
  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scroll_win), 350);
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

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("Priority"), renderer, "text", 3, NULL);
  g_signal_connect(column, "clicked", G_CALLBACK(change_sort), taskmanager);
  gtk_tree_view_column_set_clickable(column, TRUE);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);


  taskmanager->sort = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(taskmanager->store));
  gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(taskmanager->sort), taskmanager->sort_col, task_sort_func, taskmanager, NULL);



  gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(taskmanager->sort));
  gtk_box_pack_start(GTK_BOX(box), scroll_win, TRUE, TRUE, 0);


  taskmanager->statusbar = gtk_statusbar_new();
  gtk_box_pack_end(GTK_BOX(box), taskmanager->statusbar, FALSE, FALSE, 0);

  g_timeout_add(750, update_processes, taskmanager);
  g_signal_connect_swapped(taskmanager->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(taskmanager->window);

  gtk_main();
}
