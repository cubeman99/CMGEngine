#include "cmgClipboard.h"
#include <Windows.h>

namespace cmg { namespace os {

void OpenInWebBrowser(const String& url)
{
	ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
}

void OpenInWebBrowser(const StringU16& url)
{
	ShellExecuteW(0, 0, (wchar_t*) url.c_str(), 0, 0, SW_SHOW);
}

void OpenDirectoryExplorer(const Path& path)
{
	ShellExecute(NULL, NULL, path.c_str(),
		NULL, NULL, SW_SHOWNORMAL);
}

void OpenDirectoryExplorer(const StringU16& path)
{
	ShellExecuteW(NULL, NULL, (wchar_t*) path.c_str(),
		NULL, NULL, SW_SHOWNORMAL);
}

void SetClipboardText(const String& text)
{
	if (!OpenClipboard(nullptr))
		return;

	// Remove the current Clipboard contents
	if (!EmptyClipboard())
	{
		CloseClipboard();
		return;
	}

	// Set the clipboard data
	uint32 length = text.length();
	uint32 size = (length + 1) * sizeof(char);
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size);
	LPTSTR pBuffer = (LPTSTR) GlobalLock(hGlobal); 
	if (pBuffer == nullptr)
	{
		GlobalFree(hGlobal);
		hGlobal = nullptr;
	}
	else
	{
		strcpy_s((char*) pBuffer, length + 1, text.c_str());
		::SetClipboardData(CF_TEXT, hGlobal);
	}

	CloseClipboard();
	GlobalUnlock(hGlobal);
	GlobalFree(hGlobal);
}

void SetClipboardText(const StringU16& text)
{
	if (!OpenClipboard(nullptr))
		return;

	// Remove the current Clipboard contents
	if (!EmptyClipboard())
	{
		CloseClipboard();
		return;
	}

	// Set the clipboard data
	uint32 length = text.length();
	uint32 size = (length + 1) * sizeof(wchar_t);
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size);
	LPTSTR pBuffer = (LPTSTR) GlobalLock(hGlobal); 
	if (pBuffer == nullptr)
	{
		GlobalFree(hGlobal);
		hGlobal = nullptr;
	}
	else
	{
		wcscpy_s((wchar_t*) pBuffer, length + 1, (const wchar_t*) text.c_str());
		::SetClipboardData(CF_UNICODETEXT, hGlobal);
	}

	CloseClipboard();
	GlobalUnlock(hGlobal);
	GlobalFree(hGlobal);
}

String GetClipboardText()
{
	// Try opening the clipboard
	if (!OpenClipboard(nullptr))
		return "";

	// Get handle of clipboard object for ANSI text
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == nullptr)
	{
		CloseClipboard();
		return "";
	}

	// Lock the handle to get the actual text pointer
	char* pszText = static_cast<char*>(GlobalLock(hData));
	if (pszText == nullptr)
	{
		CloseClipboard();
		return "";
	}
	String text(pszText);
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();
	return text;
}

StringU16 GetClipboardUnicodeText()
{
	// Try opening the clipboard
	if (!OpenClipboard(nullptr))
		return u"";

	// Get handle of clipboard object for ANSI text
	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == nullptr)
	{
		CloseClipboard();
		return u"";
	}

	// Lock the handle to get the actual text pointer
	StringU16::value_type* pszText = static_cast<StringU16::value_type*>(
		GlobalLock(hData));
	if (pszText == nullptr)
	{
		CloseClipboard();
		return u"";
	}
	StringU16 text(pszText);
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();
	return text;
}


}}

