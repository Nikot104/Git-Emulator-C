#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<netinet/in.h>
#include<netdb.h>
#include<signal.h>
#include<dirent.h>
#include<fcntl.h>

typedef struct messageAndLength
{
	char* message;
	long int bytes;
} mAL;

int sockfd = -1;

void breaksock(int signum)
{
	//closed(sockfd);
	listen(sockfd, 0);
	printf("%s\n", "closing socket?");
	
	return;
}

void signalHandler (int signalNumber) 
{
	exit(0);
}

void error(char* msg) 
{
	perror(msg);
	exit(1);
}

void reddit(char* arg)
{
	int fp = open(".configure", O_RDONLY);
	if(fp == -1)
	{
		printf("Could not open configure file. Please create one using the configure command. Simply type ./WTFclient configure <host name> <IP>\n");
		return;
	}
	struct stat fstats;
	stat(".configure", &fstats);
	
	char *args[2];
	
	char *word = malloc(50 * sizeof(char));
	word[0] = '\0';
//	int wordprog = 0;
	char buffer[2];
	buffer[0] = '\0';
	buffer[1] = '\0';
	int fileCounter = 0;

	while(fileCounter < fstats.st_size && strcmp(buffer, "\n") != 0)
	{
		read(fp, &buffer,1);
//		printf("Read from file: %s\n", buffer);
		if(isgraph(buffer[0]))
		{
			strcat(word,buffer);
		}
		fileCounter++;
	}
//	printf("The first line we have is: %s\n", word);
	//Save word
	args[0] = malloc(sizeof(word) * sizeof(char));
	args[0][0] = '\0';
	strcpy(args[0], word);
	
	
	strcpy(word,"");
	buffer[0] = '\0';
	word[0] = '\0';
	
	while(fileCounter < fstats.st_size && strcmp(buffer, "\n") != 0)
	{
		read(fp, &buffer, 1);
//		printf("Read from file: %s\n", buffer);
		if(isgraph(buffer[0]));
		{
			strcat(word,buffer);
		}

		fileCounter++;
	}
//	printf("The second line we have is: %s\n", word);
	//Save word
	args[1] = malloc(sizeof(word) * sizeof(char));
	args[1][0] = '\0';
	strcpy(args[1], word);

	strcpy(word,"");
	word[0] = '\0';

	int finalSize = 0;
	
	if(strlen(args[1]) > strlen(args[0]))
	{
		finalSize = strlen(args[1]);
	}
	else
	{
		finalSize = strlen(args[0]);
	}
	
	char endTime[finalSize*2 + 1];
	endTime[0] = '\0';
	strcat(endTime,args[0]);
	strcat(endTime,"-");
	strcat(endTime,args[1]);
	
//	printf("%s = encripted\n", endTime);

	arg[0] = '\0';
	strcpy(arg, endTime);
	free(args[0]);
	free(args[1]);
	free(word);
	
//	printf("We finished reddit with arg: %s\n", arg);
	
	return;
}


void scanLib(char* proName, char* currDir, char* location)
{
	struct dirent *de;
	//If its the first time around, currDir should be "."
//	printf("Entering directory: %s looking for: %s\n", currDir, proName);
	DIR *dr = opendir(currDir);
	
	if(dr == NULL)
	{
		printf("Could not open directory\n");
		return;
	}
	
	while((de = readdir(dr)) != NULL)
	{
//		printf("%s\n", de->d_name);
		if(strcmp(de->d_name,proName) == 0)
		{
//			printf("We found the project\n");
			char next[sizeof(currDir) + 1 + sizeof(proName) + 1];
			next[0] = '\0';
			strcpy(next,currDir);
			strcat(next,"/");
			strcat(next,proName);
//			printf("It is located at : %s\n", next);
			strcpy(location, next);
			return;
			
		}
		
		if(de->d_type == DT_DIR && de->d_name[0] != '.')
		{
			char next[sizeof(currDir) + 1 + sizeof(de->d_name) + 1];
			next[0] = '\0';
			strcpy(next,currDir);
			strcat(next,"/");
			strcat(next,de->d_name);
//			printf("We are trying to go into: %s\n", next);
			
			scanLib(proName, next, location);
		}
	}

	return;
}



