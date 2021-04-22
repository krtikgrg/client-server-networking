#include <stdio.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8000
char ackstring[]=".cmd_kk_st_gr_60(history_krikash).txt";
char tempack[100000];
int sock_pclient;
void check()
{
	int reval=read(sock_pclient,tempack,1000);
	if(reval==-1)
	{
		perror("SERVER:read Failure");
		exit(EXIT_FAILURE);
	}
	tempack[reval]='\0';
	if(strcmp(tempack,ackstring)!=0)
	{
		printf("SERVER:DEBUG:Synchronity Vanished\n");
		fflush(NULL);
		exit(EXIT_FAILURE);
	}
	tempack[0]='\0';
	return;
}
int main(int argc, char *argv[])
{
	int server_fd,opt=1,loop=1;
	char c;
	struct sockaddr_in addr;
	unsigned int addrlen=sizeof(addr);

	server_fd=socket(AF_INET, SOCK_STREAM, 0); //ipv4,TCP,IP
	if(server_fd==-1)
	{
		perror("SERVER:socket Failure");
		exit(EXIT_FAILURE);
	}
	int reval;
	printf("SERVER:socket created\n");
	fflush(NULL);
	//removing the error address already in use
	reval=setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	if(reval==-1)
	{
		perror("SERVER:setsockopt Failure");
		exit(EXIT_FAILURE);
	}
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY; //accept from any IP
	addr.sin_port=htons(PORT); //Convert to Big Endian
	reval=bind(server_fd,(struct sockaddr *)&addr,sizeof(addr)); //bind to port 8000
	if(reval==-1)
	{
		perror("SERVER:Binding Failure");
		exit(EXIT_FAILURE);
	}
	while(loop)
	{
		printf("server>I am active\n");
		fflush(NULL);
		reval=listen(server_fd,6);
		if(reval==-1)
		{
			perror("SERVER:listen Failure");
			exit(EXIT_FAILURE);
		}
		sock_pclient=accept(server_fd, (struct sockaddr *)&addr,&addrlen);
		if(sock_pclient==-1)
		{
			perror("SERVER:accept Failure");
			exit(EXIT_FAILURE);
		}
		printf("SERVER:Connection Eastablished\n");
		fflush(NULL);
		char num[100],path[100000],temp[100],temp1[100];
		int k,nof,i,rffd,che,cpynum,ind,ssd,kri;
		while(1)
		{
			reval=read(sock_pclient, num, 100);
			if(reval==-1)
			{
				perror("SERVER:read Failure");
				exit(EXIT_FAILURE);
			}
			num[reval]='\0';
			reval=send(sock_pclient,ackstring,strlen(ackstring),0);
			if(reval==-1)
			{
				perror("SERVER:send Failure");
				exit(EXIT_FAILURE);
			}
			if(strcmp(num,"exit")==0)
				break;
			k=1;
			nof=0;
			for(i=strlen(num)-1;i>=0;i--)
			{
				nof=nof+(((int)(num[i]-'0'))*k);
				k=k*10;
			}
			while(nof--)
			{
				reval=read(sock_pclient,path,100000);
				if(reval==-1)
				{
					perror("SERVER:read Failure");
					exit(EXIT_FAILURE);
				}
				path[reval]='\0';
				reval=send(sock_pclient,ackstring,strlen(ackstring),0);
				if(reval==-1)
				{
					perror("SERVER:send Failure");
					exit(EXIT_FAILURE);
				}
				printf("SERVER:trying to send %s\n",path+2);
				fflush(NULL);
				rffd=open(path,O_RDONLY);
				if(rffd==-1)
				{
					printf("SERVER:Open system call failed, either some internal error or the file does not exist\n");
					fflush(NULL);
					char ret[3];
					ret[0]='-';
					ret[1]='1';
					ret[2]='\0';
					reval=send(sock_pclient,ret,2,0);
					if(reval==-1)
					{
						perror("SERVER:read Failure");
						exit(EXIT_FAILURE);
					}
					check();
					continue;	
				}
				che=lseek(rffd,0,SEEK_END);
				if(che==-1)
				{
					printf("SERVER:Error occured while processing the file\n");
					fflush(NULL);
					exit(EXIT_FAILURE);	
				}
				cpynum=che;
				ind=0;
				while(cpynum>0)	
				{
					temp1[ind++]=(char)((cpynum%10)+'0');
					cpynum=cpynum/10;
				}
				ssd=0;
				for(kri=ind-1;kri>=0;kri--)
				{
					temp[ssd]=temp1[kri];
					ssd++;
				}
				temp[ssd]='\0';
				reval=send(sock_pclient,temp,ssd,0);
				if(reval==-1)
				{
					perror("SERVER:send Failure");
					exit(EXIT_FAILURE);
				}
				check();
				lseek(rffd,0,SEEK_SET);
				//printf("SERVER:DEBUG:Point 1\n");
				while(che>1000)
				{
					reval=read(rffd,path,1000);
					if(reval==-1)
					{
						perror("SERVER:read Failure");
						exit(EXIT_FAILURE);
					}
					path[1000]='\0';
					reval=send(sock_pclient,path,1000,0);
					if(reval==-1)
                                        {
                                                perror("SERVER:send Failure");
                                                exit(EXIT_FAILURE);
                                        }
					check();
					che=che-1000;
				}
				reval=read(rffd,path,che);
				if(reval==-1)
                                {
                                	perror("SERVER:read Failure");
                                        exit(EXIT_FAILURE);
                                }
				path[che]='\0';
				reval=send(sock_pclient,path,che,0);
				if(reval==-1)
                                {
                                        perror("SERVER:send Failure");
                                        exit(EXIT_FAILURE);
                                }
				check();
				close(rffd);
			}
			printf("SERVER:All files sent\n");
			fflush(NULL);
		}
		printf("SERVER: Client ended connection\n");
		fflush(NULL);
		close(sock_pclient);
		printf("SERVER:Do you wish to close me?(y/n)[Only small alphabets are accepted]\n");
		scanf("%c",&c);
		if(c=='y')
		{
			printf("server> exiting :`(\n");
			fflush(NULL);
			loop=0;
		}
		scanf("%c",&c);
	}
	close(server_fd);
	close(sock_pclient);	
	return 0;
}
