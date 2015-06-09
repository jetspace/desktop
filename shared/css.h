/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

void use_css(GSettings *theme)
{
    GdkDisplay *display;
    GdkScreen *screen;
    display = gdk_display_get_default ();
    screen = gdk_display_get_default_screen (display);

    if(g_variant_get_boolean(g_settings_get_value(theme, "use-custom-theme")))
    {
        GtkCssProvider *provider;
        provider = gtk_css_provider_new ();
        gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        gsize bytes, read;
        const gchar* t = g_strdup_printf( "%s%s",g_variant_get_string(g_settings_get_value(theme, "custom-theme-path"), NULL), "/side-panel/gtk.css");
        gtk_css_provider_load_from_path (provider,g_filename_to_utf8(t, strlen(t), &read, &bytes, NULL),NULL);
        g_object_unref (provider);
    }
}