int main(int argc, char*  argv[]) 
{
	signal(SIGINT, breaksock);
	signal(SIGSEGV, signalHandler);
	//initialize stuff
	int netSocket;
	int connectStatus;
	int portNum = -1;
	char hostName[100];
	int n = -1;
	char buffer[256];
	
	struct sockaddr_in sAddInfo; 	//socket server address info struct 
	
	
	buffer[0] = '\0';
//	printf("argv[1] = %s\n", argv[1]);
	strcpy(buffer,argv[1]);
	if(strcmp(buffer,"configure") == 0)
	{
//		printf("You wanted to configure\n");
		
		char ip[strlen(argv[2])+1];
		char port[strlen(argv[3])+1];
		
		int y = 0;
		ip[0] = '\0';
		port[0] = '\0';

//		printf("%s = argv[2] length : %i\n", argv[2], strlen(argv[2]));

		strcpy(ip,argv[2]);
//		ip[strlen(argv[2])] = '\0';
		strcpy(port, argv[3]);
//		port[strlen(argv[3])] = '\0';
		
//		printf("ip = %s, port = %s\n", ip, port);
		
		char* config = ".configure";
		int fp = open(config, O_CREAT | O_WRONLY, 00644);
		
		int z = 0;
		z = write(fp, ip, sizeof(ip)-1);
//		printf("I wrote %s//z=%i vs %i\n",ip,z,sizeof(ip));
		char new[2];
		new[0] = '\n';
		new[1] = '\0';
		z = write(fp, new, sizeof(new)-1);
		z = write(fp, port, sizeof(port)-1);
//		printf("I wrote %s//z=%i vs %i\n",port,z,sizeof(port));
		
		close(fp);		
		
//		printf("Done\n");
		return 0;

	}
	else
	{
		//We assume they have a configure file
		char* data = malloc(500 * sizeof(char));
		data[0] = '\0';
		reddit(data);
		if(data == NULL || strcmp(data, "") == 0)
		{
			printf("Failed to configure\n");
			return;
		}
//		printf("Data: %s\n", data);
		char *ipee;
		ipee = strtok(data, "-");
		
//		printf("HOST: %s\n", ipee);
		strcpy(hostName, ipee);		
		ipee = strtok(NULL, "-");
		int dec = 0;
		int i = 0;
		int j = 0;
		int len = 0;
		len = strlen(ipee);
		for(i = 0; i < len; i++)
		{
			dec = dec * 10 + (ipee[i] - '0');
		}
	
		portNum = dec;

//		printf("PORT: %i\n", portNum);
		
		free(data);
	}


	if (argc < 2) 
	{
		fprintf(stderr, "NOT ENOUGH ARGUMENTS. PLEASE ENTER IP AND PORT");
		exit(1);
	}

	//make the client socket		
	netSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (netSocket < 0) 
	{
		error("ERROR creating the socket");
	}
	
	//Grab the socket
	//portNum = atoi(argv[2]);
//	printf("assigned host\n");



//	struct hostent* serverIP = gethostbyname(argv[1]);
	struct hostent* serverIP = gethostbyname(hostName);
	//ZERO out the socket address info struct
	bzero((char*) &sAddInfo, sizeof(sAddInfo));
	//assign the address family
	sAddInfo.sin_port = htons(portNum);
	sAddInfo.sin_family = AF_INET;
	sAddInfo.sin_addr.s_addr = INADDR_ANY;

//	printf("bcopy?\n");
	bcopy((char*) serverIP->h_addr, (char*) &sAddInfo.sin_addr.s_addr, serverIP->h_length);
//	printf("connect?\n");
	connectStatus = connect(netSocket, (struct sockaddr *) &sAddInfo, sizeof(sAddInfo)); //sizeof address
	
	if (connectStatus < 0) 
	{
		error("ERROR on Connect");
	}
//	printf("bzero\n");
	bzero(buffer, 255); //empties char buffer
	
//	n = read(connectStatus, buffer, 255); //sends the entry to the peeps	
//	if (n < 0)
//	{
//		error("ERROR reading from Socket");
//	} 
	
	// -------Now Connected--------
	//argv[1] = the command
	//argv[2] = additional data / file / project
	//Formatting the command
	buffer[0] = '\0';
	strcpy(buffer, argv[1]);
//	printf("Buffer: %s | argv[1]: %s\n", buffer, argv[1]);
	
	if(strcmp(buffer, "checkout") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]);
