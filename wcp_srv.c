/*Khanh Huyen VU 12012322
Je declare qu'il s'agit de mon propre travail*/

/* fichiers de la bibliothèque standard */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
/* bibliothèque standard unix */
#include <unistd.h> /* close, read, write */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <errno.h>
/* spécifique à internet */
#include <arpa/inet.h> /* inet_pton */
/* spécifique aux comptines */
#include "comptine_utils.h"

#define PORT_WCP 4321

void usage(char *nom_prog)
{
	fprintf(stderr, "Usage: %s repertoire_comptines\n"
			"serveur pour WCP (Wikicomptine Protocol)\n"
			"Exemple: %s comptines\n", nom_prog, nom_prog);
}
/** Retourne en cas de succès le descripteur de fichier d'une socket d'écoute
 *  attachée au port port et à toutes les adresses locales. */
int creer_configurer_sock_ecoute(uint16_t port);

/** Écrit dans le fichier de desripteur fd la liste des comptines présents dans
 *  le catalogue c comme spécifié par le protocole WCP, c'est-à-dire sous la
 *  forme de plusieurs lignes terminées par '\n' :
 *  chaque ligne commence par le numéro de la comptine (son indice dans le
 *  catalogue) commençant à 0, écrit en décimal, sur 6 caractères
 *  suivi d'un espace
 *  puis du titre de la comptine
 *  une ligne vide termine le message */
void envoyer_liste(int fd, struct catalogue *c);
/** Lit dans fd un entier sur 2 octets écrit en network byte order
 *  retourne : cet entier en boutisme machine. */
uint16_t recevoir_num_comptine(int fd);

/** Écrit dans fd la comptine numéro ic du catalogue c dont le fichier est situé
 *  dans le répertoire dirname comme spécifié par le protocole WCP, c'est-à-dire :
 *  chaque ligne du fichier de comptine est écrite avec son '\n' final, y
 *  compris son titre, deux lignes vides terminent le message */
void envoyer_comptine(int fd, const char *dirname, struct catalogue *c, uint16_t ic);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}
	
	int server_sockfd=creer_configurer_sock_ecoute(PORT_WCP);
	struct catalogue* cata=creer_catalogue(argv[1]);
	struct sockaddr_in client_address;
	for(;;){
		socklen_t sl = sizeof(client_address);
		int client_sockfd=accept(server_sockfd,(struct sockaddr *) &client_address, &sl);
		if(client_sockfd <0){
			perror("Echec accept");
			continue;
		}
		envoyer_liste(client_sockfd,cata);
		printf("Envoyee");
		envoyer_comptine(client_sockfd,argv[1],cata,recevoir_num_comptine(client_sockfd));
	}
	liberer_catalogue(cata);
	/* À compléter */
	return 0;
}

int creer_configurer_sock_ecoute(uint16_t port)
{
	int server_sockfd, server_len;
	server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
	if(server_sockfd < 0){
		perror("socket");
		exit(2);
	}
	struct sockaddr_in server_address = {
		.sin_family = AF_INET,
		.sin_port= htons(port),
		.sin_addr.s_addr=htonl(INADDR_ANY)
	};
	int opt=1;
	setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int));
	
	server_len=sizeof(server_address);
	int result=bind(server_sockfd,(struct sockaddr *) &server_address,server_len);
	if(result<0){
		perror("bind");
		exit(3);
	}
	if(listen(server_sockfd,1)<0){
		perror("listen");
		exit(3);
	}
	/* À définir */
	return server_sockfd;
}

void envoyer_liste(int fd, struct catalogue *c)
{
	for(int i=0;i<c->nb;i++){
		dprintf(fd, "%06d %s\n",i,c->tab[i]->titre);
	}
	dprintf(fd,"\n");
}

uint16_t recevoir_num_comptine(int fd)
{
	/* À définir */
	char recv[BUFSIZE];
	int n=read(fd,recv,sizeof(recv)-1);
	if(n<0){
		perror("Echec de lecture");
		exit(3);
	}
	recv[n]=0;
	printf("%s\n",recv);
	return atoi(recv);
}

void envoyer_comptine(int fd, const char *dirname, struct catalogue *c, uint16_t ic)
{
	int n=open(c->tab[ic]->nom_fichier, O_RDONLY);
	if(n<0){
		perror("open");
		exit(3);
	}
	char ch;
	int r=read(n,&ch,1);
	while(r>0){
		printf("%c",ch);
		if(write(fd,&ch,1) <0){
			perror("write");
			exit(3);
		}
	}
	//close(n);
	printf("Envoie fichier");
	/* À définir */
}
