#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT_N 2222
#define BUFFER_SIZE 1024

//--------------------GLOBAL GAME VARIABLES-------------------
int usedcard = 0;
char currentcard[3];
char lastcard[3];

void error(char* msg)
{
    fprintf(stderr, msg);
}

int id_to_number(char* cardid)
{
    if (!strcmp(cardid, "P7")) return 0;    //Piros 7
    if (!strcmp(cardid, "P8")) return 1;    //Piros 8
    if (!strcmp(cardid, "P9")) return 2;    //Piros 9
    if (!strcmp(cardid, "P0")) return 3;    //Piros 10
    if (!strcmp(cardid, "PA")) return 4;    //Piros alsó
    if (!strcmp(cardid, "PF")) return 5;    //Piros felső
    if (!strcmp(cardid, "PK")) return 6;    //Piros király
    if (!strcmp(cardid, "PS")) return 7;    //Piros ász
    if (!strcmp(cardid, "M7")) return 8;    //Makk 7
    if (!strcmp(cardid, "M8")) return 9;
    if (!strcmp(cardid, "M9")) return 10;
    if (!strcmp(cardid, "M0")) return 11;
    if (!strcmp(cardid, "MA")) return 12;
    if (!strcmp(cardid, "MF")) return 13;
    if (!strcmp(cardid, "MK")) return 14;
    if (!strcmp(cardid, "MS")) return 15;
    if (!strcmp(cardid, "T7")) return 16;    //Tök 7
    if (!strcmp(cardid, "T8")) return 17;
    if (!strcmp(cardid, "T9")) return 18;
    if (!strcmp(cardid, "T0")) return 19;
    if (!strcmp(cardid, "TA")) return 20;
    if (!strcmp(cardid, "TF")) return 21;
    if (!strcmp(cardid, "TK")) return 22;
    if (!strcmp(cardid, "TS")) return 23;
    if (!strcmp(cardid, "Z7")) return 24;    //Zöld 7
    if (!strcmp(cardid, "Z8")) return 25;
    if (!strcmp(cardid, "Z9")) return 26;
    if (!strcmp(cardid, "Z0")) return 27;
    if (!strcmp(cardid, "ZA")) return 28;
    if (!strcmp(cardid, "ZF")) return 29;
    if (!strcmp(cardid, "ZK")) return 30;
    if (!strcmp(cardid, "ZS")) return 31;
    
    return -1;
}

char* number_to_id(int number)
{
    switch (number)
    {
        case 0:  return "P7";    //Piros 7
        case 1:  return "P8";    //Piros 8
        case 2:  return "P9";    //Piros 9
        case 3:  return "P0";    //Piros 10
        case 4:  return "PA";    //Piros alsó
        case 5:  return "PF";    //Piros felső
        case 6:  return "PK";    //Piros király
        case 7:  return "PS";    //Piros ász
        case 8:  return "M7";    //Makk 7
        case 9:  return "M8";    //....
        case 10: return "M9";
        case 11: return "M0";
        case 12: return "MA";
        case 13: return "MF";
        case 14: return "MK";
        case 15: return "MS";   //Makk ász
        case 16: return "T7";   //Tök 7
        case 17: return "T8";   //....
        case 18: return "T9";
        case 19: return "T0";
        case 20: return "TA";
        case 21: return "TF";
        case 22: return "TK";
        case 23: return "TS";   //Tök ász
        case 24: return "Z7";   //Zöld 7
        case 25: return "Z8";   //....
        case 26: return "Z9";
        case 27: return "Z0";
        case 28: return "ZA";
        case 29: return "ZF";
        case 30: return "ZK";
        case 31: return "ZS";  //Zöld ász

        default: return "error";
    }
}

char** getargument(char* string, int size)
{
    char* token;
    char* tmp = strdup(string);

    tmp[strcspn(tmp, "\n")] = 0; //Remove \n
    int counter = 0;
    char** splitted = malloc(size*sizeof(char *));
    if (string != NULL)
    {
	while ((token = strsep(&tmp, " ")) != NULL)
	{
	    splitted[counter] = token;
	    counter++;	    
	}
    }
    free(tmp);
    return splitted;
}