//		printf("Requested operation: %s\n", buffer);	
		
		char* projPath = malloc(100*sizeof(char));
		scanLib(argv[2], ".", projPath);
		
		if(strcmp(projPath, "") == 0)
		{
			printf("File not found on client side, continuing\n");
			bzero(buffer, 255);
			buffer[0] = '\0';
			strcpy(buffer, "send ");
			strcat(buffer, argv[2]);
			//send <project name>
			n = write(netSocket, buffer, sizeof(buffer)); //actually writes to the socket
			if ( n < 0)
			{
				error("ERROR writing to socket");
			}
//			printf("Client: %s ", buffer);
			
			
			bzero(buffer,255);
			//size
			n = recv(netSocket, buffer, 255, 0);
			if (n < 0)
			{
				error("ERROR reading from server");
			}
//			printf("\nServer: %s", buffer);
			//Server:<projectSize>
			
			int projSize = 0;
			projSize = str2int(buffer);
//			printf("Creating Empty Tar: %s\n", argv[2]);
			createEmptyTar(argv[2]);
			bzero(buffer, 255);
			buffer[0] = '\0';
			strcpy(buffer, "ready");
			//ready
//			printf("Client: %s\n", buffer);
			n = write(netSocket, buffer, sizeof(buffer)); //actually writes to the socket
			if ( n < 0)
			{
				error("ERROR writing to socket");
			}
			bzero(buffer,255);	
			
			char tarNameo[1000];
			tarNameo[0] = '\0';
			strcpy(tarNameo, argv[2]);
			strcat(tarNameo,".tgz");
			
			char buffe[8096];
			int fd2 = open(tarNameo, O_WRONLY);
			if(fd2 < 0)
			{
				printf("Could not do fd2 recieve operation\n");
			}
//			printf("Recieving file from server!\n");
			//recieve file
			n = recv(netSocket, buffe, projSize,0);
			if (n < 0)
			{
				error("ERROR reading from server");
			}
//			printf("Writing to empty tar file: %s\n", tarNameo);
			write(fd2, buffe, projSize);
			
			unpackTar(tarNameo, "./");
		
			char remo[1000];
			remo[0] = '\0';
			strcpy(remo, "rm -fvd ");
			strcat(remo, argv[2]);
			strcat(remo, ".tgz");
//			printf("Remove tar command: %s\n", remo);
			system(remo);
			
			char prodir[1000];
			prodir[0] = '\0';
			strcpy(prodir, "./");
			strcpy(prodir, argv[2]);
			
			struct dirent *de;
			DIR *dr = opendir(prodir);
			if(dr == NULL)
			{
				printf("Could not open: %s\n", prodir);
				return 0;
			}
			char temp[1000];
			temp[0] = '\0';
			while((de = readdir(dr)) != NULL)
			{
				if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
				{
					strcpy(temp, de->d_name);	
				}

			}
			
			char movo[1000];
			movo[0] = '\0';
			strcpy(movo, "mv ./");
			strcat(movo, argv[2]);
			strcat(movo, "/");
			strcat(movo, temp);
			strcat(movo, " ./");
//			printf("Move to home command: %s\n", movo);
			system(movo);
			
			char reep[1000];
			reep[0] = '\0';
			strcpy(reep, "rm -fvd ./");
			strcat(reep, argv[2]);
//			printf("Remove old directory command: %s\n", reep);
			system(reep);
			
			strcpy(movo, "");
			strcpy(movo, "mv ./");
			strcat(movo, temp);
			strcat(movo, " ./");
			strcat(movo, argv[2]);
//			printf("Rename to project name command: %s\n", movo);
			system(movo);
			
			
			
			close(fd2);
			free(projPath);
			return;
		}
		else
		{
			printf("File found on client side: %s , exiting\n", projPath);
			free(projPath);
			return;
		}
	}
	if(strcmp(buffer, "update") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]);
//		printf("Requested operation: %s\n", buffer);

	}
	if(strcmp(buffer, "upgrade") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]);
//		printf("Requested operation: %s\n", buffer);

	}
	if(strcmp(buffer, "commit") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]);
//		printf("Requested operation: %s\n", buffer);

	}
	if(strcmp(buffer, "push") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]);
//		printf("Requested operation: %s\n", buffer);
		//Send project to server
		
		char projNam[1000];
		projNam[0] = '\0';
		strcpy(projNam, argv[2]);
		createNonEmptyTar(projNam, 0);
//		printf("Naming the tarName\n");
		char tarName[100];
		tarName[0] = '\0';
		strcpy(tarName, projNam);
		strcat(tarName, ".tgz");
		char syst[1000];
		syst[0] = '\0';
		strcpy(syst, "chmod 777 ");
		strcat(syst, tarName);
//		printf("Permission system call: %s\n", syst);
		system(syst);
//		printf("Creating the struct\n");
		struct stat compstats;
		stat(tarName, &compstats);
		int size = compstats.st_size;
	
//		printf("Getting the size\n");
		char* sizeSend = malloc(10*sizeof(char));
//		printf("Doing int2str with %i\n", size);
		int2str(size,sizeSend);
//		printf("sizeSend = %s\n",sizeSend);

		char sendComm[1000];
		sendComm[0] = '\0';
		strcpy(sendComm, "get ");
		strcat(sendComm, sizeSend);
		
//		printf("Writing\n");
		n = write(netSocket, sendComm, strlen(sendComm));
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}
		//size has been sent
//		printf("Done sending size\n");
		bzero(buffer, 255); //empties char buffer
	
		while(strcmp(buffer, "") == 0)
		{
			n = recv(netSocket, buffer, 255, 0);
		}	
		if (n < 0)
			error("ERROR reading from Socket");
//		printf("Server: %s\n", buffer);
		
		if(strcmp(buffer, "ready") == 0)
		{
			mAL info;
			info.bytes = (long int) size;
			info.message = (char*) malloc(size*sizeof(char));
			int fd1 = open(tarName, O_RDONLY);
			if(fd1 < 0)
				printf("Failed to open: %s\n", tarName);
//			printf("Opening to send: %s\n", tarName);
			read(fd1, info.message, size);
			write(netSocket, info.message, size);
//			printf("Finished operation: send %s\n", tarName);
			close(fd1);
			removeTar(tarName);
		}
		else
		{
			printf("Server not ready. Aborting\n");
			free(sizeSend);
		}

		return;
	}
	if(strcmp(buffer, "create") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]);
//		printf("Requested operation: %s\n", buffer);
		
		n = write(netSocket, buffer, sizeof(buffer)); //actually writes to the socket
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}
//		printf("Client: %s ", buffer);
		bzero(buffer,255);
		n = recv(netSocket, buffer, 255, 0);
		if (n < 0)
		{
			error("ERROR reading from server");
		}
//		printf("\nServer: %s", buffer);
		
		char projLoc[1000];
		projLoc[0] = '\0';
		strcpy(projLoc, "./");
		strcat(projLoc, argv[2]);
		mkdir(projLoc, 0700);
		
		strcat(projLoc, "/.manifest");
		int fp = open(projLoc, O_CREAT | O_WRONLY, 00644);
		write(fp, "1\n",strlen("1\n"));
		close(fp);
		return;

	}
	if(strcmp(buffer, "destroy") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]);
