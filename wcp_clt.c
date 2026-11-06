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
#include <sys/types.h>
#include <sys/socket.h>
/* spécifique à internet */
#include <arpa/inet.h> /* inet_pton */
/* spécifique aux comptines */
#include "comptine_utils.h"

#define PORT_WCP 4321

void usage(char *nom_prog)
{
	fprintf(stderr, "Usage: %s addr_ipv4\n"
			"client pour WCP (Wikicomptine Protocol)\n"
			"Exemple: %s 208.97.177.124\n", nom_prog, nom_prog);
}

/** Retourne (en cas de succès) le descripteur de fichier d'une socket
 *  TCP/IPv4 connectée au processus écoutant sur port sur la machine d'adresse
 *  addr_ipv4 */
int creer_connecter_sock(char *addr_ipv4, uint16_t port);

/** Lit la liste numérotée des comptines dans le descripteur fd et les affiche
 *  sur le terminal.
 *  retourne : le nombre de comptines disponibles */
uint16_t recevoir_liste_comptines(int fd);

/** Demande à l'utilisateur un nombre entre 0 (compris) et nc (non compris)
 *  et retourne la valeur saisie. */
uint16_t saisir_num_comptine(uint16_t nb_comptines);

/** Écrit l'entier ic dans le fichier de descripteur fd en network byte order */
void envoyer_num_comptine(int fd, uint16_t nc);

/** Affiche la comptine arrivant dans fd sur le terminal */
void afficher_comptine(int fd);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}

	int clt=creer_connecter_sock(argv[1],PORT_WCP);
	envoyer_num_comptine(clt,saisir_num_comptine(recevoir_liste_comptines(clt)));
	afficher_comptine(clt);
	/* À compléter */
	return 0;
}

int creer_connecter_sock(char *addr_ipv4, uint16_t port)
{
	/* À définir */
	int sockfd,len,result;
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd < 0){
		perror("socket");
		exit(2);
	}
	
	struct sockaddr_in address = {
		.sin_family=AF_INET,
		.sin_port= htons(port)
	};
	
	if(inet_pton(AF_INET,addr_ipv4,&address.sin_addr) != 1){
		fprintf(stderr, "adress ipv4 non valable\n");
		exit(1);
	}	
	
	len=sizeof(address);
	result=connect(sockfd, (struct sockaddr *)&address, len);
	if(result < 0){
		perror("connect");
		exit(1);
	}
	return sockfd;
}

uint16_t recevoir_liste_comptines(int fd)
{
	/* À définir */
	char recv[BUFSIZE];
	int n=0, res=0;
	n=read(fd,recv,sizeof(recv)-1);
	while(n>0){
		recv[n]=0;
		if(strcmp(recv,"\n")==0) break;
		printf("%s", recv);
		res++;
		if(recv[n-1] == '\n' &&recv[n-2]=='\n' &&recv[n-3] == '\n') break;
	}
	
	return res;
}

uint16_t saisir_num_comptine(uint16_t nb_comptines)
{
	/* À définir */
	int num;
	printf("Quelle comptine voulez-vous ? (Entrez un nombre entre 1 et %d)\n",nb_comptines);
	scanf("%d",&num);
	return num;
}

void envoyer_num_comptine(int fd, uint16_t nc)
{
	/* À définir */
	printf("Numero envoyer : %d\n",nc);
	dprintf(fd,"%d",nc);
}

void afficher_comptine(int fd)
{
	/* À définir */
	char c='s';
	int n=read(fd,&c,1);
	while(n>=0){
		if(c==EOF) break;
		printf("%c",c);
	}
}
