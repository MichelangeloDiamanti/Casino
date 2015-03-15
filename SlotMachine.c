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
void stopIMG();
void useSocket(char type);
void stopIMGkey(GtkWidget *widget, GdkEventKey *event);

/*var exec*/
char cmdOUT[128], cmdIN[128];

char sockRes[256];
char fiches[25], wins[10];
int countIMG=0, premi[3];

GdkColor lblColor, winColor, loseColor;
/* widget */
GtkWidget *window;
GtkWidget *container;
GtkWidget *slotIMG;
GtkWidget *images[3];
GtkWidget *pullBtn;
GtkWidget *info;
GtkWidget *WinOrLose;


FILE *fp;

int main( int argc, char *argv[])
{

	/* inizializzazione libreria GTK */
	gtk_init(&argc, &argv);

	/* creazione e settaggio iniziale della window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Casinò - Slot Machine");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("src/icon.png"));
	g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_resizable(GTK_WINDOW(window), 0);
 	gtk_widget_show(window);


	lblColor.red = 65535;
  	lblColor.green = 39321;
  	lblColor.blue = 0;
	winColor.red = 0;
  	winColor.green = 65535;
  	winColor.blue = 0;
   loseColor.red = 65535;
   loseColor.green = 0;
  	loseColor.blue = 0;

	/* creazione di un container */
	container = gtk_fixed_new();
  	gtk_container_add(GTK_CONTAINER(window), container);

	slotIMG = gtk_image_new_from_file("src/SlotMachineAnimation.gif");
	gtk_fixed_put(GTK_FIXED(container), slotIMG, 0, 0);

	images[0] = gtk_image_new();
	gtk_image_set_from_file(images[0],"src/sm8.png");
	gtk_fixed_put(GTK_FIXED(container), images[0], 60, 43);

	images[1] = gtk_image_new();
	gtk_image_set_from_file(images[1],"src/sm8.png");
	gtk_fixed_put(GTK_FIXED(container), images[1], 221, 44);

	images[2] = gtk_image_new();
	gtk_image_set_from_file(images[2],"src/sm8.png");
	gtk_fixed_put(GTK_FIXED(container), images[2], 381, 45);


	pullBtn = gtk_button_new_with_label("PULL [50 fiches]");
	gtk_fixed_put(GTK_FIXED(container), pullBtn, 380, 225);
	gtk_widget_set_size_request(pullBtn, 153, 25);
	g_signal_connect(G_OBJECT(pullBtn), "clicked", G_CALLBACK(stopIMG), NULL);

	info = gtk_label_new ("FICHES: ");
	gtk_fixed_put(GTK_FIXED(container), info, 70, 225);
	gtk_widget_modify_fg (info, GTK_STATE_NORMAL, &lblColor);
	useSocket('F');
	strcpy(fiches, "FICHES: ");
	strcat(fiches, sockRes);
	gtk_label_set_text(info,fiches);

	WinOrLose = gtk_label_new ("HAI PERSO");
	gtk_fixed_put(GTK_FIXED(container), WinOrLose, 260, 225);
	gtk_widget_modify_fg (WinOrLose, GTK_STATE_NORMAL, &loseColor);

	g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (stopIMGkey), NULL);

	gtk_widget_show(container);
	gtk_widget_show(slotIMG);
	gtk_widget_show(pullBtn);
	gtk_widget_show(info);

	

	gtk_main();
   return 0;
}

