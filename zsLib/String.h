/*

 Copyright (c) 2014, Robin Raymond
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The views and conclusions contained in the software and documentation are those
 of the authors and should not be interpreted as representing official policies,
 either expressed or implied, of the FreeBSD Project.
 
 */

#pragma once

#include <string>

#include <zsLib/types.h>

namespace zsLib
{
  class String : public std::string {

  public:
    String();
    String(const String &);

    String(CSTR);
    String(CWSTR);
    String(const std::string &);
    String(CSTR, size_t);
    String(CWSTR, size_t);
    explicit String(const std::wstring &);

    static String copyFrom(CSTR, size_t maxCharacters);
    static String copyFromUnicodeSafe(CSTR, size_t maxCharacters);

    std::wstring wstring() const;

    bool isEmpty() const;
    bool hasData() const;
    size_t getLength() const;

    operator CSTR() const;

    String &operator=(const std::string &);
    String &operator=(const std::wstring &);
    String &operator=(const String &);
    String &operator=(CSTR);
    String &operator=(CWSTR);

    int compareNoCase(CSTR) const;
    int compareNoCase(const String &) const;

    void toLower();
    void toUpper();

    void trim(CSTR chars = " \t\r\n\v\f");
    void trimLeft(CSTR chars = " \t\r\n\v\f");
    void trimRight(CSTR chars = " \t\r\n\v\f");

    size_t lengthUnicodeSafe() const;
    String substrUnicodeSafe(size_t pos = 0, size_t n = std::string::npos ) const;
    WCHAR atUnicodeSafe(size_t pos) const;

    void replaceAll(CSTR findStr, CSTR replaceStr, size_t totalOccurances = std::string::npos);
  };
}
