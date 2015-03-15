/* librerie */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <gtk/gtk.h>


/* prototipi funzioni */
GdkPixbuf *create_pixbuf(const gchar * filename);
static gboolean submitKey(GtkWidget *widget, GdkEventKey *event);
void submit();
void Reg();
void SUclick();
void CBclick();
int controlli(char C);
void exec(char* cmd);


/* flag checkbox */
char CB='0';

/* flag key kmli */
char KMLI='0';

/* flag reg or submit */
char RS='S';

/*var exec*/
char cmdOUT[128], cmdIN[128];

/* widget */
GtkWidget *window;
GtkWidget *container;
GtkWidget *frame;
GtkWidget *labelU, *labelP, *labelCP, *labelErr, *labelRS;
GtkWidget *entryU, *entryP, *entryCP;
GtkWidget *login;
GtkWidget *checkBox;
GtkWidget *signupL, *signupS;
FILE *fp;

int main( int argc, char *argv[])
{

	system(" [ -d /home/$USER/Documents/Casino ] || mkdir /home/$USER/Documents/Casino ");
 	exec(" [ -f /home/$USER/Documents/Casino/casinoKey ] && echo \"1\" || echo \"0\" ");
	if(cmdOUT[0]=='1')
	{
		exec(" cat /home/$USER/Documents/Casino/casinoKey | cut -c-6 ");
		cmdOUT[strlen(cmdOUT)-1]='\0';
		if(!strcmp(cmdOUT,"%KEEP%"))
		{
			KMLI='1';
			submit();
		}
	}

	/* inizializzazione libreria GTK */
	gtk_init(&argc, &argv);

	/* creazione e settaggio iniziale della window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Casinò - Login");
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("src/icon.png"));
	g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_resizable(GTK_WINDOW(window), 0);
 	gtk_widget_show(window);

	/* settaggio dei colori (arancione e rosso) */
	GdkColor ORNGcolor, ERRcolor;
	ORNGcolor.red = 55255;
  	ORNGcolor.green = 23901;
  	ORNGcolor.blue = 10794;
	ERRcolor.red = 40000;
  	ERRcolor.green = 0;
  	ERRcolor.blue = 0;

	/* creazione di un container */
	container = gtk_fixed_new();
  	gtk_container_add(GTK_CONTAINER(window), container);

	/* creazione di un frame */
	frame = gtk_frame_new(NULL);	
	gtk_widget_set_size_request(frame, 380, 207);	
	gtk_fixed_put(GTK_FIXED(container), frame, 0, 0);
	
	/* creazione delle label (username, password, confirm password, errori, registrazione effettuata) */
	labelU = gtk_label_new ("Username:");
	gtk_fixed_put(GTK_FIXED(container), labelU, 25, 30);
	
	labelP = gtk_label_new ("Password:");
	gtk_fixed_put(GTK_FIXED(container), labelP, 25, 70);
	
	labelCP = gtk_label_new (" Confirm\nPassword:");
	gtk_fixed_put(GTK_FIXED(container), labelCP, 25, 100);

	labelErr = gtk_label_new ("");
	gtk_widget_modify_fg (labelErr, GTK_STATE_NORMAL, &ERRcolor);
	gtk_fixed_put(GTK_FIXED(container), labelErr, 5, 190);

	labelRS = gtk_label_new ("Registrazione \neffettuata con successo");
	gtk_widget_modify_fg (labelRS, GTK_STATE_NORMAL, &ORNGcolor);
	gtk_fixed_put(GTK_FIXED(container), labelRS, 5, 170);

	/* creazione delle textbox (username, password, confirm password) */
	entryU = gtk_entry_new ();
	gtk_fixed_put(GTK_FIXED(container), entryU, 110, 25);
	gtk_entry_set_max_length (GTK_ENTRY (entryU), 256);
	gtk_widget_set_size_request(entryU, 260, 1);

	entryP = gtk_entry_new ();
	gtk_fixed_put(GTK_FIXED(container), entryP, 110, 65);
	gtk_entry_set_max_length (GTK_ENTRY (entryP), 256);
	gtk_widget_set_size_request(entryP, 260, 1);
	gtk_entry_set_visibility(entryP, 0);

	entryCP = gtk_entry_new ();
	gtk_fixed_put(GTK_FIXED(container), entryCP, 110, 105);
	gtk_entry_set_max_length (GTK_ENTRY (entryCP), 256);
	gtk_widget_set_size_request(entryCP, 260, 1);
	gtk_entry_set_visibility(entryCP, 0);


	/* creazione dei bottoni (login, signup, signup effettivo) */
	login = gtk_button_new_with_label("Login");
 	gtk_widget_set_size_request(login, 150, 25);
 	gtk_fixed_put(GTK_FIXED(container), login, 125, 110);
	gtk_widget_modify_fg (login, GTK_STATE_NORMAL, &ORNGcolor);
	g_signal_connect(G_OBJECT(login), "clicked", G_CALLBACK(submit), NULL);

	signupL = gtk_button_new_with_label ("Registrati");
 	gtk_widget_set_size_request(signupL, 150, 25);
	gtk_fixed_put(GTK_FIXED(container), signupL, 125, 140);
	g_signal_connect(G_OBJECT(signupL), "clicked", G_CALLBACK(SUclick), NULL);

	signupS = gtk_button_new_with_label ("Registrati");
 	gtk_widget_set_size_request(signupS, 150, 25);
	gtk_fixed_put(GTK_FIXED(container), signupS, 125, 155);
	gtk_widget_modify_fg (signupS, GTK_STATE_NORMAL, &ORNGcolor);
	g_signal_connect(G_OBJECT(signupS), "clicked", G_CALLBACK(Reg), NULL);

	/* creazione di una checkbox */		
	checkBox = gtk_check_button_new_with_label("Resta connesso");
	gtk_fixed_put(GTK_FIXED(container), checkBox, 249, 184);
	g_signal_connect(G_OBJECT(checkBox), "clicked", G_CALLBACK(CBclick), NULL);

	
	g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (submitKey), NULL);

	/* specifiche sui widget da mostrare e da nascondere */
	gtk_widget_show_all(container);
	gtk_widget_hide(labelCP);
	gtk_widget_hide(signupS);
 	gtk_widget_hide(entryCP);
	gtk_widget_hide(labelErr);
	gtk_widget_hide(labelRS);

	gtk_main();

  return 0;
}


