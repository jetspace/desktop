/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include "appchooser.h"
#include "../side-app/apps.h"
#define SIDE_APPCHOOSER_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SIDE_APPCHOOSER_TYPE, SiDEAppChooserPrivate))

typedef struct _SiDEAppChooserPrivate SiDEAppChooserPrivate;

struct _SiDEAppChooserPrivate
{
	GtkListStore *applications;
	GtkWidget *iconview;
};

enum
{
	COL_NAME,
	COL_EXEC,
	COL_ICON,
	N_COLS
};

G_DEFINE_TYPE(SiDEAppChooser, side_app_chooser, GTK_TYPE_DIALOG);


static void side_app_chooser_class_init(SiDEAppChooserClass *klass)
{
    g_type_class_add_private(klass, sizeof(SiDEAppChooserPrivate));
}

static void side_app_chooser_init(SiDEAppChooser *chooser)
{
  SiDEAppChooserPrivate *p = SIDE_APPCHOOSER_PRIVATE(chooser);
	p->applications = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF);
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
	 gtk_list_store_set(p->applications, &iter, COL_NAME, ent.app_name, COL_EXEC, ent.exec, COL_ICON,gtk_icon_theme_load_icon(theme, ent.icon, 32,GTK_ICON_LOOKUP_FORCE_SIZE ,NULL), -1);

	}while(ent.valid == TRUE);

}

static void enable_ok(SiDEAppChooser *p)
{
	gtk_dialog_set_response_sensitive(GTK_DIALOG(p), 0, TRUE);
}

GtkWidget *side_app_chooser_new(void)
{
  SiDEAppChooser *ret = g_object_new(side_app_chooser_get_type(), NULL);

	gtk_dialog_add_button(GTK_DIALOG(ret), "Cancle", 1);
	gtk_dialog_add_button(GTK_DIALOG(ret), "Choose", 0);

	gtk_dialog_set_response_sensitive(GTK_DIALOG(ret), 0, FALSE);
	gtk_dialog_set_default_response(GTK_DIALOG(ret), 0);

	SiDEAppChooserPrivate *p = SIDE_APPCHOOSER_PRIVATE(ret);

	p->iconview = gtk_icon_view_new_with_model(GTK_TREE_MODEL(p->applications));
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

  return GTK_WIDGET(ret);
}
