#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
//verifica se a pasta de backup existe e caso exista verifica se alguma subpasta tem aquela data como nome (nomeSubPasta)

char dataInicial[200];
char pastaOrigem[200];
char pastaDest[200];
char nomepasta[200];
int nelem=0;
int boolean=0;

typedef struct {
char nome [100];
}vector;
vector *elem=NULL;

int listarSubPastas(char *pasta)
{
    DIR *dirp;
    struct dirent *direntp;
    char str[20];
    if ((dirp = opendir(pasta)) == NULL)
        {
		printf("Diretorio de backup nao existente\n\n");
		return -1;
        }
        else
	{
		dirp=opendir(pasta);
		chdir(pasta);
		while ((direntp = readdir(dirp)) != NULL)
		{
			strcpy(str,direntp->d_name);
			printf("%s\n",str);
		}
	}
	closedir(dirp);
	chdir("..");
	return 0;
}


void dir_content(char *src, char *dest,char nomeSubPasta[200])
{	
	struct stat stat_buf;
	int i=0;
        DIR *dirp;
        struct dirent *direntp;
        char str[20];
	char caminho[100];
        if ((dirp = opendir(dest)) == NULL)
        {
		mkdir(dest,0777);
        }
		dirp=opendir(src);
		chdir(src);
		while ((direntp = readdir(dirp)) != NULL)
		{
			strcpy(caminho,src);
			strcat(caminho,"/");	
			strcpy(str,direntp->d_name);
			strcat(caminho,str);
			
			if(strcmp(str,nomeSubPasta)==0)
			{
				i=1;
				printf("Existe\n");
				//printf("CAMINHO: %s\n",caminho);
				abrirPastaDesejada(str,dest,caminho);
				chdir(src);
			}
		}
		if(direntp==NULL && i==0)
		{
			printf("Diretorio inexistente com essa data\n\n");
		}
	closedir(dirp);
}


void abrirPastaDesejada(char *pasta,char *dest,char *caminho)
{
	struct stat stat_buf;
	printf("entrou\n");
	DIR *dirp;
        struct dirent *direntp;
	char str[100];
	char nomeFicheiro[20];
	char caminhoBck[100];	
	dirp=opendir(pasta);
	chdir(pasta);
	while((direntp = readdir(dirp)) !=NULL)
	{
	    stat(direntp->d_name, &stat_buf);
            if (S_ISREG(stat_buf.st_mode))
	    {
		strcpy(str,caminho);
		strcat(str,"/");
		strcpy(nomeFicheiro,direntp->d_name);
		strcat(str,nomeFicheiro);
		//printf("STR: %s\n",str);
		if(strcmp(nomeFicheiro,"__bckinfo__.txt")!=0)
		{		
			//printf("Entrou no que nao e __bckinfo__\n");
			guardar_vec(nomeFicheiro);
			chdir("..");
			chdir("..");
			cp_file(str,dest,nomeFicheiro,0);
			chdir(pasta);
		}
		else
		{
			//printf("Entrou no __bckinfo__\n");
		}
	    }
	}
	strcpy(caminhoBck,caminho);
	strcat(caminhoBck,"/__bckinfo__.txt");
	verificarVector(elem,caminhoBck);
	chdir(pasta);
	close(dirp);
}



void guardar_vec(char file[100])
{
    nelem++;
    elem=(vector *)realloc(elem,(nelem+1)*sizeof(vector));
    strcpy(elem[nelem-1].nome,file);
    printf("Guardou no vector\n");
}

void listar_vec(vector *elem, int n)
{
    int i;
    for(i=0;i<n;i++)
    {
      printf("%s\n",elem[i].nome);
    }
}


int cp_file(char *src, char *outfile, char *infile,int exc)
{
    printf("entrou cpy\n");
    pid_t pid;
    pid_t retorno;
    int *status;
    char buf[BUFSIZ];
    size_t size;
    FILE *fp, *fp1;
    char destino[100];
    strcpy(destino,outfile);
    strcat(destino,"/");
    strcat(destino,infile);
		//printf("SRC: %s\n",src);
		//printf("DEST: %s\n",destino);
		//printf("OUT: %s\n",outfile);
		fp=fopen(src,"r");
		fp1=fopen(destino,"w");
		while(size= fread(buf,1,BUFSIZ,fp))
		{
			fwrite(buf,1,size,fp1);
		}
    		if(exc==1) chmod(destino,0744);
		fclose(fp);
		fclose(fp1);
    	printf("Copiou o ficheiro\n");
        return 0;
}

void verificarVector(vector *elem,char *caminho)
{
	printf("entrou no verificaVector\n");
	FILE *f1;
	int i=0;
	int contador=0;
	char nomeficheiro[200];
	char buffer[256];
	char tracinho[200];
	f1=fopen(caminho,"r");
	char caminhoOrigem[200];
	while(!feof(f1))
	{
		fscanf(f1,"%s %s %s\n",nomeficheiro,tracinho,nomepasta);
		for(i=0;i<nelem;i++)
		{
			if(elem[i].nome==nomeficheiro)
			{
				contador++;
				printf("%s e o nome do fichero\n",nomeficheiro);
			}
			else
			{
				strcpy(caminhoOrigem,pastaOrigem);
				strcat(caminhoOrigem,"/");
				strcat(caminhoOrigem,nomepasta);
				strcat(caminhoOrigem,"/");
				strcat(caminhoOrigem,nomeficheiro);
				getcwd(buffer,sizeof(buffer));
				printf("BUFFER=%s\n",buffer);
				cp_file(caminhoOrigem,pastaDest,nomeficheiro,0);
				//printf("NOMEFICHEIRO=%s\n",nomeficheiro);
				//printf("PASTADEST=%s\n",pastaDest);
				//printf("CAMINHOORIGEM=%s\n",caminhoOrigem);
			}
		}
		
	}
	fclose(f1);
	//if(contador==nelem) printf("Restore completed\n");
}

int main(int argc, char *argv[])
{
  int index;
  int existe;
  char inicio[256];
  char srcraiz[256];
  char destraiz[256];
  strcpy(pastaOrigem,argv[1]);
  strcpy(pastaDest,argv[2]);
  if(argc!=3)
  {
    printf("Erro, numero de argumentos invalido\n");
  }
  else
  {
	getcwd(inicio,sizeof(inicio));
	chdir(argv[1]);
	getcwd(srcraiz,sizeof(srcraiz));
	chdir(inicio);
	chdir(argv[2]);
	getcwd(destraiz,sizeof(destraiz));
	chdir(inicio);
    existe=listarSubPastas(argv[1]);
    if(existe==0)
    {
	printf("Insira a data que deseja: ");
    	scanf("%s",&nomepasta);
	//strcpy(dataInicial,nomepasta);
	dir_content(srcraiz,destraiz,nomepasta);
    }
    printf("\n\nO que esta no vetor: ");
    listar_vec(elem,nelem);
    //getDirName(index);
    
    
    /*if(existe==0)
    {
      cp_file(argv[1],argv[2]);
    }*/
  }
}
