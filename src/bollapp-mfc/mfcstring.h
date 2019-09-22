#pragma once
#include <afx.h>
#include <string>

template<class T>
inline CString toCString(const T& v) {
    std::stringstream ss;
    ss << v;
    return fromUtf8(ss.str());
}

inline CString fromUtf8(const std::string& utf8str) {
    CString retval;

    int utf8strlen = static_cast<int>(utf8str.length());
    if (utf8strlen == 0) {
        return retval;
    }

    LPTSTR ptr = retval.GetBuffer(utf8strlen + 1);
    int newlen = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), utf8strlen, ptr, utf8strlen + 1);
    if (newlen == 0) {
        TRACE("Error running MultiByteToWideChar()");
    }
    retval.ReleaseBuffer(newlen);
    return retval;
}
