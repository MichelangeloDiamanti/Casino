#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <mysql.h>

void exec(char* cmd);
void analSocket(int); 
void queryERR();
void updateLog(char* logstr);
char* getNamefromSession(char* S);
void updatePartite();

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
char *server = "localhost";
char *user = "root";
char *password = "lasvegas";
char *database = "Casino";
char cmdOUT[128], cmdIN[128], logname[50], gNfS[50];

/*
	+-------------------------------------------------------------------------------------------+
	|                                    MAIN - GESTIONE SOCKET                                 |
	+-------------------------------------------------------------------------------------------+
*/
main()
{
	system("tput reset");
	system("hostname -I | sed 's/.$//' > serverIP.txt");
	system("chmod 777 serverIP.txt");
		
	exec("date +'%Y-%m-%d_%T'");	
	cmdOUT[strlen(cmdOUT)-1]='\0';
	strcpy(logname,"./logs/ServerCasinoLog_");
	strcat(logname,cmdOUT);
	strcat(logname,".log");
	
	strcpy(cmdIN,"echo \"SERVER CASINO\n\" > ");
	strcat(cmdIN,logname);
	system(cmdIN);

	system("echo \"\e[1mSERVER CASINÒ\n\e[0m\"");
   


	/* Connect to database */
	conn = mysql_init(NULL); 
   if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) queryERR();

	updatePartite();

	int sockfd, newsockfd, portno, pid;
   socklen_t clilen;
   struct sockaddr_in serv_addr, cli_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		system("echo \"\e[38;5;160m-ERRO%OPENING_SOCKET\e[0m\"");
		updateLog("ERRO%OPENING_SOCKET");
	}	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = 54321;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		system("echo \"\e[38;5;160m-ERRO%BINDING\e[0m\"");
		updateLog("ERRO%BINDING");
	}
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	while (1) 
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
		{
			system("echo \"\e[38;5;160m-ERRO%ACCEPT\e[0m\"");
			updateLog("ERRO%ACCEPT");
		}
		pid = fork();
		if (pid < 0)
		{
			system("echo \"\e[38;5;160m-ERRO%FORK\e[0m\"");
			updateLog("ERRO%FORK");
		}
		if (pid == 0)
		{
			close(sockfd);
			analSocket(newsockfd);
			exit(0);
		}
		else close(newsockfd);
	} //fine while
	close(sockfd);

   /* close connection */
   mysql_close(conn);

	return 0;
}

