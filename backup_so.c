#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>

int nelem=0;
//value to know if it's the first time running the backup or not
int firsttime=1;
//value to know if has been reached the end of the program
int over=0;
//value to know if all files have been deleted from the source directory
int apagoutudo=0;

//structure to keep all the files and their last backup dates
typedef struct {
char nome [50];
int ano;
int mes;
int dia;
int horas;
int minutos;
int segundos;
} vector;

vector *elem=NULL;

//function to copy a file name and last backup date to vector
void guardar_vec(char file[50], int anob, int mesb, int diab, int horasb, int minutosb, int segundosb)
{
    nelem++;
    elem=(vector *)realloc(elem,(nelem+1)*sizeof(vector));
    strcpy(elem[nelem-1].nome,file);
    elem[nelem-1].ano=anob;
    elem[nelem-1].mes=mesb;
    elem[nelem-1].dia=diab;
    elem[nelem-1].horas=horasb;
    elem[nelem-1].minutos=minutosb;
    elem[nelem-1].segundos=segundosb;
}

//checks the directory and the vector to see if there are any changes(created or deleted files)
int check_vector_files(char *src, vector *elem, int n)
{
	DIR *dirp;
        struct dirent *direntp;
	int i,j;
	int d=0;
	int nelem2=0;
	vector *elem2=NULL;
	dirp=opendir(src);
	chdir(src);
	while ((direntp = readdir(dirp)) != NULL)
	{
		for(i=0;i<n;i++)
		{
			//printf("Ficheiro : %s\n",direntp->d_name);
			//printf("Vector : %s\n",elem[i].nome);
			if(strcmp(direntp->d_name,elem[i].nome)==0)
			{
				//printf("entrou igual\n");
				nelem2++;
				elem2=(vector *)realloc(elem2,(nelem2+1)*sizeof(vector));
				strcpy(elem2[nelem2-1].nome,elem[i].nome);
	    			elem2[nelem2-1].ano=elem[i].ano;
	    			elem2[nelem2-1].mes=elem[i].mes;
	    			elem2[nelem2-1].dia=elem[i].dia;
	    			elem2[nelem2-1].horas=elem[i].horas;
	    			elem2[nelem2-1].minutos=elem[i].minutos;
	    			elem2[nelem2-1].segundos=elem[i].segundos;
			}
		}
	}
	//printf("VEC 2\n");
	//listar_vec(elem2,nelem2);
	//printf("\n");
	d=nelem - nelem2;
	if(nelem2>0)
	{
	free(elem);
	elem=NULL;
	nelem=0;	
	for(j=0;j<nelem2;j++)
	{
		nelem++;
    		elem=(vector *)realloc(elem,(nelem+1)*sizeof(vector));
    		strcpy(elem[nelem-1].nome,elem2[j].nome);
    		elem[nelem-1].ano=elem2[j].ano;
    		elem[nelem-1].mes=elem2[j].mes;
    		elem[nelem-1].dia=elem2[j].dia;
    		elem[nelem-1].horas=elem2[j].horas;
    		elem[nelem-1].minutos=elem2[j].minutos;
    		elem[nelem-1].segundos=elem2[j].segundos;
	}
	}
	//printf("VEC 1\n");
	//listar_vec(elem,nelem);
	close(dirp);
	chdir("..");
	return d;
}

// prints vector
void listar_vec(vector *elem, int n)
{
    int i;
    for(i=0;i<n;i++)
    {
      printf("%s\n",elem[i].nome);
      printf("%d / %d / %d - %d:%d:%d \n",elem[i].ano,elem[i].mes,elem[i].dia,elem[i].horas,elem[i].minutos,elem[i].segundos);
    }
}

