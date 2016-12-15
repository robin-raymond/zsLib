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

#include <zsLib/XML.h>
#include <zsLib/Exception.h>
#include <zsLib/SafeInt.h>


#ifdef _WIN32
namespace std {
	inline int tolower(int c) { return _tolower(c); }
}
#endif //_WIN32

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

namespace zsLib
{

  namespace XML
  {

    namespace internal
    {
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark XML::internal::ParserPos
      #pragma mark

      //-----------------------------------------------------------------------
      ParserPos::ParserPos()
      {
      }

      //-----------------------------------------------------------------------
      ParserPos::ParserPos(const ParserPos &inPos) :
        mParser(inPos.mParser),
        mDocument(inPos.mDocument)
      {
      }

      //-----------------------------------------------------------------------
      static size_t calculateColumnFromSOL(const char *const inPos, const char *const inStart, ULONG inTabSize)
      {
        // find the start of the line
        const char *startLine = inPos;
        while (startLine > inStart)
        {
          if ('\n' == (*(startLine - 1)))
            break;

          // if the previous character is an EOL, then we have reached the start of the line
          if ('\r' == (*(startLine - 1)))
          {
            if ('\n' != (*(startLine)))
              break;                     // this is a MAC EOL so therefor it is the start of the line
          }

          --startLine;
        }

        ULONG column = 1;
        const char *pos = startLine;
        while (pos != inPos)
        {
          if ('\t' == *pos)
          {
            // this is a tab character, column changes because of it
            column = ((((column-1)/inTabSize)+1)*inTabSize)+1;
          }
          else if ('\r' != *pos)
          {
            // \r are windows EOL in this case and should not count towards the column count
            ++column;
          }

          ++pos;
        }
        return column;
      }

    } // namespace internal

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark XML::ParserPos
    #pragma mark

    //-------------------------------------------------------------------------
    ParserPos::ParserPos() :
      mRow(0),
      mColumn(0),
      mPos(NULL)
    {
    }

    //-------------------------------------------------------------------------
    ParserPos::ParserPos(const ParserPos &inPos) :
      internal::ParserPos(inPos),
      mRow(inPos.mRow),
      mColumn(inPos.mColumn),
      mPos(inPos.mPos)
    {
    }

    //-------------------------------------------------------------------------
    ParserPos::ParserPos(
                         Parser &inParser,
                         Document &inDocument
                         ) :
      mRow(0),
      mColumn(0),
      mPos(NULL)
    {
      setParser(inParser.toParser());
      setDocument(inDocument.toDocument());
      setSOF();
    }

    //-------------------------------------------------------------------------
    ParserPos::ParserPos(
                         ParserPtr inParser,
                         DocumentPtr inDocument
                         ) :
      mRow(0),
      mColumn(0),
      mPos(NULL)
    {
      setParser(inParser);
      setDocument(inDocument);
      setSOF();
    }

    //-------------------------------------------------------------------------
    bool ParserPos::isSOF() const
    {
      if (NULL == mPos)
        return false;

      ParserPtr parser = getParser();
      ZS_THROW_BAD_STATE_IF(!parser)

      return (mPos == parser->mSOF);
    }

    //-------------------------------------------------------------------------
    bool ParserPos::isEOF() const
    {
      if (NULL == mPos)
        return true;

      return (0 == (*mPos));
    }

    //-------------------------------------------------------------------------
    void ParserPos::setSOF()
    {
      ParserPtr parser = getParser();
      ZS_THROW_BAD_STATE_IF(!parser)

      mPos = parser->mSOF;
      mRow = 1;
      mColumn = 1;
      ZS_THROW_BAD_STATE_IF(NULL == mPos)
    }

    //-------------------------------------------------------------------------
    void ParserPos::setEOF()
    {
      while (!isEOF()) {
        ++(*this);
      }
    }

    //-------------------------------------------------------------------------
    void ParserPos::setParser(ParserPtr inParser)
    {
      mParser = inParser;
    }

    //-------------------------------------------------------------------------
    ParserPtr ParserPos::getParser() const
    {
      return mParser.lock();
    }

    //-------------------------------------------------------------------------
    void ParserPos::setDocument(DocumentPtr inDocument)
    {
      mDocument = inDocument;
    }

    //-------------------------------------------------------------------------
    DocumentPtr ParserPos::getDocument() const
    {
      return mDocument.lock();
    }

    //-------------------------------------------------------------------------
    ParserPos ParserPos::operator++() const
    {
      ParserPos temp(*this);
      ++temp;
      return temp;
    }

    //-------------------------------------------------------------------------
    ParserPos &ParserPos::operator++()
    {
      (*this) += 1;
      return (*this);
    }

    //-------------------------------------------------------------------------
    ParserPos ParserPos::operator--() const
    {
      ParserPos temp(*this);
      --temp;
      return temp;
    }

    //-------------------------------------------------------------------------
    ParserPos &ParserPos::operator--()
    {
      (*this) -= 1;
      return (*this);
    }

