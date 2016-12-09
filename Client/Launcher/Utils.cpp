// Launch utils
// Author(s):       iFarbod <ifarbod@outlook.com>
//
// Copyright (c) 2015-2016 The San Andreas Online Open Source Project
//
// Distributed under the MIT license (See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT)

#include "Main.h"

bool CheckRegistryIntegrity()
{
	// Own app-path
	strcpy_s(g_szExecutablePath, MAX_PATH, GetAppPath());
	WriteRegistryString(SAO_REG_KEY, SAO_REG_SUBKEY, "LauncherPath", g_szExecutablePath, MAX_PATH);

	// Game path
	if(!ReadRegistryString(SAO_REG_KEY, SAO_REG_SUBKEY, "GamePath", "", g_szGamePath, MAX_PATH) || strlen(g_szGamePath) < 1)
	{
		OPENFILENAME ofn = { 0 };

		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = g_szGameExecutable;
		ofn.nMaxFile = MAX_PATH;

		ofn.lpstrFilter = "Executable\0*.exe";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		ofn.lpstrTitle = "Please select your GTA San Andreas executable!";

		if(GetOpenFileName(&ofn))
		{
			if(CheckNameOfExecutableFromPath(GAME_EXECUTABLE_NAME, strlen(GAME_EXECUTABLE_NAME), ofn.lpstrFile, strlen(ofn.lpstrFile)))
			{
				RemoveExecutableNameFromPath(ofn.lpstrFile, strlen(ofn.lpstrFile), g_szGamePath, MAX_PATH);
				WriteRegistryString(SAO_REG_KEY, SAO_REG_SUBKEY, "GamePath", g_szGamePath, MAX_PATH);
				strcpy_s(g_szGameExecutable, MAX_PATH, ofn.lpstrFile);
				return true;
			}
			else
			{
				Error("You need to specify the location of 'gta_sa.exe' in order to play!");
				return false;
			}
		}
		else
		{
			Error("You need to specify the location of 'gta_sa.exe' in order to play!");
			return false;
		}
	}
	else
	{
		if(g_szGamePath[1] == ':' && g_szGamePath[2] == '\\' && GetLastCharacterOfNullTerminatedCString(g_szGamePath, strlen(g_szGamePath)) == '\\')
		{
			sprintf_s(g_szGameExecutable, "%s%s", g_szGamePath, GAME_EXECUTABLE_NAME);

			if(_access(g_szGameExecutable, 0) == -1)
			{
				RegDeleteKeyValue(SAO_REG_KEY, SAO_REG_SUBKEY, "GamePath");
				Error("Can't find the game's executable.\nPlease restart GTA Underground and specify the location again.");
				return false;
			}

			return true;
		}
		else
		{
			RegDeleteKeyValue(SAO_REG_KEY, SAO_REG_SUBKEY, "GamePath");
			Error("The path to the game's executable is corrupted.\nPlease restart GTA Underground and specify the location again.");
			return false;
		}
	}

	return false;
}

void Error(const char *pszErrorMessage)
{
	MessageBox(NULL, pszErrorMessage, "Error!", MB_ICONERROR | MB_OK);
}

void FatalError(const char *pszErrorMessage)
{
	MessageBox(NULL, pszErrorMessage, "Fatal error!", MB_ICONERROR | MB_OK);

	ExitProcess(1);
}

#define BUFFER_COUNT 8
#define BUFFER_LENGTH 32768

const char* va(const char* string, ...)
{
	static int currentBuffer;
	static char* buffer;

	if (!buffer)
	{
		buffer = new char[BUFFER_COUNT * BUFFER_LENGTH];
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vsnprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		//GlobalError("Attempted to overrun string in call to va()!");
		exit(1);
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}

bool StripPath1(char *pszString)
{
	for(size_t i = strlen(pszString); i > 0; --i)
	{
		if(pszString[i] == '\\')
		{
			pszString[i + 1] = '\0';
			return true;
		}
	}

	return false;
}

const char * GetAppPath()
{
	static char szAppPath[MAX_PATH];
	static unsigned int nDummy;
	HMODULE hModuleHandle;

	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&nDummy, &hModuleHandle);
	GetModuleFileName(hModuleHandle, szAppPath, MAX_PATH);

	StripPath1(szAppPath);
	return szAppPath;
}

const char * GetExePath()
{
	static char szExePath[MAX_PATH];

	GetModuleFileName(GetModuleHandle(NULL), szExePath, MAX_PATH);
	StripPath1(szExePath);

	return szExePath;
}

