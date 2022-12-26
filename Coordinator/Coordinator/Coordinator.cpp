#include <windows.h>
#include <iostream>
#include <sysinfoapi.h>
#include <string>
// 3 + 0 + 5 + 1 + 8 = 17
#define numberOfPages 17

using namespace std;

void WriterCreate(int);
void ReaderCreate(int);
void ProcessWriter(int);
void ProcessReader(int);
int BufferSize();
void CreateSem();
void CreateProcess();
void CreateMappingFile(int);

//семафора для страниц памяти
HANDLE *WritersSem = new HANDLE[numberOfPages];
HANDLE *ReadersSem = new HANDLE[numberOfPages];

HANDLE *startProc = new HANDLE[numberOfPages << 1];

//Функция для создания семафоров писателей (изначально со значением 1)
void WriterCreate(int NumSem) {
	string NameSem;
	NameSem = to_string(NumSem) + " Запись семафора";
	WritersSem[NumSem] = CreateSemaphoreA(NULL, 1, 1, NameSem.c_str());
}

//Функция для создания семафоров читателей (изначально со значением 0)
void ReaderCreate(int NumSem) {
	string NameSem;
	NameSem = to_string(NumSem) + " Чтение семафора";
	ReadersSem[NumSem] = CreateSemaphoreA(NULL, 0, 1, NameSem.c_str());
}

//Функция для создания процессов писателя
void ProcessWriter(int NumProc) {

	LPSTARTUPINFOA startup = new STARTUPINFOA[numberOfPages];
	LPPROCESS_INFORMATION procInf = new PROCESS_INFORMATION[numberOfPages];
	string NameProc;
	NameProc = "C:\\Users\\nastya\\OneDrive\\Рабочий стол\\LR4\\Apps\\WriterApp\\Debug\\WriterApp.exe";
	//Обнуление блока памяти
	ZeroMemory(&startup[NumProc], sizeof(startup[NumProc]));
	//Создание процесса
	CreateProcessA(NameProc.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startup[NumProc], &procInf[NumProc]);
	//Добавляем дескриптор писателя
	startProc[NumProc] = procInf[NumProc].hProcess;

}

//Функция для создания процессов читателя
void ProcessReader(int NumProc) {

	LPSTARTUPINFOA startup = new STARTUPINFOA[numberOfPages];
	LPPROCESS_INFORMATION procInf = new PROCESS_INFORMATION[numberOfPages];
	string NameProc;
	NameProc = "C:\\Users\\nastya\\OneDrive\\Рабочий стол\\LR4\\Apps\\ReaderApp\\Debug\\ReaderApp.exe";
	//Обнуление блока памяти
	ZeroMemory(&startup[NumProc], sizeof(startup[NumProc]));
	//Создание процесса
	CreateProcessA(NameProc.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &startup[NumProc], &procInf[NumProc]);
	//Добавляем дескриптор читателя
	startProc[numberOfPages + NumProc] = procInf[NumProc].hProcess;

}

//Функция для подсчета буферной памяти
int BufferSize() {
	int buffer;

	SYSTEM_INFO systemInf;
	GetSystemInfo(&systemInf);
	buffer = numberOfPages * systemInf.dwPageSize;

	return buffer;
}

void CreateSem() {
	for (int i = 0; i < numberOfPages; i++) {
		WriterCreate(i);
		ReaderCreate(i);
	}
}

void CreateProcess() {
	for (int i = 0; i < numberOfPages; i++) {
		ProcessWriter(i);
		ProcessReader(i);
	}
}

void CreateMappingFile(int BufferSize) {

	string nameBuffer, nameMap;
	HANDLE handleBuffer, handleMapped;
	LPVOID address;

	nameBuffer = "C:\\Users\\nastya\\OneDrive\\Рабочий стол\\LR4\\buffer.txt";
	nameMap = "buffer";

	handleBuffer = CreateFileA(nameBuffer.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	handleMapped = CreateFileMappingA(handleBuffer, NULL, PAGE_READWRITE, 0, BufferSize, nameMap.c_str());
	address = MapViewOfFile(handleMapped, FILE_MAP_WRITE, 0, 0, BufferSize);
	VirtualLock(address, BufferSize);

}

int main()
{
	int bufferSize = BufferSize();

	//Создаем проецируемый файл
	CreateMappingFile(bufferSize);

	//Создание семафоров и процессов
	CreateSem();
	CreateProcess();

	//Ожидание окончания процессов
	WaitForMultipleObjects(numberOfPages << 1, startProc, TRUE, INFINITE);

	return 0;
}