//		printf("Requested operation: %s\n", buffer);
		
		n = write(netSocket, buffer, sizeof(buffer)); //actually writes to the socket
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}
//		printf("WHAT IS SENDING: 'MESSAGE: %s ", buffer);
		bzero(buffer,255);
		n = recv(netSocket, buffer, 255, 0);
		if (n < 0)
		{
			error("ERROR reading from server");
		}
//		printf("\nServer: %s", buffer);
		
		char remo[100];
		remo[0] = '\0';
		strcpy(remo, "rm -rf ");
		strcat(remo, argv[2]);

		system(remo);	
		
		return;

	}
	if(strcmp(buffer, "add") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]); //Project Name
		strcat(buffer, " ");
		strcat(buffer, argv[3]); //File
//		printf("Requested operation: %s\n", buffer);
		
		char projectLoc[100];
		projectLoc[0] = '\0';
		strcpy(projectLoc, "./");
		strcat(projectLoc, argv[2]);
		
		char* filePath = malloc(100 * sizeof(char));
		scanLib(argv[2], ".", filePath);
		if(filePath != NULL && strcmp(filePath, "") != 0)
		{
			printf("Project exists at: %s\n", filePath);
		}
		else
		{
			printf("Project could not be found to add\n");
			free(filePath);
			return;
		}
		
		char addition[256];
		addition[0] = '\0';
		strcpy(addition, filePath);
		strcat(addition, "/.manifest");
		
//		printf("Project .manifest location: %s\n", addition);
		
		strcpy(filePath, "");
		filePath[0] = '\0';
		scanLib(argv[3], projectLoc, filePath);
//		printf("We found the added file at: %s\n", filePath);
		
//		printf("We will add: %s : to the .manifest: %s\n",filePath, addition);
		addFileToMan(filePath,addition);

		free(filePath);
		return;

	}
	if(strcmp(buffer, "remove") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]); //Project Name
		strcat(buffer, " ");
		strcat(buffer, argv[3]); //File
//		printf("Requested operation: %s\n", buffer);
		
		char* projPath = malloc(1000 * sizeof(char));
		projPath[0] = '\0';
		
		scanLib(argv[2], ".", projPath);
		if(projPath)
		{
			printf("Project exists\n");
		}
		else
		{
			printf("Project doesn't exist\n");
			free(projPath);
			return;
		}
		char manPath[1000];
		manPath[0] = '\0';
		
		strcpy(manPath,projPath);
		strcat(manPath, "/.manifest");
		
		strcpy(projPath,"");
		projPath[0] = '\0';
		scanLib(argv[3], ".", projPath);
		
		if(strcmp(projPath, "") == 0)
		{
			printf("Could not find %s\n", argv[3]);
			free(projPath);
			return;
		}
		
//		printf("Removing %s from %s\n", projPath, manPath);
		removeFromMan(projPath, manPath);
		free(projPath);
		
		return;

	}
	if(strcmp(buffer, "currentversion") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]);
//		printf("Requested operation: %s\n", buffer);
	
		n = write(netSocket, buffer, sizeof(buffer)); //actually writes to the socket
	
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}
	
		printf("Client: %s ", buffer);
	
		char manPag[8096];
		n = recv(netSocket, manPag, 8095, 0);

		if (n < 0)
		{
			error("ERROR reading from server");
		}
	
//		printf("\nServer: %s", manPag);
		return;
	}
	if(strcmp(buffer, "history") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]);
//		printf("Requested operation: %s\n", buffer);
		printf("Now Connected\n");
		printf("You have commenced a file transfer\n");	
		n = write(netSocket, buffer, sizeof(buffer)); //actually writes to the socket
	
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}
	
//		printf("WHAT IS SENDING: 'MESSAGE: %s ", buffer);
	
		bzero(buffer,255);
		char histPag[8096];
		histPag[0] = '\0';
			
		n = recv(netSocket, histPag, 8095, 0);

		if (n < 0)
		{
			error("ERROR reading from server");
		}
	
//		printf("\nServer: %s", histPag);
		
		return;
	}
	if(strcmp(buffer, "rollback") == 0)
	{
		strcat(buffer, " ");
		strcat(buffer, argv[2]); //Project name
		strcat(buffer, " ");
		strcat(buffer, argv[3]); //Version

//		printf("Requested operation: %s\n", buffer);
		printf("Now Connected\n");
		printf("You have commenced a file transfer\n");	
		n = write(netSocket, buffer, sizeof(buffer)); //actually writes to the socket
	
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}
		
//		printf("Client: %s ", buffer);
	
		bzero(buffer,255);
		strcpy(buffer,"");
		//Getsize
		n = recv(netSocket, buffer, 255, 0);
		if (n < 0)
		{
			error("ERROR reading from server");
		}
	
//		printf("\nServer: %s", buffer);

		int projSize = 0;
		projSize = str2int(buffer);
//		printf("Creating Empty Tar: %s\n", argv[2]);
		createEmptyTar(argv[2]);
		bzero(buffer, 255);
		buffer[0] = '\0';
		strcpy(buffer, "ready");
		//ready