/*
	+-------------------------------------------------------------------------------------------+
	|                      SOCKET ANALYZER - SMISTA LE DIVERSE RICHIESTE                        |
	+-------------------------------------------------------------------------------------------+
*/
void analSocket(int sock)
{
   int n,i;
   char buffer[256];
   char reqType[5];

   bzero(buffer,256);
   n = read(sock,buffer,255);
   if (n < 0) 
	{
		system("echo \"\e[38;5;160m-ERRO%READING_SOCKET\e[0m\"");
		updateLog("ERRO%READING_SOCKET");
	}
	system("echo -n \"\e[38;5;154m\"");   
	printf("-%s\n",buffer);
	system("echo -n \"\e[0m\"");
	updateLog(buffer);

	for(i=0;i<4;i++) reqType[i]=buffer[i];
	reqType[4]='\0';

/*
	+-------------------------------------------------------------------------------------------+
	|                                             RGST                                          |
	+-------------------------------------------------------------------------------------------+
//RGST%flagCreateDelete(0=reg,1=del)%username%password                           
*/

	if(!strcmp(reqType,"RGST"))
	{
		char dc, usr[256], pwd[256]; 
		short int count=0;
		char *token;
		const char delimeter[2]="%";
		char query[500], lquery[500];

		token = strtok(buffer, delimeter);
		while( token != NULL ) 
   	{
			if(count==1) dc=token[0];
			if(count==2) strcpy(usr,token);
			if(count==3) strcpy(pwd,token);
			count++;
			token = strtok(NULL, delimeter);
   	}

		if(dc=='0') //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - REGISTRA UTENTE
		{
			
			strcpy(query,"SELECT username FROM Giocatore WHERE username=\"");
			strcat(query,usr);
			strcat(query,"\"");
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);

			if (mysql_query(conn, query)) queryERR();
			res = mysql_use_result(conn);
			if((row = mysql_fetch_row(res)) == NULL) //lo username non è stato ancora usato
			{
				strcpy(query,"INSERT INTO Giocatore(username,password,fiches) VALUES(\"");
				strcat(query,usr);
				strcat(query,"\",\"");
				strcat(query,pwd); 
				strcat(query,"\",\"5000\")");
				strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
				strcat(cmdIN,query);
				strcat(cmdIN, "\e[0m\"");
				system(cmdIN);
				strcpy(lquery, "QERY%");
				strcat(lquery, query);
				updateLog(lquery);

				if (mysql_query(conn, query)) queryERR();
				mysql_use_result(conn);
				n = write(sock,"RGST%OK",7);
				if (n < 0)
				{
					system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
					updateLog("ERRO%WRITING_SOCKET");
				}
				if (n >= 0)	
				{
					system("echo \"\e[38;5;39m-RGST%OK\e[0m\"");
					updateLog("RGST%OK");
				}
			}
			else //username già in uso
			{
				n = write(sock,"RGST%ERR",8);
				if (n < 0)
				{
					system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
					updateLog("ERRO%WRITING_SOCKET");
				}
				if (n >= 0)
				{
					system("echo \"\e[38;5;39m-RGST%ERR\e[0m\"");
					updateLog("RGST%ERR");
				}
			}
			mysql_free_result(res);
			
		} 

	/*	if(dc=='1')  //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - ELIMINA UTENTE
		{
			strcpy(query,"DELETE FROM Giocatore WHERE username=\"");
			strcat(query,usr);
			strcat(query,"\"");
			system("echo -n \"\e[38;5;99m\"");
			printf("-QERY%c%s\n",37,query);
			system("echo -n \"\e[0m\"");
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);
		}		
    */
	}
