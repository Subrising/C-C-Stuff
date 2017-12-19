// assignment1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <signal.h>
#include <sys/stat.h>
#include <process.h>
#include <sys/types.h>
#include <winsock.h>

#include <string.h>

// Pipe handles
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hChildStd_ERR_Rd = NULL;
HANDLE g_hChildStd_ERR_Wr = NULL;

// Functions
PROCESS_INFORMATION CreateChildProcess(int, char *);
void ReadFromPipe(PROCESS_INFORMATION, SOCKET);
int secondMain(SOCKET, int, char *);

static char *myStrDup(char *str)
{
	char *other = malloc(strlen(str) + 1);
	if (other != NULL)
		strcpy(other, str);
	return other;
}

SOCKET make_server_socket()
{
	// Set up socket
	struct sockaddr_in saddr;

	// Create socket
	printf("Initialising socket...\n");
	printf("\tCreating the socket...\n");
	SOCKET sock_id = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_id == INVALID_SOCKET) return -1;
	printf("\t\tSocket created.\n");

	// Zero out the socket address struct
	ZeroMemory((void *)&saddr, sizeof(saddr));

	// Set socket settings
	printf("\tSetting address...\n");
	saddr.sin_addr.s_addr = htonl(INADDR_ANY); printf("\t\tIP: ANY\n");
	saddr.sin_port = htons(2809); printf("\t\tPORT: 2809\n");
	saddr.sin_family = AF_INET;

	// Bind the socket
	printf("\tBinding to address...\n");
	if (bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) == SOCKET_ERROR)
	{
		printf("\t\tFailed to bind.\n");
		return -1;
	}
	printf("\t\tSuccess.\n");

	// Start listening to see if clients are trying to connect
	printf("\tStarting Listener...\n");
	if (listen(sock_id, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("\t\tFailed to start listener.\n");
		return -1;
	}
	printf("\t\tSuccess.\n");

	printf("Socket successfully initialised.\n\n");
	return sock_id;
}

// Process commands from client
int process_requests(SOCKET socket)
{
	printf("Processing requests...\n");

	// Ask for initial commands from client
	if ((send(socket, "Command: ", 10, NULL)) < 0)
		printf("\tError sending initial command request.\n");

	while (1)
	{
		char buf[2048];
		char buf2[2048];
		int n;

		// Get the command from the client
		ZeroMemory((void *)&buf, sizeof(buf));
		n = recv(socket, buf, sizeof(buf), NULL);
		if (n <= 0)
		{
			printf("\tError receiving input from client or connection was forcefully closed!\n");
			break;
		}

		strcpy(buf2, buf);

		// Command Checks

		// User wants to exit
		if (!strcmp(buf, "exit"))
		{
			printf("\tClient requested disconnection.\n");

			break;
		}
		// Server wants to stop
		else if (!strcmp(buf, "stop"))
		{
			printf("\tServer received stop command.\n");

			return 1;
		}
		// Client entered list command
		else if (!strcmp(buf, "list"))
		{
			printf("\tServer received list command.\n");
			secondMain(socket, 1, buf);
		}
		// Client entered test command
		else if (!strcmp(buf, "test"))
		{
			printf("\tServer received test command.\n");
			secondMain(socket, 2, buf);
		}
		// Client entered sys command
		else if (!strcmp(buf, "sys"))
		{
			printf("\tServer received sys command.\n");
			secondMain(socket, 3, buf);
		}
		// Client entered list | sort command
		else if (!strcmp(buf, "list | sort"))
		{
			printf("\tServer received list | sort command.\n");
			secondMain(socket, 4, buf);
		}
		// Client entered get command
		else if (strstr(buf, "get") != NULL)
		{
			printf("\tServer received get command.\n");
			secondMain(socket, 5, buf2);
		}
		// CLient entered unknown command
		else
		{
			char temp[256];
			sprintf(temp, "Error: Unknown Command '%s'\n", buf);
			n = send(socket, temp, sizeof(temp), NULL);
			if (n < 0)
			{
				printf("Error sending unknown command notification.\n");
			}
			printf("\tReceived an unknown command.\n");
		}

		// Ask for next command from client
		n = send(socket, "Command: ", 10, NULL);
		if (n < 0)
		{
			printf("\tError sending next command request.\n");
		}
	}

	return 0;
}

int main()
{
	printf("Starting the server...\n\n");

	// Start Winsock
	WSADATA wsaData;
	int i = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (i != 0)
	{
		printf("WSAStartup FAILED: %d\n", i);
		return 1;
	}

	// Start server
	int sock = make_server_socket();
	if (sock < 0) exit(-1);

	printf("Server started.\n\n");

	// Receive client connections
	while (1)
	{
		printf("Waiting for incoming client connections...\n");
		SOCKET csock = accept(sock, NULL, NULL);
		if (csock < 0)
		{
			printf("\tInvalid connection...\n");
			continue;
		}
		printf("\tConnection to client established...\n");

		if (process_requests(csock))
		{
			if (!closesocket(csock)) 
				printf("Connection to client closed.\n\n");
			break;
		}

		if (!closesocket(csock)) 
			printf("Connection to client closed.\n\n");
	}

	closesocket(sock);
	printf("Server stopped.\n");

	// Clean up sockets
	WSACleanup();

	return 0;
}