int getrandomcard(int* deck, int* inhand, int decksize)
{
    if (usedcard == decksize)
    {
	int i;
	for (i=0; i<decksize; i++)
	{
	    if (inhand[i] != 1 && id_to_number(currentcard) != i)
	    {
		deck[i] = 1;
	    }
	}
	usedcard = 0;
    }
    int cardnumber = rand()%decksize;
    while (deck[cardnumber] != 1)
    {
	cardnumber++;
	if (cardnumber > decksize-1)
	    cardnumber = 0;
    }
    deck[cardnumber] = 0;
    usedcard++;
    return cardnumber;
}

void draw(int client_socket, int* deck, int* inhand, int decksize, char* buf)
{
    char** arguments = getargument(buf, 2);
    int drawcount = atoi(arguments[1]);
    free(arguments);
    char randomcards[1024];
    strcpy(randomcards, "");
    int j;
    for (j=0; j<drawcount; j++)
    {
	int randomcard = getrandomcard(deck ,inhand, decksize);
	inhand[randomcard] = 1;
	strcat(randomcards, number_to_id(randomcard));
	if (j != drawcount-1) 
	    strcat(randomcards, ",");
    }
    sprintf(buf, "DRAW %s\n", randomcards);
    send(client_socket, buf, strlen(buf), 0);
}

