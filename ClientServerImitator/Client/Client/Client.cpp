#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

void CALLBACK CompletionRoutine(DWORD errorCode, DWORD bytestransfered, LPOVERLAPPED lpOverlapped)
{
}

int main()
{
	HANDLE pipe;
	string pipe_name;
	char* data = new char[512];
	OVERLAPPED Overlapped;

	ZeroMemory(&Overlapped, sizeof(Overlapped));

	pipe_name = "\\\\.\\pipe\\mypipe";
	pipe = CreateFileA(pipe_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	cout << "Data from server:\n\n";
	BOOL get_data;
	while (true)
	{
		get_data = ReadFileEx(pipe, data, 512, &Overlapped, CompletionRoutine);
		if (get_data && pipe != INVALID_HANDLE_VALUE)
		{
			cout << data << endl << endl;
			SleepEx(INFINITE, TRUE);
		}
		else
		{
			cout << "Client stopped\n";
			break;
		}
	}

	return 0;
}