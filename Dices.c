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
void useSocket(char type);

/*var exec*/
char cmdOUT[128], cmdIN[128];

char sockRes[256];
char fiches[25], wins[10];
int countIMG=0, premi[3];
char IDpartita[5];

GdkColor blackColor, winColor, loseColor, infoColor;
/* widget */
GtkWidget *window;
GtkWidget *container;
GtkWidget *dices[2];
GtkWidget *info;
GtkWidget *puntata;
GtkWidget *WinOrLose;
GtkWidget *bet;
GtkWidget *faceBtn[6];
GtkWidget *scommetti;

GtkWidget *waitwin;
GtkWidget *waitlbl;
GtkWidget *waitimg;
GtkWidget *waitcon;

FILE *fp;

int main( int argc, char *argv[])
{

	strcpy(IDpartita,argv[1]);
	
	/* inizializzazione libreria GTK */
	gtk_init(&argc, &argv);

	/* creazione e settaggio iniziale della window */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Casinò - Dices");
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("src/icon.png"));
	g_signal_connect_swapped(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_resizable(GTK_WINDOW(window), 0);
	gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &blackColor);
 	gtk_widget_hide(window);

	waitwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(waitwin), "Casinò - Dices");
	gtk_window_set_position(GTK_WINDOW(waitwin), GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(waitwin), create_pixbuf("src/icon.png"));
	g_signal_connect_swapped(G_OBJECT(waitwin), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_resizable(GTK_WINDOW(waitwin), 0);
	gtk_container_set_border_width (GTK_CONTAINER (waitwin), 10);
 	gtk_widget_show(waitwin);


	infoColor.red = 65535;
  	infoColor.green = 39321;
  	infoColor.blue = 0;
	winColor.red = 0;
  	winColor.green = 65535;
  	winColor.blue = 0;
   loseColor.red = 65535;
   loseColor.green = 0;
  	loseColor.blue = 0;
	blackColor.red = 0;
   blackColor.green = 0;
  	blackColor.blue = 0;

	

	/* wait */
	waitcon = gtk_fixed_new();
  	gtk_container_add(GTK_CONTAINER(waitwin), waitcon);

	waitlbl = gtk_label_new ("In attesa di altri giocatori...");
	gtk_fixed_put(GTK_FIXED(waitcon), waitlbl, 0, 0);
	
	waitimg = gtk_image_new_from_file("src/loadinganim.gif");
	gtk_fixed_put(GTK_FIXED(waitcon), waitimg, 30, 15);

	gtk_widget_show_all(waitcon);	


	/* game 
	container = gtk_fixed_new();
  	gtk_container_add(GTK_CONTAINER(window), container);

	info = gtk_label_new ("FICHES: ");
	gtk_fixed_put(GTK_FIXED(container), info, 0, 0);
	gtk_widget_modify_fg (info, GTK_STATE_NORMAL, &infoColor);
	/*useSocket('F');
	strcpy(fiches, "FICHES: ");
	strcat(fiches, sockRes);
	gtk_label_set_text(info,fiches);

	dices[0] = gtk_image_new_from_file("src/diceanimR.gif");
	gtk_fixed_put(GTK_FIXED(container), dices[0], 0, 25);

	dices[1] = gtk_image_new_from_file("src/diceanimG.gif");
	gtk_fixed_put(GTK_FIXED(container), dices[1], 100, 25);

	puntata = gtk_label_new ("Puntata:");
	gtk_fixed_put(GTK_FIXED(container), puntata, 0, 130);
	gtk_widget_modify_fg (puntata, GTK_STATE_NORMAL, &infoColor);

	GtkAdjustment *spinner_adj;
   spinner_adj = (GtkAdjustment *) gtk_adjustment_new (0, 0, 501, 1, 1, 1);
	bet = gtk_spin_button_new(spinner_adj,1,0);
	gtk_fixed_put(GTK_FIXED(container), bet, 60, 125);
	gtk_widget_set_size_request(bet, 135, 1);

	
	faceBtn[0] = gtk_toggle_button_new_with_label("\u2680");
	gtk_fixed_put(GTK_FIXED(container), faceBtn[0], 0, 155);

	faceBtn[1] = gtk_toggle_button_new_with_label("\u2681");
	gtk_fixed_put(GTK_FIXED(container), faceBtn[1], 35, 155);

	faceBtn[2] = gtk_toggle_button_new_with_label("\u2682");
	gtk_fixed_put(GTK_FIXED(container), faceBtn[2], 70, 155);

	faceBtn[3] = gtk_toggle_button_new_with_label("\u2683");
	gtk_fixed_put(GTK_FIXED(container), faceBtn[3], 105, 155);

	faceBtn[4] = gtk_toggle_button_new_with_label("\u2684");
	gtk_fixed_put(GTK_FIXED(container), faceBtn[4], 140, 155);

	faceBtn[5] = gtk_toggle_button_new_with_label("\u2685");
	gtk_fixed_put(GTK_FIXED(container), faceBtn[5], 175, 155);

	scommetti = gtk_button_new_with_label("Scommetti");
	gtk_fixed_put(GTK_FIXED(container), scommetti, 0, 190);
	gtk_widget_modify_fg (scommetti, GTK_STATE_NORMAL, &infoColor);

	WinOrLose = gtk_label_new ("");
	gtk_fixed_put(GTK_FIXED(container), WinOrLose, 100, 195);
	gtk_widget_modify_fg (WinOrLose, GTK_STATE_NORMAL, &loseColor);

	gtk_widget_show_all(container);	
	
	*/


	do
	{
		useSocket('W');
	}while(strcmp(sockRes,"READY"));
gtk_label_set_text(waitlbl,"gesùùùùù");

	gtk_main();



	
	//gtk_widget_show(window);	
//	gtk_widget_hide(waitwin);	

   return 0;
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
		strcpy(msg,"DICE%ImWaiting%");
		strcat(msg,IDpartita);			
	}

	if(type=='F')
	{
		strcpy(msg,"DICE%getFiches%");
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

	if(strcmp(buffer,"DICE%ERR")==0)
	{	
		close(sockfd);
		return 1;
	}

	if(strcmp(buffer,"DICE%ERR")!=0)
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

void stopIMGkey(GtkWidget *widget, GdkEventKey *event)
{
	if(!strcmp(gdk_keyval_name (event->keyval),"space")) stopIMG(); 
}
*/
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