// verifies if the modification date is higher than the last backup date.
int verifica_tempo(vector *elem, int n,char file[50],int anob, int mesb, int diab, int horasb, int minutosb, int segundosb)
{
    int i;
    for(i=0;i<n;i++)
    {
      if(strcmp(elem[i].nome,file)==0)
      {
		printf("Filename: %s\n",file);
	        printf("Data Ultimo Backup: %d / %d / %d - %d : %d : %d\n",elem[i].ano,elem[i].mes,elem[i].dia,elem[i].horas,elem[i].minutos,elem[i].segundos);
	  	printf("Data Ultima Mod: %d / %d / %d - %d : %d : %d\n",anob,mesb,diab,horasb,minutosb,segundosb);
	if(elem[i].ano>anob)
	{
	  //printf("entrouano\n");
	  return 0;
	}
	else
	{
		if(elem[i].ano==anob)
		{
			if(elem[i].mes>mesb)
			{
				//printf("entroumes\n");
				return 0;
			}
			else
			{
				if(elem[i].mes==mesb)
				{
					if(elem[i].dia>diab)
					{
						//printf("entroudia\n");
						return 0;
					}
					else
					{
						if(elem[i].dia==diab)
						{
							if(elem[i].horas>horasb)
							{
								//printf("entrouhoras\n");
								return 0;
							}
							else
							{
								if(elem[i].horas==horasb)
								{
									if(elem[i].minutos>minutosb)
									{
										//printf("entrouminutos\n");
										return 0;
									}
									else
									{
										if(elem[i].minutos==minutosb)
										{
											if(elem[i].segundos>segundosb)
											{
												//printf("entrousegundos\n");
												return 0;
											}
											else
											{
												if(elem[i].segundos<segundosb)
												{
													return 1;
												}
											}					
										}
										else
										{
											return 1;
										}
									}
								}
								else
								{
									return 1;
								}
							}

						}
						else	
						{
							return 1;
						}
					}
				}
				else
				{
					return 1;
				}
			}
		}
		else
		{
			return 1;
		}
	    }	
	}
    }
}

//verifies if the directory exists
int verifica_existe(vector *elem, int n,char file[50])
{
    int i;
    for(i=0;i<n;i++)
    {
      if(strcmp(elem[i].nome,file)==0)
      {
		//printf("entrou return 1\n");
		return 1;	
      }
    }
    return 0;
}

//changes the backup date if the modification date is higher
void altera_vec(vector *elem, int n,char file[50],int anob, int mesb, int diab, int horasb, int minutosb, int segundosb)
{
  int i;
      for(i=0;i<n;i++)
      {
	if(strcmp(elem[i].nome,file)==0)
	{
	  elem[i].ano=anob;
	  elem[i].mes=mesb;
	  elem[i].dia=diab;
	  elem[i].horas=horasb;
	  elem[i].minutos=minutosb;
	  elem[i].segundos=segundosb;
	}
      }
}

//verifies previous files to write in backinfo.txt
int verifica_anteriores(vector *elem,int n,char nomefile[256],char nome[100],char *src,char*dest)
{
 	DIR *dirp;
        struct dirent *direntp;
	FILE *fp,*stream;
	int i;
	char file[50];
	dirp=opendir(src);
	chdir(src);
	while ((direntp = readdir(dirp)) != NULL)
	{
		if(strcmp(direntp->d_name,nome)==0)
		{
			return 1;
		}
		else
		{
			strcpy(file,direntp->d_name);
			//printf("FICHEIRO : %s\n",file);
			chdir("..");
			chdir(dest);
			if(stream = fopen(nomefile,"r") != NULL)
		  	{
				fp=fopen(nomefile,"a");
		  	}
		  	else
		  	{
				fp=fopen(nomefile,"w");
		  	}
			for(i=0;i<n;i++)
			{
				if(strcmp(elem[i].nome,file)==0)
				{
					//printf("FICHEIRO2 : %s\n",file);
					fprintf(fp,"%s - %d_%d_%d_%d_%d_%d \n",elem[i].nome,elem[i].ano,elem[i].mes,elem[i].dia,elem[i].horas,elem[i].minutos,elem[i].segundos);
					fclose(fp);
				}
			}
			chdir("..");
			chdir(src);
		}
	}
	close(dirp);
	return 0;
	
}

