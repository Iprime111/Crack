#include <gtk/gtk.h>
#include "WindowDrawer.h"

int main (int argc, char **argv) {

    GtkApplication *crackerApp = gtk_application_new ("org.ip111.cracker", G_APPLICATION_DEFAULT_FLAGS);
    
    if (!crackerApp) {
        fprintf(stderr, "Error occuried while creating app instance\n");
        return -1;
    }

    g_signal_connect (crackerApp, "activate", G_CALLBACK (ActivateApplication), NULL);
    g_application_run (G_APPLICATION (crackerApp), argc, argv);
    g_object_unref (crackerApp);
    DestroyAllWidgets ();

    return 0;
}