/*
	+-------------------------------------------------------------------------------------------+
	|                                             LOGI                                          |
	+-------------------------------------------------------------------------------------------+
//LOGI%Keep(0)%username%password%IP%hostname        
//LOGI%Keep(1)%sessionkey        
                   
*/

	if(!strcmp(reqType,"LOGI"))
	{
		char keep, usr[256], pwd[256], hostinfo[75], skey[128]; 
		short int count=0;
		char *token;
		const char delimeter[2]="%";
		char query[500], lquery[500];

		token = strtok(buffer, delimeter);
		while( token != NULL ) 
   	{
			if(count==1) keep=token[0];
			if(count==2 && keep=='0') strcpy(usr,token);
			if(count==2 && keep=='1') strcpy(skey,token);		
			if(count==3 && keep=='0') strcpy(pwd,token);
			if(count==4 && keep=='0') strcpy(hostinfo,token);
			if(count==5 && keep=='0')
			{
				strcat(hostinfo, "\",\"");
				strcat(hostinfo, token);
				strcat(hostinfo, "\")");			
			}
			count++;
			token = strtok(NULL, delimeter);
   	}
		
		if(keep=='0')
		{
			strcpy(query,"SELECT username FROM Giocatore WHERE username=\"");
			strcat(query,usr);
			strcat(query,"\" AND password=\"");
			strcat(query,pwd);
			strcat(query,"\"");
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);

			if (mysql_query(conn, query)) queryERR();
			res = mysql_use_result(conn);
			if((row = mysql_fetch_row(res)) == NULL) //login errato
			{
				n = write(sock,"LOGI%ERR",8);
				if (n < 0)
				{
					system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
					updateLog("ERRO%WRITING_SOCKET");
				}
				if (n >= 0)
				{
					system("echo \"\e[38;5;39m-LOGI%ERR\e[0m\"");
		         updateLog("LOGI%ERR");
				}
				mysql_free_result(res);
			}
			else
			{
				n = write(sock,"LOGI%OK",7);
				if (n < 0) 
				{
					system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
					updateLog("ERRO%WRITING_SOCKET");
				}
				if (n >= 0)	
				{
					system("echo \"\e[38;5;39m-LOGI%OK\e[0m\"");	
					updateLog("LOGI%OK");
				
					char ckey[275], key[250];
					exec("openssl rand -base64 48"); 
					strcpy(ckey, "CKEY%");
					strcat(ckey, cmdOUT);
					strcpy(key,cmdOUT);
					key[strlen(key)-1]='\0';
					ckey[strlen(ckey)-1]='\0';
					strcpy(cmdIN, "echo \"\e[38;5;39m-");
					strcat(cmdIN, ckey);
					strcat(cmdIN, "\e[0m\"");
					system(cmdIN);	
	  		      updateLog(ckey);
				
					n = write(sock,ckey,sizeof(ckey));	
					if (n < 0) 
					{
						system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
						updateLog("ERRO%WRITING_SOCKET");
					}

					
					char IDusr[4];
					mysql_free_result(res);
					strcpy(query,"SELECT id FROM Giocatore WHERE username=\"");
					strcat(query,usr);
					strcat(query,"\"");
					strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
					strcat(cmdIN,query);
					strcat(cmdIN, "\e[0m\"");
					system(cmdIN);
					strcpy(lquery, "QERY%");
					strcat(lquery, query);
					updateLog(lquery);

					if (mysql_query(conn, query)) queryERR();
					res = mysql_use_result(conn);
					if((row = mysql_fetch_row(res)) != NULL) strcpy(IDusr,row[0]);

				
					mysql_free_result(res);
					strcpy(query,"INSERT INTO Sessione(id_giocatore,chiave,ora_login,ip_macchina,hostname) VALUES(\"");
					strcat(query,IDusr);
					strcat(query,"\",\"");
					strcat(query,key); 
					strcat(query,"\",\"");
					exec("date +'%Y-%m-%d %T'");
					cmdOUT[strlen(cmdOUT)-1]='\0';
					strcat(query,cmdOUT); //ora_login 	
					strcat(query,"\",\"");
					strcat(query, hostinfo);
					 
					strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
					strcat(cmdIN,query);
					strcat(cmdIN, "\e[0m\"");
					system(cmdIN);
					strcpy(lquery, "QERY%");
					strcat(lquery, query);
					updateLog(lquery);
		
					if (mysql_query(conn, query)) queryERR();
					mysql_use_result(conn); 
					
				}
			}
		}

		if(keep=='1')
		{
			char IDusr[4];
			strcpy(query,"SELECT id_giocatore FROM Sessione WHERE chiave=\"");
			strcat(query,skey);
			strcat(query,"\"");
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);

			if (mysql_query(conn, query)) queryERR();
			res = mysql_use_result(conn);
			if((row = mysql_fetch_row(res)) != NULL) strcpy(IDusr,row[0]);
			mysql_free_result(res);

			n = write(sock,"LOGI%OK",7);
			if (n < 0) 
			{
				system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
				updateLog("ERRO%WRITING_SOCKET");
			}
			if (n >= 0)	
			{
				system("echo \"\e[38;5;39m-LOGI%OK\e[0m\"");	
				updateLog("LOGI%OK");
			}


		}


	}

/*
	+-------------------------------------------------------------------------------------------+
	|                                             LOGO                                          |
	+-------------------------------------------------------------------------------------------+
//TBLS%tables
//TBLS%header%key               
*/

	if(!strcmp(reqType,"LOGO"))
	{

		char query[500], lquery[500];
		
		strcpy(cmdIN, "echo \"");
		strcat(cmdIN, buffer);
		strcat(cmdIN, "\" | cut -c6- ");
		exec(cmdIN);
		cmdOUT[strlen(cmdOUT)-1]='\0';

		strcpy(query,"DELETE FROM Sessione WHERE chiave=\"");
		strcat(query,cmdOUT);
		strcat(query,"\"");
		strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
		strcat(cmdIN,query);
		strcat(cmdIN, "\e[0m\"");
		system(cmdIN);
		strcpy(lquery, "QERY%");
		strcat(lquery, query);
		updateLog(lquery);

		if (mysql_query(conn, query)) queryERR();
		mysql_use_result(conn);

		strcpy(buffer,"LOGO%OK");
		n = write(sock,buffer,strlen(buffer));
		if (n < 0)
		{
			system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
			updateLog("ERRO%WRITING_SOCKET");
		}
		if (n >= 0)	
		{
			system("echo -n \"\e[38;5;39m-LOGO%OK\e[0m\"");
			updateLog(buffer);
		}

	}