bool CheckNameOfExecutableFromPath(char *szCheckedName, size_t checkedNameSize, char *szPath, size_t pathSize)
{
	if(!szCheckedName || !checkedNameSize || !szPath || !pathSize || szCheckedName[checkedNameSize] != 0 || szPath[pathSize] != 0)
		return false;

	if(
		(szCheckedName[checkedNameSize - 1] != 'e' && szCheckedName[checkedNameSize - 1] != 'E') ||
		(szCheckedName[checkedNameSize - 2] != 'x' && szCheckedName[checkedNameSize - 2] != 'X') ||
		(szCheckedName[checkedNameSize - 3] != 'e' && szCheckedName[checkedNameSize - 3] != 'E') ||
		szCheckedName[checkedNameSize - 4] != '.')
	{
		return false;
	}

	if(
		(szPath[pathSize - 1] != 'e' && szPath[pathSize - 1] != 'E') ||
		(szPath[pathSize - 2] != 'x' && szPath[pathSize - 2] != 'X') ||
		(szPath[pathSize - 3] != 'e' && szPath[pathSize - 3] != 'E') ||
		szPath[pathSize - 4] != '.')
	{
		return false;
	}

	size_t i = 0, j = 0;
	size_t locationOfLastSeparator = 0;
	size_t locationInCheckedName = 0;

	for(i = 0; (i < pathSize && szPath[i] != 0); i++)
	{
		if(szPath[i] == '\\')
		{
			locationOfLastSeparator = i;
		}
	}

	if((pathSize - locationOfLastSeparator - 1) != checkedNameSize)
	{
		return false;
	}

	bool bDifferences = false;

	for(i = pathSize, j = checkedNameSize; (i != 0 && j != 0 && i > locationOfLastSeparator); i--, j--)
	{
		if(szPath[i] != szCheckedName[j])
		{
			bDifferences = true;
			break;
		}
	}

	return (!bDifferences);
}

bool RemoveExecutableNameFromPath(char *szSrc, size_t srcSize, char *szDst, size_t dstSize)
{
	if(!szSrc || !srcSize || !szDst || !dstSize || szSrc[srcSize] != 0)
		return false;

	if(
		(szSrc[srcSize - 1] == 'e' || szSrc[srcSize - 1] == 'E') &&
		(szSrc[srcSize - 2] == 'x' || szSrc[srcSize - 2] == 'X') &&
		(szSrc[srcSize - 3] == 'e' || szSrc[srcSize - 3] == 'E') &&
		szSrc[srcSize - 4] == '.')
	{
		size_t i = 0;
		size_t locationOfLastSeparator = 0;

		for(i = 0; (i < srcSize && szSrc[i] != 0); i++)
		{
			if(szSrc[i] == '\\')
			{
				locationOfLastSeparator = i;
			}
		}

		for(i = 0; (i < srcSize && i <= locationOfLastSeparator && szSrc[i] != 0); i++)
		{
			szDst[i] = szSrc[i];
		}

		szDst[i] = 0;
		return true;
	}

	return false;
}

char GetLastCharacterOfNullTerminatedCString(char *szString, size_t size)
{
	if(!szString || !size || szString[size] != 0)
		return 0;

	return szString[size - 1];
}

bool ReadRegistryString(HKEY hKeyLocation, const char * szSubKey, const char * szKey, const char * szDefault, char * szData, DWORD dwSize)
{
	HKEY hKey = NULL;

	if(RegOpenKeyEx(hKeyLocation, szSubKey, NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_SZ;
		LONG lStatus = RegQueryValueEx(hKey, szKey, NULL, &dwType, (BYTE *)szData, &dwSize);
		RegCloseKey(hKey);
		return (lStatus == ERROR_SUCCESS);
	}

	if(szDefault)
		strncpy(szData, szDefault, dwSize);

	return false;
}

bool WriteRegistryString(HKEY hKeyLocation, const char * szSubKey, const char * szKey, char * szData, DWORD dwSize)
{
	HKEY hKey = NULL;
	RegOpenKeyEx(hKeyLocation, szSubKey, NULL, KEY_WRITE, &hKey);

	if(!hKey)
		RegCreateKey(hKeyLocation, szSubKey, &hKey);

	if(hKey)
	{
		DWORD dwType = REG_SZ;
		RegSetValueEx(hKey, szKey, NULL, dwType, (BYTE *)szData, dwSize);
		RegCloseKey(hKey);
		return true;
	}

	return false;
}

DWORD Unprotect(DWORD dwAddress, size_t sSize)
{
	DWORD dwOldProt;
	VirtualProtect((LPVOID)dwAddress, sSize, PAGE_EXECUTE_READWRITE, &dwOldProt);
	return dwOldProt;
}