    //-------------------------------------------------------------------------
    size_t ParserPos::operator-(const ParserPos &inPos) const
    {
      ParserPos startPos = inPos;
      ParserPos endPos(*this);

      const char *start = startPos.mPos;

      if (!start)
      {
        if (endPos.isEOF())
          return 0;

        startPos.setSOF();
        startPos.mPos += strlen(startPos.mPos);
        start = startPos.mPos;
      }

      const char *end = endPos.mPos;
      if (!end)
      {
        if (startPos.isEOF())
          return 0;

        endPos.setSOF();
        endPos.mPos += strlen(endPos.mPos);
        end = endPos.mPos;
      }

      return end - start;
    }

    //-------------------------------------------------------------------------
    ParserPos &ParserPos::operator+=(size_t inDistance)
    {
      ParserPtr parser(getParser());

      if (NULL == mPos)
        return *this;

      while (0 != inDistance)
      {
        if (isEOF())
          return *this;

        switch (*mPos)
        {
          case '\r':
          {
            // does not advance row, just resets column
            if ('\n' != (*(mPos+1)))
            {
              // this is MAC EOL \r therefor we can do the same thing as \n
              ++mRow;
              mColumn = 1;
              break;
            }
            // this is DOS EOL \r\n - ingore the \r in the position
            break;
          }
          case '\n':
          {
            // advances row, resets column to zero
            ++mRow;
            mColumn = 1;
            break;
          }
          case '\t':
          {
            ZS_THROW_BAD_STATE_IF(!parser)
            ZS_THROW_BAD_STATE_IF(0 == mColumn)

            ULONG tabSize = parser->getTabSize();
            // if tab size were set to 3
            // 1..2..3 would go to position 4
            // 4..5..6 would go to position 7

            mColumn = ((((mColumn-1)/tabSize)+1)*tabSize)+1;
            break;
          }
          default:
          {
            ++mColumn;
            break;
          }
        }
        ++mPos;
        --inDistance;
      }

      return *this;
    }

    //-------------------------------------------------------------------------
    ParserPos &ParserPos::operator-=(size_t inDistance)
    {
      ParserPtr parser(getParser());

      if (NULL == mPos)
      {
        ParserPos temp(*this);
        temp.setSOF();
        temp += strlen(temp.mPos);
        (*this) = temp;
      }

      while (0 != inDistance)
      {
        if (isSOF())
          return *this;

        // assuming column and row should be accurate at this point, but could become inaccurate
        // if we run into a special character

        --mPos;
        --inDistance;

        switch (*mPos)
        {
          case '\r':
          case '\n':
          case '\t':
          {
            if ('\n' == *mPos)
            {
              ZS_THROW_BAD_STATE_IF(mRow < 2)   // how did the count become inaccurate
              --mRow;
            }
            if ('\r' == *mPos)
            {
              if ('\n' != (*(mPos+1)))
              {
                // this is a MAC EOL
                ZS_THROW_BAD_STATE_IF(mRow < 2)   // how did the count become inaccurate
                --mRow;
              }
            }

            ZS_THROW_BAD_STATE_IF(!parser)
            ZS_THROW_BAD_STATE_IF(NULL == parser->mSOF)

            // these characters cause the column to get messed up, simplest method is to
            // recalculate the column position from the start of the line
            mColumn = SafeInt<decltype(mColumn)>(internal::calculateColumnFromSOL(mPos, parser->mSOF, parser->mTabSize));
            break;
          }
          default:
          {
            ZS_THROW_BAD_STATE_IF(mColumn < 2)   // how did the count become inaccurate

            --mColumn;
            break;
          }
        }
      }

      return *this;
    }

    //-------------------------------------------------------------------------
    bool ParserPos::operator==(const ParserPos &inPos)
    {
      return (mPos == inPos.mPos);
    }

    //-------------------------------------------------------------------------
    bool ParserPos::operator!=(const ParserPos &inPos)
    {
      return (mPos != inPos.mPos);
    }

    //-------------------------------------------------------------------------
    char ParserPos::operator*() const
    {
      if (isEOF())
        return 0;

      return *mPos;
    }

    //-------------------------------------------------------------------------
    ParserPos::operator CSTR() const
    {
      if (mPos) return mPos;

      return "\0\0";
    }

    //-------------------------------------------------------------------------
    bool ParserPos::isString(CSTR inString, bool inCaseSensative) const
    {
      if (NULL == inString)
        return isEOF();

      ParserPos temp(*this);

      const char *pos = inString;
      while ((*pos) && (*temp))
      {
        if (inCaseSensative)
        {
          if (*temp != *pos)
            return false;
        }
        else
        {
          if (std::tolower(*temp) != std::tolower(*pos))
            return false;
        }
        ++temp;
        ++pos;
      }

      return (!(*pos));
    }

    //-------------------------------------------------------------------------
    ParserPos operator+(const ParserPos &inPos, size_t inDistance)
    {
      ParserPos temp(inPos);
      temp += inDistance;
      return temp;
    }

    //-------------------------------------------------------------------------
    ParserPos operator-(const ParserPos &inPos, size_t inDistance)
    {
      ParserPos temp(inPos);
      temp -= inDistance;
      return temp;
    }

    //-------------------------------------------------------------------------
    ParserPos operator+(const ParserPos &inPos, int inDistance)
    {
      return inPos + ((size_t)inDistance);
    }

    //-------------------------------------------------------------------------
    ParserPos operator-(const ParserPos &inPos, int inDistance)
    {
      return inPos - ((size_t)inDistance);
    }

  } // namespace XML

} // namespace zsLib
