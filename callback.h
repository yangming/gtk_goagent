#ifndef _CALLBACK_H
#define _CALLBACK_H

#include "config.h"
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pty.h>

typedef struct
{
	GtkWidget *text;
	int off;
	pid_t pid;
	int pty;
	char *python_path;
	char *goagent_path;
}UP_DATA;

void connect_goagent(GtkWidget *widget,DATA *data);

void disconnect_goagent(GtkWidget *widget,DATA *data);

void clean_buffer(GtkWidget *widget,DATA *data);

void help(GtkWidget *widget,gpointer data);

void about(GtkWidget *widget,gpointer data);

void update_goagent(GtkWidget *widget,gpointer data);

void update_gtk_goagent(GtkWidget *widget,gpointer data);

void upload(GtkWidget *widget,gpointer data);

void preferences(GtkWidget *widget,gpointer data);

void tray_on_menu(GtkWidget *widget,guint button,
		guint32 activate_time,gpointer data);

void hide_win(GtkWidget *widget,gpointer data);

void hide_window(GtkWidget *widget,GdkEventWindowState *event,gpointer data);

void tray_on_click(GtkWidget *widget,gpointer data);

void clean_data(int signum);

void select_python_path(GtkWidget *widget,gpointer data);

void select_goagent_path(GtkWidget *widget,gpointer data);

void select_gtk_goagent_path(GtkWidget *widget,gpointer data);

void select_language(GtkWidget *widget,gpointer data);

void select_font(GtkWidget *widget,gpointer data);

void select_auto_upgrade_goagent(GtkWidget *widget,gpointer data);

void select_auto_upgrade_gtk_goagent(GtkWidget *widget,gpointer data);

void exit_pre(GtkWidget *widget,gpointer data);

void save_conf_with_exit(GtkWidget *widget,gpointer data);

void upload_goagent(GtkWidget *widget,UP_DATA *data);

#endif