//		printf("Client: %s\n", buffer);
		n = write(netSocket, buffer, sizeof(buffer)); //actually writes to the socket
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}
		bzero(buffer,255);	
			
		char tarNameo[1000];
		tarNameo[0] = '\0';
		strcpy(tarNameo, argv[2]);
		strcat(tarNameo,".tgz");
			
		int fd2 = open(tarNameo, O_WRONLY);
		if(fd2 < 0)
		{
			printf("Could not do fd2 recieve operation\n");				}
		printf("Recieving file from server!\n");
		//recieve file
		n = recv(netSocket, buffer, projSize,0);
if (n < 0)
		{
			error("ERROR reading from server");
		}
//		printf("Writing to empty tar file: %s\n", tarNameo);
		write(fd2, buffer, projSize);
			
		unpackTar(tarNameo, "./");
		close(fd2);
		return;


	}

	printf("Now Connected\n");
	printf("You have commenced a file transfer\n");	
	n = write(netSocket, buffer, sizeof(buffer)); //actually writes to the socket
	
	if ( n < 0)
	{
		error("ERROR writing to socket");
	}
	
	printf("WHAT IS SENDING: 'MESSAGE: %s ", buffer);
	
	bzero(buffer,255);
		
	n = recv(netSocket, buffer, 255, 0);

	if (n < 0)
	{
		error("ERROR reading from server");
	}
	
	printf("\nServer: %s", buffer);
	
	close(netSocket);
	
	return 0;
	
}

int newProjVer(char* projPath)
{
	int newVer = 0;
	struct dirent *dp;
	DIR *dir = opendir(projPath);
	while((dp = readdir(dir)) != NULL)
	{
	
		newVer++;
	
	}
	return newVer;
}


int str2int(char* str)
{
	int final = 0;
	
	int i,j,len;
	len = strlen(str);
	for(i = 0; i<  len; i++)
	{
		final = final * 10 + (str[i] - '0');
	}
//	printf("Converrted string %s to int %i\n", str, final);
	return final;
	
}

void int2str(int numb, char* strNum)
{
//	printf("Taking number %i and converting to string\n", numb);
	char buffer[50];
	sprintf(buffer, "%i", numb);
	strcpy(strNum, buffer);
}


void createEmptyTar(char* projName)
{
	char tarComma[1000];
	tarComma[0] = '\0';
	strcpy(tarComma, "tar -czvf ");
	strcat(tarComma, projName);
	strcat(tarComma, ".tgz -T /dev/null");
//	printf("Creating empty tar using system call: %s\n", tarComma);
	system(tarComma);
}


void createNonEmptyTar(char* projName, int version)
{
	char tarComma[1000];
	tarComma[0] = '\0';
	strcpy(tarComma, " tar czvf ");
	strcat(tarComma, projName);
	strcat(tarComma, ".tgz ");
	strcat(tarComma, projName);
//	strcat(tarComma, "/");
	
//	char ver = malloc(100*sizeof(char));
//	int2str(version, ver);
	
//	strcat(tarComma, ver);
//	printf("System command create nonEmpty tgz: %s\n", tarComma);
	system(tarComma);
//	free(ver);
	
}

//Different from server, fix soon
void unpackTar(char* tarName, char* unpackLoc)
{
	char perrcomm[1000];
	perrcomm[0] = '\0';
	strcpy(perrcomm, "chmod 777 ");
	strcat(perrcomm, tarName);
//	printf("Permission system call: %s\n", perrcomm);
	system(perrcomm);
	
	char* unpackComm[1000];
	unpackComm[0] = '\0';
	strcpy(unpackComm, "tar");
//	strcat(unpackComm, unpackLoc);
	strcat(unpackComm, " xvzf ");
	strcat(unpackComm, tarName);
	strcat(unpackComm, " -C ");
	strcat(unpackComm, unpackLoc);
//	printf("Unpacking %s in Directory: %s\n", unpackLoc, tarName);
//	printf("System call: %s\n", unpackComm);
	system(unpackComm);
	
}

void removeTar(char* tarName)
{
	char* removeComm[1000];
	removeComm[0] = '\0';
	strcpy(removeComm, "rm -frv ");
	strcat(removeComm, tarName);
//	printf("Remove system call: %s\n", removeComm);
	system(removeComm);

}

void createUpdateFile(char* projName)
{
	char* projPath = malloc(1000*sizeof(char));
	projPath[0] = '\0';
	scanLib(projName, ".", projPath);
	
	strcat(projPath, "/History");
	
	int fp = open(O_CREAT | O_WRONLY, 00644);
	close(fp);
	free(projPath);
}

void manCheck(char* projPath)
{
	char path[strlen(projPath) + 10];
	path[0] = '\0';
	strcpy(path, projPath);
	strcat(path, "/.manifest");
//	printf("manChecking: %s\n", path);
	
	int fp = open(path, O_RDONLY);
	
	if(fp < 0)
	{
		printf("Failed to open .manifest, creating new one\n");
		fp = open(path, O_CREAT | O_WRONLY, 00644);
		if(fp < 0)
		{
			printf("Failed to create .manifest\n");
			return;
		}
		write(fp, "1", 1);
	}
	else
	{
		close(fp);
	}
	return;	
	
}