//verifies if a directory exists
int verifica_existe_dir(char *src,char file[50])
{
        DIR *dirp;
        struct dirent *direntp;
	dirp=opendir(src);
	chdir(src);
	  while ((direntp = readdir(dirp)) != NULL)
	  {
		if(strcmp(direntp->d_name,file)==0)
		{
			return 1;
		}
	  }
	  closedir(dirp);
	 chdir("..");
	  return 0;
}

//gets the index of the file in vector.
int get_i(vector *elem, int n,char file[50])
{
  int i;
      for(i=0;i<n;i++)
      {
	if(strcmp(elem[i].nome,file)==0)
	{
		return i;
	}
      }
     return -1;
}

// prints content on the directory
int dir_content(char *src, char *dest)
{
        DIR *dirp;
        struct dirent *direntp;
	struct stat stat_buf;
        char *str;
        if ((dirp = opendir(dest)) == NULL)
        {
		mkdir(dest,0777);
        }
        if ((dirp = opendir(src)) == NULL)
        {
		printf("A pasta source nao existe\n");
		return -1;
        }
        else
	{
	dirp=opendir(src);
	chdir(src);
	  while ((direntp = readdir(dirp)) != NULL)
	  {
                stat(direntp->d_name, &stat_buf);
                if (S_ISREG(stat_buf.st_mode)) str = "regular";
                else if (S_ISDIR(stat_buf.st_mode)) str = "directory";
                else if(S_ISLNK(stat_buf.st_mode))  str = "link";
                else str = "other";
                printf("%-25s - %s\n", direntp->d_name, str);
	  }
	  closedir(dirp);
	}
	return 0;
}

