/* dllmain.cpp : Defines the entry point for the DLL application.
 * Version: 1.0.1.0
 * Date: 2026-02-01
 * Author: 0x0-nyan
 */

#include "pch.h"
#include "HSP3Service.h"


HWND ghWnd;
BOOLEAN gbStopped;
DWORD gdwSvcType;
DWORD gdwControlsAccepted;
DWORD gdwMessage;
LPSTR gpData;
SERVICE_STATUS_HANDLE ghStatus;
SERVICE_TABLE_ENTRYA gstEntry;



BOOL APIENTRY DllMain( HINSTANCE, DWORD dwReason, LPVOID) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        gbStopped = TRUE;
        ghStatus = nullptr;
        gpData = nullptr;
        gdwSvcType = 0;
        gdwControlsAccepted = 0;
        gdwMessage = 0;
        ZeroMemory(&gstEntry, sizeof(SERVICE_TABLE_ENTRYA));
        break;
    }
    return TRUE;
}


BOOL WINAPI ReportStatusInternal(DWORD dwCurrentState, DWORD dwExitCode, DWORD dwServiceSpecificExitCode, DWORD dwWaitHint) {
    static SERVICE_STATUS svcStatus;
    svcStatus.dwServiceType = gdwSvcType;
    svcStatus.dwCurrentState = dwCurrentState;
    svcStatus.dwWin32ExitCode = dwExitCode;
    svcStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;

    if (dwCurrentState == SERVICE_START_PENDING || dwCurrentState == SERVICE_STOP_PENDING || dwCurrentState == SERVICE_CONTINUE_PENDING || dwCurrentState == SERVICE_PAUSE_PENDING) {
        svcStatus.dwControlsAccepted = NULL;
        svcStatus.dwWaitHint = dwWaitHint;
        svcStatus.dwCheckPoint++;
    }
    else {
        svcStatus.dwControlsAccepted = gdwControlsAccepted;
        svcStatus.dwWaitHint = 0;
        svcStatus.dwCheckPoint = 0;
    }
    return SetServiceStatus(ghStatus, &svcStatus);
}


DWORD CALLBACK HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID) {
    // User Defined Service Controls (Range 128 to 255)
    if (dwControl >= 128 && dwControl <= 255) {
        PostMessageA(ghWnd, dwControl | 0x8000, dwEventType, (LPARAM)lpEventData);
        return NO_ERROR;
    }

    switch (dwControl) {
    // Basic Service Controls
    case SERVICE_CONTROL_STOP:
        ReportStatusInternal(SERVICE_STOP_PENDING, 0, 0, 30000);
        PostMessageA(ghWnd, HSC_STOP, 0, 0);
        //while (gdwMessage != HSC_STOP) Sleep(10);
        return NO_ERROR;

    case SERVICE_CONTROL_PAUSE:
        ReportStatusInternal(SERVICE_PAUSE_PENDING, 0, 0, 30000);
        PostMessageA(ghWnd, HSC_PAUSE, 0, 0);
        //while (gdwMessage != HSC_PAUSE) Sleep(10);
        //gdwMessage = 0;
        return NO_ERROR;

    case SERVICE_CONTROL_CONTINUE:
        ReportStatusInternal(SERVICE_CONTINUE_PENDING, 0, 0, 30000);
        PostMessageA(ghWnd, HSC_CONTINUE, 0, 0);
        //while (gdwMessage != 0) Sleep(10);
        return NO_ERROR;

    case SERVICE_CONTROL_INTERROGATE:
        SendMessageA(ghWnd, HSC_INTERROGATE, 0, 0);
        return NO_ERROR;

    case SERVICE_CONTROL_SHUTDOWN:
        ReportStatusInternal(SERVICE_STOP_PENDING, 0, 0, 30000);
        PostMessageA(ghWnd, HSC_SHUTDOWN, 0, 0);
        //while (gdwMessage != HSC_STOP) Sleep(10);
        return NO_ERROR;

    case SERVICE_CONTROL_PARAMCHANGE:
        PostMessageA(ghWnd, HSC_PARAMCHANGE, 0, 0);
        return NO_ERROR;

    case SERVICE_CONTROL_NETBINDADD:
        PostMessageA(ghWnd, HSC_NETBINDADD, 0, 0);
        return NO_ERROR;
        
    case SERVICE_CONTROL_NETBINDREMOVE:
        PostMessageA(ghWnd, HSC_NETBINDREMOVE, 0, 0);
        return NO_ERROR;

    case SERVICE_CONTROL_NETBINDENABLE:
        PostMessageA(ghWnd, HSC_NETBINDENABLE, 0, 0);
        return NO_ERROR;
        
    case SERVICE_CONTROL_NETBINDDISABLE:
        PostMessageA(ghWnd, HSC_NETBINDDISABLE, 0, 0);
        return NO_ERROR;

    case SERVICE_CONTROL_PRESHUTDOWN:
        ReportStatusInternal(SERVICE_STOP_PENDING, 0, 0, 30000);
        PostMessageA(ghWnd, HSC_STOP, 0, 0);
        //while (gdwMessage != HSC_STOP) Sleep(10);
        return NO_ERROR;

    // Extended Service Controls
    // First 3 SC's require return value from HSP code side
    case SERVICE_CONTROL_DEVICEEVENT:
        return SendMessageA(ghWnd, HSC_DEVICEEVENT, dwEventType, (LPARAM)lpEventData);
        //return NO_ERROR;
    case SERVICE_CONTROL_HARDWAREPROFILECHANGE:
        return SendMessageA(ghWnd, HSC_HARDWAREPROFILECHANGE, dwEventType, 0);
        //return NO_ERROR;
    case SERVICE_CONTROL_POWEREVENT:
        return SendMessageA(ghWnd, HSC_POWEREVENT, dwEventType, (LPARAM)lpEventData);
        //return NO_ERROR;
    case SERVICE_CONTROL_SESSIONCHANGE:
        PostMessageA(ghWnd, HSC_SESSIONCHANGE, dwEventType, (LPARAM)lpEventData);
        return NO_ERROR;
    case SERVICE_CONTROL_TIMECHANGE:
        PostMessageA(ghWnd, HSC_TIMECHANGE, 0, (LPARAM)lpEventData);
        return NO_ERROR;
    case SERVICE_CONTROL_TRIGGEREVENT:
        PostMessageA(ghWnd, HSC_TRIGGEREVENT, 0, 0);
        return NO_ERROR;

    // SERVICE_CONTROL_USERMODEREBOOT; This macro wasn't defined in my SDK
    // As well as most of Windows don't support this control code
    case 0x40:      
        PostMessageA(ghWnd, HSC_USERMODEREBOOT, 0, 0);
        return NO_ERROR;

    default:
        return ERROR_CALL_NOT_IMPLEMENTED;
    }
    return NO_ERROR;
}

