#ifndef WINDOW_DRAWER_H_
#define WINDOW_DRAWER_H_

#include <gtk/gtk.h>

const char   DEFAULT_WINDOW_NAME [] = "Cracker";
const size_t DEFAULT_WINDOW_WIDTH   = 800;
const size_t DEFAULT_WINDOW_HEIGHT  = 600;

void ActivateApplication (GtkApplication *crackerApp, gpointer userData);

void DestroyAllWidgets ();

#endif
