#include <stdio.h>
#include <signal.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <webkit/webkit.h>

gboolean on_key_press(GtkWidget*, GdkEventKey*, gpointer);

void reload_browser(int);
void toggle_fullscreen(int);
void maximize();
void unmaximize();

static WebKitWebView* web_view;
static GtkWidget *window;
static GtkWidget *scrolled_window;
gchar* default_url = "https://github.com/pschultz/kiosk-browser/blob/master/README.md";

#ifndef GDK_KEY_F5
#define GDK_KEY_F5 GDK_F5
#endif

#ifndef GDK_KEY_F11
#define GDK_KEY_F11 GDK_F11
#endif

int main(int argc, char** argv) {
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());

  // Create a scrollable area, and put the browser instance into it
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
          GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(web_view));

  signal(SIGHUP, reload_browser);
  signal(SIGUSR1, toggle_fullscreen);

  gtk_container_add(GTK_CONTAINER(window), scrolled_window);

  if(argc > 1) {
    webkit_web_view_load_uri(web_view, argv[1]);
  }
  else {
    webkit_web_view_load_uri(web_view, default_url);
  }

  // Make sure that when the browser area becomes visible, it will get
  // mouse and keyboard events
  gtk_widget_grab_focus(GTK_WIDGET(web_view));


  maximize();
  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}

gboolean on_key_press(GtkWidget* window, GdkEventKey* key, gpointer userdata) {
  if(key->type == GDK_KEY_PRESS && key->keyval == GDK_KEY_F5) {
    reload_browser(0);
  }
  else if(key->type == GDK_KEY_PRESS && key->keyval == GDK_KEY_F11) {
    toggle_fullscreen(0);
  }

  return FALSE;
}

void reload_browser(int signum) {
  webkit_web_view_reload_bypass_cache(web_view);
}

void toggle_fullscreen(int signum) {
  if(gtk_window_get_decorated(GTK_WINDOW(window))) {
    maximize();
  }
  else {
    unmaximize();
  }
}

void maximize() {
  gtk_window_maximize(GTK_WINDOW(window));
  gtk_window_fullscreen(GTK_WINDOW(window));
  gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
}

void unmaximize() {
  gtk_window_unmaximize(GTK_WINDOW(window));
  gtk_window_unfullscreen(GTK_WINDOW(window));
  gtk_window_set_decorated(GTK_WINDOW(window), TRUE);
  gtk_window_resize(GTK_WINDOW(window), 1280, 768);
}
