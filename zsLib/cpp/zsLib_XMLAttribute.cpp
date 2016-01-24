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
      #pragma mark XML::internal::Attribute
      #pragma mark

      //-----------------------------------------------------------------------
      Attribute::Attribute() :
        mHasQuotes(true),
        mValuelessAttribute(false)
      {
      }

      //-----------------------------------------------------------------------
      bool Attribute::parse(XML::ParserPos &ioPos)
      {
        Parser::AutoStack stack(ioPos);

        mName = Parser::parseLegalName(ioPos);
        ZS_THROW_BAD_STATE_IF(mName.isEmpty())

        mValuelessAttribute = false;

        bool foundWhiteSpace = Parser::skipWhiteSpace(ioPos);
        if ('=' != *ioPos)
        {
          // valueless attribute?
          if (('/' == *ioPos) ||
              ('>' == *ioPos) ||
              (foundWhiteSpace))
          {
            // this is an attribute without a value, we will parse but it is somewhat illegal
            (ioPos.getParser())->addWarning(ParserWarningType_AttributeWithoutValue);

            // remember that it was one of these attributes
            mValuelessAttribute = true;
            mValue.clear();
            return true;
          }

          // didn't find whitespace, didn't find end of element, didn't find equals therefor illegal attribute name found
          (ioPos.getParser())->addWarning(ParserWarningType_IllegalAttributeName, ioPos);

          // skip over this illegal attribute name
          while ((*ioPos) &&
                 (!ioPos.isString("/>")) &&
                 (!ioPos.isString("?>")) &&
                 ('>' != *ioPos) &&
                 (!Parser::isWhiteSpace(ioPos)))
          {
            ++ioPos;
          }
          return false;
        }

        // attribute has a value
        mValue.clear();

        // skip over equals
        ++ioPos;

        // skip any whitespace between equals and value
        Parser::skipWhiteSpace(ioPos);

        XML::ParserPos startOfValue(ioPos);

        char quote = 0;
        if (('\'' == *ioPos) ||
            ('\"' == *ioPos))
          quote = *ioPos;

        if (0 != quote)
        {
          // skip over the quote
          ++ioPos;

          // found a quote, parse until end of quote hit
          while ((*ioPos) &&
                 (quote != *ioPos))
          {
            mValue += *ioPos;
            ++ioPos;
          }

          if (ioPos.isEOF())
          {
            (ioPos.getParser())->addWarning(ParserWarningType_AttributeValueMissingEndQuote, startOfValue);
            return false;
          }

          // skip the end quote
          ++ioPos;
          return true;
        }

        mHasQuotes = false;

        // continue until whitespace is hit or end of element
        while ((*ioPos) &&
               (!Parser::isWhiteSpace(ioPos)) &&
               (!ioPos.isString("/>")) &&
               (!ioPos.isString("?>")) &&
               ('>' != *ioPos))
        {
          mValue += *ioPos;
          ++ioPos;
        }

        if (mValue.isEmpty())
        {
          // no value was found, something went horribly wrong
          (ioPos.getParser())->addWarning(ParserWarningType_AttributeValueNotFound, startOfValue);
          return false;
        }

        return true;
      }

      //-----------------------------------------------------------------------
      static String normalizeAttributeValue(const String &input)
      {
        String output = XML::Parser::convertFromEntities(input);
        return XML::Parser::makeAttributeEntitySafe(output, '\"');
      }

      //-----------------------------------------------------------------------
      size_t Attribute::getOutputSizeXML(const GeneratorPtr &inGenerator) const
      {
        bool hasQuotes = mHasQuotes;

        size_t result = 0;
        ZS_THROW_INVALID_USAGE_IF(mName.isEmpty())
        result += mName.getLength();
        if (!mValuelessAttribute)
        {
          String value = mValue;
          if (0 != (XML::Generator::XMLWriteFlag_NormalizeCDATA & inGenerator->getXMLWriteFlags())) {
            value = normalizeAttributeValue(mValue);
            hasQuotes = true;
          }
          if (hasQuotes) {
            result += strlen("=\"\"");
          } else {
            result += strlen("=");
          }
          result += value.getLength();
        }
        return result;
      }

      //-----------------------------------------------------------------------
      void Attribute::writeBufferXML(const GeneratorPtr &inGenerator, char * &ioPos) const
      {
        bool hasQuotes = mHasQuotes;

        ZS_THROW_INVALID_USAGE_IF(mName.isEmpty())
        Generator::writeBuffer(ioPos, mName);
        if (!mValuelessAttribute)
        {
          Generator::writeBuffer(ioPos, "=");
          char quote[2];
          quote[0] = (String::npos == mValue.find('\"') ? '\"' : '\'');
          quote[1] = 0;
          String value = mValue;
          if (0 != (XML::Generator::XMLWriteFlag_NormalizeCDATA & inGenerator->getXMLWriteFlags())) {
            quote[0] = '\"';
            value = normalizeAttributeValue(mValue);
            hasQuotes = true;
          }
          if (hasQuotes) {
            Generator::writeBuffer(ioPos, &(quote[0]));
          }
          Generator::writeBuffer(ioPos, value);
          if (hasQuotes) {
            Generator::writeBuffer(ioPos, &(quote[0]));
          }
        }
      }

      //-----------------------------------------------------------------------
      size_t Attribute::actualWriteJSON(
                                        const GeneratorPtr &inGenerator,
                                        char * &ioPos
                                        ) const
      {
        const Generator &generator = (*inGenerator);

        size_t result = 0;

        bool hasQuotes = mHasQuotes;

        result += generator.fill(ioPos, generator.jsonStrs().mAttributeNameOpen);

        char prefix[2];
        prefix[0] = inGenerator->mJSONAttributePrefix;
        prefix[1] = 0;
        result += generator.copy(ioPos, &(prefix[0]));

        result += generator.copy(ioPos, XML::Parser::convertToJSONEncoding(mThis.lock()->getName()));

        if (hasQuotes) {
          result += generator.fill(ioPos, generator.jsonStrs().mAttributeNameCloseStr);
        } else {
          result += generator.fill(ioPos, generator.jsonStrs().mAttributeNameCloseNumber);
        }

        result += generator.copy(ioPos, XML::Parser::convertToJSONEncoding(mThis.lock()->getValueDecoded()));

        if (hasQuotes) {
          result += generator.fill(ioPos, generator.jsonStrs().mAttributeValueCloseStr);
        } else {
          result += generator.fill(ioPos, generator.jsonStrs().mAttributeValueCloseNumber);
        }
        return result;
      }

      //-----------------------------------------------------------------------
      size_t Attribute::getOutputSizeJSON(const GeneratorPtr &inGenerator) const
      {
        char *ioPos = NULL;
        return actualWriteJSON(inGenerator, ioPos);
      }

      //-----------------------------------------------------------------------
      void Attribute::writeBufferJSON(const GeneratorPtr &inGenerator, char * &ioPos) const
      {
        actualWriteJSON(inGenerator, ioPos);
      }

      //-----------------------------------------------------------------------
      NodePtr Attribute::cloneAssignParent(NodePtr inParent) const
      {
        AttributePtr newObject(XML::Attribute::create());

        newObject->mName = mName;
        newObject->mValue = mValue;
        newObject->mValuelessAttribute = mValuelessAttribute;

        Parser::safeAdoptAsLastChild(inParent, newObject);

        (mThis.lock())->cloneChildren(mThis.lock(), newObject);    // should do nothing since there are no children
        return newObject;
      }

    } // namespace internal

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    #pragma mark
    #pragma mark XML::Attribute
    #pragma mark

    //-------------------------------------------------------------------------
    Attribute::Attribute(const make_private &) :
      internal::Attribute()
    {
    }

    //-------------------------------------------------------------------------
    AttributePtr Attribute::create()
    {
      AttributePtr object(make_shared<Attribute>(make_private{}));
      object->mThis = object;
      return object;
    }

    //-------------------------------------------------------------------------
    String Attribute::getName() const
    {
      return mName;
    }

    //-------------------------------------------------------------------------
    void Attribute::setName(String inName)
    {
      // remember the attribute's parent
      NodePtr parent = getParent();

      // orphan this attribute temporarily
      orphan();

      mName = inName;

      // if there was a parent
      if (parent)
      {
        if (parent->isElement())
        {
          // remove any attribute with this name
          parent->toElement()->deleteAttribute(inName);

          // add this new attribute
          parent->toElement()->setAttribute(mThis.lock());
        }
        else if (parent->isDeclaration())
        {
          // remove any attribute with this name
          parent->toDeclaration()->deleteAttribute(inName);

          // add this new attribute
          parent->toDeclaration()->setAttribute(mThis.lock());
        }
        else
        {
          ZS_THROW_INVALID_USAGE("attributes are only allowed on elements and declarations")
        }
      }
    }

    //-------------------------------------------------------------------------
    void Attribute::setValue(String inValue)
    {
      mValue = inValue;
      mValuelessAttribute = false;

      if (String::npos != mValue.find('\''))
      {
        if (String::npos != mValue.find('\"'))
        {
          ZS_THROW_INVALID_USAGE("value cannot contain both set of quotes, one must be escaped using entities")
        }
      }
    }

    //-------------------------------------------------------------------------
    void Attribute::setQuoted(bool inQuoted)
    {
      mHasQuotes = inQuoted;
    }

    //-------------------------------------------------------------------------
    NodePtr Attribute::getFirstSibling() const
    {
      NodePtr parent = getParent();
      if (parent)
      {
        if (parent->isElement())
          return parent->toElement()->mFirstAttribute;

        if (parent->isDeclaration())
          return parent->toDeclaration()->mFirstAttribute;

        ZS_THROW_INVALID_USAGE("attributes only belong on elements and declarations")
      }
      return NodePtr();
    }

    //-------------------------------------------------------------------------
    NodePtr Attribute::getLastSibling() const
    {
      NodePtr parent = getParent();
      if (parent)
      {
        if (parent->isElement())
          return parent->toElement()->mLastAttribute;

        if (parent->isDeclaration())
          return parent->toDeclaration()->mLastAttribute;

        ZS_THROW_INVALID_USAGE("attributes only belong on elements and declarations")
      }
      return NodePtr();
    }

    //-------------------------------------------------------------------------
    void Attribute::orphan()
    {
      NodePtr parent = getParent();

      // if there is no parent, then already an orphan
      if (parent)
      {
        if (parent->isElement())
        {
          ElementPtr elementParent = parent->toElement();

          if (elementParent->mFirstAttribute.get() == this)
            elementParent->mFirstAttribute = (mNextSibling ? mNextSibling->toAttribute() : AttributePtr());

          if (elementParent->mLastAttribute.get() == this)
            elementParent->mLastAttribute = (mPreviousSibling ? mPreviousSibling->toAttribute() : AttributePtr());
        }
        else if (parent->isDeclaration())
        {
          DeclarationPtr declarationParent = parent->toDeclaration();

          if (declarationParent->mFirstAttribute.get() == this)
            declarationParent->mFirstAttribute = (mNextSibling ? mNextSibling->toAttribute() : AttributePtr());

          if (declarationParent->mLastAttribute.get() == this)
            declarationParent->mLastAttribute = (mPreviousSibling ? mPreviousSibling->toAttribute() : AttributePtr());
        }
        else
        {
          ZS_THROW_INVALID_USAGE("attributes only belong on elements and declarations")
        }
      }

      if (mNextSibling)
        mNextSibling->mPreviousSibling = mPreviousSibling;

      if (mPreviousSibling)
        mPreviousSibling->mNextSibling = mNextSibling;

      // leave this node as an orphan
      mParent = NodePtr();
      mNextSibling = NodePtr();
      mPreviousSibling = NodePtr();
    }

    //-------------------------------------------------------------------------
    void Attribute::adoptAsFirstChild(NodePtr inNode)
    {
      ZS_THROW_INVALID_USAGE("attributes cannot have children")
    }

    //-------------------------------------------------------------------------
    void Attribute::adoptAsLastChild(NodePtr inNode)
    {
      ZS_THROW_INVALID_USAGE("attributes cannot have children")
    }

    //-------------------------------------------------------------------------
    void Attribute::adoptAsPreviousSibling(NodePtr inNode)
    {
      if (!inNode)
        return;

      NodePtr parent = mParent.lock();

      ZS_THROW_INVALID_USAGE_IF(!parent)  // you cannot add as a sibling if there is no parent
      ZS_THROW_INVALID_USAGE_IF(!inNode->isAttribute())  // you cannot add another but attributes as sibling to attributes

      // orphan the node first
      inNode->orphan();

      // nodes both share the same parent
      inNode->mParent = mParent;

      inNode->mPreviousSibling = mPreviousSibling;
      inNode->mNextSibling = toNode();

      if (mPreviousSibling)
        mPreviousSibling->mNextSibling = inNode;

      mPreviousSibling = inNode;
      if (parent->isElement())
      {
        if (parent->toElement()->mFirstAttribute.get() == this)
          parent->toElement()->mFirstAttribute = inNode->toAttribute();
      }
      else if (parent->isDeclaration())
      {
        if (parent->toDeclaration()->mFirstAttribute.get() == this)
          parent->toDeclaration()->mFirstAttribute = inNode->toAttribute();
      }
      else
      {
        ZS_THROW_BAD_STATE("attributes can only exist on elements and declarations")
      }
    }

    //-------------------------------------------------------------------------
    void Attribute::adoptAsNextSibling(NodePtr inNode)
    {
      if (!inNode)
        return;

      NodePtr parent = mParent.lock();

      ZS_THROW_INVALID_USAGE_IF(!parent)  // you cannot add as a sibling if there is no parent
      ZS_THROW_INVALID_USAGE_IF(!inNode->isAttribute())  // you cannot add another but attributes as sibling to attributes

      // orphan the node first
      inNode->orphan();

      // nodes both share the same parent
      inNode->mParent = mParent;

      inNode->mPreviousSibling = toNode();
      inNode->mNextSibling = mNextSibling;

      if (mNextSibling)
        mNextSibling->mPreviousSibling = inNode;

      mNextSibling = inNode;
      if (parent->isElement())
      {
        if (parent->toElement()->mLastAttribute.get() == this)
          parent->toElement()->mLastAttribute = inNode->toAttribute();
      }
      else if (parent->isDeclaration())
      {
        if (parent->toDeclaration()->mLastAttribute.get() == this)
          parent->toDeclaration()->mLastAttribute = inNode->toAttribute();
      }
      else
      {
        ZS_THROW_BAD_STATE("attributes can only exist on elements and declarations")
      }
    }

    //-------------------------------------------------------------------------
    NodePtr Attribute::clone() const
    {
      return cloneAssignParent(NodePtr());
    }

    //-------------------------------------------------------------------------
    void Attribute::clear()
    {
      mName.clear();
      mValue.clear();
      mValuelessAttribute = false;
    }

    //-------------------------------------------------------------------------
    String Attribute::getValue() const
    {
      return mValue;
    }

    //-------------------------------------------------------------------------
    String Attribute::getValueDecoded() const
    {
      return Parser::convertFromEntities(mValue);
    }

  } // namespace XML

} // namespace zsLib