/*
	+-------------------------------------------------------------------------------------------+
	|                                             TBLS                                          |
	+-------------------------------------------------------------------------------------------+
//TBLS%tables
//TBLS%header%skey       
//TBLS%games%gamename             
*/

	if(!strcmp(reqType,"TBLS"))
	{

		short int count=0;
		char *token;
		const char delimeter[2]="%";
		char type[25], skey[125], game[15];
		char query[500], lquery[500];
		
		token = strtok(buffer, delimeter);
		while( token != NULL ) 
   	{
			if(count==1) strcpy(type,token);
			if(count==2)
			{ 
				if(!strcmp(type,"header")) strcpy(skey,token);
				if(!strcmp(type,"games") || !strcmp(type,"start")) strcpy(game,token);
			}
			if(count==3) strcpy(skey,token);

			count++;
			token = strtok(NULL, delimeter);
   	}

		if(!strcmp(type,"tables")) //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - RESTITUISCE TABS PER CLIENT
		{
			char buffer[255];
	
	
			strcpy(buffer, "TBLS%");

			if (mysql_query(conn, "SELECT COUNT(1) FROM Gioco")) queryERR();
			res = mysql_use_result(conn);		
			system("echo \"\e[38;5;99m-SELECT COUNT(1) FROM Gioco\e[0m\"");
			updateLog("SELECT COUNT(1) FROM Gioco");
			if((row = mysql_fetch_row(res)) != NULL) strcat(buffer,row[0]);
	
			mysql_free_result(res);
			if (mysql_query(conn, "SELECT nome_gioco FROM Gioco")) queryERR();
			res = mysql_use_result(conn);
			system("echo \"\e[38;5;99m-SELECT nome_gioco FROM Gioco\e[0m\"");
			updateLog("SELECT nome_gioco FROM Gioco");
			while ((row = mysql_fetch_row(res)) != NULL)
			{
				strcat(buffer,"%");
				strcat(buffer,row[0]);
			}
			mysql_free_result(res);
			
			n = write(sock,buffer,strlen(buffer));
			if (n < 0)
			{
				system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
				updateLog("ERRO%WRITING_SOCKET");
			}
			if (n >= 0)	
			{
				system("echo -n \"\e[38;5;39m-\"");
				strcpy(cmdIN, "echo -n ");							
				strcat(cmdIN, buffer);
				system(cmdIN);
				system("echo \"\e[0m\"");
				updateLog(buffer);
			}
		}

		if(!strcmp(type,"header")) //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - RESTITUISCE INFO HEADER PER CLIENT
		{
			char HNF[75];
			
			strcpy(HNF,getNamefromSession(skey));	
			
			strcpy(query,"SELECT fiches FROM Giocatore WHERE username=\"");
			strcat(query,HNF);
			strcat(query,"\"");
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);
			if (mysql_query(conn, query)) queryERR();
			strcat(HNF,"  -  ");
			res = mysql_use_result(conn);
			if((row = mysql_fetch_row(res)) != NULL) strcat(HNF,row[0]);
			mysql_free_result(res);

			strcpy(buffer, "TBLS%");
			strcat(buffer,HNF);
			n = write(sock,buffer,strlen(buffer));
			if (n < 0)
			{
				system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
				updateLog("ERRO%WRITING_SOCKET");
			}
			if (n >= 0)	
			{
				strcpy(cmdIN, "echo \"\e[38;5;39m-");
				strcat(cmdIN, buffer);
				strcat(cmdIN, "\e[0m\"");
				system(cmdIN);
				updateLog(buffer);
			}

		}
		if(!strcmp(type,"games")) //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - RESTITUISCE PARTITE RELATIVE AL GIOCO
		{
			strcpy(buffer, "TBLS");
			strcpy(query,"SELECT PA.id, PA.num_giocatori, count(GP.giocatore_id) FROM Partita AS PA LEFT JOIN giocatori_partite AS GP ON PA.id = GP.partita_id WHERE PA.id_gioco=(SELECT id FROM Gioco WHERE nome_gioco=\"");
			strcat(query,game);
			strcat(query,"\") GROUP BY PA.id");
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);
			if (mysql_query(conn, query)) queryERR();
			res = mysql_use_result(conn);
			int maxg, numg;
			while (row = mysql_fetch_row(res))
			{
				maxg=atoi(row[1]);
				numg=atoi(row[2]);
				if(numg<maxg)
				{
					strcat(buffer,"%Tavolo ");
					strcat(buffer,row[0]); //id partita
					strcat(buffer," - Giocatori: ");
					strcat(buffer,row[2]); //n giocatori
					strcat(buffer,"/");
					strcat(buffer,row[1]); //giocatori presenti
				}
			}
			mysql_free_result(res);
			
			n = write(sock,buffer,strlen(buffer));
			if (n < 0)
			{
				system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
				updateLog("ERRO%WRITING_SOCKET");
			}
			if (n >= 0)	
			{
				strcpy(cmdIN, "echo \"\e[38;5;39m-");							
				strcat(cmdIN, buffer);
				strcat(cmdIN, "\e[0m\"");
				system(cmdIN);
				updateLog(buffer);
			}

		}

		if(!strcmp(type,"start")) //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - RESTITUISCE NOME GIOCO DA ID PARTITA
		{
			int maxg, numg;
			strcpy(query,"SELECT PA.num_giocatori, COUNT(GP.giocatore_id) FROM Partita AS PA LEFT JOIN giocatori_partite AS GP ON PA.id=GP.partita_id  WHERE PA.id=");
			strcat(query,game);
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);
			if (mysql_query(conn, query)) queryERR();
			res = mysql_use_result(conn);
			if((row = mysql_fetch_row(res)) != NULL)
			{
				maxg=atoi(row[0]);
				numg=atoi(row[1]);
			}
			mysql_free_result(res);

			if(numg<maxg)
			{

				strcpy(query,"SELECT * FROM giocatori_partite WHERE partita_id=");
				strcat(query,game);
				strcat(query," AND giocatore_id=(SELECT id_giocatore FROM Sessione WHERE chiave=\"");
				strcat(query,skey);
				strcat(query,"\")");
				strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
				strcat(cmdIN,query);
				strcat(cmdIN, "\e[0m\"");
				system(cmdIN);
				strcpy(lquery, "QERY%");
				strcat(lquery, query);
				updateLog(lquery);
				if (mysql_query(conn, query)) queryERR();
				res = mysql_use_result(conn);

				if((row = mysql_fetch_row(res)) != NULL)
				{
					mysql_free_result(res);
					strcpy(buffer,"TBLS%ALREADY");
					n = write(sock,buffer,strlen(buffer));
					if (n < 0)
					{
						system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
						updateLog("ERRO%WRITING_SOCKET");
					}
					if (n >= 0)	
					{
						strcpy(cmdIN, "echo \"\e[38;5;39m-");							
						strcat(cmdIN, buffer);
						strcat(cmdIN, "\e[0m\"");
						system(cmdIN);
						updateLog(buffer);
					}
					
				}
				else
				{
					mysql_free_result(res);
					strcpy(query,"INSERT INTO giocatori_partite(giocatore_id,partita_id) VALUES((SELECT id_giocatore FROM Sessione WHERE chiave=\"");
					strcat(query,skey);
					strcat(query,"\"),");
					strcat(query,game);
					strcat(query,")");
					strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
					strcat(cmdIN,query);
					strcat(cmdIN, "\e[0m\"");
					system(cmdIN);
					strcpy(lquery, "QERY%");
					strcat(lquery, query);
					updateLog(lquery);
					if (mysql_query(conn, query)) queryERR();
					mysql_use_result(conn);
				
					if(numg==maxg-1)
					{
					
						strcpy(query,"UPDATE Partita SET data_inizio=\"");
						exec("date +'%Y-%m-%d %T'");
						cmdOUT[strlen(cmdOUT)-1]='\0';
						strcat(query,cmdOUT);
						strcat(query,"\" WHERE id=");
						strcat(query,game);
						strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
						strcat(cmdIN,query);
						strcat(cmdIN, "\e[0m\"");
						system(cmdIN);
						strcpy(lquery, "QERY%");
						strcat(lquery, query);
						updateLog(lquery);
						if (mysql_query(conn, query)) queryERR();
						mysql_use_result(conn);

						updatePartite();
					}


					strcpy(query,"SELECT nome_gioco FROM Gioco WHERE id=(SELECT id_gioco FROM Partita WHERE id=");
					strcat(query,game);
					strcat(query,")");
					strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
					strcat(cmdIN,query);
					strcat(cmdIN, "\e[0m\"");
					system(cmdIN);
					strcpy(lquery, "QERY%");
					strcat(lquery, query);
					updateLog(lquery);
					if (mysql_query(conn, query)) queryERR();
					res = mysql_use_result(conn);
					strcpy(buffer, "TBLS%");			
					if((row = mysql_fetch_row(res)) != NULL) strcat(buffer,row[0]);
					mysql_free_result(res);
			
					n = write(sock,buffer,strlen(buffer));
					if (n < 0)
					{
						system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
						updateLog("ERRO%WRITING_SOCKET");
					}
					if (n >= 0)	
					{
						strcpy(cmdIN, "echo \"\e[38;5;39m-");							
						strcat(cmdIN, buffer);
						strcat(cmdIN, "\e[0m\"");
						system(cmdIN);
						updateLog(buffer);
					}
				}

			}
			else
			{
				strcpy(buffer,"TBLS%FULL");
				n = write(sock,buffer,strlen(buffer));
				if (n < 0)
				{
					system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
					updateLog("ERRO%WRITING_SOCKET");
				}
				if (n >= 0)	
				{
					strcpy(cmdIN, "echo \"\e[38;5;39m-");							
					strcat(cmdIN, buffer);
					strcat(cmdIN, "\e[0m\"");
					system(cmdIN);
					updateLog(buffer);
				}
			}



		}



	}