/*
+----------------------------------------------------------------------------------------------+
|                                          FUNZIONE LOGIN                                      |
+----------------------------------------------------------------------------------------------+
*/
void submit()
{
	int e;
	if(KMLI=='0') e=controlli('L');
	if(KMLI=='1') e=controlli('K');
	
	if(e==1)
	{
		system("./casino &");
		if(KMLI=='0') gtk_main_quit();
		if(KMLI=='1') exit(0);
	}
	if(e==0) KMLI=0;
}


/*
+----------------------------------------------------------------------------------------------+
|                                       FUNZIONE REGISTRAZIONE                                 |
+----------------------------------------------------------------------------------------------+
*/
void Reg()
{
	int e;
	e=controlli('S');
	if(e==1)
	{
		gtk_widget_hide(labelCP);
		gtk_widget_hide(signupS);
		gtk_widget_hide(entryCP);
		gtk_widget_hide(labelErr);
		gtk_widget_show(signupL);
		gtk_widget_show(login);
		gtk_widget_show(checkBox);
		gtk_widget_show(labelRS);
		RS='S';
	}

}


/*
+----------------------------------------------------------------------------------------------+
|                                            CONTROLLI                                         |
+----------------------------------------------------------------------------------------------+
*/

int controlli(char C)
{

	char msg[500];
	char errChar='0', errMsg[40];
	char IP[16],IPdig;
	int i;

	if(C=='K')
	{
		char skey[128];
		exec("cat /home/$USER/Documents/Casino/casinoKey | cut -c7-");
		cmdOUT[strlen(cmdOUT)-1]='\0';
		strcpy(skey,cmdOUT);
		strcpy(msg,"LOGI%1%");
		strcat(msg, skey);
	}
	
	if(C=='S' || C=='L')
	{
		char username[256];
		char password[256];
		char cpassword[256];
		
		

		/* copia il contenuto delle textbox nelle stringhe */
		strcpy(username,gtk_entry_get_text(entryU));
		strcpy(password,gtk_entry_get_text(entryP));
		if(C=='S') strcpy(cpassword,gtk_entry_get_text(entryCP));

		/* controllo che le stringhe non siano vuote */	
		if( (C=='S' && (strcmp(username,"")==0 || strcmp(password,"")==0 || strcmp(cpassword,"")==0)) ||
			 (C=='L' && (strcmp(username,"")==0 || strcmp(password,"")==0)) )
		{
			gtk_label_set_text(labelErr,"I campi non possono rimanere vuoti");
			gtk_widget_show(labelErr);
			gtk_widget_hide(labelRS);
			return 0;
		}


		/* controllo che lo username non contenga caratteri non consentiti */
		for(i=0;i<strlen(username);i++)
		{
			if(username[i]=='%' || username[i]=='\'' || username[i]==' ')
			{
				errChar=username[i];			
				strcpy(errMsg,"Il carattere < > non è consentito (username)");
				errMsg[14]=errChar;			
				gtk_label_set_text(labelErr,errMsg);
				gtk_widget_show(labelErr);
				gtk_widget_hide(labelRS);
				return 0;
			}		
		}
	
		/* controllo che la password non contenga caratteri non consentiti */
		for(i=0;i<strlen(password);i++)
		{
			if(password[i]=='%' || password[i]=='\'' || password[i]==' ')
			{
				errChar=password[i];			
				strcpy(errMsg,"Il carattere < > non è consentito (password)");
				errMsg[14]=errChar;			
				gtk_label_set_text(labelErr,errMsg);
				gtk_widget_show(labelErr);
				gtk_widget_hide(labelRS);
				return 0;
			}		
		}

		if(C=='S')
		{
			/* controllo che le password siano uguali */
			if(strcmp(password,cpassword)!=0)
			{
				gtk_label_set_text(labelErr,"Le password non corrispondono");
				gtk_widget_show(labelErr);
				gtk_widget_hide(labelRS);
				return 0;
			}
		}


		/* creazione del messaggio */
		char pwdcrpt[32];
		if(C=='S') strcpy(msg,"RGST%0%");
		if(C=='L') strcpy(msg,"LOGI%0%");
		strcat(msg,username);
		strcat(msg,"%");

		strcpy(cmdIN,"openssl passwd -1 -salt MDLAGM ");
		strcat(cmdIN,password);
		strcat(cmdIN," | grep -Po '(?<=(MDLAGM\\$)).*'");
		exec(cmdIN);	
		cmdOUT[strlen(cmdOUT)-1]='\0';
		strcpy(pwdcrpt,cmdOUT);
		strcat(msg,pwdcrpt);
	
		if(C=='L')
		{
			exec("hostname -I");
			cmdOUT[strlen(cmdOUT)-2]='\0';
			strcat(msg, "%");
			strcat(msg,cmdOUT);
			strcat(msg, "%");
			exec("hostname");
			cmdOUT[strlen(cmdOUT)-1]='\0';
			strcat(msg,cmdOUT);
		}
	}

	/* apertura del socket ed invio del messaggio */
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
  	char buffer[256];

	portno = 54321;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{ 
		gtk_label_set_text(labelErr,"Errore di connessione");
		gtk_widget_show(labelErr);
		gtk_widget_hide(labelRS);
		close(sockfd);
		return 0;	
	}	
	fp = fopen("serverIP.txt","r");
	i=0;
	while((IPdig=fgetc(fp))!=EOF)
	{
		IP[i]=IPdig;
		i++;
	}
	IP[i-1]='\0';
	fclose(fp);
	server = gethostbyname(IP);
	if (server == NULL) 
	{ 
		gtk_label_set_text(labelErr,"Errore di connessione");
		gtk_widget_show(labelErr);
		gtk_widget_hide(labelRS);
		close(sockfd);
		return 0;	
	}	

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{ 
		gtk_label_set_text(labelErr,"Errore di connessione");
		gtk_widget_show(labelErr);
		gtk_widget_hide(labelRS);
		close(sockfd);
		return 0;	
	}	
	 
	bzero(buffer,256);
	strcpy(buffer,msg);
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0)
	{ 
		gtk_label_set_text(labelErr,"Errore di connessione");
		gtk_widget_show(labelErr);
		gtk_widget_hide(labelRS);
		close(sockfd);
		return 0;	
	}	


	bzero(buffer,256);
	n = read(sockfd,buffer,256);
	if (n < 0)
	{ 
		gtk_label_set_text(labelErr,"Errore di connessione");
		gtk_widget_show(labelErr);
		gtk_widget_hide(labelRS);
		close(sockfd);
		return 0;	
	}	

	/* controllo che lo username sia valido */
	if(strcmp(buffer,"RGST%ERR")==0)
	{	
		gtk_label_set_text(labelErr,"lo username inserito non è disponibile");
		gtk_widget_show(labelErr);
		gtk_widget_hide(labelRS);
		close(sockfd);
		return 0;
	}

	if(strcmp(buffer,"LOGI%ERR")==0)
	{	
		gtk_label_set_text(labelErr,"Login errato");
		gtk_widget_show(labelErr);
		gtk_widget_hide(labelRS);
		close(sockfd);
		return 0;
	}

	/* se la registrazione è andata a buon fine viene visualizzata la schermata precedente */
	if(strcmp(buffer,"RGST%OK")==0 || strcmp(buffer,"LOGI%OK")==0)
	{

		if(C=='L')
		{
			bzero(buffer,256);
			n = read(sockfd,buffer,256);
			if (n < 0)
			{ 
				gtk_label_set_text(labelErr,"Errore di connessione");
				gtk_widget_show(labelErr);
				gtk_widget_hide(labelRS);
				close(sockfd);
				return 0;	
			}	
			else
			{
				strcpy(cmdIN, "echo \"");
				strcat(cmdIN, buffer);
				strcat(cmdIN, "\" | cut -c6- > /home/$USER/Documents/Casino/casinoKey");
				exec(cmdIN);
				if(CB=='1')	system("sed -i '1s/^/%KEEP%/' /home/$USER/Documents/Casino/casinoKey");
				if(CB=='0')	system("sed -i '1s/^/%DONTKEEP%/' /home/$USER/Documents/Casino/casinoKey");
			}
		}
		
		close(sockfd);		
		return 1;
	}


}