void listFiles(char* direc)
{
	char path[1000];
	struct dirent *dp;
	DIR *dir = opendir(direc);
	if(!dir)
		return;
	
	while((dp = readdir(dir)) != NULL)
	{
		if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			strcpy(path,direc);
			strcat(path,"/");
			strcat(path,dp->d_name);
//			printf("%s\n", path);
			listFiles(path);

		}
	}
	
}

int checkfileinman(char* manPath, char* filePath)
{
	char manWork[8096];
	int manCount = 0;
	manWork[0] = '\0';
	int fp = open(manPath, O_RDWR);
	struct stat fstats;
	stat(manPath, &fstats);
	
	char letter[2];
	letter[0] = '\0';
	letter[1] = '\0';
	while(manCount < fstats.st_size)
	{
		read(fp, &letter, 1);
	//	printf("Read from file: %s\n", buffer);
		strcat(manWork,letter);
		manCount++;
	}
//	printf("Looking for %s in %s\n", filePath, manWork);
	
	char *token;
	token = strstr(manWork, filePath);
	if(token)
	{
		close(fp);
		return 1; //Success	
	}
	close(fp);
	return 0; //Fail
}

int getFileVersion(char* manDir, char* filePath)
{
	int ver = 1;
	char manWork[8096];
	int manCount = 0;
	manWork[0] = '\0';
	int fp = open(manDir, O_RDWR);
	struct stat fstats;
	stat(manDir, &fstats);
	
	char letter[2];
	letter[0] = '\0';
	letter[1] = '\0';
	while(manCount < fstats.st_size)
	{
		read(fp, &letter, 1);
	//	printf("Read from file: %s\n", buffer);
		strcat(manWork,letter);
		manCount++;
	}
	
	char* p;
	char* token;
	p = strstr(manWork, filePath);
	if(p)
	{
		token = strtok(manWork, filePath); //Will get everything before file name
		token = strtok(NULL, " ");
		token = strtok(NULL, " ");
//		printf("%s is version: %s\n", filePath, token);
		
		int dec = 0;
		int i,len;
		len = strlen(token);
		for( i = 0; i < len; i++)
		{
			dec = dec * 10 + (token[i] - '0');
		}
		ver = dec;
		
	}
	else
	{
		ver = 0;
	}
	
	return ver;
}

void addFileToMan(char* fileDirec,char* manDirec) //Open .manifest and add a line to it
{
	char manWork[8096];
	int manCount = 0;
	manWork[0] = '\0';
	int fp = open(manDirec, O_RDWR);
	struct stat fstats;
	stat(manDirec, &fstats);
	
	char letter[2];
	letter[0] = '\0';
	letter[1] = '\0';
	while(manCount < fstats.st_size)
	{
		read(fp, &letter, 1);
	//	printf("Read from file: %s\n", buffer);
		strcat(manWork,letter);
		manCount++;
	}
	close(fp);
	
	char* p;
	p = strstr(manWork, fileDirec);
	if(p)
	{
		printf("File already in .manifest\n");
	}
	else
	{
		printf("File not yet in .manifest\n");
		char toAdd[1000];
		toAdd[0] = '\0';
		strcpy(toAdd, fileDirec);
		strcat(toAdd, " 1 ");
		
		int h = 0;
		h = createHash(fileDirec);
		char* hi = malloc(1000*sizeof(char));
		hi[0] = '\0';
		int2str(h, hi);
		strcat(toAdd, hi);
		free(hi);
//		strcat(toAdd, "");
//		printf("Adding to manifest: %s\n", toAdd);
		
		strcat(manWork, toAdd);
		

//		printf("Total manifest: %s\n", manWork);
	
		char sys[1000];
		sys[0] = '\0';
		strcpy(sys, "rm -fv ");
		strcat(sys, manDirec);
//		printf("System call: %s\n", sys);
		
		int fr = open(manDirec, O_CREAT | O_WRONLY, 00644);
		if(fr < 0)
		{
			printf("Failed to create new manifest\n");
			return;
		}
	
		
		char* manWo = malloc(8096*sizeof(char));
		manWo[0] = '\0';
		strcpy(manWo, manWork);
				
//		printf("New manifest size ~%i\n", strlen(manWo));
		
		write(fr, manWo, strlen(manWo));
			
		free(manWo);
		close(fr);
	}

	return;
}

