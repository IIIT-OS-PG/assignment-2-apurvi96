#include<iostream> //basic inp outpuy
#include<stdio.h>
#include<string.h>
#include<sys/socket.h> //for socket creation
#include<arpa/inet.h> // in_addr structure
#include<sys/types.h> // defines various basic derived types
#include<errno.h>
#include<unistd.h> //read write
#include<cstring>
#include<string>

using namespace std;
int main()
{
	int s_fd;

	s_fd=socket(AF_INET,SOCK_STREAM,0);

	if(s_fd<0)
	{
		perror("ERROR IN SOCKET CREATION");
		exit(EXIT_FAILURE);
	}

	int opt=3;
	int setopt=setsockopt(s_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT ,&opt,sizeof(opt));

	if(setopt<0)
	{
		perror(" SOCKOPT FAILED");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in ip_server;
	ip_server.sin_family=AF_INET;
	ip_server.sin_addr.s_addr =inet_addr("127.0.0.1");
	ip_server.sin_port=htons(8080);

	int con=connect(s_fd,(struct sockaddr*)&ip_server,sizeof(ip_server));

	if(con<0)
	{
		perror("CANNOT CONNECT");
		exit(1);
	}

	char *s;
	char si[]="hello";
	s=si;

	send(s_fd,s,strlen(s),0);
	int fsize;
	recv(s_fd,&fsize,sizeof(fsize),0);
	cout<<fsize<<"\n";

	char buffer[2048]={0};
	//open file and copy it 
	char filep[]="./files/songc.mp3";
	 char *filepath;
	 filepath=filep;
	FILE *fop=fopen(filepath,"wb"); //rb used for non text files
	if(fop<0)
	{
		perror("unable to open file");
		exit(1);
	}

	int byteread;

	while((byteread=recv(s_fd,buffer,2048,0))>0 && fsize>0 )
	{
		fwrite(buffer,sizeof(char),byteread,fop);
		memset(buffer,'\0',2048);
		fsize=fsize-byteread;
	}

	fclose(fop);
	
	return 0;
}