/*
+----------------------------------------------------------------------------------------------+
|                                     CAMBIO FORM (DA LOGIN A SIGNUP)                          |
+----------------------------------------------------------------------------------------------+
*/
void SUclick()
{
	gtk_widget_show(labelCP);
	gtk_widget_show(signupS);
	gtk_widget_show(entryCP);

	gtk_widget_hide(signupL);
	gtk_widget_hide(login);
	gtk_widget_hide(checkBox);

	RS='R';
}

/*
+----------------------------------------------------------------------------------------------+
|                                       GESTIONE DELLA CHECKBOX                                |
+----------------------------------------------------------------------------------------------+
*/
void CBclick()
{
	if(CB=='0') CB='1';
	else if(CB=='1') CB='0';
}

/*
+----------------------------------------------------------------------------------------------+
|                                       LOGIN PREMENDO INVIO                                   |
+----------------------------------------------------------------------------------------------+
*/
static gboolean submitKey(GtkWidget *widget, GdkEventKey *event)
{
	if(!strcmp(gdk_keyval_name (event->keyval),"Return"))
	{
		if(RS=='R') Reg();
		if(RS=='S') submit();
	} 
	return FALSE;
}


/*
+----------------------------------------------------------------------------------------------+
|                                       FUNZIONE PER L' ICONA                                  |
+----------------------------------------------------------------------------------------------+
*/
GdkPixbuf *create_pixbuf(const gchar * filename)
{
   GdkPixbuf *pixbuf;
   GError *error = NULL;
   pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   if(!pixbuf) {
      fprintf(stderr, "%s\n", error->message);
      g_error_free(error);
   }

   return pixbuf;
}


/*
	+-------------------------------------------------------------------------------------------+
	|                                             EXEC                                          |
	+-------------------------------------------------------------------------------------------+
*/
/* funzione che restituisce una stringa dopo aver eseguito un comando da terminale  */
void exec(char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) exit(0);
    char buffer[128];
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		strcpy(cmdOUT,buffer);
    }
    pclose(pipe);
}


//gcc SignUp.c -o SignUp `pkg-config --libs --cflags gtk+-3.0` -w
