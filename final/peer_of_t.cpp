
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
#include<openssl/sha.h>
#include<bits/stdc++.h>
using namespace std;
pthread_mutex_t lockem;


struct DataforThread
{
	int fd;
	int tport;
	int pport;
	char *file;
	int chunk;
	FILE* filefd;
};
typedef struct DataforThread threaddata;
//sha caculation
string sha256_hash_string (unsigned char hash[SHA256_DIGEST_LENGTH])
{
   stringstream ss;
   for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}
string sha256(const string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

string sha256_file(FILE *file,int file_size)
{
    if(!file) return NULL;
    string finalHash="";
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    const int bufSize = 512;
    unsigned char *buffer =(unsigned char*) malloc(bufSize+1);
    int bytesRead = 0;
    if(!buffer) return NULL;
	int i=0;
	while((bytesRead = fread(buffer, sizeof(char), bufSize, file))){
	SHA256_Update(&sha256, buffer, bytesRead);
	SHA256_Final(hash, &sha256);
        string outputBuffer = sha256_hash_string(hash);
	string finalAnswer = outputBuffer.substr(0, 20);
	finalHash += finalAnswer;
        memset ( buffer , '\0', 512);
}

    fclose(file);
    free(buffer);
    return finalHash;
    }

void *client_download_f(void *downarg)
{

	threaddata *client_d=(threaddata *)downarg;
	int cportn=client_d->pport;
	FILE* fop=client_d->filefd;
	//char* filep=client_d->file;
	//cout<<"filefd is "<<fop<<"\n";
		//socket create
			int s_fd=socket(AF_INET,SOCK_STREAM,0);

			if(s_fd<0)
			{
				perror("ERROR IN SOCKET CREATION");
				pthread_exit(NULL);

			}

		//setopt 
			int opt=3;
			int setopt=setsockopt(s_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT ,&opt,sizeof(opt));

			if(setopt<0)
			{
				perror(" SOCKOPT FAILED");
				pthread_exit(NULL);

			}

			//server structure
				struct sockaddr_in ip_server;
				ip_server.sin_family=AF_INET;
				ip_server.sin_addr.s_addr =inet_addr("127.0.0.1");
			//	cout<<"cport is\n"<<cportn<<"\n";
				ip_server.sin_port=htons(cportn);	

			//connecting to server
			char inp;
			//cout<<"enter c to connect";
			//cin>>inp;
			int con;

			//if(inp=='c')
			//{
				con=connect(s_fd,(struct sockaddr*)&ip_server,sizeof(ip_server));
					if(con<0)
					{
						perror("CANNOT CONNECT");
							pthread_exit(NULL);

						//break;
						//exit(1);
					}
	
			int chunk=client_d->chunk;
			send(s_fd,&chunk,sizeof(chunk),0);

			
			//open file and copy it 
			// char filep[]="./files/testc.txt";
			//  char *filepath;
			//  filepath=client_d->file;
			//  cout<<"file in client is "<<filepath<<"\n";
		//	FILE *fop=fopen(filep,"ab+"); //rb used for non text files
			// if(fop<0)
			// {
			// 	perror("unable to open file");
			// 	pthread_exit(NULL);

				
			// }

			//int fsize=256*1024;
			int fsize=3500;
			int pos=(chunk-1)*fsize;
			    pthread_mutex_lock(&lockem); 
			rewind(fop);

			fseek(fop,pos,SEEK_SET);
			cout<<"writing at "<<pos<<"\n";
			cout<<"chunk is "<<chunk<<"\n";

		char buffer[512]={0};

			int byteread;

			while((byteread=recv(s_fd,buffer,512,0))>0 && fsize>0 )
			{
				//cout<<"bufffer coming as "<<buffer<<"\n";
				//cout<<"data witen "<<;
				fwrite(buffer,sizeof(char),byteread,fop);
				memset(buffer,'\0',512);
				fsize=fsize-byteread;
			}
				    pthread_mutex_unlock(&lockem); 
			//fclose(fop);
		pthread_exit(NULL);

	

}





void *client(void *clientarg)
{

	//cout<<"client in peert\n";
	threaddata *cport=(threaddata *)clientarg;
	int cportn=cport->tport;
	int pportn=cport->pport;
		//socket create
				int s_fd=socket(AF_INET,SOCK_STREAM,0);
					if(s_fd<0)
					{
						perror("ERROR IN SOCKET CREATION");
						pthread_exit(NULL);
					}


		//setopt 
				int opt=3;
				int setopt=setsockopt(s_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT ,&opt,sizeof(opt));
				if(setopt<0)
				{
					perror(" SOCKOPT FAILED");
					pthread_exit(NULL);		
				}

		//server structure

			struct sockaddr_in ip_server;
			ip_server.sin_family=AF_INET;
			ip_server.sin_addr.s_addr =inet_addr("127.0.0.1");
			//cout<<"cport is\n"<<cportn<<"\n";
			ip_server.sin_port=htons(cportn);

			//client structure
				struct sockaddr_in ip_client;
				memset(&ip_client,'0',sizeof(ip_client));

				ip_client.sin_family=AF_INET;
				ip_client.sin_addr.s_addr =inet_addr("127.0.0.1");
				//cout<<"port of peer in client "<<pportn<<endl;
				ip_client.sin_port=htons(pportn);

	int bd=bind(s_fd,(struct sockaddr *)&ip_client,sizeof(ip_client));


	//connecting to server
			//char inp[2048];
			string input;
			int flag=0;
				int con=connect(s_fd,(struct sockaddr*)&ip_server,sizeof(ip_server));
					if(con<0)
					{
						perror("CANNOT CONNECT");
						pthread_exit(NULL);
					}
					cin.ignore();
					string username="";

			while(1)
			{

					char* command;
				int rack,sack=1;
				char msg[1024]={'\0'};


					cout<<"\nenter command to connect\n";
					//char ar[100];
					string input;
					getline(cin,input);
					string input1=input;
					//char inp1[2048];
					////strcpy(inp1,inp);
					//cout<<input<<" ";
					command=strtok((char *)input.c_str()," ");
					//cout<<"\ninput after strtok "<<input<<"\n";
					//cout<<"command is "<<command<<endl;
					

//----------------------------------------------------------------------------------------------------------					
					
					if(strcmp(command,"create_user")==0)
					{
						char *user=strtok(NULL," ");
						char *password=strtok(NULL," ");
						cout<<"enter create_user"<<endl;

						if(user==NULL || password==NULL)
						{
							cout<<"INVALID COMMAND\n";
							continue;
						}
						//cout<<"give username:"<<endl;
						//getline(cin,username);
						//cin.ignore();


						//int a=10;
						//cout<<"sending command "<<input1<<"\n";
						send(s_fd,(char *)input1.c_str(),sizeof(input1),0);
						//int ack;
						recv(s_fd,msg,sizeof(msg),0);
						if(strcmp(msg,"USER_ALREADY")==0)
						{
							cout<<"USER ALREADY PRESENT\n";
							send(s_fd,&sack,sizeof(sack),0);
							continue;
						}

						else if(strcmp(msg,"success")==0)
						{
							cout<<"USER CREATED SUCCESFULLY\n";

			 				//char msg[1024]="success";
							send(s_fd,&sack,sizeof(sack),0);
							continue;
						}

						
							
					}

					if(strcmp(command,"login")==0)

					{
						cout<<"inside login\n";

						char *user=strtok(NULL," ");
						char *password=strtok(NULL," ");
						if(user==NULL || password==NULL)
						{
							cout<<"INVALID COMMAND\n";
							continue;
						}
						if(flag==1)
						{
							cout<<"\nALREADY LOGIN AT THIS TERMINAL !!!\n";
							continue;
						}
						
						
						//username=strtok(NULL," ");
						send(s_fd,(char *)input1.c_str(),sizeof(input1),0);
						memset(msg,'\0',sizeof(msg));
						recv(s_fd,msg,sizeof(msg),0);

						if(strcmp(msg,"correct")==0)
						{
							cout<<"LOGIN SUCCESSFUL\n";
							flag=1;
							username=user;
							//cout<<"username is "<<username<<endl;

						}

						else if(strcmp(msg,"incorrect")==0)
						{
							cout<<"INCORRECT DATA OR USER DOESNT EXIST!!! \n";
						}

						send(s_fd,&sack,sizeof(sack),0);
						continue;



					}
//-------------------------------------------------------------------------------------------------					
					if(strcmp(command,"create_group")==0)

					{
						cout<<"inside create_group\n";


						//checks if command parameter are correct
						char* token;
						if((token=strtok(NULL," "))==NULL)
						{
							cout<<"INVALID COMMAND\n";
							continue;
						}
						//username=strtok(NULL," ");
						if(flag==0)
						{
							cout<<"\nPLEASE LOGIN!!!\n";
							continue;
						}

						//send data
						string tosend="";
						tosend=input1+" "+username;
						send(s_fd,(char *)tosend.c_str(),sizeof(tosend),0);
						
						//recieve msg
						memset(msg,'\0',sizeof(msg));
						recv(s_fd,msg,sizeof(msg),0);

						if(strcmp(msg,"success")==0)
						{
							cout<<"GROUP CREATED SUCCESSFULLY\n";
							
							
							//cout<<"username is "<<username<<endl;

						}

						else if(strcmp(msg,"used")==0)
						{
							cout<<"ALREADY CREATED\n";
						}

						send(s_fd,&sack,sizeof(sack),0);
						continue;
					}
//-------------------------------------------------------------------------------------

					if(strcmp(command,"join_group")==0)

					{
						cout<<"inside join_group\n";


						//checks if command parameter are correct
						char* token;
						if((token=strtok(NULL," "))==NULL)
						{
							cout<<"INVALID COMMAND\n";
							continue;
						}
						//username=strtok(NULL," ");
						if(flag==0)
						{
							cout<<"\nPLEASE LOGIN!!!\n";
							continue;
						}

						//send data
						string tosend="";
						tosend=input1+" "+username;
						send(s_fd,(char *)tosend.c_str(),sizeof(tosend),0);
						
						//recieve msg
						memset(msg,'\0',sizeof(msg));
						recv(s_fd,msg,sizeof(msg),0);

						if(strcmp(msg,"success")==0)
						{
							cout<<"GROUP  REQUEST SENT \n";
							
							
							//cout<<"username is "<<username<<endl;

						}

						else if(strcmp(msg,"fail")==0)
						{
							cout<<"NO SUCH GROUP !!!\n";
						}

						else if(strcmp(msg,"already")==0)
						{
							cout<<"ALREADY IN GROUP !!!\n";
						}


						send(s_fd,&sack,sizeof(sack),0);
						continue;
					}

//--------------------------------------------------------------------------------------------------
					//-----------remaining-----------------
					if(strcmp(command,"leave_group")==0)

					{
						cout<<"inside leave_group\n";


						//checks if command parameter are correct
						char* token;
						if((token=strtok(NULL," "))==NULL)
						{
							cout<<"INVALID COMMAND\n";
							continue;
						}
						//username=strtok(NULL," ");
						if(flag==0)
						{
							cout<<"\nPLEASE LOGIN!!!\n";
							continue;
						}

						//send data
						string tosend="";
						tosend=input1+" "+username;
						send(s_fd,(char *)tosend.c_str(),sizeof(tosend),0);
						
						//recieve msg
						memset(msg,'\0',sizeof(msg));
						recv(s_fd,msg,sizeof(msg),0);

						if(strcmp(msg,"success")==0)
						{
							cout<<"LEFT GROUP\n";
							
							
							//cout<<"username is "<<username<<endl;

						}

						else if(strcmp(msg,"fail")==0)
						{
							cout<<"NO SUCH GROUP !!!\n";
						}

						else if(strcmp(msg,"not")==0)
						{
							cout<<"YOU'RE NOT IN GROUP !!!\n";
						}

						else if(strcmp(msg,"owner")==0)
						{
							cout<<"YOU'RE OWNER CANNOT LEAVE !!!\n";
						}


						send(s_fd,&sack,sizeof(sack),0);
						continue;
					}
//--------------------------------------------------------------------------------------------------------
	
					if(strcmp(command,"list_requests")==0)

					{
						cout<<"inside list_requests\n";


						//checks if command parameter are correct
						char* token;
						if((token=strtok(NULL," "))==NULL)
						{
							cout<<"INVALID COMMAND\n";
							continue;
						}
						//username=strtok(NULL," ");
						if(flag==0)
						{
							cout<<"\nPLEASE LOGIN!!!\n";
							continue;
						}

						//send data
						string tosend="";
						tosend=input1+" "+username;
						send(s_fd,(char *)tosend.c_str(),sizeof(tosend),0);
						
						//recieve msg
						memset(msg,'\0',sizeof(msg));
						recv(s_fd,msg,sizeof(msg),0);

						

						if(strcmp(msg,"fail")==0)
						{
							cout<<"NO SUCH GROUP !!!\n";
						}

						else if(strcmp(msg,"not")==0)
						{
							cout<<"NO PENDING REQUEST!!!\n";
						}

						else
						{
							string msg1=msg;
							//cout<<"received msg: "<<msg1<<endl;
							int msg2=stoi(msg1);
							//cout<<"int converted: "<<msg2<<endl;
							send(s_fd,&sack,sizeof(sack),0);

							cout<<"PENDING REQUESTS ARE :"<<endl;
							for(int i=0;i<msg2;i++)
							{
								char user_p[1024]={'\0'};
								recv(s_fd,user_p,sizeof(user_p),0);
								cout<<user_p<<endl;
								send(s_fd,&sack,sizeof(sack),0);


							}
							recv(s_fd,&sack,sizeof(sack),0);

						}

						

						//recv(s_fd,&sack,sizeof(sack),0);
						send(s_fd,&sack,sizeof(sack),0);
						continue;
					}
//----------------------------------------------------------------------------------------------

					if(strcmp(command,"accept_request")==0)

					{
						cout<<"inside accept_request\n";


						//checks if command parameter are correct
						char* token;
						if((token=strtok(NULL," "))==NULL)
						{
							cout<<"INVALID COMMAND\n";
							continue;
						}
						//username=strtok(NULL," ");
						if(flag==0)
						{
							cout<<"\nPLEASE LOGIN!!!\n";
							continue;
						}

						//send data
						string tosend="";
						tosend=input1+" "+username;
						send(s_fd,(char *)tosend.c_str(),sizeof(tosend),0);
						
						//recieve msg
						memset(msg,'\0',sizeof(msg));
						recv(s_fd,msg,sizeof(msg),0);

						

						if(strcmp(msg,"fail")==0)
						{
							cout<<"NO SUCH GROUP !!!\n";
						}

						else if(strcmp(msg,"not_allowed")==0)
						{
							cout<<"PERMISSION DENIED!!!\n";
						}

						else if(strcmp(msg,"not_there")==0)
						{
							cout<<"USER NOT IN LIST!!!\n";
						}


						else if(strcmp(msg,"done")==0)
						{
							cout<<"REQUEST ACCEPTED!!!\n";
						}

						

						//recv(s_fd,&sack,sizeof(sack),0);
						send(s_fd,&sack,sizeof(sack),0);
						continue;
					}
//----------------------------------------------------------------------------------------------
					if(strcmp(command,"list_groups")==0)

					{
						cout<<"inside list_groups\n";


						
						//username=strtok(NULL," ");
						if(flag==0)
						{
							cout<<"\nPLEASE LOGIN!!!\n";
							continue;
						}

						//send data
						//string tosend="";
						//tosend=input1+" "+username;
						send(s_fd,(char *)input1.c_str(),sizeof(input1),0);
						
						//recieve msg
						memset(msg,'\0',sizeof(msg));
						recv(s_fd,msg,sizeof(msg),0);

						

						if(strcmp(msg,"empty")==0)
						{
							cout<<"NO GROUP !!!\n";
						}

						else
						{
							string msg1=msg;
							//cout<<"received msg: "<<msg1<<endl;
							int msg2=stoi(msg1);
							//cout<<"int converted: "<<msg2<<endl;
							send(s_fd,&sack,sizeof(sack),0);

							cout<<"Groups ARE :"<<endl;
							for(int i=0;i<msg2;i++)
							{
								char user_p[1024]={'\0'};
								recv(s_fd,user_p,sizeof(user_p),0);
								cout<<user_p<<endl;
								send(s_fd,&sack,sizeof(sack),0);


							}
							recv(s_fd,&sack,sizeof(sack),0);

						}

						

						//recv(s_fd,&sack,sizeof(sack),0);
						send(s_fd,&sack,sizeof(sack),0);
						continue;
						// else
						// {
						// 	//cout<<"received "<<msg<<endl;
						// 	char *token=strtok(msg," ");
						// 	while(token!=NULL)
						// 	{
						// 		cout<<token<<endl;
						// 		token=strtok(NULL," ");
						// 	}
						// }


						

			
					}
//----------------------------------------------------------------------------------------------
					if(strcmp(command,"upload_file")==0)
					{
						cout<<"inside upload\n";
						// if(flag==0)
						// {   	
						// 	cout<<"PLEASE LOGIN:\n";
						// 	//cout<<"hiiiiiiiii\n";
						// 	//cin.ignore(NULL);
						
						// 	continue;
						// }
						char *filepath=strtok(NULL," ");
						char *g_id=strtok(NULL," ");
						cout<<"group id is "<<g_id<<"\n";
						cout<<"file  id is "<<filepath<<"\n";
						//string send="";
						
						FILE *fop=fopen(filepath,"rb"); //rb used for non text files
							if(fop<0)
							{
								perror("unable to open file");
								pthread_exit(NULL);
							}


							//calculate file size
							cout<<"open file \n";
							fseek(fop,0,SEEK_END); //data after seek_end+0
							int fsize=ftell(fop); //gives file pointer location
							rewind(fop); //sets pointer to beginning of file
							cout<<fsize<<"\n";


							//caluclating sha
							string sha=sha256_file(fop,fsize);
							//cout<<"sha is \n"<<sha;
							//cout<<"\nsize of sha is\n"<<sha.size()<<"\n";

							//sending commandd and other datas
							string sendata=input1+" "+username+" "+to_string(fsize);
							fclose(fop);
							send(s_fd,(char *)sendata.c_str(),sendata.length(),0);
							int ack;
							recv(s_fd,&ack,sizeof(ack),0);

							//sending sha
							int shai=0;

							while(shai!=sha.size())
							{
								string send_sha=sha.substr(shai,20);
								send(s_fd,(char *)send_sha.c_str(),send_sha.length(),0);
								cout<<"sent sha "<<send_sha<<endl;
								//int ack;
								recv(s_fd,&ack,sizeof(ack),0);


								shai+=20;
							}
							char* msg;

							msg=(char *)"end";
							send(s_fd,msg,sizeof(msg),0);
							recv(s_fd,&ack,sizeof(ack),0);

							continue;


					}
				}

			// else if(strcmp(inp,"c")==0)
			// {
			// 	con=connect(s_fd,(struct sockaddr*)&ip_server,sizeof(ip_server));
			// 		if(con<0)
			// 		{
			// 			perror("CANNOT CONNECT");
			// 				pthread_exit(NULL);
			// 		}
			
			// 		//receive filesize
			// 		int fsize,null_buff_size;
			// 		recv(s_fd,&fsize,sizeof(fsize),0);
			// 		cout<<fsize<<"\n";

			// 		//recieve ports of client 
			// 		int client_port[3];
			// 		recv(s_fd,client_port,sizeof(client_port),0);

			// 		cout<<"c1: "<<client_port[0]<<"\n";
			// 		cout<<"c2: "<<client_port[1]<<"\n";

			// 		cout<<"c3: "<<client_port[2]<<"\n";



			// 		//write null file
			// 		if(fsize<=2048)
			// 		{
			// 			null_buff_size=fsize;
			// 		}
			// 		else
			// 		{
			// 			null_buff_size=2048;
			// 		}
			// 		char buffer_null[null_buff_size];
			// 		memset(buffer_null,'\0',null_buff_size);



			// 		char buffer[2048];
			// 		//open file and copy it 
			// 		// char filep[]="./files/testc.txt";
			// 		 char *filepath;
			// 		 filepath=cport->file;
			// 		// cout<<"file in client is "<<filepath<<"\n";
			// 		FILE *fop=fopen(filepath,"wb+"); //rb used for non text files
			// 		if(fop<0)
			// 		{
			// 			perror("unable to open file");
			// 			pthread_exit(NULL);

						
			// 		}

			// 		int n=fsize;
			// 		while(n>0)
			// 		{
			// 			//cout<<n<<"  n is\n";
			// 			int minus;
			// 			if(n<2048)
			// 			{
			// 				null_buff_size=n;
			// 				minus=fwrite(buffer_null,sizeof(char),null_buff_size,fop);
			// 				//cout<<"wrote "<<minus<<"\n";
			// 				memset(buffer_null,'\0',null_buff_size);
			// 			}
			// 			else
			// 			{
			// 			minus=fwrite(buffer_null,sizeof(char),null_buff_size,fop);
			// 			//cout<<"wrote "<<minus<<"\n";
			// 			memset(buffer_null,'\0',null_buff_size);
			// 				}
			// 			// if(n<=2048)
			// 			// {
			// 			// 	fwrite(buffer_null,sizeof(char),null_buff_size,fop);
			// 			// }
			// 			n=n-minus;

			// 		}
					
			// 			//cout<<"end";
			// 			// fclose(fop);
			// 					//pthread_exit(NULL);

			// 			//return;
			// 			int client=3;
			// 			int i=0;
			// 			//threaddata data_download[client];
			// 				pthread_t client_download[3];
			// 			int chunk_to_take[]={1,2,3};

			// 			pthread_mutex_init(&lock,NULL);
			// 			while(i<3)
			// 			{

			// 				threaddata *data_download=(threaddata *)malloc(sizeof(threaddata));
			// 				cout<<"running CLIENT "<<client_port[i]<<"\n";
			// 				int client_p;
			// 				client_p=client_port[i];
			// 				//threaddata data_download;
			// 				data_download->filefd=fop;
			// 				data_download->file=cport->file;
			// 				data_download->port=client_p;
			// 				data_download->chunk=chunk_to_take[i];
			// 				pthread_create(&client_download[i],NULL,client_download_f,(void *)data_download);
			// 			//	pthread_join(client_download[i],NULL);

			// 				i++;
			// 			}
			// 			i=0;
			// 			while(i<3){
			// 				pthread_join(client_download[i],NULL);
			// 				i++;
			// 			}
			// 			    pthread_mutex_destroy(&lock); 
			// 			fclose(fop);

					

			// }
		
			
			close(s_fd);
			
	
}

void *handlerequest_f(void *request_data)
{
	cout<<"entered \n";
	threaddata *req_data=(threaddata *)request_data;

	 int client_fd=req_data->fd;

	char buffer[512]={0};


	// cout<<buffer<<"\n";

	
	//char filep[]="./files/song.mp3";
	 char *filepath;

	  filepath=req_data->file;
	  cout<<"handle request "<<filepath<<"\n";
	FILE *fop=fopen(filepath,"rb"); //rb used for non text files
	if(fop<0)
	{
		perror("unable to open file");
		pthread_exit(NULL);

		//exit(1);
	}
	
	cout<<"open file \n";
	int bytesent;



	//receive chunk

	int chunk;
	recv(client_fd,&chunk,sizeof(chunk),0);
	cout<<"CHUNK IS "<<chunk<<"\n";

	int fsize=3500;
	int pos=(chunk-1)*fsize;

	fseek(fop,pos,SEEK_SET);
	cout<<"reading from byte "<<pos<<"\n";
	while((bytesent=fread(buffer,sizeof(char),512,fop))>0 && fsize>0 )
	{
		cout<<"bufffer sent as "<<buffer<<"\n";
		send(client_fd,buffer,bytesent,0);
		memset(buffer,'\0',512);
		fsize=fsize-bytesent;
	}

	 fclose(fop);

	 close(client_fd);
	pthread_exit(NULL);
}

void *server(void *clientarg)
{

	//cout<<"server in peert\n";

	threaddata *sport=(threaddata *)clientarg;
	int sportn=sport->pport;
	int s_fd;

	s_fd=socket(AF_INET,SOCK_STREAM,0);

		if(s_fd<0)
		{
			perror("ERROR IN SOCKET CREATION");
			pthread_exit(NULL);
			//exit(EXIT_FAILURE);
		}

	//cout<<"Server socket created\n";

	int opt=3;
	int setopt=setsockopt(s_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT ,&opt,sizeof(opt));

	if(setopt<0)
	{
		perror(" SOCKOPT FAILED");
		pthread_exit(NULL);
		//exit(EXIT_FAILURE);
	}

	struct sockaddr_in ip_server,ip_client;
	memset(&ip_server,'0',sizeof(ip_server));

	ip_server.sin_family=AF_INET;
	ip_server.sin_addr.s_addr =inet_addr("127.0.0.1");
	ip_server.sin_port=htons(sportn);

	int bd=bind(s_fd,(struct sockaddr *)&ip_server,sizeof(ip_server));

	if(bd<0)
	{
		perror("BIND FAILED");
		pthread_exit(NULL);
	}



	if(listen(s_fd,10)<0)
	{
		perror("LISTEN FAILED");
		pthread_exit(NULL);
	}

		//perror("LISTEN FAILED");
		//exit(1);
	

	
		cout<<"wait accept\n";
	int ip_client_length,client_fd;
					pthread_t handlerequest[10];
					int i=0;
	//client_fd=accept(s_fd,(struct sockaddr *)&ip_client,(socklen_t*)&ip_client_length);
	while((client_fd=accept(s_fd,(struct sockaddr *)&ip_client,(socklen_t*)&ip_client_length))>0)
	{

			cout<<"enter while\n";

	//cout<<"inside while1";
				//threaddata request_data;
				threaddata *request_data=(threaddata *)malloc(sizeof(threaddata));

				request_data->fd=client_fd;
				request_data->file=sport->file;


				pthread_create(&handlerequest[i],NULL,handlerequest_f,(void *)request_data);
				pthread_detach(handlerequest[i]);
				i++;
				


	}
//cout<<"at end";
	

	
	close(s_fd);
	

	
}

int main()
{

	char filepath[1000],filepath1[1000];
	threaddata *sport=(threaddata *)malloc(sizeof(threaddata));
	threaddata *cport=(threaddata *)malloc(sizeof(threaddata));
	cout<<"ENTER tracker PORT \n";
	cin>>cport->tport;

	cout<<"ENTER peer PORT \n";
	 cin>>sport->pport;
	 cport->pport=sport->pport;
	 //cout<<"in main \n";
	 //cout<<"tport "<<cport->tport<<endl;
	 //cout<<"pport "<<sport->pport<<endl;
	// cout<<"ENTER FILE path to download in  client:\n";
	// cin>>filepath1;
	// cport.file=filepath1;


	//  cout<<"ENTER FILE path to upload from  server:\n";
	//  cin>>filepath;
	//  sport.file=filepath;

	pthread_t threadserver,threadclient;
	pthread_create(&threadserver,NULL,server,(void *)sport);
	pthread_create(&threadclient,NULL,client,(void *)cport);
	pthread_join(threadserver,NULL);
	pthread_join(threadclient,NULL);

}