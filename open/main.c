/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <side/config.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gio/gio.h>

char *get_mime_type(char *file)
{
  char *ct = g_content_type_guess(file, NULL, 0, NULL);
  char *mime = g_content_type_get_mime_type(ct);
  g_free(ct);
  return mime;
}

#define MIMEDB "/usr/share/side/mime.conf"

int main(int argc, char **argv)
{
  for (int x = 1; x < argc; x++)
  {
    char *mime_type = get_mime_type(argv[x]);


    char *app = side_lookup_value(MIMEDB, mime_type);
    char *subtype;

    if(!app)
    {
      fprintf(stderr, "can't find full match for type %s\n", mime_type);
      subtype = strtok(mime_type, "/");
      app = side_lookup_value(MIMEDB, subtype);
    }

    if(!app)
    {
      fprintf(stderr, "Unable to open MIME type %s or subtype %s\n",mime_type, subtype);
      g_free(mime_type);
      continue;
    }

    char *op = malloc(strlen(app) + strlen(argv[x]) +1);
    strcat(op, app);
    strcat(op, " \"");
    strcat(op, argv[x]);
    strcat(op, "\" &");
    system(op);
    free(op);

    g_free(mime_type);

  }
}
