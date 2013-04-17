#include "callback.h"
#include <locale.h>
#include <stdlib.h>
#include <limits.h>

void get_connect(DATA *data);
gboolean is_python_and_goagent_path(char *python_path,char *goagent_path);

DATA *sig_data;
//void _get_connect(DATA *data);

void connect_goagent(GtkWidget *widget,DATA *data)
{
	static pthread_t thread;

	if(data->off)
	{
		message_box(widget,_("Connected Now\n"));
		return;
	}

	if(!is_python_and_goagent_path(data->python_path,data->goagent_path))
		return;

	pthread_create(&thread,NULL,(void *)get_connect,data);
	data->thread=thread;
	data->off=1;
}

void disconnect_goagent(GtkWidget *widget,DATA *data)
{
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));

	if(!data->off)
	{
		message_box(widget,_("No Connected Now!"));
		return;
	}

	data->off=0;
	kill(data->pid,SIGKILL);
	while(waitpid(-1,NULL,WNOHANG)!=-1);
	pthread_cancel(data->thread);

	while(gtk_events_pending())
		gtk_main_iteration();

	pthread_mutex_lock(&data->mutex);
	gtk_text_buffer_set_text(buffer,"",0);
	pthread_mutex_unlock(&data->mutex);
}

void clean_buffer(GtkWidget *widget,DATA *data)
{
	while(gtk_events_pending())
		gtk_main_iteration();
	pthread_mutex_lock(&data->mutex);
	gtk_text_buffer_set_text(gtk_text_view_get_buffer(
				GTK_TEXT_VIEW(data->text)),"",0);
	pthread_mutex_unlock(&data->mutex);
}

void help(GtkWidget *widget,gpointer data)
{}

void about(GtkWidget *widget,gpointer data)
{}

void properties(GtkWidget *widget,gpointer data)
{
	/*setlocale(LC_ALL,"");
	setlocale(LC_CTYPE,"zh_CN.UTF-8");
	setenv("LANG","zh_CN.UTF-8",1);*/
}

void tray_on_menu(GtkWidget *widget,guint button,
		guint32 activate_time,gpointer data)
{
	gtk_menu_popup(GTK_MENU(data),NULL,NULL,gtk_status_icon_position_menu,GTK_STATUS_ICON(widget),button,activate_time);
}

void hide_win(GtkWidget *widget,gpointer data)
{
	gtk_widget_hide_all(GTK_WIDGET(data));
}

void hide_window(GtkWidget *widget,GdkEventWindowState *event,gpointer data)
{
	if(event->changed_mask == GDK_WINDOW_STATE_ICONIFIED && event->new_window_state == GDK_WINDOW_STATE_ICONIFIED)
		gtk_widget_hide_all(widget);
}

void tray_on_click(GtkWidget *widget,gpointer data)
{
	if(!gtk_widget_get_visible(GTK_WIDGET(data)))
	{
		gtk_widget_show_all(GTK_WIDGET(data));
		gtk_window_present(GTK_WINDOW(data));
	}
	else
		gtk_widget_hide_all(GTK_WIDGET(data));
	//gtk_status_icon_set_visible(GTK_STATUS_ICON(widget),FALSE);
}


/*void _get_connect(DATA *data)
{
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));
	GtkTextIter end;
	char buf[1024];
	int len;

	//len=read(pipefd[0],buf,sizeof(buf)-1);
	if(len==-1)
		return;
	gtk_text_buffer_get_end_iter(buffer,&end);
	if(gtk_text_iter_get_offset(&end)>INT_MAX)
	{
		while(gtk_events_pending())
			gtk_main_iteration();

		gtk_text_buffer_set_text(buffer,"",0);
		return;
	}
	while(gtk_events_pending())
		gtk_main_iteration();
	gtk_text_buffer_insert(buffer,&end,buf,len);
}*/

void get_connect(DATA *data)
{
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(data->text));
	GtkTextIter end;
	int pipefd[2];
	char buf[1024];
	int len;
	guint offset;

	//gtk_text_buffer_get_iter_at_mark(buffer,&iter,mark);
	pipe(pipefd);
	sig_data=data;

	if((data->pid=fork())==0)
	{
		close(pipefd[0]);
		dup2(pipefd[1],STDERR_FILENO);
		dup2(pipefd[1],STDOUT_FILENO);

		//execl("/usr/bin/python","python","/home/brisk/vbox-share/goagent/local/proxy.py",NULL);
		if(execl(data->python_path,"python",data->goagent_path,NULL)==-1)
		{
			write(STDERR_FILENO,strerror(errno),strlen(strerror(errno)));
			kill(getppid(),SIGUSR1);
			_exit(-1);
		}
	}

	close(pipefd[1]);

	/*while(len=read(pipefd[0],buf,sizeof(buf))-1)
	{
		gtk_text_buffer_get_end_iter(buffer,&end);

		if(len<=0)
			continue;

		if(gtk_text_iter_get_offset(&end)>INT_MAX)
		{
			gtk_text_buffer_set_text(buffer,"",0);
			bzero(buf,sizeof(buf));
			continue;
		}

		if(strstr(buf,"ERROR") || strstr(buf,"WARNING"))
		{
			gtk_text_buffer_insert(buffer,&end,buf,len);
			gtk_text_buffer_insert(buffer,&end,"\n",1);
		}
		bzero(buf,sizeof(buf));
	}*/
	while(1)
	{
		len=read(pipefd[0],buf,sizeof(buf)-1);
		if(len<=0)
			continue;

		while(gtk_events_pending())
			gtk_main_iteration();

		pthread_mutex_lock(&data->mutex);
		gtk_text_buffer_get_end_iter(buffer,&end);
		gtk_text_iter_get_offset(&end);
		pthread_mutex_unlock(&data->mutex);

		if(offset>INT_MAX)
		{
			while(gtk_events_pending())
				gtk_main_iteration();

			pthread_mutex_lock(&data->mutex);
			gtk_text_buffer_set_text(buffer,"",0);
			pthread_mutex_unlock(&data->mutex);

			bzero(buf,sizeof(buf));
			continue;
		}

		while(gtk_events_pending())
			gtk_main_iteration();

		pthread_mutex_lock(&data->mutex);
		gtk_text_buffer_insert(buffer,&end,buf,len);
		pthread_mutex_unlock(&data->mutex);
		//gtk_text_buffer_insert(buffer,&end,"\n",1);
		bzero(buf,sizeof(buf));
	}
}

void kill_pthread(int signum)
{
	//message_box(NULL,strerror(errno));
	sig_data->off=0;
	while(waitpid(-1,NULL,WNOHANG)!=-1);
	pthread_cancel(sig_data->thread);
}

gboolean is_python_and_goagent_path(char *python_path,char *goagent_path)
{
	if(python_path==NULL)
	{
		message_box(NULL,_("You Should Set Python Path!"));
		return FALSE;
	}

	if(goagent_path==NULL)
	{
		message_box(NULL,_("You Should Set GoAgent Path!"));
		return FALSE;
	}

	return TRUE;
}
