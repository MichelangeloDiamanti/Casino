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
void exec(char* cmd);
int useSocket(char type, char* other);
void updateTables();
void logout();
void startMatch(GtkButton *button, gpointer id);

/*var exec*/
char cmdOUT[128], cmdIN[128];

char tbls[256];

/* widget */
GtkWidget *window;
GtkWidget *table;
GtkWidget *header;
GtkWidget *notebook;
	
GtkWidget *container[25];
GtkWidget *tablebox;
GtkWidget *headerIMG;
GtkWidget *labelNB;
GtkWidget *lBox;
GtkWidget *tblButton;
GtkWidget *logOut;
GtkWidget *refresh;
FILE *fp;

int main( int argc, char *argv[])
{

	system(" [ -d /home/$USER/Documents/Casino ] || mkdir /home/$USER/Documents/Casino ");
 	exec(" [ -f /home/$USER/Documents/Casino/casinoKey ] && echo \"1\" || echo \"0\" ");
	if(cmdOUT[0]=='0')
	{
		system("./LogIn &");
		gtk_main_quit();
	}


	/* inizializzazione libreria GTK */
	gtk_init(&argc, &argv);

	/* creazione e settaggio iniziale della window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Casinò");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("src/icon.png"));
	gtk_window_maximize(GTK_WINDOW(window));
	gtk_window_set_resizable(GTK_WINDOW(window), 1);
	g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
 	gtk_widget_show(window);

	table = gtk_table_new(1000,250,FALSE);
   gtk_container_add (GTK_CONTAINER (window), table);
	
	
	header = gtk_label_new("");
	useSocket('H',"");
	strcpy(cmdIN,"echo -n \"  \" && echo \"");
	strcat(cmdIN,tbls);
	strcat(cmdIN,"\" | cut -c6-");
	exec(cmdIN);
	cmdOUT[strlen(cmdOUT)-1]='\0';
	if(!strcmp(cmdOUT,"    -  "))
	{
		system("./LogIn");
		exit(1);
	}
	gtk_label_set_text(header,cmdOUT);
	//controllo esistenza chiave

	logOut = gtk_button_new_with_label ("LogOut \u233D");
	g_signal_connect(G_OBJECT(logOut), "clicked", G_CALLBACK(logout), NULL);
	refresh = gtk_button_new_with_label ("Refresh \u21BB");
	g_signal_connect(G_OBJECT(refresh), "clicked", G_CALLBACK(updateTables), NULL);
	
	notebook = gtk_notebook_new();
   gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
	
	gtk_table_attach_defaults(GTK_TABLE(table), header, 0,2,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table), refresh, 246,248,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table), logOut, 248,250,0,1);
	gtk_table_attach_defaults(GTK_TABLE(table), notebook, 0,250,1,1000);

	updateTables();

	gtk_widget_show_all(table);
	gtk_widget_show_all(notebook);

	gtk_main();
   return 0;
}

/*
+----------------------------------------------------------------------------------------------+
|                                           UPDATE TABLES                                      |
+----------------------------------------------------------------------------------------------+
*/
void updateTables()
{
	short int err=0, count=0, GI=0, nG=0;
	int i,p;
	char Giochi[25][25], nGC[3], Partite[10][30];
	char *token;
	const char delimeter[2]="%";


	gtk_widget_destroy(notebook);
	notebook = gtk_notebook_new();
   gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
	gtk_table_attach_defaults(GTK_TABLE(table), notebook, 0,250,1,1000);


	
	do
	{
		err = useSocket('T',"");
	}while(err==1);


	token = strtok(tbls, delimeter);
	while( token != NULL ) 
  	{
		if(count==1)
		{
			strcpy(nGC,token);
			nG=atoi(nGC);
		}
		if(count>1)
		{
			strcpy(Giochi[GI],token);
			GI++;
		}
		count++;
		token = strtok(NULL, delimeter);
  	}


   for (i=0; i < nG; i++)
	{   
		container[i] = gtk_table_new(1000,2,FALSE); //tabella (tab del notabook)         
		
		tablebox = gtk_scrolled_window_new(NULL,NULL); //container con scrollbar (per la vbox)

	  	lBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); //vbox per i pulsanti
	   gtk_container_add (GTK_CONTAINER (tablebox), lBox);
    
		headerIMG = gtk_image_new_from_file("src/BackGroundHeader.png");
		if(!strcmp(Giochi[i],"SlotMachine")) gtk_image_set_from_file(headerIMG,"src/SlotMachineHeader.png");
   	if(!strcmp(Giochi[i],"Roulette")) headerIMG = gtk_image_new_from_file("src/RouletteHeader.png");
		if(!strcmp(Giochi[i],"BlackJack")) headerIMG = gtk_image_new_from_file("src/BlackJackHeader.png");
		if(!strcmp(Giochi[i],"Poker")) headerIMG = gtk_image_new_from_file("src/PokerHeader.png");
		if(!strcmp(Giochi[i],"Dices")) headerIMG = gtk_image_new_from_file("src/DicesHeader.png");

		gtk_table_attach_defaults(GTK_TABLE(container[i]), headerIMG, 0,2,0,1);
		gtk_table_attach_defaults(GTK_TABLE(container[i]), tablebox, 0,250,1,1000);

		do
		{
			err = useSocket('P',Giochi[i]);
		}while(err==1);

		count=0;
		token = strtok(tbls, delimeter);
		while( token != NULL ) 
	  	{
			if(count >= 1) strcpy(Partite[count-1],token);
			count++;
			token = strtok(NULL, delimeter);
	  	}
		
		for(p=0; p<count-1; p++)
		{ 
			int idp=0;
			strcpy(cmdIN,"echo \"");
			strcat(cmdIN,Partite[p]);
			strcat(cmdIN,"\" | grep -Po '(?<=(Tavolo )).*(?= - )'");
			exec(cmdIN);
			cmdOUT[strlen(cmdOUT)-1]='\0';
			idp=atoi(cmdOUT);

			tblButton = gtk_button_new_with_label(Partite[p]);
			gtk_button_set_alignment (tblButton, 0, 0.5);
			gtk_box_pack_start(GTK_BOX(lBox), tblButton, TRUE, TRUE, 0);
			gtk_box_set_child_packing(lBox,tblButton,0,0,0,GTK_PACK_START);
			g_signal_connect(G_OBJECT(tblButton), "clicked", G_CALLBACK(startMatch), idp);
		}
	
  		
		labelNB = gtk_label_new(Giochi[i]);
		gtk_notebook_append_page (GTK_NOTEBOOK (notebook), container[i], labelNB);

   }
	gtk_widget_show_all(notebook);
}