void CALLBACK ServiceMainA(DWORD dwArgc, LPSTR *pArgv) {
    ghStatus = RegisterServiceCtrlHandlerExA("", HandlerEx, NULL);
    if (!ghStatus) {
        PostMessageA(ghWnd, HSC_ERROR, GetLastError(), HSE_FAIL_REGISTER);
        return;
    }
    
    ReportStatusInternal(SERVICE_START_PENDING, 0, 0, 30000);
    size_t lenMax = 0;
    for (int i = 0; i < dwArgc; i++) {
        if (strlen(pArgv[i]) > lenMax) lenMax = strlen(pArgv[i]);
    }

    PostMessageA(ghWnd, HSC_OK, dwArgc, lenMax);
    while (1) {
        switch (gdwMessage) {
        case HSC_STOP:
            //ReportStatusInternal(SERVICE_STOPPED, 0, 0, 0);
            return;
        case HSC_SHUTDOWN:
            //ReportStatusInternal(SERVICE_STOPPED, 0, 0, 0);
            return;
        case HSR_ENUM_ARGS:
            for (int i = 0; i < dwArgc; i++) {
                if (!SendMessageA(ghWnd, HSR_ENUM_ARGS, strlen(pArgv[i]), (LPARAM)pArgv[i])) break;
            }
            gdwMessage = 0;
            break;
        case HSR_GET_ARGS:
            if ((DWORD)gpData > dwArgc) {
                gdwMessage = 0;
                break;
            }
            gpData = pArgv[(DWORD)gpData];
            gdwMessage = 0;
            break;

        }

        Sleep(10);
    }
    return;
}


DWORD WINAPI StartServiceWrapper(LPVOID) {
    gbStopped = FALSE;
    if (!StartServiceCtrlDispatcherA(&gstEntry)) {
        PostMessageA(ghWnd, HSC_ERROR, GetLastError(), HSE_FAIL_START);
        gbStopped = TRUE;
        return 0;
    }
    gbStopped = TRUE;
    return 1;

}


HANDLE WINAPI HSP3StartService(HWND hWnd, DWORD dwServiceType, DWORD dwControlsAccepted) {
    if (!hWnd) return 0;
    ZeroMemory(&gstEntry, sizeof(SERVICE_TABLE_ENTRYA));
    gstEntry.lpServiceName = (LPSTR)"";
    gstEntry.lpServiceProc = ServiceMainA;
    ghWnd = hWnd;
    gdwSvcType = (dwServiceType == 0 ? SERVICE_WIN32_OWN_PROCESS : SERVICE_USER_OWN_PROCESS);
    gdwControlsAccepted = dwControlsAccepted;
    return CreateThread(NULL, 0, StartServiceWrapper, NULL, 0, NULL);
}


void WINAPI ReportStatus(DWORD dwCurrentState, DWORD dwWaitHint) {
    if (dwCurrentState == HSR_ENUM_ARGS) {
        gdwMessage = HSR_ENUM_ARGS;
        return;
    }
    if (dwCurrentState < SERVICE_START_PENDING || dwCurrentState > SERVICE_PAUSED) return;
    if (dwCurrentState == SERVICE_PAUSED) gdwMessage = HSC_PAUSE;
    if (dwCurrentState == SERVICE_RUNNING) gdwMessage = 0;
    ReportStatusInternal(dwCurrentState, 0, 0, dwWaitHint);
    return;
}


void WINAPI StopService(DWORD dwExitCode, DWORD dwServiceSpecificExitCode) {
    if (!gbStopped) {
        ReportStatusInternal(SERVICE_STOPPED, dwExitCode, dwServiceSpecificExitCode, 0);
        gdwMessage = HSC_STOP;
        gbStopped = TRUE;
    }
    return;
}

LPSTR WINAPI GetServiceArgumentInternal(DWORD dwIndex, PDWORD dwLength) {
    if (!gbStopped) {
        gpData = (LPSTR)dwIndex;
        gdwMessage = HSR_GET_ARGS;
        while (gdwMessage == HSR_GET_ARGS) Sleep(10);
        dwLength[0] = strlen(gpData);
        return gpData;
    }
    else {
        return nullptr;
    }
}