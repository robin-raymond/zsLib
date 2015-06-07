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

namespace zsLib {ZS_DECLARE_SUBSYSTEM(zsLib)}

#pragma warning(push)
#pragma warning(disable: 4290)

#define ZSLIB_XML_UNKNOWN_ELEMENT_NAME_OUTPUT "unknown"

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
      #pragma mark XML::internal::Element
      #pragma mark

      //-----------------------------------------------------------------------
      void Element::parse(XML::ParserPos &ioPos)
      {
        ElementPtr temp = mThis.lock();
        actualParse(temp, ioPos);
      }

      //-----------------------------------------------------------------------
      size_t Element::getOutputSizeXML(const GeneratorPtr &inGenerator) const
      {
        class Walker : public WalkSink
        {
        public:
          Walker(size_t &outResult, const GeneratorPtr &inGenerator) : mResult(outResult), mGenerator(inGenerator) {}

          virtual bool onElementEnter(ElementPtr inNode)
          {
            mResult += strlen("<");
            if (inNode->getValue().isEmpty()) {
              mResult += strlen(ZSLIB_XML_UNKNOWN_ELEMENT_NAME_OUTPUT);
            } else {
              mResult += inNode->mName.getLength();
            }

            if (inNode->mFirstAttribute)
            {
              NodePtr child = inNode->mFirstAttribute;
              while (child)
              {
                AttributePtr attribute = child->toAttribute();
                mResult += strlen(" ");
                mResult += Generator::getOutputSize(mGenerator, attribute);
                child = child->getNextSibling();
              }
            }

            bool forceEndTag = (0 != (XML::Generator::XMLWriteFlag_ForceElementEndTag & (mGenerator->getXMLWriteFlags())));

            if ((inNode->hasChildren()) || (forceEndTag))
            {
              mResult += strlen(">");

              // inner content would go here

              mResult += strlen("</");
              if (inNode->getValue().isEmpty()) {
                mResult += strlen(ZSLIB_XML_UNKNOWN_ELEMENT_NAME_OUTPUT);
              } else {
                mResult += inNode->mName.getLength();
              }
              mResult += strlen(">");
            }
            else
            {
              mResult += strlen(" />");
            }
            return false;
          }

          virtual bool onText(TextPtr inNode)                      {mResult += inNode->getOutputSizeXML(mGenerator); return false;}
          virtual bool onComment(CommentPtr inNode)                {mResult += inNode->getOutputSizeXML(mGenerator); return false;}
          virtual bool onDeclarationEnter(DeclarationPtr inNode)   {mResult += inNode->getOutputSizeXML(mGenerator); return false;}
          virtual bool onUnknown(UnknownPtr inNode)                {mResult += inNode->getOutputSizeXML(mGenerator); return false;}

        private:
          const GeneratorPtr &mGenerator;
          size_t &mResult;
        };

        ElementPtr self(mThis.lock());

        size_t result = 0;
        XML::Node::FilterList filter;
        filter.push_back(XML::Node::NodeType::Element);
        filter.push_back(XML::Node::NodeType::Text);
        filter.push_back(XML::Node::NodeType::Comment);
        filter.push_back(XML::Node::NodeType::Declaration);
        filter.push_back(XML::Node::NodeType::Unknown);
        Walker walker(result, inGenerator);
        self->walk(walker, &filter);

        return result;
      }

      //-----------------------------------------------------------------------
      void Element::writeBufferXML(const GeneratorPtr &inGenerator, char * &ioPos) const
      {
        class Walker : public WalkSink
        {
        public:
          Walker(const GeneratorPtr &inGenerator, char * &ioPos) :
            mGenerator(inGenerator),
            mPos(ioPos)
          {}

          virtual bool onElementEnter(ElementPtr inNode)
          {
            Generator::writeBuffer(mPos, "<");
            if (inNode->getValue().isEmpty()) {
              Generator::writeBuffer(mPos, ZSLIB_XML_UNKNOWN_ELEMENT_NAME_OUTPUT);
            } else {
              Generator::writeBuffer(mPos, inNode->mName);
            }

            if (inNode->mFirstAttribute)
            {
              NodePtr child = inNode->mFirstAttribute;
              while (child)
              {
                AttributePtr attribute = child->toAttribute();
                Generator::writeBuffer(mPos, " ");
                Generator::writeBuffer(mGenerator, attribute, mPos);
                child = child->getNextSibling();
              }
            }

            bool forceEndTag = (0 != (XML::Generator::XMLWriteFlag_ForceElementEndTag & (mGenerator->getXMLWriteFlags())));

            if ((inNode->hasChildren()) || (forceEndTag))
              Generator::writeBuffer(mPos, ">");
            else
              Generator::writeBuffer(mPos, " />");

            return false;
          }

          virtual bool onElementExit(ElementPtr inNode)
          {
            bool forceEndTag = (0 != (XML::Generator::XMLWriteFlag_ForceElementEndTag & (mGenerator->getXMLWriteFlags())));

            if ((!inNode->hasChildren()) && (!forceEndTag))
              return false;

            // put on the final end tag which won't know about
            Generator::writeBuffer(mPos, "</");
            if (inNode->getValue().isEmpty()) {
              Generator::writeBuffer(mPos, ZSLIB_XML_UNKNOWN_ELEMENT_NAME_OUTPUT);
            } else {
              Generator::writeBuffer(mPos, inNode->mName);
            }
            Generator::writeBuffer(mPos, ">");
            return false;
          }

          virtual bool onText(TextPtr inNode)                      {Generator::writeBuffer(mGenerator, inNode, mPos); return false;}
          virtual bool onComment(CommentPtr inNode)                {Generator::writeBuffer(mGenerator, inNode, mPos); return false;}
          virtual bool onDeclarationEnter(DeclarationPtr inNode)   {Generator::writeBuffer(mGenerator, inNode, mPos); return false;}
          virtual bool onUnknown(UnknownPtr inNode)                {Generator::writeBuffer(mGenerator, inNode, mPos); return false;}

        private:
          const GeneratorPtr &mGenerator;
          char * &mPos;
        };

        ElementPtr self(mThis.lock());

        XML::Node::FilterList filter;
        filter.push_back(XML::Node::NodeType::Element);
        filter.push_back(XML::Node::NodeType::Text);
        filter.push_back(XML::Node::NodeType::Comment);
        filter.push_back(XML::Node::NodeType::Declaration);
        filter.push_back(XML::Node::NodeType::Unknown);
        Walker walker(inGenerator, ioPos);
        self->walk(walker, &filter);
      }

      //-----------------------------------------------------------------------
      size_t Element::actualWriteJSON(
                                      const GeneratorPtr &inGenerator,
                                      char * &ioPos
                                      ) const
      {
        class Walker : public WalkSink
        {
        public:
          Walker(
                 const GeneratorPtr &inGenerator,
                 char * &ioPos,
                 size_t &result
                 ) :
          mGeneratorPtr(inGenerator),
          mGenerator(*inGenerator),
          mPos(ioPos),
          mResult(result),
          mWriteFlags(inGenerator->getJSONWriteFlags()),
          mStrs(inGenerator->jsonStrs())
          {}

          virtual bool onElementEnter(ElementPtr el)
          {
            Generator::GeneratorJSONElementModes mode = Generator::GeneratorJSONElementMode_ObjectType;
            Generator::GeneratorJSONELementChildStates childState = Generator::GeneratorJSONELementChildState_None;
            Generator::GeneratorJSONELementArrayPositions position = Generator::GeneratorJSONELementArrayPositions_First;
            Generator::GeneratorJSONTextModes textMode = Generator::GeneratorJSONTextMode_Number;
            bool nextInList = false;
            mGenerator.getJSONEncodingMode(el, mode, childState, position, textMode, nextInList);

            switch (mode) {
              case Generator::GeneratorJSONElementMode_ObjectType:
              {
                if (nextInList) {
                  mResult += mGenerator.fill(mPos, mStrs.mNextObjectInList);
                }
                mResult += mGenerator.fill(mPos, mStrs.mObjectNameOpen);
                mResult += mGenerator.copy(mPos, XML::Parser::convertToJSONEncoding(el->getValue()));
                mResult += mGenerator.fill(mPos, mStrs.mObjectNameClose);
                break;
              }
              case Generator::GeneratorJSONElementMode_ArrayType:
              {
                switch (position) {
                  case Generator::GeneratorJSONELementArrayPositions_First:
                  {
                    if (nextInList) {
                      mResult += mGenerator.fill(mPos, mStrs.mNextArrayInList);
                    }
                    mGenerator.plusDepth();
                    if (el->getValue().isEmpty()) {
                      mResult += mGenerator.fill(mPos, mStrs.mFirstArrayEmptyName);
                    } else {
                      mResult += mGenerator.fill(mPos, mStrs.mFirstArrayNameOpen);
                      mResult += mGenerator.copy(mPos, XML::Parser::convertToJSONEncoding(el->getValue()));
                      mResult += mGenerator.fill(mPos, mStrs.mFirstArrayNameClose);
                    }
                    break;
                  }
                  case Generator::GeneratorJSONELementArrayPositions_Middle:
                  {
                    mResult += mGenerator.fill(mPos, mStrs.mMiddleArray);
                    break;
                  }
                  case Generator::GeneratorJSONELementArrayPositions_Last:
                  {
                    mResult += mGenerator.fill(mPos, mStrs.mLastArrayOpen);
                    break;
                  }
                }
                break;
              }
            }

            switch (childState) {
              case Generator::GeneratorJSONELementChildState_None:      {
                mResult += mGenerator.fill(mPos, mStrs.mChildNone);
                break;
              }
              case Generator::GeneratorJSONELementChildState_TextOnly:  {
                if (Generator::GeneratorJSONTextMode_String == textMode) {
                  mResult += mGenerator.fill(mPos, mStrs.mChildTextOnlyOpen);
                }
                NodePtr node = el->getFirstChild();
                while (node) {
                  if (mPos) {
                    Generator::writeBuffer(mGeneratorPtr, node, mPos);
                  } else {
                    mResult += Generator::getOutputSize(mGeneratorPtr, node);
                  }
                  node = node->getNextSibling();
                }
                if (Generator::GeneratorJSONTextMode_String == textMode) {
                  mResult += mGenerator.fill(mPos, mStrs.mChildTextOnlyClose);
                }
                break;
              }
              case Generator::GeneratorJSONELementChildState_Complex:   {
                mGenerator.plusDepth();
                mResult += mGenerator.fill(mPos, mStrs.mChildComplexOpen);

                bool first = true;

                NodePtr attribute = el->getFirstAttribute();
                while (attribute) {
                  if (!first) {
                    mResult += mGenerator.fill(mPos, mStrs.mNextAttribute);
                  }
                  first = false;

                  mResult += mGenerator.fill(mPos, mStrs.mAttributeEntry);

                  if (mPos) {
                    Generator::writeBuffer(mGeneratorPtr, attribute, mPos);
                  } else {
                    mResult += Generator::getOutputSize(mGeneratorPtr, attribute);
                  }

                  attribute = attribute->getNextSibling();
                }

                bool found = false;

                // scope: see if there are any text elements at all inside
                {
                  NodePtr node = el->getFirstChild();
                  while (node) {
                    if (node->isText()) {
                      found = true;
                      break;
                    }
                    node = node->getNextSibling();
                  }
                }

                if (found) {
                  if (!first) {
                    mResult += mGenerator.fill(mPos, mStrs.mNextText);
                  }
                  first = false;
                  mResult += mGenerator.fill(mPos, mStrs.mTextNameOpen);
                  mResult += mGenerator.copy(mPos, mGenerator.mJSONForcedText);
                  if (Generator::GeneratorJSONTextMode_String == textMode) {
                    mResult += mGenerator.fill(mPos, mStrs.mTextNameCloseStr);
                  } else {
                    mResult += mGenerator.fill(mPos, mStrs.mTextNameCloseNumber);
                  }

                  NodePtr node = el->getFirstChild();
                  while (node) {
                    if (node->isText()) {
                      if (mPos) {
                        Generator::writeBuffer(mGeneratorPtr, node, mPos);
                      } else {
                        mResult += Generator::getOutputSize(mGeneratorPtr, node);
                      }
                    }
                    node = node->getNextSibling();
                  }

                  if (Generator::GeneratorJSONTextMode_String == textMode) {
                    mResult += mGenerator.fill(mPos, mStrs.mTextValueCloseStr);
                  } else {
                    mResult += mGenerator.fill(mPos, mStrs.mTextValueCloseNumer);
                  }
                }

                if ((!first) &&
                    (el->getFirstChildElement())) {
                  mResult += mGenerator.fill(mPos, mStrs.mNextInnerElementAfterText);
                }
                break;
              }
            }

            return false;
          }

          virtual bool onElementExit(ElementPtr el)
          {
            Generator::GeneratorJSONElementModes mode = Generator::GeneratorJSONElementMode_ObjectType;
            Generator::GeneratorJSONELementChildStates childState = Generator::GeneratorJSONELementChildState_None;
            Generator::GeneratorJSONELementArrayPositions position = Generator::GeneratorJSONELementArrayPositions_First;
            Generator::GeneratorJSONTextModes textMode = Generator::GeneratorJSONTextMode_Number;
            bool nextInList = false;
            mGenerator.getJSONEncodingMode(el, mode, childState, position, textMode, nextInList);

            switch (childState) {
              case Generator::GeneratorJSONELementChildState_None:      {
                break;
              }
              case Generator::GeneratorJSONELementChildState_TextOnly:  {
                break;
              }
              case Generator::GeneratorJSONELementChildState_Complex:   {
                mGenerator.minusDepth();
                mResult += mGenerator.fill(mPos, mStrs.mChildComplexClose);
                break;
              }
            }

            switch (mode) {
              case Generator::GeneratorJSONElementMode_ObjectType:
              {
                break;
              }
              case Generator::GeneratorJSONElementMode_ArrayType:
              {
                switch (position) {
                  case Generator::GeneratorJSONELementArrayPositions_First:
                  {
                    break;
                  }
                  case Generator::GeneratorJSONELementArrayPositions_Middle:
                  {
                    break;
                  }
                  case Generator::GeneratorJSONELementArrayPositions_Last:
                  {
                    mGenerator.minusDepth();
                    mResult += mGenerator.fill(mPos, mStrs.mLastArrayClose);
                    break;
                  }
                }
                break;
              }
            }

            return false;
          }

        private:
          const GeneratorPtr &mGeneratorPtr;
          const Generator &mGenerator;
          char * &mPos;
          size_t &mResult;
          ULONG mWriteFlags;
          const JSONStrs &mStrs;
        };

        size_t result = 0;

        ElementPtr self(mThis.lock());

        XML::Node::FilterList filter;
        filter.push_back(XML::Node::NodeType::Element);
        Walker walker(inGenerator, ioPos, result);
        self->walk(walker, &filter);

        return result;
      }

      //-----------------------------------------------------------------------
      size_t Element::getOutputSizeJSON(const GeneratorPtr &inGenerator) const
      {
        char *pos = NULL;
        return actualWriteJSON(inGenerator, pos);
      }
      
      //-----------------------------------------------------------------------
      void Element::writeBufferJSON(const GeneratorPtr &inGenerator, char * &ioPos) const
      {
        actualWriteJSON(inGenerator, ioPos);
      }

      //-----------------------------------------------------------------------
      NodePtr Element::cloneAssignParent(NodePtr inParent) const
      {
        ElementPtr newObject(XML::Element::create());
        Parser::safeAdoptAsLastChild(inParent, newObject);

        ElementPtr startingElement = mThis.lock();

        newObject->mName = mName;
        newObject->cloneAttributes(startingElement);

        // going to walk the tree in a non-recursive fashion
        NodePtr walk = startingElement;
        NodePtr cloneWalk = newObject;

        bool allowChildren = true;

        while (walk)
        {
          if ((walk->getFirstChild()) &&
              (allowChildren))
          {
            // there is a child
            walk = walk->getFirstChild();
            if (!walk->isElement())
            {
              // clone and continue walking
              cloneWalk = Parser::cloneAssignParent(cloneWalk, walk);
              continue;
            }

            // this is an element, do some special handling
            ElementPtr newObject(XML::Element::create());
            Parser::safeAdoptAsLastChild(cloneWalk, newObject);

            newObject->mName = walk->toElement()->mName;
            newObject->cloneAttributes(walk->toElement());
            cloneWalk = newObject;
            continue;
          }

          if (walk->getNextSibling())
          {
            allowChildren = true;

            // cannot walk beyond the start of the clone
            if (walk == startingElement->toNode())
              break;

            walk = walk->getNextSibling();
            if (!walk->isElement())
            {
              // clone and continue walking
              cloneWalk = Parser::cloneAssignParent(cloneWalk->getParent(), walk);
              continue;
            }

            // this is an element, do some special handling
            ElementPtr newObject(XML::Element::create());
            Parser::safeAdoptAsLastChild(cloneWalk->getParent(), newObject);

            newObject->mName = walk->toElement()->mName;
            newObject->cloneAttributes(walk->toElement());
            cloneWalk = newObject;
            continue;
          }

          // cannot walk beyond the starting point when cloning
          if (walk == startingElement->toNode())
            break;

          allowChildren = false;     // do not allow a child node to be processed next
          walk = walk->getParent();
          cloneWalk = cloneWalk->getParent();
        }

        return newObject;
      }

      //-----------------------------------------------------------------------
      void Element::cloneAttributes(ElementPtr inOriginalElement)
      {
        NodePtr newObject = mThis.lock();

        NodePtr child = inOriginalElement->mFirstAttribute;
        while (child)
        {
          NodePtr newChild = Parser::cloneAssignParent(newObject, child);
          child = child->getNextSibling();
        }
      }

      //-----------------------------------------------------------------------
      void Element::actualParse(ElementPtr &ioCurrentElement, XML::ParserPos &ioPos)
      {
        ULONG totalPushes = 0;  // used to keep track how many stack positions are pushed on by this method doing its parse

        // push the pos manually for the current element
        (ioPos.getParser())->pushPos(ioPos);
        ++totalPushes;

        while (*ioPos)
        {
          // this must be a <
          ZS_THROW_BAD_STATE_IF('<' != *ioPos)

          ++ioPos;
          ioCurrentElement->mName = Parser::parseLegalName(ioPos);

          // could not have got in here if this was an illegal name
          ZS_THROW_BAD_STATE_IF(ioCurrentElement->mName.isEmpty())

          parseAttributes(ioCurrentElement, ioPos);

          bool foundEndSlash = parseTagSingleElementEndSlash(ioPos);

          // there must be an end '>' at this point
          if ('>' != *ioPos)
          {
            (ioPos.getParser())->addWarning(ParserWarningType_NoEndBracketFound);
          }
          else
          {
            // skip over the close >
            ++ioPos;
          }

          // check if this is a single element
          if (!foundEndSlash)
            foundEndSlash = parseIsDocumentSingleElement(ioCurrentElement, ioPos);

          // is this an element with no contents or is the stack too deep to continue parsing at this level?
          if (foundEndSlash)
          {
            parsePopElement(ioCurrentElement, ioPos, totalPushes);
            if (!ioCurrentElement)
            {
              parsePopPushes(ioPos, totalPushes);
              return;
            }
          }

          bool foundEndTag = false;
          bool foundStartTag = false;
          while (*ioPos)
          {
            if (Parser::parseAnyExceptElement(ioPos, ioCurrentElement))
              continue;

            // must be a new element, or an end of element
            ZS_THROW_BAD_STATE_IF('<' != *ioPos)

            foundStartTag = parseNewElement(ioCurrentElement, ioPos, totalPushes);
            if (foundStartTag)
              break;

            // this must be an end tag
            ZS_THROW_BAD_STATE_IF(!ioPos.isString("</"))

            foundEndTag = true;
            parseEndTag(ioCurrentElement, ioPos, totalPushes);
            if (!ioCurrentElement)
            {
              parsePopPushes(ioPos, totalPushes);
              return;
            }
          }

          if ((!foundEndTag) &&
              (!foundStartTag))
          {
            (ioPos.getParser())->addWarning(ParserWarningType_NoEndTagFound);
          }
        }

        parsePopPushes(ioPos, totalPushes);
      }

      //-----------------------------------------------------------------------
      void Element::parseAttributes(ElementPtr inCurrentElement, XML::ParserPos &ioPos)
      {
        while ((*ioPos) &&
               ('>' != *ioPos) &&
               ('/' != *ioPos))
        {
          if (Parser::skipWhiteSpace(ioPos))
            continue;

          if (!Parser::isLegalName(*ioPos, true))
          {
            // this isn't a legal name for an attribute, skip it
            (ioPos.getParser())->addWarning(ParserWarningType_IllegalAttributeName, ioPos);

            while ((*ioPos) &&
                   ('>' != *ioPos) &&
                   ('/' != *ioPos) &&
                   (!Parser::isWhiteSpace(ioPos)))
            {
              ++ioPos;
            }
            continue;
          }

          Parser::AutoStack stack(ioPos);

          AttributePtr attribute = XML::Attribute::create();
          if (!attribute->parse(ioPos))
            continue;

          // attribute was found, add it to the attribute list
          bool duplicateAttribute = inCurrentElement->setAttribute(attribute);
          if (duplicateAttribute)
          {
            (ioPos.getParser())->addWarning(ParserWarningType_DuplicateAttribute);
          }
        }
      }

      //-----------------------------------------------------------------------
      bool Element::parseTagSingleElementEndSlash(XML::ParserPos &ioPos)
      {
        if ('/' != *ioPos)
          return false;

        ++ioPos;
        Parser::skipWhiteSpace(ioPos);

        bool warnedBogusStuff = false;
        while ((*ioPos) &&
               ('>' != *ioPos))
        {
          if (!warnedBogusStuff)
          {
            (ioPos.getParser())->addWarning(ParserWarningType_ContentAfterCloseSlashInElement, ioPos);
            warnedBogusStuff = true;
          }
          ++ioPos;
        }

        return true;
      }

      //-----------------------------------------------------------------------
      bool Element::parseIsDocumentSingleElement(ElementPtr inCurrentElement, XML::ParserPos &ioPos)
      {
        bool caseSensative = (ioPos.getDocument())->isElementNameIsCaseSensative();

        const Parser::NoChildrenElementList &singleElements = (ioPos.getParser())->getContainsNoChildrenElements();
        for (Parser::NoChildrenElementList::const_iterator iter = singleElements.begin(); iter != singleElements.end(); ++iter)
        {
          if (caseSensative)
          {
            if ((*iter) == inCurrentElement->mName)
              return true;
          }
          else
          {
            if (0 == (*iter).compareNoCase(inCurrentElement->mName))
              return true;
          }
        }

        return false;
      }

      //-----------------------------------------------------------------------
      bool Element::parseNewElement(ElementPtr &ioCurrentElement, XML::ParserPos &ioPos, ULONG &ioTotalPushes)
      {
        if (!Parser::isLegalName(*(ioPos + 1), true))
          return false;

        // this is a new element
        (ioPos.getParser())->pushPos(ioPos);
        ++ioTotalPushes;

        ElementPtr element = XML::Element::create();
        ioCurrentElement->adoptAsLastChild(element);
        ioCurrentElement = element;
        return true;
      }

      //-----------------------------------------------------------------------
      void Element::parseEndTag(ElementPtr &ioCurrentElement, XML::ParserPos &ioPos, ULONG &ioTotalPushes)
      {
        XML::ParserPos temp(ioPos);

        temp += (strlen("</"));
        Parser::skipWhiteSpace(temp);

        // this could be the end tag for this element
        String elementName = Parser::parseLegalName(temp);
        if (elementName.isEmpty())
        {
          if ('>' == *temp)
          {
            // generic close tag, skip over and done parsing this element
            ioPos = temp;
            ++ioPos;

            parsePopElement(ioCurrentElement, ioPos, ioTotalPushes);
            return;
          }

          if (temp.isEOF())
          {
            // already warned about, no need to warn a second time
            //(temp.getDocument())->addWarning(ParserWarningType_NoEndBracketFound, ioPos);
            ioPos = temp;
            return;
          }

          ZS_THROW_BAD_STATE("This should have been skipped by the outer parser")
        }

        // if this end tag does not belong to the current element, the break out and give to outer element a chance to parse
        bool caseSensative = (ioPos.getDocument())->isElementNameIsCaseSensative();

        while (true)
        {
          // attempt to find the element it belongs to
          if (caseSensative)
          {
            if (ioCurrentElement->mName == elementName)
              break;
          }
          else
          {
            if (0 == ioCurrentElement->mName.compareNoCase(elementName))
              break;
          }

          parsePopElement(ioCurrentElement, ioPos, ioTotalPushes);
          if (!ioCurrentElement)
            return;
        }

        // the end tag belongs to the element
        Parser::skipWhiteSpace(temp);
        if ((*temp) &&
            ('>' != *temp))
        {
          // found bogus junk to skip
          (temp.getParser())->addWarning(ParserWarningType_ContentAfterCloseElementName, temp);

          while ((*temp) &&
                 ('>' != *temp))
          {
            ++temp;
          }
        }

        if ('>' == *temp)
        {
          // closed tag, skip over and done parsing this element
          ioPos = temp;
          ++ioPos;

          parsePopElement(ioCurrentElement, ioPos, ioTotalPushes);
          return;
        }

        ZS_THROW_BAD_STATE_IF(!temp.isEOF())

        (ioPos.getParser())->addWarning(ParserWarningType_NoEndBracketFound, ioPos);
        ioPos = temp;
      }

      //-----------------------------------------------------------------------
      void Element::parsePopElement(ElementPtr &ioCurrentElement, XML::ParserPos &ioPos, ULONG &ioTotalPushes)
      {
        // this element is popped off the stack
        ZS_THROW_BAD_STATE_IF(0 == ioTotalPushes)
        (ioPos.getParser())->popPos();
        --ioTotalPushes;

        // this element is done, need to return control to the previous element
        NodePtr parent = ioCurrentElement->getParent();
        if (parent->isDocument())
        {
          ioCurrentElement = ElementPtr();
          return;
        }
        ioCurrentElement = parent->toElement();
        ZS_THROW_BAD_STATE_IF(!ioCurrentElement)
      }

      //-----------------------------------------------------------------------
      void Element::parsePopPushes(XML::ParserPos &ioPos, ULONG &ioTotalPushes)
      {
        // this is done just in case the parser suddenly completes with some remaining stack still pushed at the end
        // since we aren't using the auto-stack to ensure the stack remains properly intact
        while (0 != ioTotalPushes)
        {
          (ioPos.getParser())->popPos();
          --ioTotalPushes;
        }
      }


    } // namespace internal

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark XML::Element
    #pragma mark

    //-------------------------------------------------------------------------
    Element::Element(const make_private &) :
      internal::Element()
    {
    }

    //-------------------------------------------------------------------------
    ElementPtr Element::create(const char *name)
    {
      ElementPtr newObject(make_shared<Element>(make_private{}));
      newObject->mThis = newObject;
      if (name) newObject->setValue(name);
      return newObject;
    }

    //-------------------------------------------------------------------------
    String Element::getText(
                            bool inCompressWhiteSpace,
                            bool inIncludeTextOfChildElements
                            )
    {
      // result will include all entities, to be converted later if required
      String result;

      if (!inIncludeTextOfChildElements)
      {
        NodePtr child = getFirstChild();
        while (child)
        {
          if (child->isText())
          {
            result += (child->toText()->getValue());
          }

          child = child->getNextSibling();
        }
      }
      else
      {
        class TextWalker : public WalkSink
        {
        public:
          TextWalker(String &outResult) : mResult(outResult) {}
          virtual bool onText(TextPtr inNode)
          {
            mResult += inNode->getValue();
            return false;
          }

        private:
          String &mResult;
        };

        TextWalker walker(result);
        walk(walker, NodeType::Text);
      }

      if (inCompressWhiteSpace)
      {
        // compress white space (additional &nbsp; do not get compressed)
        result = Parser::compressWhiteSpace(result);
      }

      return result;
    }

    //-------------------------------------------------------------------------
    String Element::getTextDecoded(
                                   bool inCompressWhiteSpace,
                                   bool inIncludeTextOfChildElements
                                   )
    {
      // result will include all entities, to be converted later if required
      String result;

      if (!inIncludeTextOfChildElements)
      {
        NodePtr child = getFirstChild();
        while (child)
        {
          if (child->isText())
          {
            result += (child->toText()->getValueDecoded());
          }

          child = child->getNextSibling();
        }
      }
      else
      {
        class TextWalker : public WalkSink
        {
        public:
          TextWalker(String &outResult) : mResult(outResult) {}
          virtual bool onText(TextPtr inNode)
          {
            mResult += inNode->getValueDecoded();
            return false;
          }

        private:
          String &mResult;
        };

        TextWalker walker(result);
        walk(walker, NodeType::Text);
      }

      if (inCompressWhiteSpace)
      {
        // compress white space (additional &nbsp; do not get compressed)
        result = Parser::compressWhiteSpace(result);
      }

      return result;
    }

    //-------------------------------------------------------------------------
    AttributePtr Element::findAttribute(String inName) const
    {
      DocumentPtr document(getDocument());

      bool caseSensative = true;
      if (document)
        caseSensative = document->isAttributeNameIsCaseSensative();

      NodePtr child = mFirstAttribute;
      while (child)
      {
        AttributePtr attribute(child->toAttribute());
        if (caseSensative)
        {
          if (inName == attribute->getName())
            return attribute;
        }
        else
        {
          if (0 == inName.compareNoCase(attribute->getName()))
            return attribute;
        }
        child = child->getNextSibling();
      }
      return AttributePtr();
    }

    //-------------------------------------------------------------------------
    String Element::getAttributeValue(String inName) const
    {
      AttributePtr attribute = findAttribute(inName);
      if (attribute)
        return attribute->getValue();

      return String();
    }

    //-------------------------------------------------------------------------
    AttributePtr Element::findAttributeChecked(String inName) const throw(Exceptions::CheckFailed)
    {
      AttributePtr result = findAttribute(inName);
      ZS_THROW_CUSTOM_IF(Exceptions::CheckFailed, !result)
      return result;
    }

    //-------------------------------------------------------------------------
    String Element::getAttributeValueChecked(String inName) const throw(Exceptions::CheckFailed)
    {
      AttributePtr result = findAttribute(inName);
      ZS_THROW_CUSTOM_IF(Exceptions::CheckFailed, !result)
      return result->getValue();
    }

    //-------------------------------------------------------------------------
    bool Element::setAttribute(String inName, String inValue, bool inQuoted)
    {
      AttributePtr attribute = Attribute::create();
      attribute->setName(inName);
      attribute->setValue(inValue);
      attribute->setQuoted(inQuoted);
      return setAttribute(attribute);
    }

    //-------------------------------------------------------------------------
    bool Element::setAttribute(AttributePtr inAttribute)
    {
      ZS_THROW_INVALID_USAGE_IF(!inAttribute)

      bool replaced = deleteAttribute(inAttribute->getName());
      mThis.lock()->adoptAsLastChild(inAttribute);
      return replaced;
    }

    //-------------------------------------------------------------------------
    bool Element::deleteAttribute(String inName)
    {
      AttributePtr attribute = findAttribute(inName);
      if (!attribute)
        return false;

      attribute->orphan();
      return true;
    }

    //-------------------------------------------------------------------------
    bool Element::hasAttributes() const
    {
      return (bool)mFirstAttribute;
    }

    //-------------------------------------------------------------------------
    AttributePtr Element::getFirstAttribute() const
    {
      return mFirstAttribute;
    }

    //-------------------------------------------------------------------------
    AttributePtr Element::getLastAttribute() const
    {
      return mLastAttribute;
    }

    //-------------------------------------------------------------------------
    AttributePtr Element::getFirstAttributeChecked() const throw(Exceptions::CheckFailed)
    {
      AttributePtr result = getFirstAttribute();
      ZS_THROW_CUSTOM_IF(Exceptions::CheckFailed, !result)
      return result;
    }

    //-------------------------------------------------------------------------
    AttributePtr Element::getLastAttributeChecked() const throw(Exceptions::CheckFailed)
    {
      AttributePtr result = getLastAttribute();
      ZS_THROW_CUSTOM_IF(Exceptions::CheckFailed, !result)
      return result;
    }

    //-------------------------------------------------------------------------
    void Element::adoptAsFirstChild(NodePtr inNode)
    {
      if (!inNode)
        return;

      if (!inNode->isAttribute())
      {
        Node::adoptAsFirstChild(inNode);
        return;
      }

      AttributePtr attribute = inNode->toAttribute();

      // first the node has to be orphaned
      inNode->orphan();

      // ensure this attribute does not already exist
      deleteAttribute(attribute->getName());

      // adopt this as the parent
      inNode->mParent = toNode();

      // point existing first child's previous sibling to new node
      if (mFirstAttribute)
        mFirstAttribute->mPreviousSibling = inNode;

      // point new node's next sibling to current first child
      inNode->mNextSibling = mFirstAttribute;

      // first child is now new node
      mFirstAttribute = attribute;

      // if there wasn't a last child then it is now pointing to the new node
      if (!mLastAttribute)
        mLastAttribute = attribute;
    }

    //-------------------------------------------------------------------------
    void Element::adoptAsLastChild(NodePtr inNode)
    {
      if (!inNode)
        return;

      if (!inNode->isAttribute())
      {
        Node::adoptAsLastChild(inNode);
        return;
      }

      AttributePtr attribute = inNode->toAttribute();

      // first the node has to be orphaned
      inNode->orphan();

      // ensure this attribute does not already exist
      deleteAttribute(attribute->getName());

      // adopt this as the parent
      inNode->mParent = toNode();

      // point existing last child's next sibling to new node
      if (mLastAttribute)
        mLastAttribute->mNextSibling = inNode;

      // point new node's next sibling to current first child
      inNode->mPreviousSibling = mLastAttribute;

      // first child is now new node
      mLastAttribute = attribute;

      // if there wasn't a last child then it is now pointing to the new node
      if (!mFirstAttribute)
        mFirstAttribute = attribute;
    }

    //-------------------------------------------------------------------------
    NodePtr Element::clone() const
    {
      return cloneAssignParent(NodePtr());
    }

    //-------------------------------------------------------------------------
    void Element::clear()
    {
      mName.clear();
      NodePtr child = mFirstAttribute;
      while (child)
      {
        NodePtr tempNext = child->getNextSibling();
        child->orphan();
        child->clear();
        child = tempNext;
      }
      Node::clear();
    }

    //-------------------------------------------------------------------------
    String Element::getValue() const
    {
      return mName;
    }

  } // namespace XML

} // namespace zsLib

#pragma warning(pop)
