// getSys.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/stat.h>
#include <winsock.h>

#pragma warning(disable : 4996)


int main()
{
	OSVERSIONINFOEX osvi;
	char buf[1024];

	ZeroMemory(&osvi, sizeof(osvi));

	osvi.dwOSVersionInfoSize = sizeof(osvi);
	if (!(GetVersionEx((OSVERSIONINFO*)&osvi)))
	{
		printf("\tError retrieving system info.\n");
	}
	else
	{
		ZeroMemory((void *)&buf, sizeof(buf));

		char version[10];

		strcpy(buf, "OS: Windows \nVersion: ");

		_ltoa(osvi.dwMajorVersion, version, 10);
		strcat(buf, version);

		strcat(buf, ".");

		_ltoa(osvi.dwMinorVersion, version, 10);
		strcat(buf, version);

		strcat(buf, "\n");

		printf("%s", buf);
	}

	SYSTEM_INFO machine;
	GetSystemInfo(&machine);
	int processorType = machine.wProcessorArchitecture;
	char processor[100];
	if (processorType == 9)
		sprintf(processor, "Processor: PROCESSOR_ARCHITECTURE_AMD64\n");
	else if (processorType == 5)
		sprintf(processor, "Processor: PROCESSOR_ARCHITECTURE_ARM\n");
	else if (processorType == 6)
		sprintf(processor, "Processor: PROCESSOR_ARCHITECTURE_IA64\n");
	else if (processorType == 0)
		sprintf(processor, "Processor: PROCESSOR_ARCHITECTURE_INTEL\n");
	else
		sprintf(processor, "Processor: PROCESSOR_ARCHITECTURE_UNKNOWN");

	printf("%s", processor);

    return 0;
}

