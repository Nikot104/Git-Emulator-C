#include<stdlib.h>
#include<stdio.h>

int main (int argc, char* argv[]) 
{	
	//test case 1 no configure 
	system("./server/WTFserver 45535");
	system("./WTFclient checkout projA"); 
	system("^C"); //close the server 
	
	//test case 2 configures a file then runs a command
	system("./WTFclient configure state.cs.rutgers.edu 45535"); 
	system("./server/WTFserver 45535");	
	
	//test case 3: Checkout
	system("./WTFclient checkout projA"); //create command
	
	//test case 4: Push
	system("WTFclient push projA"); //remove folder clients
	
	//test case 5: CurrentVersion
	system("./WTFclient currentversion projA");	
	
	//test case 6: History
	system("./WTFclient history projTest");
	
	//test case 7: Wrong number of arguments
	system("./WTFclient");
	
	system("^C"); //close the server 
	
	return 0;
}
