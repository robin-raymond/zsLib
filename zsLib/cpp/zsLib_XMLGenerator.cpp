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
#include <zsLib/Numeric.h>

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

#define ZS_INTERNAL_STACK_BUFFER_PADDING_SPACE 1024

#ifndef _WIN32
#define strcpy_s(x, y, z) strcpy(x, z)
#endif //ndef _WIN32

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
      #pragma mark JSONStrs
      #pragma mark

      static JSONStrs gJSONPretty =
      {
        '*',          // mIndent
        ' ',          // mIndentChar

        ",\n*",       // mNextObjectInList
        "\"",         // mObjectNameOpen
        "\" : ",      // mObjectNameClose

        ",\n*",       // mNextArrayInList
        "[\n*",       // mFirstArrayEmptyName
        "\"",         // mFirstArrayNameOpen
        "\" : [\n*",  // mFirstArrayNameClose
        ",\n*",       // mMiddleArray
        ",\n*",       // mLastArrayOpen
        "\n*]",       // mLastArrayClose

        "\"\"",       // mChildNone
        "\"",         // mChildTextOnlyOpen
        "\"",         // mChildTextOnlyClose

        "{\n*",       // mChildComplexOpen
        "\n*}",       // mChildComplexClose

        ",\n*",       // mNextAttribute
        "",           // mAttributeEntry

        ",\n*",       // mNextText
        "\"",         // mTextNameOpen
        "\" : \"",    // mTextNameCloseStr
        "\" : ",      // mTextNameCloseNumber

        "\"",         // mTextValueCloseStr
        "",           // mTextValueCloseNumer

        ",\n*",       // mNextInnerElementAfterText

        "\"",         // mAttributeNameOpen;
        "\" : \"",    // mAttributeNameCloseStr;
        "\" : ",      // mAttributeNameCloseNumber;

        "\"",         // mAttributeValueCloseStr;
        "",           // mAttributeValueCloseNumber;

        NULL
      };

      static JSONStrs gJSONNormal =
      {
        '*',          // mIndent
        ' ',          // mIndentChar

        ",",          // mNextObjectInList
        "\"",         // mObjectNameOpen
        "\":",        // mObjectNameClose

        ",",          // mNextArrayInList
        "[",          // mFirstArrayEmptyName
        "\"",         // mFirstArrayNameOpen
        "\":[",       // mFirstArrayNameClose
        ",",          // mMiddleArray
        ",",          // mLastArrayOpen
        "]",          // mLastArrayClose

        "\"\"",       // mChildNone
        "\"",         // mChildTextOnlyOpen
        "\"",         // mChildTextOnlyClose

        "{",          // mChildComplexOpen
        "}",          // mChildComplexClose

        ",",          // mNextAttribute
        "",           // mAttributeEntry

        ",",          // mNextText
        "\"",         // mTextNameOpen
        "\":\"",      // mTextNameCloseStr
        "\":",        // mTextNameCloseNumber
        
        "\"",         // mTextValueCloseStr
        "",           // mTextValueCloseNumer
        
        ",",          // mNextInnerElementAfterText

        "\"",         // mAttributeNameOpen;
        "\":\"",      // mAttributeNameCloseStr;
        "\":",        // mAttributeNameCloseNumber;

        "\"",         // mAttributeValueCloseStr;
        "",           // mAttributeValueCloseNumber;

        NULL
      };

      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark (helpers)
      #pragma mark

      //-----------------------------------------------------------------------
      static Log::Params slog(const char *message)
      {
        return Log::Params(message, "Generator");
      }

      //-------------------------------------------------------------------------
      static bool objectObjectCheck(const NodePtr &onlyThisNode)
      {
        if (onlyThisNode->isDocument()) {
          ElementPtr el = onlyThisNode->toDocument()->getFirstChildElement();
          if (el) {
            if (el->getValue().isEmpty()) {
              return false;
            }
          }
        }
        if (onlyThisNode->isElement()) {
          if (onlyThisNode->toElement()->getValue().isEmpty()) {
            return false;
          }
        }
        return true;
      }
      
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      //-----------------------------------------------------------------------
      #pragma mark
      #pragma mark XML::internal::Generator
      #pragma mark

      //-----------------------------------------------------------------------
      Generator::Generator(UINT writeFlags) :
        mCaseSensitive(true),
        mDepth(0),
        mWriteFlags(writeFlags),
        mJSONForcedText(ZS_JSON_DEFAULT_FORCED_TEXT),
        mJSONAttributePrefix(ZS_JSON_DEFAULT_ATTRIBUTE_PREFIX),
        mStrs(*((XML::Generator::JSONWriteFlag_PrettyPrint & mWriteFlags) != 0 ? &gJSONPretty : &gJSONNormal))
      {
      }

      //-----------------------------------------------------------------------
      size_t Generator::getOutputSize(const GeneratorPtr &inGenerator, NodePtr inNode)
      {
        if (!inNode)
          return 0;

        switch (inGenerator->mGeneratorMode)
        {
          case GeneratorMode_XML:
          {
            switch (inNode->getNodeType())
            {
              case XML::Node::NodeType::Document:      return inNode->toDocument()->getOutputSizeXML(inGenerator);
              case XML::Node::NodeType::Element:       return inNode->toElement()->getOutputSizeXML(inGenerator);
              case XML::Node::NodeType::Attribute:     return inNode->toAttribute()->getOutputSizeXML(inGenerator);
              case XML::Node::NodeType::Text:          return inNode->toText()->getOutputSizeXML(inGenerator);
              case XML::Node::NodeType::Comment:       return inNode->toComment()->getOutputSizeXML(inGenerator);
              case XML::Node::NodeType::Declaration:   return inNode->toDeclaration()->getOutputSizeXML(inGenerator);
              case XML::Node::NodeType::Unknown:       return inNode->toUnknown()->getOutputSizeXML(inGenerator);
              default:
              {
                ZS_THROW_BAD_STATE("missing node type in getOutputSize table")
              }
            }
            break;
          }
          case GeneratorMode_JSON:
          {
            switch (inNode->getNodeType())
            {
              case XML::Node::NodeType::Document:      return inNode->toDocument()->getOutputSizeJSON(inGenerator);
              case XML::Node::NodeType::Element:       return inNode->toElement()->getOutputSizeJSON(inGenerator);
              case XML::Node::NodeType::Attribute:     return inNode->toAttribute()->getOutputSizeJSON(inGenerator);
              case XML::Node::NodeType::Text:          return inNode->toText()->getOutputSizeJSON(inGenerator);
              case XML::Node::NodeType::Comment:       return inNode->toComment()->getOutputSizeJSON(inGenerator);
              case XML::Node::NodeType::Declaration:   return inNode->toDeclaration()->getOutputSizeJSON(inGenerator);
              case XML::Node::NodeType::Unknown:       return inNode->toUnknown()->getOutputSizeJSON(inGenerator);
              default:
              {
                ZS_THROW_BAD_STATE("missing node type in getOutputSize table")
              }
            }
            break;
          }
        }
        return 0;
      }

      //-----------------------------------------------------------------------
      void Generator::writeBuffer(const GeneratorPtr &inGenerator, NodePtr inNode, char * &ioPos)
      {
        if (!inNode)
          return;

        switch (inGenerator->mGeneratorMode)
        {
          case GeneratorMode_XML:
          {
            switch (inNode->getNodeType())
            {
              case XML::Node::NodeType::Document:      return inNode->toDocument()->writeBufferXML(inGenerator, ioPos);
              case XML::Node::NodeType::Element:       return inNode->toElement()->writeBufferXML(inGenerator, ioPos);
              case XML::Node::NodeType::Attribute:     return inNode->toAttribute()->writeBufferXML(inGenerator, ioPos);
              case XML::Node::NodeType::Text:          return inNode->toText()->writeBufferXML(inGenerator, ioPos);
              case XML::Node::NodeType::Comment:       return inNode->toComment()->writeBufferXML(inGenerator, ioPos);
              case XML::Node::NodeType::Declaration:   return inNode->toDeclaration()->writeBufferXML(inGenerator, ioPos);
              case XML::Node::NodeType::Unknown:       return inNode->toUnknown()->writeBufferXML(inGenerator, ioPos);
              default:
              {
                ZS_THROW_BAD_STATE("missing node type in writeBuffer table")
              }
            }
            break;
          }
          case GeneratorMode_JSON:
          {
            switch (inNode->getNodeType())
            {
              case XML::Node::NodeType::Document:      return inNode->toDocument()->writeBufferJSON(inGenerator, ioPos);
              case XML::Node::NodeType::Element:       return inNode->toElement()->writeBufferJSON(inGenerator, ioPos);
              case XML::Node::NodeType::Attribute:     return inNode->toAttribute()->writeBufferJSON(inGenerator, ioPos);
              case XML::Node::NodeType::Text:          return inNode->toText()->writeBufferJSON(inGenerator, ioPos);
              case XML::Node::NodeType::Comment:       return inNode->toComment()->writeBufferJSON(inGenerator, ioPos);
              case XML::Node::NodeType::Declaration:   return inNode->toDeclaration()->writeBufferJSON(inGenerator, ioPos);
              case XML::Node::NodeType::Unknown:       return inNode->toUnknown()->writeBufferJSON(inGenerator, ioPos);
              default:
              {
                ZS_THROW_BAD_STATE("missing node type in writeBuffer table")
              }
            }
            break;
          }
        }
      }

      //-----------------------------------------------------------------------
      void Generator::writeBuffer(char * &ioPos, CSTR inString)
      {
        if (NULL == inString)
          return;

        size_t length = strlen(inString);
        strcpy_s(ioPos, length+1, inString);
        ioPos += length;
      }

      //-----------------------------------------------------------------------
      size_t Generator::indent(char * &ioPos) const
      {
        if ((XML::Generator::JSONWriteFlag_PrettyPrint & mWriteFlags) == 0)
          return 0;

        if (NULL == ioPos) return mDepth;

        for (size_t index = 0; index < mDepth; ++index) {
          *ioPos = mStrs.mIndentChar;
          ++ioPos;
        }

        return mDepth;
      }

      //-----------------------------------------------------------------------
      size_t Generator::copy(char * &ioPos, CSTR inString) const
      {
        if (NULL == inString) return 0;

        size_t length = strlen(inString);
        if (NULL == ioPos) return length;

        strcpy_s(ioPos, length+1, inString);
        ioPos += length;
        return length;
      }

      //-----------------------------------------------------------------------
      size_t Generator::fill(char * &ioPos, CSTR inString) const
      {
        if (NULL == inString) return 0;

        size_t length = 0;

        for (; '\0' != *inString; ++inString) {
          if (mStrs.mIndent == *inString) {
            length += indent(ioPos);
            continue;
          }

          ++length;

          if (!ioPos) continue;

          *ioPos = *inString;
          ++ioPos;
        }

        return length;
      }

      //-----------------------------------------------------------------------
      void Generator::getJSONEncodingMode(
                                          const ElementPtr &el,
                                          GeneratorJSONElementModes &outMode,
                                          GeneratorJSONELementChildStates &outChildState,
                                          GeneratorJSONELementArrayPositions &outPositionIfApplicable,
                                          GeneratorJSONTextModes &outTextModeIfApplicable,
                                          bool &outNextInList
                                          ) const
      {
        outNextInList = false;
        outPositionIfApplicable = GeneratorJSONELementArrayPositions_First;
        outTextModeIfApplicable = GeneratorJSONTextMode_Number;

        if (el->toNode() == mGeneratorRoot) {
          outMode = GeneratorJSONElementMode_ObjectType;
        } else {
          ElementPtr prevSibling = el->getPreviousSiblingElement();
          ElementPtr nextSibling = el->getNextSiblingElement();

          bool beforeMatch = false;
          bool afterMatch = false;

          String currentName = el->getValue();

          if ((prevSibling) ||
              (nextSibling)) {
            String prevName;
            String nextName;

            if (prevSibling) {
              prevName = prevSibling->getValue();
            }
            if (nextSibling) {
              nextName = nextSibling->getValue();
            }

            bool caseSensitive = mCaseSensitive;

            if (caseSensitive) {
              beforeMatch = (prevSibling) && (currentName == prevName);
              afterMatch = (nextSibling) && (currentName == nextName);
            } else {
              beforeMatch = (prevSibling) && (0 == currentName.compareNoCase(prevName));
              afterMatch = (nextSibling) && (0 == currentName.compareNoCase(nextName));
            }
          }

          if ((beforeMatch) || (afterMatch)) {
            outMode = GeneratorJSONElementMode_ArrayType;
            if (beforeMatch) {
              if (afterMatch) {
                outPositionIfApplicable = GeneratorJSONELementArrayPositions_Middle;
              } else {
                outPositionIfApplicable = GeneratorJSONELementArrayPositions_Last;
              }
            } else {
              outPositionIfApplicable = GeneratorJSONELementArrayPositions_First;
            }
          } else {
            outMode = GeneratorJSONElementMode_ObjectType;
          }
        }

        if (el->getFirstAttribute()) {
          outChildState = GeneratorJSONELementChildState_Complex;
        } else {
          if (el->hasChildren()) {
            if (el->getFirstChildElement()) {
              outChildState = GeneratorJSONELementChildState_Complex;
            } else {
              outChildState = GeneratorJSONELementChildState_None;

              NodePtr node = el->getFirstChild();
              while (node)
              {
                if (node->isText()) {
                  outChildState = GeneratorJSONELementChildState_TextOnly;
                  break;
                }
                node = node->getNextSibling();
              }
            }
          } else {
            outChildState = GeneratorJSONELementChildState_None;
          }
        }

        switch (outChildState) {
          case GeneratorJSONELementChildState_None: break;
          case GeneratorJSONELementChildState_TextOnly:
          case GeneratorJSONELementChildState_Complex:
          {
            outTextModeIfApplicable = GeneratorJSONTextMode_Number;
            NodePtr node = el->getFirstChild();
            while (node)
            {
              if (node->isText()) {
                TextPtr text = node->toText();
                if (XML::Text::Format_JSONNumberEncoded != text->getFormat()) {
                  outTextModeIfApplicable = GeneratorJSONTextMode_String;
                  break;
                }
              }
              node = node->getNextSibling();
            }
            break;
          }
        }

        if (el->getPreviousSiblingElement()) {
          outNextInList = true;
        }
      }

    } // namespace internal

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark XML::Generator
    #pragma mark

    //-------------------------------------------------------------------------
    GeneratorPtr Generator::createXMLGenerator(XMLWriteFlags writeFlags)
    {
      GeneratorPtr pThis(make_shared<Generator>(make_private{}, writeFlags));
      pThis->mThis = pThis;
      pThis->mGeneratorMode = GeneratorMode_XML;
      return pThis;
    }

    //-------------------------------------------------------------------------
    GeneratorPtr Generator::createJSONGenerator(
                                                const char *forcedText,
                                                char attributePrefix
                                                )
    {
      return Generator::createJSONGenerator(JSONWriteFlag_None, forcedText, attributePrefix);
    }

    //-------------------------------------------------------------------------
    GeneratorPtr Generator::createJSONGenerator(
                                                JSONWriteFlags writeFlags,
                                                const char *forcedText,
                                                char attributePrefix
                                                )
    {
      GeneratorPtr pThis(make_shared<Generator>(make_private{}, writeFlags));
      pThis->mThis = pThis;
      pThis->mGeneratorMode = GeneratorMode_JSON;
      pThis->mJSONForcedText = (forcedText ? forcedText : "");
      pThis->mJSONAttributePrefix = attributePrefix;
      return pThis;
    }

    //-------------------------------------------------------------------------
    Generator::Generator(
                         const make_private &,
                         UINT writeFlags
                         ) :
      internal::Generator(writeFlags)
    {
    }

    //-------------------------------------------------------------------------
    size_t Generator::getOutputSize(const NodePtr &onlyThisNode) const
    {
      mGeneratorRoot = onlyThisNode;

      NodePtr root = onlyThisNode->getRoot();
      if (root->isDocument()) {
        mCaseSensitive = root->toDocument()->isElementNameIsCaseSensative();
      }

      char *ioPos = NULL;
      size_t result = 0;

      bool objectOpen = objectObjectCheck(onlyThisNode);

      if (GeneratorMode_JSON == mGeneratorMode) {
        if (objectOpen) {
          plusDepth();
          result += fill(ioPos, mStrs.mChildComplexOpen);
        }
      }

      result += internal::Generator::getOutputSize(mThis.lock(), onlyThisNode);

      if (GeneratorMode_JSON == mGeneratorMode) {
        if (objectOpen) {
          minusDepth();
          result += fill(ioPos, mStrs.mChildComplexClose);
        }
      }

      mGeneratorRoot.reset();

      ZS_THROW_BAD_STATE_IF(0 != mDepth)

      return result;
    }

    //-------------------------------------------------------------------------
    std::unique_ptr<char[]> Generator::write(const NodePtr &onlyThisNode, size_t *outLength) const
    {
      size_t totalSize = getOutputSize(onlyThisNode);

      mGeneratorRoot = onlyThisNode;

      NodePtr root = onlyThisNode->getRoot();
      if (root->isDocument()) {
        mCaseSensitive = root->toDocument()->isElementNameIsCaseSensative();
      }

      std::unique_ptr<char[]> buffer(new char[totalSize+1]);
      char *ioPos = buffer.get();
      *ioPos = 0;

      bool objectOpen = objectObjectCheck(onlyThisNode);

      if (GeneratorMode_JSON == mGeneratorMode) {
        if (objectOpen) {
          plusDepth();
          fill(ioPos, mStrs.mChildComplexOpen);
        }
      }

      Generator::writeBuffer(mThis.lock(), onlyThisNode, ioPos);

      if (GeneratorMode_JSON == mGeneratorMode) {
        if (objectOpen) {
          minusDepth();
          fill(ioPos, mStrs.mChildComplexClose);
        }
      }

      *ioPos = 0;
      if (NULL != outLength)
        *outLength = totalSize;

      mGeneratorRoot.reset();

      ZS_THROW_BAD_STATE_IF(0 != mDepth)

      return buffer;
    }

    //-------------------------------------------------------------------------
    Generator::XMLWriteFlags Generator::getXMLWriteFlags() const
    {
      return static_cast<XMLWriteFlags>(mWriteFlags);
    }

    //-------------------------------------------------------------------------
    Generator::JSONWriteFlags Generator::getJSONWriteFlags() const
    {
      return static_cast<JSONWriteFlags>(mWriteFlags);
    }

  } // namespace XML

} // namespace zsLib