/*
	+-------------------------------------------------------------------------------------------+
	|                                             SLOT                                          |
	+-------------------------------------------------------------------------------------------+
//SLOT%getFiches%skey
//TBLS%updateFiches%skey%valuetoadd        
*/
	if(!strcmp(reqType,"SLOT"))
	{
		short int count=0;
		char *token;
		const char delimeter[2]="%";
		char type[25], skey[125], other[15];
		char query[500], lquery[500];
		
		token = strtok(buffer, delimeter);
		while( token != NULL ) 
   	{
			if(count==1) strcpy(type,token);
			if(count==2) strcpy(skey,token);
			if(count==3) strcpy(other,token);
			count++;
			token = strtok(NULL, delimeter);
   	}

		if(!strcmp(type,"updateFiches")) //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - RESTITUISCE TABS PER CLIENT
		{
			strcpy(query,"UPDATE Giocatore SET fiches = fiches + (");
			strcat(query,other);
			strcat(query,") WHERE Giocatore.id=(SELECT id_giocatore FROM Sessione WHERE chiave=\"");
			strcat(query,skey);
			strcat(query,"\")");
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);
			if (mysql_query(conn, query)) queryERR();
			mysql_use_result(conn);
			strcpy(type,"getFiches");
		}

		if(!strcmp(type,"getFiches")) //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - RESTITUISCE TABS PER CLIENT
		{
			strcpy(buffer,"SLOT%");
			strcpy(query,"SELECT fiches FROM Giocatore WHERE id=(SELECT id_giocatore FROM Sessione WHERE chiave=\"");
			strcat(query,skey);
			strcat(query,"\")");
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);
			if (mysql_query(conn, query)) queryERR();
			res = mysql_use_result(conn);
			if((row = mysql_fetch_row(res)) != NULL) strcat(buffer,row[0]);
		}
		n = write(sock,buffer,strlen(buffer));
		if (n < 0)
		{
			system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
			updateLog("ERRO%WRITING_SOCKET");
		}
		if (n >= 0)	
		{
			strcpy(cmdIN,"echo \"\e[38;5;39m-");
			strcat(cmdIN,buffer);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			updateLog(buffer);
		}


	}

