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
#include<pthread.h>

using namespace std;

void* client(void *clientarg)
{

	cout<<"client of peer \n";


	cout<<"ENTER NO. OF PORTS TO READ FILE\n";
	cin>>n;

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

	

	char inp;
	cout<<"enter c to connect to peert\n ";
	cin>>inp;
	int con;
	if(inp=='c')
	{
			con=connect(s_fd,(struct sockaddr*)&ip_server,sizeof(ip_server));

				cout<<"after connected \n";
			


			if(con<0)
			{
				perror("CANNOT CONNECT");
				exit(1);
			}

			// char *s;
			// char si[]="hello from peer3";
			// s=si;

			// send(s_fd,s,strlen(s),0);
			

			
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

	}


	
	close(s_fd);
	
	
}


void* server(void *serverarg)
{

	cout<<"server of peer3\n";
	int s_fd;

	s_fd=socket(AF_INET,SOCK_STREAM,0);

	if(s_fd<0)
	{
		perror("ERROR IN SOCKET CREATION");
		exit(EXIT_FAILURE);
	}

	cout<<"Server socket created\n";

	int opt=3;
	int setopt=setsockopt(s_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT ,&opt,sizeof(opt));

	if(setopt<0)
	{
		perror(" SOCKOPT FAILED");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in ip_server,ip_client;
	memset(&ip_server,'0',sizeof(ip_server));

	ip_server.sin_family=AF_INET;
	ip_server.sin_addr.s_addr =INADDR_ANY;
	ip_server.sin_port=htons(2034);

	int bd=bind(s_fd,(struct sockaddr *)&ip_server,sizeof(ip_server));

	if(bd<0)
	{
		perror("BIND FAILED");
		exit(1);
	}



	if(listen(s_fd,10)<0)
	{
		perror("LISTEN FAILED");
		exit(1);
	}

		//perror("LISTEN FAILED");
		//exit(1);
	

	int ip_client_length;

	int client_fd=accept(s_fd,(struct sockaddr *)&ip_client,(socklen_t*)&ip_client_length);

	if(client_fd<0)
	{
		perror("CLIENT FD ERROR");
	}


	char buffer[2048]={0};
	recv(client_fd,buffer,sizeof(buffer),0);

	cout<<buffer<<"\n";
	
  

	//char filep[]="./files/test.txt";
	 //char *filepath;
	 //filepath=filep;
	FILE *fop=fopen("./files/test.txt","rb"); //rb used for non text files
	if(fop<=0)
	{
		perror("unable to open file");
		exit(1);
	}
	
	cout<<"open file \n";
	int bytesent;

	//file size
	fseek(fop,0,SEEK_END); //data after seek_end+0
	int fsize=ftell(fop); //gives file pointer location
	rewind(fop); //sets pointer to beginning of file

	//cout<<"size was \n"<<fsize;

	send(client_fd,&fsize,sizeof(fsize),0);
	//cout<<"sent file size\n";

	while((bytesent=fread(buffer,sizeof(char),2048,fop))>0 && fsize>0 )
	{
		send(client_fd,buffer,bytesent,0);
		memset(buffer,'\0',2048);
		fsize=fsize-bytesent;
	}

	  fclose(fop);
	close(s_fd);
	close(client_fd);

	
}


int main()
{

	pthread_t threadserver,threadclient;
	pthread_create(&threadserver,NULL,server,NULL);
	pthread_create(&threadclient,NULL,client,NULL);
	pthread_join(threadserver,NULL);
	pthread_join(threadclient,NULL);


	//thread server_o(server);
	//thread client_o(client);

	
	// client_o.join();
	// server_o.join();

}