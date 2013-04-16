#ifndef _DIALOG_H
#define _DIALOG_H

#include "menu.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

typedef struct
{
	gchar *name;
	gchar *email;
	gchar *readme;
}HELP;

typedef struct
{
	GtkWidget *text;
	pthread_t thread;
	pthread_mutex_t mutex;
	int off;
	pid_t pid;
}DATA;

void init_help_data(HELP *help);

void about_dialog(GtkWidget *about,gpointer data);

void really_quit(GtkWidget *widget,gpointer data);

void error_message(GtkWidget *widget,gchar *data);

void message_box(GtkWidget *widget,gchar *data);

void help_dialog(GtkWidget *help,gpointer data);

#endif
