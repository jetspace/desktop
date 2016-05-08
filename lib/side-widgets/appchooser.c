/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include "appchooser.h"
#include "../side-app/apps.h"
#define SIDE_APP_CHOOSER_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SIDE_APP_CHOOSER_TYPE, SiDEAppChooserPrivate))

typedef struct _SiDEAppChooserPrivate SiDEAppChooserPrivate;

struct _SiDEAppChooserPrivate
{
	GtkListStore *applications;
	GtkTreeModel *sort;
	GtkWidget *iconview;
	SiDEAppChooserResult res;
};

enum
{
	COL_NAME,
	COL_EXEC,
	COL_ICON,
	COL_ICON_NAME,
	N_COLS
};

G_DEFINE_TYPE(SiDEAppChooser, side_app_chooser, GTK_TYPE_DIALOG);


static void side_app_chooser_class_init(SiDEAppChooserClass *klass)
{
    g_type_class_add_private(klass, sizeof(SiDEAppChooserPrivate));
}

static void side_app_chooser_init(SiDEAppChooser *chooser)
{
  SiDEAppChooserPrivate *p = SIDE_APP_CHOOSER_PRIVATE(chooser);
	p->applications = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	side_apps_load();
	AppEntry ent;
	GtkIconTheme *theme = gtk_icon_theme_get_default();
	GtkTreeIter iter;
	do
	{
	 ent = side_apps_get_next_entry();

	 if(ent.show == FALSE || ent.valid == FALSE)
		 continue;
	 gtk_list_store_append(p->applications, &iter);
	 gtk_list_store_set(p->applications, &iter, COL_NAME, ent.app_name, COL_EXEC, ent.exec, COL_ICON,gtk_icon_theme_load_icon(theme, ent.icon, 32,GTK_ICON_LOOKUP_FORCE_SIZE ,NULL), COL_ICON_NAME, ent.icon, -1);

	}while(ent.valid == TRUE);

}

static void enable_ok(SiDEAppChooser *p)
{
	gtk_dialog_set_response_sensitive(GTK_DIALOG(p), 0, TRUE);
}

static void selected(GtkDialog *d, gint id, SiDEAppChooser *p)
{
	if(id != 0)
		return;

	SiDEAppChooserPrivate *pr = SIDE_APP_CHOOSER_PRIVATE(p);
	GList *list = gtk_icon_view_get_selected_items(GTK_ICON_VIEW(pr->iconview));
  GList *it;



  for(it = list; it != NULL; it = g_list_next(it))
    {
      if(gtk_icon_view_path_is_selected(GTK_ICON_VIEW(pr->iconview), it->data))
        {
          char *n, *e, *in;
          GtkTreeIter i;

          gtk_tree_model_get_iter(pr->sort, &i, it->data);
          gtk_tree_model_get(pr->sort, &i, COL_NAME, &n,COL_EXEC, &e, COL_ICON_NAME, &in, -1);

					pr->res.exec = e;
					pr->res.name = n;
					pr->res.icon = in;
        }
    }
}

GtkWidget *side_app_chooser_new(void)
{
  SiDEAppChooser *ret = g_object_new(side_app_chooser_get_type(), NULL);

	gtk_dialog_add_button(GTK_DIALOG(ret), "Cancle", 1);
	gtk_dialog_add_button(GTK_DIALOG(ret), "Choose", 0);

	gtk_dialog_set_response_sensitive(GTK_DIALOG(ret), 0, FALSE);
	gtk_dialog_set_default_response(GTK_DIALOG(ret), 0);

	SiDEAppChooserPrivate *p = SIDE_APP_CHOOSER_PRIVATE(ret);

	GtkTreeModel *sort = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(p->applications));
	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(sort), COL_NAME, GTK_SORT_ASCENDING);

	p->sort = sort;

	p->iconview = gtk_icon_view_new_with_model(GTK_TREE_MODEL(sort));
	gtk_icon_view_set_text_column(GTK_ICON_VIEW(p->iconview), COL_NAME);
    gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(p->iconview), COL_ICON);
	gtk_icon_view_set_selection_mode(GTK_ICON_VIEW(p->iconview), GTK_SELECTION_BROWSE);
	g_signal_connect_swapped(G_OBJECT(p->iconview), "selection-changed", G_CALLBACK(enable_ok), ret);


	GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 200);
	gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scroll), 300);
	gtk_container_add(GTK_CONTAINER(scroll), p->iconview);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(ret))), scroll, TRUE, TRUE, 0);

	gtk_widget_show_all(gtk_dialog_get_content_area(GTK_DIALOG(ret)));

	g_signal_connect(ret, "response" , G_CALLBACK(selected) , ret);

    return GTK_WIDGET(ret);
}


char *side_app_chooser_get_name(SiDEAppChooser *ac)
{
	SiDEAppChooserPrivate *p = SIDE_APP_CHOOSER_PRIVATE(ac);
	return p->res.name;
}

char *side_app_chooser_get_icon(SiDEAppChooser *ac)
{
	SiDEAppChooserPrivate *p = SIDE_APP_CHOOSER_PRIVATE(ac);
	return p->res.icon;
}

char *side_app_chooser_get_exec(SiDEAppChooser *ac)
{
	SiDEAppChooserPrivate *p = SIDE_APP_CHOOSER_PRIVATE(ac);
	return p->res.exec;
}