// function to check if files have been created, modified or deleted and applies the actions to backup
int cp_reg_files(char *src,char *dest)
{
        DIR *dirp;
        struct dirent *direntp;
        struct stat stat_buf;
	char nomedir[256];
	char novodest[256];
	char nomefile[256];
	time_t t=time(NULL);
	struct tm mytime=*localtime(&t);
	time_t tmodificado=time(NULL);
	struct tm tmod;
	int num;
	int cont=0;
	int cont2=0;
	int dif=0;
	int create=0;
	int apagou=0;
	FILE *bk,*stream;
	chdir("..");
    dirp = opendir(src);
	chdir(src);
    while ((direntp = readdir(dirp)) != NULL)
    {
	//printf("entrou while\n");
        stat(direntp->d_name, &stat_buf);
        if (S_ISREG(stat_buf.st_mode))
        {
		//printf("entrouif\n");
		int exc=0;
		char src1[100];
		strcpy(src1,src);
		strcat(src1,"/");
		strcat(src1,direntp->d_name);
		if(S_IXUSR & stat_buf.st_mode) {/*printf("entrou\n");*/ exc=1;}
		chdir("..");
		if(firsttime==1)
		{
		  chdir(dest);
		  sprintf(nomedir,"%d_%d_%d_%d_%d_%d",mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
		  mkdir(nomedir,0777);
		  strcpy(novodest,dest);
		  strcat(novodest,"/");
		  strcat(novodest,nomedir);
		  strcpy(nomefile,nomedir);
		  strcat(nomefile,"/");
		  strcat(nomefile,"__bckinfo__.txt");
		  //printf("%s\n",nomefile);
		  if(stream = fopen(nomefile,"r") != NULL)
		  {
			bk=fopen(nomefile,"a");
		  }
		  else
		  {
			bk=fopen(nomefile,"w");
		  }
		  fprintf(bk,"%s - %d_%d_%d_%d_%d_%d \n",direntp->d_name,mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
		  fclose(bk);
		  guardar_vec(direntp->d_name,mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
		  chdir("..");
		  cp_file(src1, novodest, direntp->d_name,exc);
		  //listar_vec(elem,nelem);
		}
		else
		{
			printf("Nome Ficheiro : %s\n",direntp->d_name);
			printf("\n");
			dif=check_vector_files(src,elem,nelem);
			printf("VEC APOS\n");
			listar_vec(elem,nelem);
			printf("DIF - %d\n",dif);
			if(dif>0)
			{
				apagou=1;
				chdir(dest);
				sprintf(nomedir,"%d_%d_%d_%d_%d_%d",mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
				mkdir(nomedir,0777);
		  		strcpy(novodest,dest);
		  		strcat(novodest,"/");
		  		strcat(novodest,nomedir);
				strcpy(nomefile,nomedir);
		  		strcat(nomefile,"/");
		  		strcat(nomefile,"__bckinfo__.txt");
				if(stream = fopen(nomefile,"r") != NULL)
		  		{
					bk=fopen(nomefile,"w");
		  		}
				chdir("..");
			}
		  if(verifica_existe(elem,nelem,direntp->d_name)==0)
		  {
			printf("entrouverifica_existe\n");
			cont++;
			create=1;
			apagoutudo=0;
			chdir(dest);
		  	sprintf(nomedir,"%d_%d_%d_%d_%d_%d",mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
		  	mkdir(nomedir,0777);
		  	strcpy(novodest,dest);
		  	strcat(novodest,"/");
		  	strcat(novodest,nomedir);
			strcpy(nomefile,nomedir);
		  	strcat(nomefile,"/");
		  	strcat(nomefile,"__bckinfo__.txt");
			if(stream = fopen(nomefile,"r") != NULL)
		  	{
				bk=fopen(nomefile,"a");
		  	}
		  	else
		  	{
				bk=fopen(nomefile,"w");
		  	}
		  	//printf("%s\n",novodest);
			//fprintf(bk,"%s - %d / %d / %d - %d:%d:%d \n",direntp->d_name,mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
			fclose(bk);
			//listar_vec(elem,nelem);
		  	guardar_vec(direntp->d_name,mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
			//listar_vec(elem,nelem);
		  	chdir("..");
		  	cp_file(src1, novodest, direntp->d_name,exc);
			chdir(src);
			printf("entrourewind\n");
			rewinddir(dirp);
			chdir("..");
			//verifica_anteriores(elem,nelem,nomefile,direntp->d_name,src,dest);
			//chdir("..");
		  }
		  else
		  {
			//printf("entrou se ja existe\n");
			cont++;
			apagoutudo=0;
		  tmodificado=stat_buf.st_mtime;
		  tmod=*localtime(&tmodificado);
		  //printf("Verifica Tempo : %d\n",verifica_tempo(elem,nelem,direntp->d_name,tmod.tm_year+1900,tmod.tm_mon+1,tmod.tm_mday,tmod.tm_hour,tmod.tm_min,tmod.tm_sec));
		  if(verifica_tempo(elem,nelem,direntp->d_name,tmod.tm_year+1900,tmod.tm_mon+1,tmod.tm_mday,tmod.tm_hour,tmod.tm_min,tmod.tm_sec)==1)
		  {
			cont2++;
		      chdir(dest);
		      sprintf(nomedir,"%d_%d_%d_%d_%d_%d",mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
		      mkdir(nomedir,0777);
		      strcpy(novodest,dest);
		      strcat(novodest,"/");
		      strcat(novodest,nomedir);
		      strcpy(nomefile,nomedir);
		      strcat(nomefile,"/");
		      strcat(nomefile,"__bckinfo__.txt");
		      if(stream = fopen(nomefile,"r") != NULL)
		      {
				bk=fopen(nomefile,"a");
		      }
		      else
		      {
				bk=fopen(nomefile,"w+");
		      }
		      num = get_i(elem,nelem,direntp->d_name);
		      fprintf(bk,"%s - %d_%d_%d_%d_%d_%d \n",direntp->d_name,elem[num].ano,elem[num].mes,elem[num].dia,elem[num].horas,elem[num].minutos,elem[num].segundos);
		      fclose(bk);
		      verifica_anteriores(elem,nelem,nomefile,direntp->d_name,src,dest);
		      //printf("%s\n",novodest);
		      altera_vec(elem,nelem,direntp->d_name,mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
		      chdir("..");
		      cp_file(src1, novodest, direntp->d_name,exc);
		      //listar_vec(elem,nelem);
		  }
		  else
		  {
			//printf("entrou se nao modificou o tempo\n");
			if(cont2>0 || apagou==1 || create==1)
			{
				printf("entrou onde nao devia\n");
			chdir(dest);
		      	if(stream = fopen(nomefile,"r") != NULL)
		      	{	
				bk=fopen(nomefile,"a");
		      	}
		      	else
		      	{
				bk=fopen(nomefile,"w");
		      	}
			num = get_i(elem,nelem,direntp->d_name);
			printf("num - %d\n",num);
		      fprintf(bk,"%s - %d_%d_%d_%d_%d_%d \n",direntp->d_name,elem[num].ano,elem[num].mes,elem[num].dia,elem[num].horas,elem[num].minutos,elem[num].segundos);
			fclose(bk);
			chdir("..");
			}
		  }
		 }
		}
		//printf("depoiscpfile\n");
		chdir(src);
        }
    }
    if(cont==0 && firsttime==0 && apagoutudo==0)
    {
	//printf("entrou cont\n");
	chdir("..");
	chdir(dest);
	apagoutudo=1;
	//printf("entrou\n");
	sprintf(nomedir,"%d_%d_%d_%d_%d_%d",mytime.tm_year+1900,mytime.tm_mon+1,mytime.tm_mday,mytime.tm_hour,mytime.tm_min,mytime.tm_sec);
	mkdir(nomedir,0777);
	strcpy(nomefile,nomedir);
	strcat(nomefile,"/");
	strcat(nomefile,"__bckinfo__.txt");
	//printf("%s\n", nomefile);
	bk=fopen(nomefile,"w+");
	fclose(bk);
	chdir("..");
	chdir(src);
    }
    if(firsttime==1)
    {
      firsttime=0;
    }
    closedir( dirp ) ;
    return 0;
}

//function that copies a file from a directory to another
int cp_file(char *src, char *outfile, char *infile,int exc)
{
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
	pid=fork();
	//FILHO
	if(pid==0)
	{
		fp=fopen(src,"r");
		fp1=fopen(destino,"w");
		while(size= fread(buf,1,BUFSIZ,fp))
		{
			fwrite(buf,1,size,fp1);
		}
    		if(exc==1) chmod(destino,0744);
		fclose(fp);
		fclose(fp1);
		exit(27);
	}
	else
	{
		retorno=waitpid(pid,status);
	}
    
        return 0;
}

static void sig_usr(int signo)
{
	if(signo == SIGUSR1)
	{
		printf("received SIGUSR1\n");
		over=1;
	}
}
int main(int argc, char *argv[])
{
  int tempo;
  tempo=atoi(argv[3]);
  char inicio[256];
  char srcraiz[256];
  char destraiz[256];
  /*
	time_t t=time(NULL);
	struct tm mytime=*localtime(&t);
	mytime.tm_year+1900;
	
	
	st_mtime;
	time_t tmodificado=time(NULL);
	struct tm tmod;
	tmodificado=stat_buf.st_mtime;
	tmod=*localtime(&tmodificado);
  */
  elem=(vector *)malloc(sizeof(elem));
  while(over==0)
  {
	printf("Entrou no tempo\n");
  	if(argc!=4)
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
      		if(dir_content(argv[1],argv[2])==0)
      		{
	   		while(1)
	   		{
	     			cp_reg_files(srcraiz,destraiz);
	     			sleep(tempo);
	   		}
      		}
	}
	signal(SIGUSR1, sig_usr);
  }
}