/*
	+-------------------------------------------------------------------------------------------+
	|                                            DICES                                          |
	+-------------------------------------------------------------------------------------------+
//DICE%getFiches%skey

*/
	if(!strcmp(reqType,"DICE"))
	{
		short int count=0;
		char *token;
		const char delimeter[2]="%";
		char type[25], skey[125], other[15];
		char query[500], lquery[500];
		
		token = strtok(buffer, delimeter);
		while( token != NULL ) 
   	{
			if(count==1) strcpy(type,token);
			if(count==2) strcpy(skey,token); //if type==ImWaiting then skey==idpartita
			if(count==3) strcpy(other,token);
			count++;
			token = strtok(NULL, delimeter);
   	}

		if(!strcmp(type,"ImWaiting")) //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - RESTITUISCE TABS PER CLIENT
		{
			int maxg, numg;
			do
			{
				strcpy(query,"SELECT PA.num_giocatori, COUNT(GP.giocatore_id) FROM Partita AS PA LEFT JOIN giocatori_partite AS GP ON PA.id=GP.partita_id  WHERE PA.id=");
				strcat(query,skey);
				strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
				strcat(cmdIN,query);
				strcat(cmdIN, "\e[0m\"");
				system(cmdIN);
				strcpy(lquery, "QERY%");
				strcat(lquery, query);
				updateLog(lquery);
				if (mysql_query(conn, query)) queryERR();
				res = mysql_use_result(conn);
				if((row = mysql_fetch_row(res)) != NULL)
				{
					maxg=atoi(row[0]);
					numg=atoi(row[1]);
				}
				mysql_free_result(res);
				sleep(1);
			}while(numg<maxg);
			strcpy(buffer,"DICE%READY");

		}

/*
		if(!strcmp(type,"updateFiches")) //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - RESTITUISCE TABS PER CLIENT
		{
			strcpy(query,"UPDATE Giocatore SET fiches = fiches + (");
			strcat(query,other);
			strcat(query,") WHERE Giocatore.id=(SELECT id_giocatore FROM Sessione WHERE chiave=\"");
			strcat(query,skey);
			strcat(query,"\")");
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);
			if (mysql_query(conn, query)) queryERR();
			mysql_use_result(conn);
			strcpy(type,"getFiches");
		}
*/
		if(!strcmp(type,"getFiches")) //(╯°□°)︻╦̵̵͇̿̿̿̿══╤─ - - - RESTITUISCE TABS PER CLIENT
		{
			strcpy(buffer,"DICE%");
			strcpy(query,"SELECT fiches FROM Giocatore WHERE id=(SELECT id_giocatore FROM Sessione WHERE chiave=\"");
			strcat(query,skey);
			strcat(query,"\")");
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			strcpy(lquery, "QERY%");
			strcat(lquery, query);
			updateLog(lquery);
			if (mysql_query(conn, query)) queryERR();
			res = mysql_use_result(conn);
			if((row = mysql_fetch_row(res)) != NULL) strcat(buffer,row[0]);
		}
	

		n = write(sock,buffer,strlen(buffer));
		if (n < 0)
		{
			system("echo \"\e[38;5;160m-ERRO%WRITING_SOCKET\e[0m\"");
			updateLog("ERRO%WRITING_SOCKET");
		}
		if (n >= 0)	
		{
			strcpy(cmdIN,"echo \"\e[38;5;39m-");
			strcat(cmdIN,buffer);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			updateLog(buffer);
		}


		


	}


}

