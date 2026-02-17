/* HSP3Service.as : Windows Service Plug-in for HSP3
 * Version: 1.0.1
 * Date: 2026-02-03
 * Author: 0x0-nyan
 */

#ifdef __hsp30__
#ifndef __HSP3SERVICE_AS__
#define __HSP3SERVICE_AS__

	// Current state of service (dwCurrentState)
	#define SERVICE_STOPPED							0x00000001
	#define SERVICE_START_PENDING					0x00000002
	#define SERVICE_STOP_PENDING					0x00000003
	#define SERVICE_RUNNING							0x00000004
	#define SERVICE_CONTINUE_PENDING				0x00000005
	#define SERVICE_PAUSE_PENDING					0x00000006
	#define SERVICE_PAUSED							0x00000007

	// Accepted basic service controls (dwControlsAccepted)
	#define SERVICE_ACCEPT_STOP						0x00000001
	#define SERVICE_ACCEPT_PAUSE_CONTINUE			0x00000002
	#define SERVICE_ACCEPT_SHUTDOWN					0x00000004
	#define SERVICE_ACCEPT_PARAMCHANGE				0x00000008
	#define SERVICE_ACCEPT_NETBINDCHANGE			0x00000010
	#define SERVICE_ACCEPT_PRESHUTDOWN				0x00000100
	
	// Accepted Extended Service Controls (dwControlsAccepted)
	#define SERVICE_ACCEPT_HARDWAREPROFILECHANGE	0x00000020
	#define SERVICE_ACCEPT_POWEREVENT				0x00000040
	#define SERVICE_ACCEPT_SESSIONCHANGE			0x00000080
	#define SERVICE_ACCEPT_TIMECHANGE				0x00000200
	#define SERVICE_ACCEPT_TRIGGEREVENT				0x00000400
	#define SERVICE_ACCEPT_USERMODEREBOOT			0x00000800

	// Some useful service error codes
	#define NO_ERROR 0
	#define ERROR_SERVICE_ALREADY_RUNNING			1056
	#define ERROR_SERVICE_DOES_NOT_EXIST			1060
	#define ERROR_EXCEPTION_IN_SERVICE				1064
	#define ERROR_SERVICE_SPECIFIC_ERROR			1066

	// Basic Service Controls
	#define HSC_OK									0x8000
	#define HSC_STOP								0x8001
	#define HSC_PAUSE								0x8002
	#define HSC_CONTINUE							0x8003
	#define HSC_INTERROGATE							0x8004
	#define HSC_SHUTDOWN							0x8005
	#define HSC_PARAMCHANGE							0x8006
	#define HSC_NETBINDADD							0x8007
	#define HSC_NETBINDREMOVE						0x8008
	#define HSC_NETBINDENABLE						0x8009
	#define HSC_NETBINDDISABLE						0x800A
	#define HSC_PRESHUTDOWN							0x800F

	// Extended Service Controls
	#define HSC_DEVICEEVENT							0x800B	// gosub and return value required
	#define HSC_HARDWAREPROFILECHANGE				0x800C	// gosub and return value required
	#define HSC_POWEREVENT							0x800D	// gosub and return value required
	#define HSC_SESSIONCHANGE						0x800E
	#define HSC_TIMECHANGE							0x8010
	#define HSC_TRIGGEREVENT						0x8020
	#define HSC_USERMODEREBOOT						0x8040

	// User Defined Service Controls (0x8080 to 0x80FF)
	#define HSC_USER_0								0x8080
	#define HSC_USER_1								0x8081
	#define HSC_USER_2								0x8082
	/******* Add your own SC's here... *******/
	#define HSC_USER_125							0x80FD
	#define HSC_USER_126							0x80FE
	#define HSC_USER_127							0x80FF

	// Error Codes in This Plugin
	#define HSC_ERROR								0x9000
	#define HSE_FAIL_START							0x9001
	#define HSE_FAIL_REGISTER						0x9002

	// Reserved Macros
	#define HSR_ENUM_ARGS							0xA000
	#define HSR_GET_ARGS							0xA001
	#define EnumServiceArguments(%1)				oncmd gosub %1, HSR_ENUM_ARGS : ReportStatus HSR_ENUM_ARGS, 0

	
	// Main Module
	#module MHSP3Service
	
	// Load libraries and register functions
	#uselib "HSP3Service.dll"
	#cfunc __HSP3StartService "_HSP3StartService@12" sptr, sptr, sptr
	#func global ReportStatus "_ReportStatus@8" sptr, sptr
	#func __StopService "_StopService@8" sptr, sptr
	#cfunc __GetServiceArgumentInternal "_GetServiceArgumentInternal@8" sptr, sptr

	#uselib "kernel32.dll"
	#func _WaitForSingleObject "WaitForSingleObject" sptr, sptr
	#func _CloseHandle "CloseHandle" sptr

	_bThreadClosed = 1
	_hThread = 0
	#deffunc HSP3StartService int _idWnd, int _dwSvcType, int _dwControlsAccepted, local idPreviousWnd
		idPreviousWnd = ginfo(3)
		gsel _idWnd, 0
		_hThread = __HSP3StartService(hwnd, _dwSvcType, _dwControlsAccepted)
		gsel idPreviousWnd, 0
		if (_hThread) {
			_bThreadClosed = 0
			return _hThread
		} else {
			_bThreadClosed = 1
			return 0
		}

	#deffunc StopService int _dwExitCode, int _dwServiceSpecificExitCode
		if (_hThread) {
			__StopService _dwExitCode, _dwServiceSpecificExitCode
			_WaitForSingleObject _hThread, 0xFFFFFFFF
			_CloseHandle _hThread
			_bThreadClosed = 1
		}
		return

	#deffunc ExitServiceProcess int _nExitCode
		if (_hThread > 0 && _bThreadClosed == 0) {
			__StopService 0, 0
			_WaitForSingleObject _hThread, 0xFFFFFFFF
			_CloseHandle _hThread
		}
		end@hsp _nExitCode

	#defcfunc GetServiceArgument int _dwIndex, local lenArgument, local pszArgument
		if (_hThread) {
			lenArgument = 0
			pszArgument = __GetServiceArgumentInternal(_dwIndex, varptr(lenArgument))
			if (pszArgument == 0) : return ""
			dupptr _szArgument, pszArgument, lenArgument, 2
			return _szArgument
		} else {
			return ""
		}
		
	#global

#endif
#endif