// Set up pipes, create child proccesses, and read from pipes to client
int secondMain(SOCKET sendSocket, int selectOption, char * sbuf) 
{
	SECURITY_ATTRIBUTES sec;
	printf("\n>Parent Execution Starting...\n");

	// Set the bInheritHandle flag so pipe handles are inherited. 
	sec.nLength = sizeof(SECURITY_ATTRIBUTES);
	sec.bInheritHandle = TRUE;
	sec.lpSecurityDescriptor = NULL;

	// Create pipe for child STDERR 
	if (!CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &sec, 0)) {
		exit(1);
	}
	// Make sure that the read handle to the pipe for STDERR is not inherited
	if (!SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0)) {
		exit(1);
	}
	// Create pipe for child STDOUT
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sec, 0)) {
		exit(1);
	}
	// Make sure that the read handle to the pipe for STDOUT is not inherited
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
		exit(1);
	}

	// Create child process
	PROCESS_INFORMATION pi = CreateChildProcess(selectOption, sbuf);

	// Read from child STDOUT pipe and print to client
	printf("\n>Child Process Results STDOUT:\n\n");
	ReadFromPipe(pi, sendSocket);

	printf("\n>Parent Execution Ending...\n");

	return 0;
}

// Create child process for given client command
PROCESS_INFORMATION CreateChildProcess(int selectOption, char * sbuf) 
{
	// Set up flags and process information
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	BOOL bSuccess = FALSE;

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	// STARTUPINFO Structure set up
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.hStdError = g_hChildStd_ERR_Wr;
	si.hStdOutput = g_hChildStd_OUT_Wr;
	si.dwFlags |= STARTF_USESTDHANDLES;


	// Tokenise client command to use when start child process
	char *argv[100];
	int argc = 0;

	char *str = strtok(sbuf, " ");

	while (str != NULL)
	{
		argv[argc++] = myStrDup(str);
		str = strtok(NULL, " ");
	}

	printf("What is argc up to? %d\n", argc);

	for (int i = 0; i < argc; i++)
	{
		printf("Arg #%d = '%s'\n", i, argv[i]);
	}
	putchar('\n');

	// Starting child process for list -l command
	if (selectOption == 1)
	{
		bSuccess = CreateProcess(NULL,
			"cmd.exe /k dir /Q /a",    
			NULL,          
			NULL,          
			TRUE,         
			0,            
			NULL,         
			NULL,           
			&si,  
			&pi);  
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(g_hChildStd_ERR_Wr);
		CloseHandle(g_hChildStd_OUT_Wr);
	}
	// Starting child process for test command
	else if (selectOption == 2)
	{
		bSuccess = CreateProcess("C:\\Users\\multi\\Documents\\Visual Studio 2015\\Projects\\redirectTest\\Debug\\redirectTest.exe",
			NULL,    
			NULL,         
			NULL,         
			TRUE,         
			0,          
			NULL,         
			NULL,         
			&si,  
			&pi);  
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(g_hChildStd_ERR_Wr);
		CloseHandle(g_hChildStd_OUT_Wr);
	}
	// Starting child process for sys command
	else if (selectOption == 3)
	{
		bSuccess = CreateProcess("C:\\Users\\multi\\Documents\\Visual Studio 2015\\Projects\\getSys\\Debug\\getSys.exe",
			NULL,      
			NULL,          
			NULL,          
			TRUE,          
			0,             
			NULL,         
			NULL,          
			&si,  
			&pi);  
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(g_hChildStd_ERR_Wr);
		CloseHandle(g_hChildStd_OUT_Wr);
	}
	// Starting child proces for list | sort command
	else if (selectOption == 4)
	{
		bSuccess = CreateProcess(NULL,
			"cmd.exe /k dir | sort",   
			NULL,         
			NULL,        
			TRUE,         
			0,            
			NULL,      
			NULL,         
			&si,  
			&pi);  
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(g_hChildStd_ERR_Wr);
		CloseHandle(g_hChildStd_OUT_Wr);
	}
	// Starting child process for get command
	else if (selectOption == 5)
	{
		char tempCommand[256];
		sprintf(tempCommand, "cmd.exe /k findstr /C:\"%s\" %s", argv[4], argv[1]);
		bSuccess = CreateProcess(NULL,
			tempCommand,   
			NULL,         
			NULL,        
			TRUE,          
			0,            
			NULL,        
			NULL,         
			&si, 
			&pi);  
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(g_hChildStd_ERR_Wr);
		CloseHandle(g_hChildStd_OUT_Wr);
	}

	// If an error occurs, exit the application. 
	if (!bSuccess) {
		exit(1);
	}
	return pi;
}

// Read output from the child process' pipe for STDOUT
// Send contents of child process' pipe to client
void ReadFromPipe(PROCESS_INFORMATION pi, SOCKET sendSock) {
	DWORD dwRead;
	CHAR buf[2048];
	BOOL bSuccess = FALSE;
	int bytecount = 0;

	// Read from child pipe and store contents into buf
	bSuccess = ReadFile(g_hChildStd_OUT_Rd, buf, sizeof(buf), &dwRead, NULL);
	bytecount += dwRead;

	// Null terminate buf
	buf[bytecount] = 0;

	printf("%s", buf);

	printf("Number of bytes read = %d\n", dwRead);

	printf("Final Output = %s\n", buf);
	printf("Final Output Length = %d\n", strlen(buf));

	printf("Sending output to client...\n");

	// Send output of child to client
	send(sendSock, buf, strlen(buf), 0);
	
	ZeroMemory((void *)&buf, sizeof(buf));
}
