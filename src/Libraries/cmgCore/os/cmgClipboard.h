#ifndef _CMG_CORE_OS_CLIPBOARD_H_
#define _CMG_CORE_OS_CLIPBOARD_H_

#include <cmgCore/io/cmgPath.h>

namespace cmg { namespace os {

void OpenInWebBrowser(const String& url);
void OpenInWebBrowser(const StringU16& url);
void OpenDirectoryExplorer(const Path& path);
void OpenDirectoryExplorer(const StringU16& path);
void SetClipboardText(const String& text);
void SetClipboardText(const StringU16& text);
String GetClipboardText();
StringU16 GetClipboardUnicodeText();

}}

#endif // _CMG_CORE_OS_CLIPBOARD_H_