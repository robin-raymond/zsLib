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

#ifndef ZSLIB_INTERNAL_XML_H_b78f79d54e0d6193bc57647097d8ce0e
#define ZSLIB_INTERNAL_XML_H_b78f79d54e0d6193bc57647097d8ce0e

#include <list>

#include <zsLib/types.h>
#include <zsLib/String.h>

#define ZS_INTERNAL_XML_DEFAULT_TAB_SIZE 2

namespace zsLib
{

  namespace XML
  {

    namespace internal
    {

      class Node;
      class Document;
      class Element;
      class Attribute;
      class Text;
      class Comment;
      class Declaration;
      class Unknown;

      class ParserPos;
      class Parser;
      class ParserHelper;

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class Node
      {
      protected:
        struct make_private {};

        friend class XML::Document;
        friend class XML::Element;
        friend class XML::Attribute;
        friend class XML::Text;
        friend class XML::Comment;
        friend class XML::Declaration;
        friend class XML::Unknown;
        friend class XML::Parser;

        friend class Document;
        friend class Element;
        friend class Attribute;
        friend class Text;
        friend class Comment;
        friend class Declaration;
        friend class Unknown;
        friend class Parser;

      protected:
        Node();
        Node(const Node &) = delete;

        void cloneChildren(const NodePtr &inSelf, NodePtr inNewObject) const;

      protected:
        NodeWeakPtr mParent;

        NodePtr mPreviousSibling;
        NodePtr mNextSibling;

        NodePtr mFirstChild;
        NodePtr mLastChild;

        void *mUserData;
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class Document
      {
      public:
        Document(
                 bool inElementNameIsCaseSensative,
                 bool inAttributeNameIsCaseSensative
                 );

        Document(const Document &) = delete;

        size_t getOutputSizeXML(const GeneratorPtr &inGenerator) const;
        void writeBufferXML(const GeneratorPtr &inGenerator, char * &ioPos) const;

        size_t actualWriteJSON(
                               const GeneratorPtr &inGenerator,
                               char * &ioPos
                               ) const;

        size_t getOutputSizeJSON(const GeneratorPtr &inGenerator) const;
        void writeBufferJSON(const GeneratorPtr &inGenerator, char * &ioPos) const;

        NodePtr cloneAssignParent(NodePtr inParent) const;

      protected:
        DocumentWeakPtr mThis;

        bool        mElementNameCaseSensative;
        bool        mAttributeNameCaseSensative;
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class Element
      {
      protected:
        friend class XML::Attribute;

      public:
        typedef std::list<AttributePtr> AttributeList;

        Element() {}
        Element(const Element &) = delete;

      public:
        void parse(XML::ParserPos &ioPos);

        size_t getOutputSizeXML(const GeneratorPtr &inGenerator) const;
        void writeBufferXML(const GeneratorPtr &inGenerator, char * &ioPos) const;

        size_t actualWriteJSON(
                               const GeneratorPtr &inGenerator,
                               char * &ioPos
                               ) const;

        size_t getOutputSizeJSON(const GeneratorPtr &inGenerator) const;
        void writeBufferJSON(const GeneratorPtr &inGenerator, char * &ioPos) const;

        NodePtr cloneAssignParent(NodePtr inParent) const;

      private:
        void cloneAttributes(ElementPtr inOriginalElement);

        static void actualParse(ElementPtr &ioCurrentElement, XML::ParserPos &ioPos);
        static void parseAttributes(ElementPtr inCurrentElement, XML::ParserPos &ioPos);
        static bool parseTagSingleElementEndSlash(XML::ParserPos &ioPos);
        static bool parseIsDocumentSingleElement(ElementPtr inCurrentElement, XML::ParserPos &ioPos);
        static bool parseNewElement(ElementPtr &ioCurrentElement, XML::ParserPos &ioPos, ULONG &ioTotalPushes);
        static void parseEndTag(ElementPtr &ioCurrentElement, XML::ParserPos &ioPos, ULONG &ioTotalPushes);
        static void parsePopElement(ElementPtr &ioCurrentElement, XML::ParserPos &ioPos, ULONG &ioTotalPushes);
        static void parsePopPushes(XML::ParserPos &ioPos, ULONG &ioTotalPushes);

      protected:
        ElementWeakPtr mThis;

        String mName;

        AttributePtr mFirstAttribute;
        AttributePtr mLastAttribute;
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class Attribute
      {
      public:
        bool parse(XML::ParserPos &ioPos);

        size_t getOutputSizeXML(const GeneratorPtr &inGenerator) const;
        void writeBufferXML(const GeneratorPtr &inGenerator, char * &ioPos) const;

        size_t actualWriteJSON(
                               const GeneratorPtr &inGenerator,
                               char * &ioPos
                               ) const;

        size_t getOutputSizeJSON(const GeneratorPtr &inGenerator) const;
        void writeBufferJSON(const GeneratorPtr &inGenerator, char * &ioPos) const;

        NodePtr cloneAssignParent(NodePtr inParent) const;

      protected:
        Attribute();
        Attribute(const Attribute &) = delete;

      protected:
        String mName;
        String mValue;

        bool mValuelessAttribute;
        bool mHasQuotes;
        AttributeWeakPtr mThis;
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class Text
      {
      public:
        void parse(XML::ParserPos &ioPos);

        size_t getOutputSizeXML(const GeneratorPtr &inGenerator) const;
        void writeBufferXML(const GeneratorPtr &inGenerator, char * &ioPos) const;

        size_t getOutputSizeJSON(const GeneratorPtr &inGenerator) const;
        void writeBufferJSON(const GeneratorPtr &inGenerator, char * &ioPos) const;

        NodePtr cloneAssignParent(NodePtr inParent) const;

        String getValueInFormat(
                                UINT format,
                                bool normalize = false,
                                bool encode0xDCharactersInText = false
                                ) const;

      protected:
        Text();
        Text(const Text &) = delete;

        String mValue;
        UINT mFormat;
        UINT mOutputFormat;

        TextWeakPtr mThis;
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class Comment
      {
      public:
        Comment() {}
        Comment(const Comment &) = delete;

        void parse(XML::ParserPos &ioPos);

        size_t getOutputSizeXML(const GeneratorPtr &inGenerator) const;
        void writeBufferXML(const GeneratorPtr &inGenerator, char * &ioPos) const;

        size_t getOutputSizeJSON(const GeneratorPtr &inGenerator) const;
        void writeBufferJSON(const GeneratorPtr &inGenerator, char * &ioPos) const;

        NodePtr cloneAssignParent(NodePtr inParent) const;

      protected:
        String mValue;

        CommentWeakPtr mThis;
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class Declaration
      {
      protected:
        friend class XML::Attribute;

      public:
        Declaration() {}
        Declaration(const Declaration &) = delete;

        void parse(XML::ParserPos &ioPos);

        size_t getOutputSizeXML(const GeneratorPtr &inGenerator) const;
        void writeBufferXML(const GeneratorPtr &inGenerator, char * &ioPos) const;

        size_t getOutputSizeJSON(const GeneratorPtr &inGenerator) const;
        void writeBufferJSON(const GeneratorPtr &inGenerator, char * &ioPos) const;

        NodePtr cloneAssignParent(NodePtr inParent) const;

      protected:
        DeclarationWeakPtr mThis;

        AttributePtr mFirstAttribute;
        AttributePtr mLastAttribute;
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class Unknown
      {
      public:
        Unknown() {}
        Unknown(const Unknown &) = delete;

        void parse(XML::ParserPos &ioPos, const char *start = NULL, const char *ending = NULL);

        size_t getOutputSizeXML(const GeneratorPtr &inGenerator) const;
        void writeBufferXML(const GeneratorPtr &inGenerator, char * &ioPos) const;

        size_t getOutputSizeJSON(const GeneratorPtr &inGenerator) const;
        void writeBufferJSON(const GeneratorPtr &inGenerator, char * &ioPos) const;

        NodePtr cloneAssignParent(NodePtr inParent) const;

      protected:
        String mValue;

        UnknownWeakPtr mThis;
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class ParserPos
      {
      protected:
        ParserPos();
        ParserPos(const ParserPos &);

      protected:
        ParserWeakPtr mParser;
        DocumentWeakPtr mDocument;
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class Parser
      {
      protected:
        struct make_private {};

      public:
        friend class XML::Node;
        friend class XML::Document;
        friend class XML::Element;
        friend class XML::Attribute;
        friend class XML::Text;
        friend class XML::Comment;
        friend class XML::Declaration;
        friend class XML::Unknown;
        friend class XML::Parser;

        friend class Node;
        friend class Document;
        friend class Element;
        friend class Attribute;
        friend class Text;
        friend class Comment;
        friend class Declaration;
        friend class Unknown;

      public:
        enum ParserModes
        {
          ParserMode_AutoDetect,
          ParserMode_XML,
          ParserMode_JSON,
        };

        class AutoStack
        {
        public:
          AutoStack(const XML::ParserPos &inPos);
          ~AutoStack();
        private:
          ParserPtr mParser;
        };

        typedef std::list<String> NoChildrenElementList;

        typedef std::list<XML::ParserWarning> Warnings;
        typedef std::list<XML::ParserPos> ParserStack;

        friend class XML::ParserPos;

      public:
        Parser();

        void clearStack();
        void pushPos(const XML::ParserPos &inPos);
        XML::ParserPos popPos();

        void addWarning(ParserWarningTypes inWarning);
        void addWarning(ParserWarningTypes inWarning, const XML::ParserPos &inPos);

      protected:
        const NoChildrenElementList &getContainsNoChildrenElements() const {return mSingleElements;}

      protected:
        static bool isAlpha(char inLetter);
        static bool isDigit(char inLetter);
        static bool isAlphaNumeric(char inLetter);
        static bool isHexDigit(char inLetter);

        static bool isLegalName(char inLetter, bool inFirstLetter);

        static WCHAR findEntity(const String &inLookup);

        static bool isWhiteSpace(char inLetter);
        static bool isWhiteSpace(const XML::ParserPos &inPos);
        static bool skipWhiteSpace(XML::ParserPos &inPos);

        static bool parseAnyExceptElement(XML::ParserPos &ioPos, NodePtr parent);

        static String parseLegalName(XML::ParserPos &inPos);
        static bool skipMismatchedEndTag(XML::ParserPos &ioPos);

        static String compressWhiteSpace(const String &inString);

        static NodePtr cloneAssignParent(NodePtr inNewParent, NodePtr inExistingChild);
        static void safeAdoptAsLastChild(NodePtr inParent, NodePtr inNewChild);

        static String parseJSONString(XML::ParserPos &ioPos);
        static String parseJSONNumber(XML::ParserPos &ioPos);
        bool parseSimpleJSONValue(XML::ParserPos &ioPos, String &outResult, bool &outIsQuoted);

      protected:
        ParserWeakPtr mThis;

        ParserModes mParserMode;

        ULONG       mTabSize;               // how big is the tab size for the document
        const char *mSOF;                   // start of file

        bool        mEnableWarnings;
        Warnings    mWarnings;
        ParserStack mParserStack;

        NoChildrenElementList  mSingleElements;

        String mJSONForcedText;
        char mJSONAttributePrefix;
      };

      struct JSONStrs;

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark JSONStrs
      #pragma mark

      struct JSONStrs
      {
        const char mIndent;
        const char mIndentChar;

        const char *mNextObjectInList;
        const char *mObjectNameOpen;
        const char *mObjectNameClose;

        const char *mNextArrayInList;
        const char *mFirstArrayEmptyName;
        const char *mFirstArrayNameOpen;
        const char *mFirstArrayNameClose;
        const char *mMiddleArray;
        const char *mLastArrayOpen;
        const char *mLastArrayClose;

        const char *mChildNone;
        const char *mChildTextOnlyOpen;
        const char *mChildTextOnlyClose;

        const char *mChildComplexOpen;
        const char *mChildComplexClose;

        const char *mNextAttribute;
        const char *mAttributeEntry;

        const char *mNextText;
        const char *mTextNameOpen;
        const char *mTextNameCloseStr;
        const char *mTextNameCloseNumber;

        const char *mTextValueCloseStr;
        const char *mTextValueCloseNumer;

        const char *mNextInnerElementAfterText;

        const char *mAttributeNameOpen;
        const char *mAttributeNameCloseStr;
        const char *mAttributeNameCloseNumber;

        const char *mAttributeValueCloseStr;
        const char *mAttributeValueCloseNumber;

        const char *mNULL;
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      class Generator
      {
      protected:
        struct make_private {};

      public:
        friend class XML::Node;
        friend class XML::Document;
        friend class XML::Element;
        friend class XML::Attribute;
        friend class XML::Text;
        friend class XML::Comment;
        friend class XML::Declaration;
        friend class XML::Unknown;
        friend class XML::Parser;

        friend class Node;
        friend class Document;
        friend class Element;
        friend class Attribute;
        friend class Text;
        friend class Comment;
        friend class Declaration;
        friend class Unknown;
        friend class Parser;

      public:
        enum GeneratorModes
        {
          GeneratorMode_XML,
          GeneratorMode_JSON,
        };

        enum GeneratorJSONElementModes
        {
          GeneratorJSONElementMode_ObjectType,
          GeneratorJSONElementMode_ArrayType,
        };

        enum GeneratorJSONELementChildStates
        {
          GeneratorJSONELementChildState_None,
          GeneratorJSONELementChildState_TextOnly,
          GeneratorJSONELementChildState_Complex,
        };

        enum GeneratorJSONELementArrayPositions
        {
          GeneratorJSONELementArrayPositions_First,
          GeneratorJSONELementArrayPositions_Middle,
          GeneratorJSONELementArrayPositions_Last,
        };

        enum GeneratorJSONTextModes
        {
          GeneratorJSONTextMode_String,
          GeneratorJSONTextMode_Number,
        };

      public:
        Generator(UINT writeFlags);
        Generator(const Generator &) = delete;

      protected:
        static size_t getOutputSize(const GeneratorPtr &inGenerator, NodePtr inNode);
        static void writeBuffer(const GeneratorPtr &inGenerator, NodePtr inNode, char * &ioPos);

        static void writeBuffer(char * &ioPos, CSTR inString);

        JSONStrs &jsonStrs() const {return mStrs;}

        size_t indent(char * &ioPos) const;                 // output 1 indent length (based on depth)
        size_t copy(char * &ioPos, CSTR inString) const;    // non-interpreted copy
        size_t fill(char * &ioPos, CSTR inString) const;    // interpreted output

        void plusDepth() const  {++mDepth;}
        void minusDepth() const {--mDepth;}

        void getJSONEncodingMode(
                                 const ElementPtr &el,
                                 GeneratorJSONElementModes &outMode,
                                 GeneratorJSONELementChildStates &outChildState,
                                 GeneratorJSONELementArrayPositions &outPositionIfApplicable,
                                 GeneratorJSONTextModes &outTextModeIfApplicable,
                                 bool &outNextInList
                                 ) const;

      protected:
        GeneratorWeakPtr mThis;

        UINT mWriteFlags;
        GeneratorModes mGeneratorMode;

        String mJSONForcedText;
        char mJSONAttributePrefix;

        mutable NodePtr mGeneratorRoot;

        mutable bool mCaseSensitive;
        mutable ULONG mDepth;
        JSONStrs &mStrs;
      };

    } // namespace internal

  } // namespace XML

} // namespace zsLib

#endif //ZSLIB_INTERNAL_XML_H_b78f79d54e0d6193bc57647097d8ce0e