void removeFromMan(char* fileDirec, char* manDirec) //Takes a substring (fileDirec) and removes it from the manifest, it does this by deleting the manifest and then recreating it
{
//	printf("manDirec: %s, trying to remove %s\n", manDirec, fileDirec);
	char manWork[8096];
	int manCount = 0;
	manWork[0] = '\0';
	int fp = open(manDirec, O_RDWR);
	struct stat fstats;
	stat(manDirec, &fstats);
	
	char letter[2];
	letter[0] = '\0';
	letter[1] = '\0';
	while(manCount < fstats.st_size)
	{
		read(fp, &letter, 1);
	//	printf("Read from file: %s\n", buffer);
		strcat(manWork,letter);
		manCount++;
	}
	
	close(fp);
	int status = remove(manDirec);
	if(status != 0)
		printf("Removal Failed\n");
	
	//remove that part from the manFile
	
	//test = manWork
	char manWorko[8096];
	strcpy(manWorko,manWork);
	char* token;
	token = strtok(manWorko, "\n");
	while(token != NULL)
	{
		if(strstr(token, fileDirec))
		{
			break;
		}
		token = strtok(NULL, "\n");
	}
//	printf("Found the substring containing: %s // %s\n", fileDirec, token);
	
	char tokenNew[1000];
	strcpy(tokenNew, "\n");
	strcat(tokenNew, token);
	char *manCopy = malloc(8096*sizeof(char));
	manCopy[0] = '\0';
	strcpy(manCopy, manWork);
//	printf("file WITH substring: %s\n",manCopy);
	removeSubstr(manCopy,tokenNew); 
//	printf("file WITHOUT substring: %s\n",manCopy);
	
	///op = file
	
	int fd = open(manDirec, O_CREAT | O_WRONLY, 00644);
		
	write(fd, manCopy, strlen(manCopy));
	/*
	while(manCount < sizeof(manWork))
	{
		write(fd, manWork[manCount], 1);
		manCount++;
	}
	*/
	free(manCopy);
	return;

}


void removeSubstr(char* str, char* sub) //Takes the read in file, and removes a substring from it (token) then fixes it... to be used to update manifests
{
	char *match;
	int len = strlen(sub);
	while((match = strstr(str, sub)))
	{
		*match = '\0';
		strcat(str, match+len);
	}
}

void createFile(char* filePath, char* toDo) //Creates a file and writes to it... to be used for manifest creation
{
	int fp = open(filePath, O_CREAT | O_WRONLY, 00644);
	int fileCount = 0;
	while(fileCount < sizeof(toDo))
	{
		write(fp, toDo[fileCount], 1);
	}
	close(fp);
}

void checkHist() //Make sure the History file exists, to use when server starts up for first time.
{
	int fp = open("History", O_RDWR);
	if(fp < 0)
	{
		printf("History File does not exist, creating new one\n");
		fp = open("History", O_CREAT | O_WRONLY, 00644);
		close(fp);
	}
	else
	{
		printf("History file exists\n");
		close(fp);
	}
	
}

void writeToHist(char* histDirec, char* line)
{
	checkHist();
	char histWork[8096];
	int histCount = 0;
	histWork[0] = '\0';
	int fp = open(histDirec, O_RDWR);
	struct stat fstats;
	stat(histDirec, &fstats);
	
	char letter[2];
	letter[0] = '\0';
	letter[1] = '\0';
	while(histCount < fstats.st_size)
	{
		read(fp, letter[0], 1);
		strcat(histWork, letter);
		histCount++;
	}
	strcat(histWork, "\n");
	strcat(histWork, line);
	histCount = 0;
	while(histCount < sizeof(histWork))
	{
		write(fp, histWork[histCount], 1);
		histCount++;
	}
	close(fp);
	return;
}

void updateProVer(char* projName)
{
	struct dirent *de;
	DIR *dr = opendir(projName);
	//Rummage through the projectDir, "./projName" and 
	if(dr == NULL)
	{
		printf("Could not open directory\n");
		return;
	}
	
	int lastSize = 0;
	while((de = readdir(dr)) != NULL)
	{
		printf("%s\n", de->d_name);
		if(strcmp(de->d_name, ".Manifest") != 0 && strcmp(de->d_name, "History") != 0)
		{
			
		}
	}


}

void createDump()
{
	mkdir("./Dump", 0700);
}
void moveToDump(char* fileDir) //Read a file from fileDir, create new file in Dump with just the filename, it should be the last token from fileDir. Then copy the contents to the new file. close both.
{
	char fileRed[8096];
	fileRed[0] = '\0';
	int fp = open(fileDir, O_RDONLY);
	struct stat fstats;
	stat(fileDir, &fstats);
	char buffer;
	int fileCount = 0;
	
	while(fileCount < fstats.st_size)
	{
		read(fp,fileRed[fileCount],1);
		fileCount++;
	}	
	close(fp);
	
	//create new file in ./Dump with O_CREAT
	
}

void buildDir(char* directo)
{
	//tokenize the path, every token must be concatinated to old one, check to see if directory exists
	
	char buffer[sizeof(directo)];
	buffer[0] = '\0';
	char* token;
	token = strtok(directo, "/");
	strcat(buffer, token);
	strcat(buffer, "/");
	token = strtok(NULL, "/");

	while(token != NULL)
	{
		strcat(buffer, token);
//		printf("Making directory: %s\n", buffer);
		if(mkdir(buffer,0700) == -1)
		{
			printf("Error creating directory\n");
		}
		strcat(buffer, "/");
	
		token = strtok(NULL, "/");
	}

}

void tarTheDump(char* toSend) // Can also be used by projects!... Just the name!
{
	char tarCom[1000];
	tarCom[0] = '\0';
	
	strcpy(tarCom, "tar cfz ");
	strcat(tarCom, toSend);
	strcat(tarCom, ".tgz ");
	strcat(tarCom, toSend);
	
//	printf("System command to tar: %s\n", tarCom);
	
	system(tarCom);
	
}