/*
+----------------------------------------------------------------------------------------------+
|                                           INIZIO PARTITA                                     |
+----------------------------------------------------------------------------------------------+
*/

void startMatch(GtkButton *button, gpointer id)
{ 

	gtk_widget_set_sensitive(button, FALSE);
	
	char sid[128];
	sprintf(sid, "%d", id);
	strcat(sid,"%");
	exec("cat /home/$USER/Documents/Casino/casinoKey | grep -Po '(?<=(%)).*' | grep -Po '(?<=(%)).*'");
	cmdOUT[strlen(cmdOUT)-1]='\0';
	strcat(sid,cmdOUT);
	
	useSocket('S',sid); 
	
	int numg, maxg;
	char ng[5], mg[5], newLbl[45];
	
	if(!strcmp(tbls,"TBLS%FULL"))
	{
		gtk_button_set_label(button,"Partita piena :(");
	}
	else
	{
		if(strcmp(tbls,"TBLS%ALREADY"))
		{
			strcpy(cmdIN,"echo \"");
			strcat(cmdIN,gtk_button_get_label(button));
			strcat(cmdIN,"\" | grep -Po '(?<=(: )).*(?=/)'");
			exec(cmdIN);
			cmdOUT[strlen(cmdOUT)-1]='\0';
			numg=atoi(cmdOUT);

			strcpy(cmdIN,"echo \"");
			strcat(cmdIN,gtk_button_get_label(button));
			strcat(cmdIN,"\" | grep -Po '(?<=(/)).*'");
			exec(cmdIN);
			cmdOUT[strlen(cmdOUT)-1]='\0';
			strcpy(mg,cmdOUT);
			maxg=atoi(cmdOUT);

			strcpy(cmdIN,"echo \"");
			strcat(cmdIN,gtk_button_get_label(button));
			strcat(cmdIN,"\" | grep -Po '.*(?=:)'");
			exec(cmdIN);
			cmdOUT[strlen(cmdOUT)-1]='\0';
			strcpy(newLbl,cmdOUT);
			strcat(newLbl,": ");
			sprintf(ng, "%d", numg+1);
			strcat(newLbl, ng);		
			strcat(newLbl, "/");
			strcat(newLbl, mg);
			if((numg+1)<maxg) strcat(newLbl, "   -In attesa di altri giocatori...");
			//if(numg+1==maxg)
			//{
				if(!strcmp(tbls,"TBLS%BlackJack")){}
				if(!strcmp(tbls,"TBLS%Roulette")){}
				if(!strcmp(tbls,"TBLS%Poker")){}
				if(!strcmp(tbls,"TBLS%Dices"))
				{
					strcpy(cmdIN,"echo \"");
					strcat(cmdIN,gtk_button_get_label(button));
					strcat(cmdIN,"\" | grep -Po '(?<=(Tavolo )).*(?= - )'");
					exec(cmdIN);
					cmdOUT[strlen(cmdOUT)-1]='\0';
					strcpy(cmdIN, "./Dices ");
					strcat(cmdIN, cmdOUT);
					strcat(cmdIN, " &");	
					system(cmdIN);
				}

				if(!strcmp(tbls,"TBLS%SlotMachine")) system("./SlotMachine &");
				
			//}
		}
		else
		{
			strcpy(newLbl,gtk_button_get_label(button));
			strcat(newLbl, "   -In attesa di altri giocatori...");
		}
		gtk_button_set_label(button,newLbl);
	}

	

}


