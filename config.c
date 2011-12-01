#include <stdio.h>
#include <string.h>

typedef struct { char* Name; char* Data; } configData;
typedef struct { int size; configData* data; } config;

config defaultConfig, userConfig;

char printConfig(void* fp, config conf)
{
	int i = 0;
	char r = 1;
	for(;i<conf.size;i++)
		r &= (fprintf(fp,"%s=%s\n", conf.data[i].Name , conf.data[i].Data)>=0);
	return r;
}

int main(int argc, char **argv, char **envp)
{
	defaultConfig = (config){ 3 , (configData[])
		{
			(configData){"MAKE","make"},
			(configData){"CC","gcc"},
			(configData){"ASM","nasm"}
		} };
		
	printf("Configurating...\n\n");
	int i = 0;
	char* dir;
	char* pwd;
	#ifdef DBG
		printf("Environement variables: \n");
	#endif
	for (; envp[i]; i++)
	{
		#ifdef DBG
			printf(" * env %i: %s\n",i,envp[i]);
		#endif
		if( *((short int*)envp[i]) == (short int)'_' + ((short int)('=')<<8) )
		{
			int len = strlen(envp[i])-8;
			dir = malloc(len);
			memcpy(dir,envp[i]+2,len);
		} else if ( !strncmp(envp[i],"PWD=",4) )
		{
			int len = strlen(envp[i])-3;
			pwd = malloc(len);
			memcpy(pwd,envp[i]+4,len);
			pwd[len-1]='/';
		}
	}
	dir = strcat(strcat(pwd,dir),"src/");
	printf("Dir: %s\n",pwd);	
	
	printConfig(stdout,defaultConfig);
	
	char* fileName = strcat(pwd,"Makefile.conf");
	FILE *fp = fopen(fileName,"r+");
	if (fp == NULL)
	{
		printf("No config file found!\n");
		printf(" * Creating empty file...");
			fp = fopen(fileName,"w+");
			if(fp == NULL)
			{
				fprintf(stderr,"Could not create config file!\n");
				goto error;
			}
		printf("Done!\n * Creating default config file...");
			char r = printConfig(fp,userConfig);
			printf("%i\n",(int)r);
			if(!r)
			{
				fprintf(stderr,"Could not write to config file!");
				goto error;
			}
		printf("Done!\n");
		userConfig = defaultConfig;
	} else 	{
		printf("...\n");
		int  i    = 0,
		     size = 0;
		char c       ;
		
		while((c = fgetc(fp)) != EOF)
			size += (c == '\n')?1:0;
		fseek(fp,0,SEEK_SET);
		
		printf("Size: %i\n",size);
		
		userConfig = (config){size,malloc(size*sizeof(configData))};
		
		for(;i<size;i++)
		{
			char* name,data;
			fscanf(fp,"%s=%s\n", name, data);
			int len = strlen(name);
			userConfig.data[i].Name = malloc(len);
			memcpy(userConfig.data[i].Name,name,len);
			len = strlen(data);
			userConfig.data[i].Data = malloc(len);
			memcpy(userConfig.data[i].Data,data,len);
		}
		
		printConfig(stdout,userConfig);
	}
	
error:
	fclose(fp);	
	return 0;
}
