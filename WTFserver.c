#include<stdlib.h>
#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<dirent.h>
#include<signal.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>

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
	exit(1);
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

void* socketThread(void *arg)
{
	int n = 0;
	char buffer[256];
	buffer[0] = '\0';
	int newSock = *((int*)arg);
//	read(newSock, buffer, 255);
//	printf("Recieved from client: %s\n",buffer);
	
//	strcpy(buffer, "");

	//GetCommand
	bzero(buffer, 255); //empties char buffer
	n = recv(newSock, buffer, 255, 0);	
	if (n < 0)
		error("ERROR reading from Socket");
	printf("MESSAGE FROM CLIENT: %s\n", buffer);
	
	if(strcmp(buffer, "") == 0)
	{
		printf("Server not needed\n");
	}
	else
	{
	char* tokey;
	tokey = strtok(buffer, " ");
	printf("Keyword: %s\n", tokey);
	if(strcmp(tokey, "send") == 0)
	{
		//SendSize
		tokey = strtok(NULL, " ");
		printf("Going to send project: %s\n", tokey);
		char projDir[1000];
		projDir[0] = '\0';
		
		int prover = 0;
		prover = newProjVer(tokey);
		printf("Got project version: %i\n", prover);
		printf("Creating nonemptytar\n");
		createNonEmptyTar(tokey, prover);
		printf("Naming the tarName\n");
		char tarName[100];
		tarName[0] = '\0';
		strcpy(tarName, tokey);
		strcat(tarName, ".tgz");
		char syst[1000];
		syst[0] = '\0';
		strcpy(syst, "chmod 777 ");
		strcat(syst, tarName);
		printf("Permission system call: %s\n", syst);
		system(syst);
		printf("Creating the struct\n");
		struct stat compstats;
		stat(tarName, &compstats);
		int size = compstats.st_size;
	
		printf("Getting the size\n");
		char* sizeSend = malloc(10*sizeof(char));
		printf("Doing int2str with %i\n", size);
		int2str(size,sizeSend);
		printf("sizeSend = %s\n",sizeSend);

		printf("Writing\n");
		n = write(newSock, sizeSend, strlen(sizeSend));
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}
		//size has been sent
		
		bzero(buffer, 255); //empties char buffer
	
		while(strcmp(buffer, "") == 0)
		{
			n = recv(newSock, buffer, 255, 0);
		}	
		if (n < 0)
			error("ERROR reading from Socket");
		printf("MESSAGE FROM CLIENT: %s\n", buffer);
		
		if(strcmp(buffer, "ready") == 0)
		{
			mAL info;
			info.bytes = (long int) size;
			info.message = (char*) malloc(size*sizeof(char));
			int fd1 = open(tarName, O_RDONLY);
			read(fd1, info.message, size);
			write(newSock, info.message, size);
			printf("Finished operation: send %s\n", tarName);
			close(fd1);
			removeTar(tarName);
		}
		else
		{
			printf("Client not ready. Aborting\n");
			free(sizeSend);
		}
		
	}
	if(strcmp(tokey, "currentversion") == 0)
	{
		tokey = strtok(NULL, " ");
		char projNam[1000];
		projNam[0] = '\0';
		strcpy(projNam, tokey);
		strcat(projNam, "/");
		
		struct dirent *pd;
		DIR *pDir;
	
		int ver = -2;
		pDir = opendir(tokey);
		while((pd = readdir(pDir)) != NULL)
		{
			ver++;
		}
		if(ver <= 0)
		{
			printf("No such project yet exists\n");
		}
		else
		{
			char manComm[1000];
			manComm[0] = '\0';
			strcpy(manComm, "./");
			strcat(manComm,tokey);
			strcat(manComm, "/");
			char *isnt = malloc(100*sizeof(char));
			isnt[0] = '\0';
			int2str(ver,isnt);
			strcat(manComm, isnt);
			strcat(manComm,"/.manifest");
			printf(".manifest for %s located at %s\n", tokey, manComm);
			free(isnt);
			int fp = open(manComm, O_RDONLY);
			if(fp < 0)
			{
				printf("Could not open .manifest");
				return;
			}
			char manPag[8096];
			manPag[0] = '\0';
			
			char lettro[2];
			lettro[0] = '\0';
			lettro[1] = '\0';
			
			struct stat buf;
			fstat(fp, &buf);
			int count = 0;
			
			while(count < buf.st_size)
			{
				read(fp, lettro, 1);
				strcat(manPag, lettro);
				count++;
			}

			printf("Writing\n");
			n = write(newSock, manPag, strlen(manPag));
			if ( n < 0)
			{
				error("ERROR writing to socket");
			}

		}

	}
	if(strcmp(tokey, "history") == 0)
	{
		tokey = strtok(NULL, " ");
		char projNam[1000];
		projNam[0] = '\0';
		strcpy(projNam, tokey);
		
		char histNam[1000];
		projNam[0] = '\0';
		strcpy(histNam, tokey);
		strcpy(histNam, "/History");
		
		char histor[8096];
		histor[0] = '\0';
		int fp = open(histNam, O_RDONLY);
		struct stat st;
		stat(histNam, &st);
		int size = st.st_size;
		int fileCount = 0;
		char let[2];
		let[0] = '\0';
		let[1] = '\0';
		while(fileCount < size)
		{
			read(fp,let,1);
			strcat(histor, let);
			fileCount++;
		}
		printf("We read the whole history file: %s\n", histor);
		printf("Writing\n");
		n = write(newSock, histor, strlen(histor));
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}

		close(fp);
		return;

	}
	if(strcmp(tokey, "create") == 0)
	{
		tokey = strtok(NULL, " ");
		printf("Going to create project: %s\n", tokey);
		char projLoc[1000];
		projLoc[0] = '\0';
		strcpy(projLoc, "./");
		strcat(projLoc, tokey);
		mkdir(projLoc, 0700);
		
		strcat(projLoc, "/.manifest");
		int fp = open(projLoc, O_CREAT | O_WRONLY, 00644);
		write(fp, "1\n",strlen("1\n"));
		close(fp);

	}
	if(strcmp(tokey, "destroy") == 0)
	{
		tokey = strtok(NULL, " ");
		printf("Going to destroy project: %s\n", tokey);
		char* removeComm[1000];
		removeComm[0] = '\0';
		strcpy(removeComm, "rm -frv ");
		strcat(removeComm, tokey);
		printf("Destroy system call: %s\n", removeComm);
		system(removeComm);

	}
	if(strcmp(tokey, "get") == 0)
	{
		tokey = strtok(NULL, " ");
		printf("size:%s length of tokey:%i\n",tokey, strlen(tokey));
		int sizeo = 0;
		int k, l, len;
		len = strlen(tokey);
		for(k = 0; k < len; k++)
		{
			sizeo = sizeo * 10 + (tokey[k] - '0');
		}
		printf("Preparing to recieve project of size: %i\n", sizeo);
		createEmptyTar(); //Gonna be called reception.tgz
		char red[1000];
		red[0] = '\0';
		strcpy(red, "ready");
		printf("Sending ready\n");
		n = write(newSock, red, strlen(red));
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}
		//size has been sent
		
		bzero(buffer, 255); //empties char buffer
		char projHold[8096];
		projHold[0] = '\0';
		
		int total = 0;
		while(total < sizeo)
		{
			n = recv(newSock, projHold, sizeo, 0);
			printf("Read %i bytes\n", n);
			total += n;
		}
		
		if (n < 0)
			error("ERROR reading from Socket");
		int fid = open("reception.tgz", O_WRONLY);
		if(fid<0)
			printf("Failed to open reception.tgz\n");
		total = 0;
		while(total < sizeo)
		{
			n = write(fid, projHold, sizeo);
			printf("Wrote %i bytes to reception.tgz\n", n);
			total += n;	
		}
		
		close(fid);
		printf("Making the reception folder and unpacking it there\n");
		system("mkdir reception"); //create reception folder
		//system("tar -C /reception -xvf reception.tar"); //extract to reception folder
		unpackTar("reception.tgz", "./reception");
		printf("Done with creating and filling reception\n");
		char projNamo[1000];
		projNamo[0] = '\0';
		struct dirent *de;
		DIR *dr = opendir("./reception");
		if(dr == NULL)
		{
			printf("Failed to open reception\n");
			return;
		}
		while((de = readdir(dr)) != NULL)
		{
			if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
			{
				printf("In reception folder we found: %s\n", de->d_name);
				strcpy(projNamo, de->d_name);
				printf("ProjNamo:%s\n", projNamo);
			}
		}
		
		closedir(dr);
		
		char homeDir[1000];
		homeDir[0] = '\0';
		strcpy(homeDir, "/reception/");
		strcat(homeDir, projNamo);
		printf("Original path of project: %s\n", homeDir);
		printf("Getting new project version\n");
		int pover = 0;
		pover = newProjVer(projNamo) + 1;
		
		char *vers = malloc(100*sizeof(char));
		vers[0] = '\0';
		int2str(pover, vers);
		
		char sycall[1000];
		sycall[0] = '\0';
		strcpy(sycall, "mv .");
		strcat(sycall, homeDir);
		strcat(sycall, " ./reception/");
		strcat(sycall, vers);
		printf("Rename system call: %s\n", sycall);
		system(sycall);
		
		strcpy(sycall, "");
		strcpy(sycall, "mv ./reception/");
		strcat(sycall, vers);
		strcat(sycall, " ./");
		strcat(sycall, projNamo);
		printf("Move system call: %s\n", sycall);
		system(sycall);
		
		system("rm -fvr ./reception");
		system("rm -fv ./reception.tgz");
		free(vers);
		//bacc

	}
	if(strcmp(tokey, "rollback") == 0)
	{
		tokey = strtok(NULL, " ");
		printf("Going to send project: %s\n", tokey);
		char projNam[1000];
		projNam[0] = '\0';
		strcpy(projNam, tokey);
		tokey = strtok(NULL, " ");
		char projVer[1000];
		projVer[0] = '\0';
		strcpy(projVer, tokey);

		char projDir[1000];
		projDir[0] = '\0';
		
		int prover = 0;
		prover = str2int(projVer);
		printf("Got project version: %i\n", prover);
		printf("Creating nonemptytar\n");
		createNonEmptyTar(projNam, prover);
		printf("Naming the tarName\n");
		char tarName[100];
		tarName[0] = '\0';
		strcpy(tarName, projNam);
		strcat(tarName, ".tgz");
		char syst[1000];
		syst[0] = '\0';
		strcpy(syst, "chmod 777 ");
		strcat(syst, tarName);
		printf("Permission system call: %s\n", syst);
		system(syst);
		printf("Creating the struct\n");
		struct stat compstats;
		stat(tarName, &compstats);
		int size = compstats.st_size;
	
		printf("Getting the size\n");
		char* sizeSend = malloc(10*sizeof(char));
		printf("Doing int2str with %i\n", size);
		int2str(size,sizeSend);
		printf("sizeSend = %s\n",sizeSend);

		printf("Writing\n");
		n = write(newSock, sizeSend, strlen(sizeSend));
		if ( n < 0)
		{
			error("ERROR writing to socket");
		}
		//size has been sent
		
		bzero(buffer, 255); //empties char buffer
	
		while(strcmp(buffer, "") == 0)
		{
			n = recv(newSock, buffer, 255, 0);
		}	
		if (n < 0)
			error("ERROR reading from Socket");
		printf("MESSAGE FROM CLIENT: %s\n", buffer);
		
		if(strcmp(buffer, "ready") == 0)
		{
			mAL info;
			info.bytes = (long int) size;
			info.message = (char*) malloc(size*sizeof(char));
			int fd1 = open(tarName, O_RDONLY);
			read(fd1, info.message, size);
			write(newSock, info.message, size);
			printf("Finished operation: send %s\n", tarName);
			close(fd1);
			removeTar(tarName);
		}
		else
		{
			printf("Client not ready. Aborting\n");
			free(sizeSend);
		}

	}
	}
	printf("Exit\n");
	close(newSock);
	pthread_exit(NULL);
}


