#include <assert.h>

#include "WindowDrawer.h"
#include "FilePatcher.h"

static void CreateWindow (GtkApplication *crackerApp, const char *uiFilename);
static GtkWidget *GetUiObject (GtkBuilder *builder, const char *objectId);
static gboolean UpdateProgress (gpointer userData);
static void LoadImages (GError **error);

static GtkProgressBar *ProgressBar = NULL;
static GtkImage       *Image       = NULL;
static GtkWidget      *Window      = NULL;

static const size_t     IMAGES_COUNT                = 6;
static const char      *ImagesNames  [IMAGES_COUNT] = 
                        {"../img/1.jpg", "../img/2.jpg", "../img/3.jpg", "../img/4.jpg", "../img/5.jpg", "../img/6.jpg"};
static GdkPixbuf *ImageBuffers [IMAGES_COUNT] = {NULL, NULL, NULL, NULL, NULL, NULL};

static const double PROGRESS_BAR_STEP      = 0.005;
static const gulong PROGRESS_BAR_LAG_SLEEP = 80000;
static const double EPS = 1e-5;
static guint progressBarThread = 0;


void ActivateApplication (GtkApplication *crackerApp, gpointer userData) {
    assert (crackerApp);
    CreateWindow (crackerApp, "../ui/MainWindow.ui");
}

static void SetComFile (GtkFileChooser *fileChooser, gpointer userData) {
    assert (fileChooser);

    if (ComFilename) {
        free (ComFilename);
    }

    gchar *filename = gtk_file_chooser_get_filename (fileChooser);

    //TODO: check file hash
    ComFilename = (char *) filename;
}

static gboolean UpdateProgress (gpointer userData) {
    
    gdouble fraction = gtk_progress_bar_get_fraction (ProgressBar);

    if (abs (fraction - 1) < EPS) {
        gtk_image_set_from_pixbuf (Image, ImageBuffers [5]);

        g_source_remove (progressBarThread);
        progressBarThread = 0;
        return false;
    }

    if (abs (fraction - 0.75) < 0.05) {
        g_usleep (PROGRESS_BAR_LAG_SLEEP);
    }

    gtk_progress_bar_pulse (ProgressBar);
    gtk_progress_bar_set_fraction (ProgressBar, fraction + PROGRESS_BAR_STEP);

    int percent = (int) (fraction * 100);
    gtk_image_set_from_pixbuf (Image, ImageBuffers [percent / 20]);

    return true;
}

static void BeginCrack (GtkButton *button, gpointer userData) {
    assert (ProgressBar);
    assert (Image);

    if (!ComFilename)
        return;

    if (progressBarThread != 0)
        return;

    PatchFile ();

    progressBarThread = g_timeout_add (25, UpdateProgress, NULL);
}

static GtkWidget *GetUiObject (GtkBuilder *builder, const char *objectId) {

    GtkWidget *fileChooser = GTK_WIDGET (gtk_builder_get_object (builder, objectId));
    if (!fileChooser) {
        g_critical ("Error while getting file object: %s", objectId);
        return NULL;
    }

    return fileChooser;
}

static void LoadImages (GError **error) {

    for (size_t imageNumber = 0; imageNumber < IMAGES_COUNT; imageNumber++) {
        ImageBuffers [imageNumber] = gdk_pixbuf_new_from_file (ImagesNames[imageNumber], error);

        if (!ImageBuffers [imageNumber]) {
            g_critical ("Unable to load image: %s", (*error)->message);
            return;
        }
    }
}

void CreateWindow (GtkApplication *crackerApp, const char *uiFilename) {
    assert (crackerApp);
    GError *error = NULL;

    GtkBuilder *builder = gtk_builder_new ();
    if (!gtk_builder_add_from_file (builder, uiFilename, &error)) {
        g_critical ("Cannot open file: %s", error->message);
        g_error_free (error);
    }

               Window      = GetUiObject (builder, "MainWindow");
    GtkWidget *fileChooser = GetUiObject (builder, "FileChooser");
    GtkWidget *beginButton = GetUiObject (builder, "BeginButton");

    ProgressBar = GTK_PROGRESS_BAR (GetUiObject (builder, "ProgressBar"));
    Image       = GTK_IMAGE        (GetUiObject (builder, "ImageFrame"));

    g_signal_connect (fileChooser, "file-set", G_CALLBACK (SetComFile), NULL);
    g_signal_connect (beginButton, "clicked",  G_CALLBACK (BeginCrack), NULL);

    LoadImages (&error);

    gtk_image_set_from_pixbuf (Image, ImageBuffers [0]);

    gtk_window_set_application (GTK_WINDOW (Window), crackerApp);
    gtk_widget_set_visible (Window, TRUE);

    g_object_unref (builder);

}

void DestroyAllWidgets () {
    g_object_unref (Window);

    for (size_t imageNumber = 0; imageNumber < IMAGES_COUNT; imageNumber++) {
        g_object_unref (ImageBuffers [imageNumber]);
    }
}