void untarDump(char* tarName)
{
	char tarCom[1000];
	tarCom[0] = '\0';
	char tarCpy[1000];
	tarCpy[0] = '\0';
	strcpy(tarCpy,tarName);
	char* token;
	token = strtok(tarCpy, ".");
//	printf("Name of project we are decompressing: %s\n", token);
	
	char* tarDir = malloc(1000*sizeof(char));
	tarDir[0] = '\0';
	scanLib(token, ".", tarDir);
	//Only in server
	char buffer[50];
	
	int i = 0;
	i = newProjVer(tarDir);
	sprintf(buffer, "%i", i);
	strcat(tarDir, "/");
	strcat(tarDir, buffer);
	
	mkdir(tarDir, 0700);
	
	//Only in server
	strcpy(tarCom, "tar -C ");
	strcat(tarCom, tarDir);
	strcat(tarCom, " -zxvf ");
	strcat(tarCom, tarName);
	
//	printf("System command to tar: %s\n", tarCom);
	
	system(tarCom);
	free(tarDir);

}
void removeDump(char* toRemove) //This can be used to remove the dump folder, and the dump tar... if not then toRemove should have the .tar file
{
	if(strcmp(toRemove, "Dump") == 0)
	{
//		printf("Removing ./Dump\n");
		remove("./Dump");
		remove("./Dump.tgz");
	}
	else
	{
		char tarDump[100];
		tarDump[0] = '\0';
		strcpy(tarDump, "./");
		strcat(tarDump, toRemove);
		
//		printf("Removing %s\n", tarDump);
		remove(tarDump);
	}
}


void checkout(char* projName, char* sendMessage)
{
	
	char sendInfo[8096];
	sendInfo[0] = '\0';
	
	struct dirent *de;
	DIR *dr = opendir(projName);
	
	if(dr == NULL)
	{
		printf("Could not open directory\n");
		return;
	}
	
	while((de = readdir(dr)) != NULL)
	{
//		printf("%s\n", de->d_name);
		if(strcmp(de->d_name,projName) == 0)
		{
			
		}
	}

}
		
//Reads a file, adds up ascii values and thats the hash
int createHash(char* projDir)
{
	int hashy = 0;
	int fp = open(projDir, O_RDONLY);
	struct stat fstats;
	stat(projDir, &fstats);
	int fileCount = 0;
	char letter;
	while(fileCount < fstats.st_size)
	{
	 	read(fp, letter, 1);
		hashy += (int)letter;
		fileCount++;
	}
//	printf("The final hashcode for %s is %i\n", projDir, hashy);
	close(fp);
	return fstats.st_size;

}

void manBattle(char* clientMan, char* serverMan, char* outputMan)
{
	char cli[8096];
	cli[0] = '\0';
	strcpy(cli, clientMan);
	
	char ser[8096];
	ser[0] = '\0';
	strcpy(ser, serverMan);
	
	char* serTok;
	serTok = strtok(ser, "\n");
	int serversion;
	serversion = str2int(serTok);
	serTok = strtok(NULL, "\n"); //Skip version
	
	char** tokenArr;
	tokenArr = (char**) malloc(sizeof(char*) * 20); // holds 20 files
	int tokenCount = 0;
	
	while(serTok != NULL)
	{
		tokenArr[tokenCount] = malloc(sizeof(char) * sizeof(serTok));
		tokenArr[tokenCount][0] = '\0';
		strcpy(tokenArr[tokenCount], serTok);
		serTok = strtok(NULL, "\n");
		tokenCount++;
	}
//	printf("Token array 1 filled with %i tokens\n",tokenCount);
	
	
	char* cliTok;
	cliTok = strtok(cli, "\n");
	int cliversion;
	cliversion = str2int(cliTok);
	cliTok = strtok(NULL, "\n"); 

	if(serversion <= cliversion)
	{
		printf("Server manifest cannot be out of date\n");
		return;
	}
	char** tokenArrCli;
	tokenArrCli = (char**) malloc(sizeof(char*) * 20); // holds 20 files
	int tokenCountC = 0;
	
	while(cliTok != NULL)
	{
		tokenArrCli[tokenCountC] = malloc(sizeof(char) * sizeof(cliTok));
		tokenArrCli[tokenCountC][0] = '\0';
		strcpy(tokenArrCli[tokenCountC], cliTok);
		cliTok = strtok(NULL, "\n");
		tokenCountC++;
	}
//	printf("Token array 2 filled with %i tokens\n",tokenCountC);
	
	int flags[tokenCountC];
	
	int i = 0;
	int j = 0;
	for(i = 0; i < tokenCountC; i++)
	{
		flags[i] = -1;
		char* disect;
		disect = strtok(tokenArrCli[i], " ");
//		printf("Disect = %s\n", disect);
		
		for(j = 0; j < tokenCount; j++)
		{
			char* p;
			p = strstr(tokenArr[j], disect);
			
			if(p)
			{
				printf("File exists on other manifest\n");
				disect = strtok(NULL, " ");
				p = strstr(tokenArr[j], disect);
				if(p)
				{
					printf("Files have the same version\n");
					flags[i] = 0;
				}
				else
				{
					printf("Files have different version\n");
					flags[i] = 2;
				}
			}
		}
		if(flags[i] == -1)
		{
			printf("File not found on server, thus must be uploaded\n");
			flags[i] = 1;
		}
		
		
	}

}





