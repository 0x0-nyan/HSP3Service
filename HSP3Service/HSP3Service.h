#pragma once
#pragma comment(lib, "advapi32.lib")

// Basic Service Controls
#define HSC_OK						0x8000
#define HSC_STOP					0x8001
#define HSC_PAUSE					0x8002
#define HSC_CONTINUE				0x8003
#define HSC_INTERROGATE				0x8004
#define HSC_SHUTDOWN				0x8005
#define HSC_PARAMCHANGE				0x8006
#define HSC_NETBINDADD				0x8007
#define HSC_NETBINDREMOVE			0x8008
#define HSC_NETBINDENABLE			0x8009
#define HSC_NETBINDDISABLE			0x800A
#define HSC_PRESHUTDOWN				0x800F

// Extended Service Controls
#define HSC_DEVICEEVENT				0x800B		// gosub and return value required
#define HSC_HARDWAREPROFILECHANGE	0x800C		// gosub and return value required
#define HSC_POWEREVENT				0x800D		// gosub and return value required
#define HSC_SESSIONCHANGE			0x800E
#define HSC_TIMECHANGE				0x8010
#define HSC_TRIGGEREVENT			0x8020
#define HSC_USERMODEREBOOT			0x8040

// User Defined Service Controls
#define HSC_USER_0					0x8080
#define HSC_USER_1					0x8081
#define HSC_USER_2					0x8082
/******* Add your own SC's here... *******/
#define HSC_USER_125				0x80FD
#define HSC_USER_126				0x80FE
#define HSC_USER_127				0x80FF

// Error Codes in This Plugin
#define HSC_ERROR					0x9000
#define HSE_FAIL_START				0x9001
#define HSE_FAIL_REGISTER			0x9002

// Reserved Macros
#define HSR_ENUM_ARGS				0xA000
#define HSR_GET_ARGS				0xA001

#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)


extern "C" {
	DLLEXPORT HANDLE WINAPI HSP3StartService(HWND, DWORD, DWORD);
	//DLLEXPORT BOOL WINAPI ReportStatusInternal(DWORD, DWORD, DWORD, DWORD);
	DLLEXPORT void WINAPI ReportStatus(DWORD, DWORD);
	DLLEXPORT void WINAPI StopService(DWORD, DWORD);
	DLLEXPORT LPSTR WINAPI GetServiceArgumentInternal(DWORD, PDWORD);
}