/*
	+-------------------------------------------------------------------------------------------+
	|                                      GET NAME FROM SESSION                                |
	+-------------------------------------------------------------------------------------------+
*/
char* getNamefromSession(char* S)
{
	char usrname[50], query[500], lquery[500];

	strcpy(query,"SELECT username FROM Giocatore WHERE id=(SELECT id_giocatore FROM Sessione WHERE chiave=\"");
	strcat(query,S);
	strcat(query,"\")");
	strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
	strcat(cmdIN,query);
	strcat(cmdIN, "\e[0m\"");
	system(cmdIN);
	strcpy(lquery, "QERY%");
	strcat(lquery, query);
	updateLog(lquery);
	if (mysql_query(conn, query)) queryERR();
	res = mysql_use_result(conn);
	if((row = mysql_fetch_row(res)) != NULL) strcpy(usrname,row[0]);
	mysql_free_result(res);
	strcpy(gNfS,usrname);
}


/*
	+-------------------------------------------------------------------------------------------+
	|                                        UPDATE PARTITE                                     |
	+-------------------------------------------------------------------------------------------+
*/
void updatePartite()
{
	int g=0,p=0, gid=0, r=0;
	char Par[5], query[500], lquery[500], gamesID[25][5];
	system("echo \"\e[38;5;99m-QERY%SELECT id FROM Gioco\e[0m\"");
	updateLog("QERY%SELECT id FROM Gioco");
	if (mysql_query(conn, "SELECT id FROM Gioco")) queryERR();
	res = mysql_use_result(conn);		
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		strcpy(gamesID[gid],row[0]);
		gid++;
	}
	mysql_free_result(res);
	
	for(r=0;r<gid;r++)
	{
		strcpy(query,"SELECT COUNT(1) FROM Partita WHERE id_gioco=");
		strcat(query,gamesID[r]);
 		strcat(query," AND data_inizio IS NULL");
		strcpy(lquery,"QERY%");
		strcat(lquery,query);
		strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
		strcat(cmdIN,query);
		strcat(cmdIN, "\e[0m\"");
		system(cmdIN);
		updateLog(lquery);
		
		if (mysql_query(conn, query)) queryERR();
		res = mysql_use_result(conn);		
		if((row = mysql_fetch_row(res)) != NULL) strcpy(Par,row[0]);
		mysql_free_result(res);
		p=atoi(Par);
		
		for(g=0;g<(6-p);g++)
		{
			strcpy(query,"INSERT INTO Partita(id_gioco,num_giocatori) VALUES(");
			strcat(query,gamesID[r]);
			strcat(query,",");
			exec("shuf -i 2-7 -n 1");
			cmdOUT[strlen(cmdOUT)-1]='\0';
			if(gamesID[r][0]=='5') strcpy(cmdOUT,"1");
			strcat(query,cmdOUT);
	 		strcat(query,")");
			strcpy(lquery,"QERY%");
			strcat(lquery,query);
			strcpy(cmdIN,"echo \"\e[38;5;99m-QERY%");
			strcat(cmdIN,query);
			strcat(cmdIN, "\e[0m\"");
			system(cmdIN);
			updateLog(lquery);
							
			if (mysql_query(conn, query)) queryERR();	
			mysql_use_result(conn);
					
		}
	}
}

/*
	+-------------------------------------------------------------------------------------------+
	|                                           QUERY ERROR                                     |
	+-------------------------------------------------------------------------------------------+
*/
void queryERR()
{

	system("echo -n \"\e[38;5;160m-ERRO%QUERY%\"");
	fprintf(stderr, "%s\n", mysql_error(conn));
	system("echo \"\e[0m\"");	
	updateLog("ERRO%QUERY%");
	exit(1);
}

/*
	+-------------------------------------------------------------------------------------------+
	|                                           LOG UPDATER                                     |
	+-------------------------------------------------------------------------------------------+
*/
void updateLog(char* logstr)
{
	strcpy(cmdIN,"echo \"-");
	strcat(cmdIN,logstr);
	strcat(cmdIN,"\" >> ");
	strcat(cmdIN,logname);
	system(cmdIN);
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


//gcc -o ServerCasino $(mysql_config --cflags) ServerCasino.c $(mysql_config --libs) -w