pthread_mutex_t projLock;
pthread_mutex_t fileLock;
pthread_mutex_t manLock;
pthread_mutex_t histLock;
int main(int argc, char*  argv[]) 
{
	signal(SIGINT, breaksock);
	signal(SIGSEGV, signalHandler);
	
	int sockfd = -1;
	int newsockfd = -1;
	int portNum = -1;
	int cliSize = -1;
	int n = -1;
	char buffer[256];

	struct sockaddr_in sAddInfo;
	struct sockaddr_in cAddInfo;
	
	if (argc < 2) 
	{
		fprintf(stderr, "NOT ENOUGH ARGUMENTS. PLEASE ENTER IP AND PORT");
		exit(1);
	}

	//Grab the ip and socket
	portNum = atoi(argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	
	if (sockfd < 0) 
	{
		error("ERROR creating the socket");
	}
	
	//ZERO out the socket info struct
	bzero((char*) &sAddInfo, sizeof(sAddInfo));
	
	//do the norm int to a special network port int 
	sAddInfo.sin_port = htons(portNum);

	sAddInfo.sin_family = AF_INET;
	
	//flag for any type of network will be accepted
	sAddInfo.sin_addr.s_addr = INADDR_ANY;
	
	//bind the server socket to local port (this is what the client will connect to)
	if (bind(sockfd, (struct sockaddr *) &sAddInfo, sizeof(sAddInfo)) < 0)
	{
		error("ERROR on BINDING");
	}
	
	listen(sockfd, 5); //listen for the client connection request fucking 0!!
	
	cliSize = sizeof(cAddInfo); //this is the s
	
	// -------------
	
	pthread_t tid[50];
	int i = 0;
	int j = 0;
	while(i < 50)
	{
		//Accept is blocking
		newsockfd = accept(sockfd, (struct sockaddr *) &cAddInfo, &cliSize);
		if (newsockfd < 0) 
		{
			error("ERROR on accept");
		}
	
		if(pthread_create(&tid[i], NULL, socketThread, &newsockfd) != 0)
		{
			printf("Failed to create thread\n");
		}
		else
		{
			printf("Created thread\n");
		}
		
		if(i >= 50)
		{
			printf("Waiting to join\n");
			i = 0;
			for(i = 0; i < 50; i++)
			{
				pthread_join(tid[i], NULL);
			}
			break;
		}
	
		i++;
	}
	
	return 0;
	
}		



void renam(char* untarNam, char* projName)
{
	char renaComm[1000];
	renaComm[0] = '\0';
	strcpy(renaComm, "mv ");
//	strcat(renaComm, proj)
//	strcat(renaComm, "/");
	strcat(renaComm, untarNam);
	strcat(renaComm, " ");
	strcat(renaComm, projName);
	printf("Rename system call: %s\n", renaComm);
	system(renaComm);
	
}

void scanLib(char* proName, char* currDir, char* location)
{
	struct dirent *de;
	//If its the first time around, currDir should be "."
	printf("Entering directory: %s looking for: %s\n", currDir, proName);
	DIR *dr = opendir(currDir);
	
	if(dr == NULL)
	{
		printf("Could not open directory\n");
		return;
	}
	
	while((de = readdir(dr)) != NULL)
	{
		printf("%s\n", de->d_name);
		if(strcmp(de->d_name,proName) == 0)
		{
			printf("We found the project\n");
			char next[sizeof(currDir) + 1 + sizeof(proName) + 1];
			next[0] = '\0';
			strcpy(next,currDir);
			strcat(next,"/");
			strcat(next,proName);
			printf("It is located at : %s\n", next);
			strcpy(location, next);
			return;
			
		}
		
		if(de->d_type == DT_DIR)
		{
			char next[sizeof(currDir) + 1 + sizeof(de->d_name) + 1];
			next[0] = '\0';
			strcpy(next,currDir);
			strcat(next,"/");
			strcat(next,de->d_name);
			printf("We are trying to go into: %s\n", next);
			
			scanLib(proName, next, location);
		}
	}

	return;
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
	printf("Converrted string %s to int %i\n", str, final);
	return final;
	
}

void int2str(int numb, char* strNum)
{
	printf("Taking number %i and converting to string\n", numb);
	char buffer[50];
	sprintf(buffer, "%i", numb);
	strcpy(strNum, buffer);
}

void createEmptyTar()
{
	printf("Creating empty tar\n");
	char tarComma[1000];
	tarComma[0] = '\0';
	strcpy(tarComma, "tar -czvf ");
	strcat(tarComma, "reception");
	strcat(tarComma, ".tgz -T /dev/null");
	printf("Creating empty tar using system call: %s\n", tarComma);
	system(tarComma);
}

int newProjVer(char* projPath)
{
	printf("Getting %s version\n");
	int newVer = -2;
	char projDir[1000];
	projDir[0] = '\0';
	strcpy(projDir, "./");
	strcat(projDir, projPath);
	printf("Attempting to open: %s\n", projDir);
	struct dirent *dp;
	DIR *dir = opendir(projDir);
	if(dir == NULL)
	{
		printf("Failed to open directory for file version\n");
	}
	while((dp = readdir(dir)) != NULL)
	{
		printf("%s\n", dp->d_name);
		newVer++;
	}
	close(dir);
	printf("Returning: %i\n", newVer);
	return newVer;
}

void createHistory(char* projLoc)
{

	char histLoc[1000];
	histLoc[0] = '\0';
	strcpy(histLoc, projLoc);
	strcat(histLoc, "/History");
	
	int fp = open(histLoc, O_CREAT | O_WRONLY, 00644);
	if(fp<0)
		printf("Failed to create history file\n");
	close(fp);
	
}

void createNonEmptyTar(char* projName, int version)
{
	printf("Creating a filled tar named: %s version %i\n",projName, version);
	char tarComma[1000];
	tarComma[0] = '\0';
	strcpy(tarComma, " tar -czvf ");
	strcat(tarComma, projName);
	strcat(tarComma, ".tgz ");
	strcat(tarComma, projName);
	strcat(tarComma, "/");
	
	char *ver = malloc(100*sizeof(char));
	bzero(ver,100);
	ver[0] = '\0';
	int2str(version, ver);
	
	strcat(tarComma, ver);
	strcat(tarComma, "/");
	printf("System command create nonEmpty tgz: %s\n", tarComma);
	system(tarComma);
	free(ver);
	
}

void unpackTar(char* tarName, char* unpackLoc)
{
	char* unpackComm[1000];
	unpackComm[0] = '\0';
	strcpy(unpackComm, "tar -C ");
	strcat(unpackComm, unpackLoc);
	strcat(unpackComm, " -xvf ");
	strcat(unpackComm, "reception.tgz");
	printf("Unpacking %s in Directory: %s\n", tarName, unpackLoc);
	printf("System unpack call: %s\n", unpackComm);
	system(unpackComm);
}

void removeTar(char* tarName)
{
	char* removeComm[1000];
	removeComm[0] = '\0';
	strcpy(removeComm, "rm -frv ");
	strcat(removeComm, tarName);
	printf("Remove system call: %s\n", removeComm);
	system(removeComm);

}



