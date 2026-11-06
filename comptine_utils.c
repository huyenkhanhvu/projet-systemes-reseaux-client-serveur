/*Khanh Huyen VU 12012322
Je declare qu'il s'agit de mon propre travail*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "comptine_utils.h"




int est_nom_fichier_comptine(char *nom_fich)
{
	int n=strlen(nom_fich);
	if(n>4){
		if(nom_fich[n-4]=='.' && nom_fich[n-3]=='c' && nom_fich[n-2]=='p' && nom_fich[n-1]=='t'){
			return 1;
		}
	}
	return 0;
}

int read_until_nl(int fd, char *buf)
{
	
	//lseek(fd,0,SEEK_SET);
	char ch='w';
	for(int n=0; read(fd,&ch,1)!=0;n++){
		printf("%c",ch);
		if(ch=='\n'){
			buf[n]=ch;
			return n;
		}
	}
	return 0;
}
struct comptine *init_cpt_depuis_fichier(const char *dir_name, const char *base_name)
{
	
	struct comptine * comp = malloc(sizeof(struct comptine));
	if(comp == NULL){
		perror("Echec creation comptine");
		return NULL;
	}
	comp->nom_fichier=malloc(strlen(dir_name)+strlen(base_name) +2);
	if(comp->nom_fichier == NULL){
		perror("Echec allocation");
		free(comp);
		return NULL;
	}
	sprintf(comp->nom_fichier, "%s/%s",dir_name,base_name);
	if(est_nom_fichier_comptine(comp->nom_fichier)==0){
		fprintf(stderr, "Le fichier n'a pas l'extension .cpt");
		free(comp->nom_fichier);
		free(comp);
		return NULL;
	}
	int fd=open(comp->nom_fichier, O_RDONLY);
	if(fd==-1){
		perror("Echec l'ouverture du fichier");
		free(comp->nom_fichier);
		free(comp);
		return NULL;
	}
	comp->titre=malloc(sizeof(BUFSIZE));
	if(comp->titre == NULL){
		perror("Echec allocation");
		//close(fd);
		free(comp->nom_fichier);
		free(comp);
		return NULL;
	}
	int byte = read_until_nl(fd, comp->titre);
	if(byte == 0){
		perror("Echec lors de la lecture du fichier");
		free(comp->titre);
		free(comp->nom_fichier);
		free(comp);
		//close(fd);
		return NULL;
	}
	comp->titre[byte] = '\0';
	//close(fd);
	return comp;
}

void liberer_comptine(struct comptine *cpt)
{
	
	free(cpt->nom_fichier);
	free(cpt->titre);
	free(cpt);
}

struct catalogue *creer_catalogue(const char *dir_name)
{
	
	DIR *dR;
	dR = opendir(dir_name);
	struct dirent *dir;
	if(dR==NULL){
		perror("opendir");
		return NULL;
	}
	
	struct catalogue *cat = malloc(sizeof(struct catalogue));
	if(cat == NULL){
		perror("Echec allocation");
		closedir(dR);
		return NULL;
	}
	cat->nb=0;
	if(dR!=0){
		while((dir = readdir(dR)) != NULL){
			if(dir->d_type == DT_REG){
				cat->nb++;
			}
		}
		closedir(dR);
	}
	dR=opendir(dir_name);
	cat->tab=malloc(sizeof(struct comptine *) *cat->nb);
	if(cat->tab==NULL){
		perror("Echec allocation");
		closedir(dR);
		free(cat);
		return NULL;
	}
	if(dR!=0){
		int i=0;
		while((dir=readdir(dR))!= NULL){
			if(dir->d_type==DT_REG){
				cat->tab[i++]=init_cpt_depuis_fichier(dir_name, dir->d_name);
			}
		}
		closedir(dR);
	}
	return cat;
}

void liberer_catalogue(struct catalogue *c)
{
	
	if(c==NULL){
		return;
	}
	for(int i=0;i<c->nb;i++){
		liberer_comptine(c->tab[i]);
	}
	free(c->tab);
	free(c);
}