/*
+----------------------------------------------------------------------------------------------+
|                                        UPDATE TABLES QUERY                                   |
+----------------------------------------------------------------------------------------------+
*/
int useSocket(char type, char* other)
{
	char IP[16],IPdig;
	int i;
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
  	char buffer[256], msg[500];;

	if(type=='T')	strcpy(msg,"TBLS%tables");
	if(type=='H')
	{
		strcpy(msg,"TBLS%header%");
		exec("cat /home/$USER/Documents/Casino/casinoKey | grep -Po '(?<=(%)).*' | grep -Po '(?<=(%)).*'");
		cmdOUT[strlen(cmdOUT)-1]='\0';
		strcat(msg,cmdOUT);
	}
	if(type=='P')
	{
		strcpy(msg,"TBLS%games%");
		strcat(msg, other);
	}
	if(type=='L')
	{
		strcpy(msg,"LOGO%");
		strcat(msg, other);
	}
	if(type=='S')
	{
		strcpy(msg,"TBLS%start%");
		strcat(msg, other);
	}
	
	portno = 54321;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{ 
		close(sockfd);
		return 1;	
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
		close(sockfd);
		return 1;	
	}	

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{ 
		close(sockfd);
		return 1;	
	}	
	 
	bzero(buffer,256);
	strcpy(buffer,msg);
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0)
	{ 
		close(sockfd);
		return 1;	
	}	


	bzero(buffer,256);
	n = read(sockfd,buffer,256);
	if (n < 0)
	{ 
		close(sockfd);
		return 1;	
	}	

	if(strcmp(buffer,"TBLS%ERR")==0)
	{	
		close(sockfd);
		return 1;
	}

	if(strcmp(buffer,"TBLS%ERR")!=0)
	{
		strcpy(tbls,buffer);
		close(sockfd);		
	}

}

/*
+----------------------------------------------------------------------------------------------+
|                                                 LOGOUT                                       |
+----------------------------------------------------------------------------------------------+
*/
void logout()
{
	exec("cat /home/$USER/Documents/Casino/casinoKey | cut -c-6 ");
	cmdOUT[strlen(cmdOUT)-1]='\0';

	if(!strcmp(cmdOUT,"%KEEP%")) exec("cat /home/$USER/Documents/Casino/casinoKey | cut -c7- ");
	if(!strcmp(cmdOUT,"%DONTK")) exec("cat /home/$USER/Documents/Casino/casinoKey | cut -c11- ");
	cmdOUT[strlen(cmdOUT)-1]='\0';
	useSocket('L',cmdOUT);
	if(!strcmp(tbls,"LOGO%OK"))
	{ 
		system("rm /home/$USER/Documents/Casino/casinoKey");
		system("./LogIn &");
		exit(0);
	}

	
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
