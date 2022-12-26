#define _CRT_SECURE_NO_WARNINGS // чтобы не было предупреждений при использовании функций для работы с файлами из языка "Си"
#include <stdlib.h>
#include <windows.h>
#include <iostream>
#include <sysinfoapi.h>
#include <string>
#include <time.h>

// 3 + 0 + 5 + 1 + 8 = 17
#define numberOfPages 17

using namespace std;

//семафора для страниц памяти
HANDLE *WritersSem = new HANDLE[numberOfPages];
HANDLE *ReadersSem = new HANDLE[numberOfPages];

//Функция для открытия семафора писателя
void WriterSem(int NumSem) {
	string nameSem;
	nameSem = to_string(NumSem) + " Запись семафора";
	WritersSem[NumSem] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, nameSem.c_str());
}

//Функция для открытия семафора читателя
void ReaderSem(int NumSem) {
	string nameSem;
	nameSem = to_string(NumSem) + " Чтение семафора";
	ReadersSem[NumSem] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, nameSem.c_str());
}

void OpenSem() {
	for (int i = 0; i < numberOfPages; i++) {
		WriterSem(i);
		ReaderSem(i);
	}
}

//Функция для подсчета буферной памяти
int BufferSizeCounter() {
	int buffer;

	SYSTEM_INFO systemInf;
	GetSystemInfo(&systemInf);
	buffer = numberOfPages * systemInf.dwPageSize;

	return buffer;
}

int main()
{
	srand(time(NULL));

	int bufferSize = BufferSizeCounter();

	string nameBuffer, nameMap;
	HANDLE HandleMapped;
	LPVOID address;

	nameBuffer = "C:\\Users\\nastya\\OneDrive\\Рабочий стол\\LR4\\buffer.txt";
	nameMap = "buffer";

	HandleMapped = OpenFileMappingA(GENERIC_WRITE, FALSE, nameMap.c_str());
	address = MapViewOfFile(HandleMapped, FILE_MAP_WRITE, 0, 0, bufferSize);

	FILE *fileOne, *fileTwo;
	string nameLogfile, nameLogfileExcel;
	nameLogfile = "C:\\Users\\nastya\\OneDrive\\Рабочий стол\\LR4\\w_logs\\writer_log_file_" + to_string(GetCurrentProcessId()) + ".txt";
	fileOne = fopen(nameLogfile.c_str(), "w");

	nameLogfileExcel = "C:\\Users\\nastya\\OneDrive\\Рабочий стол\\LR4\\w_excel\\writer_log_file_" + to_string(GetCurrentProcessId()) + ".txt";
	fileTwo = fopen(nameLogfileExcel.c_str(), "w");

	OpenSem();

	
	char *data = new char[4]{ 'a', 'b', 'c', '\0' };

	SYSTEM_INFO systemInf;
	GetSystemInfo(&systemInf);
	DWORD start = GetTickCount();

	while (GetTickCount() < start + 15000)
	{

		fprintf(fileOne, "Process: %d\t  State: WAITING  \t\t  Time: %d  \n", GetCurrentProcessId(), GetTickCount());
		fprintf(fileTwo, "%d %d\n", GetTickCount(), 0);

		//Ожидание пока страница семафора получит значение 1
		DWORD pageSemOne = WaitForMultipleObjects(numberOfPages, WritersSem, FALSE, INFINITE);

		fprintf(fileOne, "Process: %d\t  State: WRITING page %d\t  Time: %d  \n", GetCurrentProcessId(), pageSemOne, GetTickCount());
		fprintf(fileTwo, "%d %d\n", GetTickCount(), 1);

		memcpy((LPVOID)((long long)address + (pageSemOne * systemInf.dwPageSize)), data, strlen(data) * sizeof(char));
		Sleep(500 + rand() % 1001);

		string logname = "C:\\Users\\nastya\\OneDrive\\Рабочий стол\\LR4\\p_excel\\pageSemOne_" + to_string(pageSemOne) + ".txt";
		FILE *logs = fopen(logname.c_str(), "a");
		fprintf(logs, "%d %d\n", GetTickCount(), 1);
		ReleaseSemaphore(ReadersSem[pageSemOne], 1, NULL);
		fprintf(logs, "%d %d\n", GetTickCount(), 0);
		fclose(logs);

		fprintf(fileOne, "Process: %d\t  State: RELEASED  \t\t  Time: %d  \n\n", GetCurrentProcessId(), GetTickCount());
		fprintf(fileTwo, "%d %d\n", GetTickCount(), 2);

	}

	return 0;
}