int main(int argc, char* argv[])
{
//--------------------SERVER VARIABLES--------------------
    int server_socket;
    int client_socket[4];
    int maxclient;
    int connectedclient = 0;
    int bytes = BUFFER_SIZE;
    int maxsocket, newsocket, recvsize, i, j;
    fd_set sockets;
    struct sockaddr_in server, client;
    unsigned int server_size, client_size;
    char opt = 1;
    char buffer[BUFFER_SIZE];
    char message[1024];
//--------------------GAME VARIABLES--------------------
    int deck[32];
    int decksize = 32;
    int inhand[32];
    int surr[4] = {1, 1, 1, 1};
    int surrcount = 0;
    int currentplayer = 0;
    int draw7 = 0;
    int request = 0;
    char requested[3];
    int start = 1;
    int end = 0;

//--------------------GAME INIT--------------------
    srand(time(NULL));
    for (i = 0; i < 32; i++)
    {
        deck[i] = 1;
        inhand[i] = 0;
    }

//--------------------CONNECTION INIT--------------------
    char* tmp;
    maxclient = strtol(argv[1], &tmp, 10);

    if (maxclient < 2 || maxclient > 4)
    {
        error("Wrong argument (must be between 2 and 4)\n");
        exit(1);
    }

    for (i = 0; i < maxclient; i++)
    {
        client_socket[i] = 0;
    }

//--------------------CONNECTION--------------------
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_N);

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof opt);
    setsockopt(server_socket, SOL_SOCKET, SO_KEEPALIVE, (char*) &opt, sizeof opt);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        error("Socket creation error\n");
        exit(1);
    }

    if (bind(server_socket, (struct sockaddr*) &server, sizeof(server)) < 0)
    {
        error("Cannot bind to the socket\n");
        exit(1);
    }

    if (listen(server_socket, 10) < 0)
    {
        error("Cannot listen to the socket\n");
        exit(1);
    }
    
    printf("Várakozás a játékosokra...\n");
    
    while (!end)
    {
	FD_ZERO(&sockets);
	FD_SET(server_socket, &sockets);
	maxsocket = server_socket;
      
	for (i = 0; i < maxclient; i++)
        {
	    if (client_socket[i] > 0)
	    {
		FD_SET(client_socket[i], &sockets);
	    }
	  
            if (client_socket[i] > maxsocket)
                maxsocket = client_socket[i];
        }

	
	if (select(maxsocket + 1, &sockets, NULL, NULL, NULL) < 0)
        {
            error("Error with select()\n");
            exit(1);
        }
        
//--------------------WAITING FOR PLAYERS--------------------        

        if (connectedclient < maxclient)
        {
            if (FD_ISSET(server_socket, &sockets))
            {
                if ((newsocket = accept(server_socket, (struct sockaddr*) &client, (socklen_t*) &client_size)) < 0)
                {
                    error("Error with accept()");
                    exit(1);
                }

                printf("Új játékos csatlakozott! Socket: %d Port: %d \n", newsocket, ntohs(server.sin_port));

                for (i = 0; i < maxclient; i++)
                {
                    if (client_socket[i] == 0)
                    {
                        client_socket[i] = newsocket;
			surr[i] = 0;
                        connectedclient++;
                        break;
                    }
                }
                
                //Ha mindenki csatlakozott
                if (connectedclient == maxclient) 
		{
		    printf("Minden játékos csatlakozott! Játék kezdése...\n");
		    
		    //----------OSZTÁS----------
		    for (i=0; i<maxclient; i++)
		    {
			sprintf(buffer, "DRAW 5\n");
			draw(client_socket[i], deck, inhand, decksize, buffer);
			if (i == currentplayer)
			{
			    sprintf(buffer, "TURN %d\n", currentplayer);
			    send(client_socket[i], buffer, strlen(buffer), 0);
			}
			else
			{
			    sprintf(buffer, "NOTURN %d\n", currentplayer);
			    send(client_socket[i], buffer, strlen(buffer), 0);
			}
		    }
		    printf("Lapok kiosztva.\n");
		    
		    //----------KEZDŐ LAP----------
		    strncpy(currentcard, number_to_id(getrandomcard(deck, inhand, decksize)), 3);
		    while (currentcard[1] == '7')
		    {
			deck[id_to_number(currentcard)] = 1;
			usedcard--;
			strncpy(currentcard, number_to_id(getrandomcard(deck, inhand, decksize)), 3);
		    }
		    sprintf(message, "MESSAGE A kezdő lap: %s\n", currentcard);
		    for (i=0; i<maxclient; i++)
		    {
			sprintf(buffer, "CURRENT %s\n", currentcard);
			send(client_socket[i], buffer, strlen(buffer), 0);
		    }
		    for (i=0; i<maxclient; i++)
		    {
			send(client_socket[i], message, strlen(message), 0);
		    }
		}
            }
        }
        else
        {
//--------------------CLIENT REQUESTS--------------------
            for (i = 0; i < maxclient; i++)
            {
                if (FD_ISSET(client_socket[i] , &sockets))
                {
                    if ((recvsize = recv(client_socket[i], buffer, bytes, 0)) == 0)
                    {
                        printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(client.sin_addr) , ntohs(client.sin_port));

			surrcount++;
			surr[i] = 1;
                        close(client_socket[i]);
                        client_socket[i] = 0;
                    }
                    else
                    {
			if (i == currentplayer)
			{
			    strncpy(lastcard, currentcard, 3);
			    
			    if (strstr(buffer, "WIN"))
			    {
				sprintf(buffer, "WIN 1,%d\n", currentplayer);
				send(client_socket[i], buffer, strlen(buffer), 0);
				for (j=0; j<maxclient; j++)
				{
				    if (j != i)
				    {
					sprintf(buffer, "WIN 0,%d\n", currentplayer);
					send(client_socket[j], buffer, strlen(buffer), 0);
				    }
				}
				end = 1;
			    }
			    
			    if (strstr(buffer, "DRAW"))
			    {
				draw(client_socket[i], deck, inhand, decksize, buffer);
				printf("Sending DRAW to player %d...\n", i+1);
				sprintf(message, "MESSAGE A(z) %d. játékos húzott.\n", currentplayer+1);
			    }
			    
			    if (strstr(buffer, "CURRENT"))
			    {
				request = 0;
				char** arg = getargument(buffer, 2);
				strncpy(currentcard, arg[1], 2);
				currentcard[2] = '\0';
				free(arg);
				
				int argn = id_to_number(currentcard);
				if (argn < 0)
				{
				    error("ID to number error\n");
				    exit(1);
				}
				inhand[argn] = 0;
				if (strlen(currentcard) != 2)
				{
				    error("Error with card\n");
				    exit(1);
				}
				sprintf(message, "MESSAGE A lerakott lap: %s\n", currentcard);
			    }
			    
			    if (strstr(buffer, "REQUEST"))
			    {
				request = 1;
				char** arg = getargument(buffer, 3);
				strncpy(currentcard, arg[1], 2);
				currentcard[2] = '\0';
				strncpy(requested, arg[2], 2);
				requested[2] = '\0';
				free(arg);
				sprintf(message, "MESSAGE A kért lap: %s\n", requested);
			    }
			    
			    if (strstr(buffer, "SKIP"))
			    {
				sprintf(message, "MESSAGE A(z) %d. játékos kimaradt.\n", currentplayer+1);
			    }
			    
			    if (strstr(buffer, "SURRENDER"))
			    {
				surrcount++;
				surr[currentplayer] = 1;
				printf("Host surrendered\n");
				sprintf(message, "MESSAGE A(z) %d. játékos feladta.\n", currentplayer+1);
				
				close(client_socket[i]);
				client_socket[i] = 0;
			    }
			    
			    for (j=0; j<maxclient; j++)
			    {
				if (surr[j]) continue;
				
				sprintf(buffer, "NOTURN %d\n", currentplayer);
				send(client_socket[j], buffer, strlen(buffer), 0);
				
				if (!request)
				{
				    sprintf(buffer, "CURRENT %s\n", currentcard);
				    send(client_socket[j], buffer, strlen(buffer), 0);
				    printf("Sending CURRENT to player %d...\n", j+1);
				}
				else
				{
				    sprintf(buffer, "REQUEST %s\n", requested);
				    send(client_socket[j], buffer, strlen(buffer), 0);
				    printf("Sending REQUEST to player %d...\n", j+1);
				}
				
				send(client_socket[j], message, strlen(message), 0);
			    }
			    
			    currentplayer++;
			    if (currentplayer == maxclient) currentplayer = 0;
			    while (surr[currentplayer])
			    {
				currentplayer++;
				if (currentplayer == maxclient) currentplayer = 0;
			    }
			    
			    if (surrcount == maxclient-1)
			    {
				sprintf(buffer, "SURRWIN %d\n", currentplayer+1);
				send(client_socket[currentplayer], buffer, strlen(buffer), 0);
				end = 1;
			    }
			    
			    if (currentcard[1] == 'S' && strcmp(currentcard, lastcard))
			    {
				sprintf(buffer, "SKIP randomcucc\n");
				send(client_socket[currentplayer], buffer, strlen(buffer), 0);
				printf("Sending SKIP to player %d...\n", currentplayer+1);
			    }
			    else
			    {
				sprintf(buffer, "TURN %d\n", currentplayer);
				send(client_socket[currentplayer], buffer, strlen(buffer), 0);
			    }
			    
			    if (currentcard[1] == '7' && strcmp(currentcard, lastcard) && !start)
			    {
				draw7 += 2;
				sprintf(buffer, "HETES %d\n", draw7);
				send(client_socket[currentplayer], buffer, strlen(buffer), 0);
				printf("Sending HETES to player %d...\n", currentplayer+1);
			    }
			    else
			    {
				draw7 = 0;
			    }
			}
			else
			{
			    sprintf(buffer, "WRONGTURN %d\n", currentplayer);
			    send(client_socket[i], buffer, strlen(buffer), 0);
			    printf("Sending WRONGTURN to player %d...\n", i+1);
			}
			
			start = 0;
                    }
                }
            }
        }
    }
    
    for (i = 0; i < maxclient; i++)
    {
	close(client_socket[i]);
    }
    close(server_socket);
}