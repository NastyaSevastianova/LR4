#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

int main()
{
	HANDLE pipe;
	string pipe_name, event_name;
	char* data = new char[512];
	OVERLAPPED Overlapped;
	DWORD written;

	ZeroMemory(&Overlapped, sizeof(Overlapped));

	event_name = "EVENT";
	Overlapped.hEvent = CreateEventA(NULL, TRUE, FALSE, event_name.c_str());

	pipe_name = "\\\\.\\pipe\\mypipe";
	pipe = CreateNamedPipeA(pipe_name.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 256, 256, 0, NULL);
	ConnectNamedPipe(pipe, &Overlapped);

	cout << "You can input data. Input \"0\" to stop.\n\n";
	while (true)
	{
		cin >> data;
		if (strcmp(data, "0") != 0)
		{
			cout << endl;
			WriteFile(pipe, data, strlen(data) + 1, &written, &Overlapped);
			WaitForSingleObject(Overlapped.hEvent, INFINITE);
		}
		else
		{
			cout << "\nServer stopped\n";
			break;
		}
	}

	DisconnectNamedPipe(pipe);
	return 0;
}