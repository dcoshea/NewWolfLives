/*************************************************************************
** NewWolf
** Copyright (C) 1999-2002 by DarkOne
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**************************************************************************
** system information /Win32 version/
*************************************************************************/
#include "../common/common.h"
#include "win32.h"

// ------------------------- * Devider * -------------------------
bool sys_win9x, sys_mmx, sys_xmm;

// ------------------------- * Devider * -------------------------

#pragma warning(disable:4035)
__inline __int64 GetCycleNumber(void) {__asm RDTSC}
#pragma warning(default:4035)

/*
** Sys_CPUspeed
**
** returns cpu speed in MHz. Interesting,
** would it be such a time, when CPU speed
** would not fit in signed 32 bit integer???
*/
int Sys_CPUspeed(void)
{
	LARGE_INTEGER HWst, HW, HWf;
	__int64 CPU;

	if(!QueryPerformanceFrequency(&HWf)) return 0;

	QueryPerformanceCounter(&HWst);
	CPU=GetCycleNumber();
	__asm
	{
		mov ebx, 5000000
	CPUwait:
		dec ebx
		jnz CPUwait
	}
	QueryPerformanceCounter(&HW);
	CPU=GetCycleNumber()-CPU;
	HW.QuadPart-=HWst.QuadPart;
	if(HW.QuadPart==0) return 0;
	return (int)(CPU*HWf.QuadPart/HW.QuadPart/1000000);
}

/*
** Sys_CPUinfo
*/
void Sys_CPUinfo(void)
{
	unsigned long i0, i3;
	char vendor[16];
	int speed;

	sys_mmx=sys_xmm=false;

	_asm
	{
		mov eax, 0
		cpuid
		mov dword ptr [vendor], ebx
		mov dword ptr [vendor+4], edx
		mov dword ptr [vendor+8], ecx
		mov dword ptr [vendor+12], 0

		mov eax, 1
		cpuid
		mov i0, eax
		mov i3, edx
	}

	Con_Printf("-> CPU speed: ");
	speed=Sys_CPUspeed();
	if(speed>0)
		Con_Printf("%dMHz\n", speed);
	else
		Con_Printf("unable to detect\n");
	Con_Printf("-> Family %d, Model %d, Stepping %d\n", (i0>>8)&0x0f, (i0>>4)&0x0f, i0&0x0f);
	Con_Printf("-> Extended info: 0x%08X\n", i3);
	Con_Printf("-> FPU %sDetected\n", i3&(1<<0)?"":"NOT ");
	Con_Printf("-> MMX %sDetected\n", i3&(1<<23)?sys_mmx=true,"":"NOT ");
	Con_Printf("-> XMM(PIII) %sDetected\n", i3&(1<<25)?sys_xmm=true,"":"NOT ");
	Con_Printf("-> CPU ID: %s\n", vendor);
}

/*
** Sys_MEMinfo
*/
void Sys_MEMinfo(void)
{
	MEMORYSTATUS w32mem;

	GlobalMemoryStatus(&w32mem);
	Con_Printf("-> Physical memory: %dMb (%dMb free)\n", w32mem.dwTotalPhys/1024/1024, w32mem.dwAvailPhys/1024/1024);
	Con_Printf("-> Total memory in use: %d%%\n", w32mem.dwMemoryLoad);
}

/*
** Sys_OSinfo
*/
void Sys_OSinfo(void)
{
	OSVERSIONINFO info;
	char *osname;

	info.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&info);

	sys_win9x=true;
	switch(info.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
		osname="Win32s";
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		info.dwBuildNumber&=0xFFFF;
		if(info.dwMinorVersion<10) // Windows 95
		{
			if(info.dwBuildNumber==1111)
				osname="Windows 95 OSR2.0";
			else if(info.dwBuildNumber==1212)
				osname="Windows 95 OSR2.1";
			else
				osname="Windows 95";
		}
		else if(info.dwMinorVersion<90) // Windows 98
		{
			if(info.dwBuildNumber==2222)
				osname="Windows 98 SE";
			else
				osname="Windows 98";
		}
		else // Windows ME
			osname="Windows Me";
		break;
	case VER_PLATFORM_WIN32_NT:
		sys_win9x=false;
		if(info.dwMajorVersion>=5) // NT based Windows (2000, XP)
		{ 
			if(info.dwMinorVersion>=1)
				osname="Windows XP";
			else
				osname="Windows 2000";
		}
		else // Windows NT
			osname="Windows NT";
		break;
	default:
		osname="Unknown Win32 based";
		break;
	}

	Con_Printf("-> OS: %s %u.%u.%u %s\n", osname, info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber, info.szCSDVersion);
}

/*
** Sys_Info
**
** displays host system information
*/
void Sys_Info(void)
{
	SYSTEMTIME time;

	GetLocalTime(&time);
	Con_Printf(" ******************************\n");
	Con_Printf("NewWolf version %4.2f %s [%s]\n", VERSION, BUILDSTRING, __DATE__);
	Con_Printf("Built on %s, %s\n", __DATE__, __TIME__);
	Con_Printf("Ran on    %02d/%02d/%02d, %02d:%02d:%02d\n\n", time.wMonth, time.wDay, time.wYear, time.wHour, time.wMinute, time.wSecond);

	Con_Printf("Sys_Info: CPU info\n");
	Sys_CPUinfo();
	Con_Printf("Sys_Info: Memory info\n");
	Sys_MEMinfo();
	Con_Printf("Sys_Info: OS info\n");
	Sys_OSinfo();
}

