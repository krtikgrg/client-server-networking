#include <stdio.h>
#include<fcntl.h>
#include<errno.h>
#include <arpa/inet.h>
#include<unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8000
char ackstring[]=".cmd_kk_st_gr_60(history_krikash).txt";
char tempack[100000];
struct sockaddr_in addr;
struct sockaddr_in saddr;
int reval,sock_fd;
void check()
{
	reval=read(sock_fd,tempack,1000);
	if(reval==-1)
	{
		perror("CLIENT:read Failure");
		exit(EXIT_FAILURE);
	}
	tempack[reval]='\0';
	if(strcmp(tempack,ackstring)!=0)
	{
		printf("CLIENT:DEBUG:Synchronity Vanished\n");
		fflush(NULL);
		exit(EXIT_FAILURE);
	}
	tempack[0]='\0';
	return;
}
void write_on_console(int ctr,int nobc)
{
	int i,cd;
	cd=write(1,"\r",1);
	if(ctr==nobc)
	{
		cd=write(1,"100.00%\n",8);
		return ;
	}
	float ans=((float)ctr)/((float)nobc);
	char ch[3];
	ch[1]='\0';
	for(i=0;i<4;i++)
	{
		ans=ans*10;
		int a=ans;
		ch[0]=a+'0';
		cd=write(1,ch,1);
		if(i==1)
			cd=write(1,".",1);
		ans=ans-a;
	}
	cd=write(1,"%",1);
	return;
}
int getcmd()
{
	long unsigned int sz=100000;
	char *g_comm;
	g_comm=(char *)malloc(100000*sizeof(char));
	if(g_comm==NULL)
	{
		perror("Error occured in malloc");
		exit(1);
	}
	printf("client>");
	getline(&g_comm,&sz,stdin);
	g_comm=(char *)realloc(g_comm,(strlen(g_comm)+2)*sizeof(char));
	if(g_comm==NULL)
	{
		perror("Error occured in realloc");
		exit(1);
	}

	//printf("CLIENT:DEBUG:Point 1\n");

	char **cmd_brk;
	cmd_brk=(char **)malloc(1000*sizeof(char *));
	if(cmd_brk==NULL)
	{
		perror("Error occured in malloc");
		exit(1);
	}

	int ctr=0,no_of_cmds=1000;
	cmd_brk[ctr]=strtok(g_comm," ");
	while(cmd_brk[ctr++]!=NULL)
	{
		if(ctr==no_of_cmds)
		{
			no_of_cmds=((ctr/1000)+1)*1000;
			cmd_brk=(char **)realloc(cmd_brk,no_of_cmds*sizeof(char *));
			if(cmd_brk==NULL)
			{
				perror("Error occured in realloc");
				exit(1);
			}
		}
		cmd_brk[ctr]=strtok(NULL," ");
	}
	cmd_brk[ctr-2][strlen(cmd_brk[ctr-2])-1]='\0';
	cmd_brk=(char **)realloc(cmd_brk,(ctr+1)*sizeof(char *));
	if(cmd_brk==NULL)
	{
		perror("Error occured in realloc");
		exit(1);
	}
	ctr--;
	if(strcmp(cmd_brk[0],"exit")==0)
	{
		free(g_comm);
		free(cmd_brk);
		return 0;
	}
	else if(strcmp(cmd_brk[0],"get")==0)
	{
		int no_of_files=ctr-1;
		if(no_of_files>0)
		{
			char temp[100],cpy[100];
			int n=0,ind=0;
			n=no_of_files;
			while(n>0)
			{
				temp[ind]=(char)((n%10)+'0');
				ind++;
				n=n/10;
			}
			int tmp=0;
			for(int kri=ind-1;kri>=0;kri--)
				cpy[tmp++]=temp[kri];
			cpy[tmp]='\0';
			reval=send(sock_fd,cpy,tmp,0);
			if(reval==-1)
			{
				perror("CLIENT:send Failure");
				exit(EXIT_FAILURE);
			}
			check();
			//printf("CLIENT:DEBUG:Point 2\n");
			//if(no_of_files>0)
			//{
			int i,j,k,fdwf,noc,noccpy,che;
			char path[100000],cters[100];
			for(i=1;i<=no_of_files;i++)
			{
				if(no_of_files>1)
				{
					printf("CLIENT:trying to get file no.%d\n",i);
					fflush(NULL);
				}
				strcpy(path,"./");
				strcat(path,cmd_brk[i]);
				//printf("CLIENT:DEBUG:%s\n",path);
				reval=send(sock_fd,path,strlen(path),0);
				if(reval==-1)
				{
					perror("CLIENT:read Failure");
					exit(EXIT_FAILURE);
				}
				check();
				reval=read(sock_fd,cters,100);
				if(reval==-1)
				{
					perror("CLIENT:read Failure");
					exit(EXIT_FAILURE);
				}
				cters[reval]='\0';
				reval=send(sock_fd,ackstring,strlen(ackstring),0);
				if(reval==-1)
				{
					perror("CLIENT:send Failure");
					exit(EXIT_FAILURE);
				}
				//printf("CLIENT:DEBUG:REVAL for cters was %d\n",reval);
				if(strlen(cters)==2 && cters[0]=='-' && cters[1]=='1')
				{
					printf("CLIENT:SERVER gave a negative response about the file you provided. Basically file does not exist\n");
					fflush(NULL);
					continue;
				}
				fdwf=open(path,O_CREAT | O_WRONLY | O_TRUNC, 0666);
				if(fdwf==-1)
				{
					perror("CLIENT:Unable to create or open the new file");
					free(g_comm);
					free(cmd_brk);
					exit(EXIT_FAILURE);;
				}
				k=1;
				noc=0;
				//printf("CLIENT:DEBUG:cters is %s\n",cters);
				for(j=strlen(cters)-1;j>=0;j--)
				{
					if(cters[j]>='0' && cters[j]<='9')
					{
						noc=noc+(((int)(cters[j]-'0'))*k);
						k=k*10;
					}
					else
					{
						printf("CLIENT:Fatal error, forced exit\n");
						fflush(NULL);
						exit(EXIT_FAILURE);
					}
				}
				noccpy=noc;
				//printf("CLIENT:DEBUG:original noccpy is %d\n",noccpy);
				//printf("CLIENT:DEBUG:Point 3\n");
				while(noccpy>1000)
				{
					che=read(sock_fd,path,1000);
					if(che==-1)
					{
						perror("CLIENT:read Failure");
						exit(EXIT_FAILURE);
					}
					path[1000]='\0';
					reval=send(sock_fd,ackstring,strlen(ackstring),0);
					if(reval==-1)
					{
						perror("CLIENT:send Failure");
						exit(EXIT_FAILURE);
					}
					write(fdwf,path,1000);
					noccpy-=1000;
					write_on_console(noc-noccpy,noc);
				}
				//printf("CLIENT:DEBUG:NU Point\n");
				che=read(sock_fd,path,noccpy);
				if(che==-1)
				{
					perror("CLIENT:read Failure");
					exit(EXIT_FAILURE);
				}
				reval=send(sock_fd,ackstring,strlen(ackstring),0);
				if(reval==-1)
				{
					perror("CLIENT:send Failure");
					exit(EXIT_FAILURE);
				}
				//printf("CLIENT:DEBUG:READ PASSES\n");
				//printf("CLIENT:DEBUG:noccpy1 is %d\n",noccpy);
				path[noccpy]='\0';
				//printf("CLIENT:DEBUG:noccpy2 is %d\n",noccpy);
				write(fdwf,path,noccpy);
				//printf("CLIENT:DEBUG:WRITE PASSES\n");
				noccpy=0;
				write_on_console(noc-noccpy,noc);
				//printf("CLIENT:DEBUG:CONSOLE WRITE PASSES\n");
				close(fdwf);
				printf("Fetched the file\n");
				fflush(NULL);
			}
		}
		else
		{
                	printf("CLIENT:No files provided as argument\n");
			fflush(NULL);
		}
	}
	else
	{
		printf("CLIENT:Non recognisable operation expected by the user\n");
		fflush(NULL);
	}
	//printf("CLIENT:DEBUG:Point 4\n");
	free(g_comm);
	free(cmd_brk);
	return 1;

}
int main(int argc,char *argv[])
{
	sock_fd=socket(AF_INET,SOCK_STREAM,0);
	if(sock_fd==-1)
	{
		perror("CLIENT:socket Failure");
		exit(EXIT_FAILURE);
	}
	memset(&saddr,'0',sizeof(saddr));
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(PORT);
	reval=inet_pton(AF_INET,"127.0.0.1",&saddr.sin_addr);
	if(reval==-1)
	{
		perror("CLIENT:Invalid Address");
		exit(EXIT_FAILURE);
	}
	reval=connect(sock_fd,(struct sockaddr *)&saddr,sizeof(saddr));
	if(reval==-1)
	{
		perror("CLIENT:Connection Failed");
		exit(EXIT_FAILURE);
	}
	printf("CLIENT:Connection Eastablished\n");
	fflush(NULL);
	int yes=1;
	while(yes)
		yes=getcmd();
	reval=send(sock_fd,"exit",4,0);
	if(reval==-1)
	{
		perror("CLIENT:send Failure");
		exit(EXIT_FAILURE);
	}
	check();
	close(sock_fd);
	printf("CLIENT:Connection Closed\n");
	return 0;
}



