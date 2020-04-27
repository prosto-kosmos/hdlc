// ---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#include <cmath>
#include "File2.cpp"

typedef void(*TCallBackModFunction)(unsigned char *Data, int LengthOfDataInByte,
	int LengthOfDataInBit, char correct);
TCallBackModFunction fCallBackModFunction;
HANDLE fhwnd;

#pragma argsused

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason,
	void* lpReserved) {

	return 1;
}

// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) void *__stdcall Init
	(TCallBackModFunction CallBackFunction, int *countMillerFiles,
	char *fileForOutFiles, char *tmpDir, char *modDir, HANDLE hwnd,
	char *FileName, int *Period) {
	//инициализируем переменную функции обратного вызова
	fCallBackModFunction = CallBackFunction;
	//выделяем область памяти на объект типа HDLC_CLASS
	HDLC_CLASS *HDLC_OBJ = new HDLC_CLASS(CallBackFunction);
	return HDLC_OBJ;

}

// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) void __stdcall Free(void *Table) {
	//создаем указатель на область памяти, которую мы выделили в Init
	HDLC_CLASS *HDLC_OBJ = (HDLC_CLASS*)Table;
	//очищаем зарезервированную память
	delete HDLC_OBJ;
}

// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) void __stdcall AddData(void *Table,
	unsigned char *Data, int LengthOfDataInByte, int LengthOfDataInBit,
	char correct) {
	//создаем указатель на область памяти, которую мы выделили в Init
	HDLC_CLASS *HDLC_OBJ = (HDLC_CLASS*)Table;
	//вызываем метод класса HDLC_CLASS - AddData
	HDLC_OBJ->AddData(Data, LengthOfDataInByte);
}

// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) void __stdcall EndOfData(void *Table) {

}

// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) void __stdcall ShowModuleWindow(void *Table) {

}
// ---------------------------------------------------------------------------