/*
+----------------------------------------------------------------------------------------------+
|                                            CALCOLO PREMI                                     |
+----------------------------------------------------------------------------------------------+
*/
void stopIMG()
{
	int p=0;

	exec("shuf -i 1-8 -n 1");
	cmdOUT[strlen(cmdOUT)-1]='\0';
	p=atoi(cmdOUT);	
	premi[countIMG]=p;

	if(countIMG<=2)
	{
		if(p==1) gtk_image_set_from_file(images[countIMG],"src/sm1.png");
		if(p==2) gtk_image_set_from_file(images[countIMG],"src/sm2.png");
		if(p==3) gtk_image_set_from_file(images[countIMG],"src/sm3.png");
		if(p==4) gtk_image_set_from_file(images[countIMG],"src/sm4.png");
		if(p==5) gtk_image_set_from_file(images[countIMG],"src/sm5.png");
		if(p==6) gtk_image_set_from_file(images[countIMG],"src/sm6.png");
		if(p==7) gtk_image_set_from_file(images[countIMG],"src/sm7.png");		
		if(p==8) gtk_image_set_from_file(images[countIMG],"src/sm8.png");	
	}
	
	if(countIMG==0)
	{
		gtk_widget_show(images[0]);
		gtk_button_set_label(pullBtn,"PULL");
		strcpy(wins,"-50");	
		useSocket('W');
		strcpy(fiches, "FICHES: ");
		strcat(fiches, sockRes);
		gtk_label_set_text(info,fiches);
		gtk_label_set_text(WinOrLose,"HAI PERSO");
		gtk_widget_modify_fg (WinOrLose, GTK_STATE_NORMAL, &loseColor);
	}

	if(countIMG==1)  gtk_widget_show(images[1]);
	
	if(countIMG==2)
	{
		gtk_widget_show(images[2]);

		if(premi[0]==premi[1] && premi[1]==premi[2])
		{
			
			if(premi[0]==1 || 
				premi[0]==2 || 
				premi[0]==3 || 
				premi[0]==4 || 
				premi[0]==5 || 	
				premi[0]==6) strcpy(wins,"550");
			if(premi[0]==7) strcpy(wins,"1050");
			if(premi[0]==8) strcpy(wins,"2550");
		
			useSocket('W');
			strcpy(fiches, "FICHES: ");
			strcat(fiches, sockRes);
			gtk_label_set_text(info,fiches);
			gtk_label_set_text(WinOrLose,"HAI VINTO!\n       (TRIS)");
			gtk_widget_modify_fg (WinOrLose, GTK_STATE_NORMAL, &winColor);
		} 

		if((premi[0]==premi[1] && premi[1]!=premi[2]) ||
			(premi[0]!=premi[1] && premi[1]==premi[2]))
		{				
			strcpy(wins,"50");		
			useSocket('W');
			strcpy(fiches, "FICHES: ");
			strcat(fiches, sockRes);
			gtk_label_set_text(info,fiches);
			gtk_label_set_text(WinOrLose,"HAI VINTO!\n   (COPPIA)");
			gtk_widget_modify_fg (WinOrLose, GTK_STATE_NORMAL, &winColor);
		} 


		gtk_widget_show(WinOrLose);
		gtk_button_set_label(pullBtn,"RIPROVA");
	}

	if(countIMG>=3)
	{
		gtk_button_set_label(pullBtn,"PULL [50 fiches]");
		gtk_widget_hide(images[0]);
		gtk_widget_hide(images[1]);
		gtk_widget_hide(images[2]);
		gtk_widget_hide(WinOrLose);
		countIMG=-1;
	}
	
	countIMG++; 
}

/*
+----------------------------------------------------------------------------------------------+
|                                       SOCKET                                                 |
+----------------------------------------------------------------------------------------------+
*/


void useSocket(char type)
{
	char IP[16],IPdig;
	int i;
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
  	char buffer[256], msg[500];

	if(type=='W')
	{
		strcpy(msg,"SLOT%updateFiches%");
		exec("cat /home/$USER/Documents/Casino/casinoKey | grep -Po '(?<=(%)).*' | grep -Po '(?<=(%)).*'");
		cmdOUT[strlen(cmdOUT)-1]='\0';
		strcat(msg,cmdOUT);
		strcat(msg,"%");
		strcat(msg,wins);
		
	}
	if(type=='F')
	{
		strcpy(msg,"SLOT%getFiches%");
		exec("cat /home/$USER/Documents/Casino/casinoKey | grep -Po '(?<=(%)).*' | grep -Po '(?<=(%)).*'");
		cmdOUT[strlen(cmdOUT)-1]='\0';
		strcat(msg,cmdOUT);
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

	if(strcmp(buffer,"SLOT%ERR")==0)
	{	
		close(sockfd);
		return 1;
	}

	if(strcmp(buffer,"SLOT%ERR")!=0)
	{
		strcpy(cmdIN,"echo \"");
		strcat(cmdIN,buffer);
		strcat(cmdIN,"\" | cut -c6-");
		exec(cmdIN);
		cmdOUT[strlen(cmdOUT)-1]='\0';
		strcpy(sockRes,cmdOUT);
		close(sockfd);		
	}

}

/*
+----------------------------------------------------------------------------------------------+
|                                          SPACE SIMULA PULL                                   |
+----------------------------------------------------------------------------------------------+
*/
void stopIMGkey(GtkWidget *widget, GdkEventKey *event)
{
	if(!strcmp(gdk_keyval_name (event->keyval),"space")) stopIMG(); 
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
