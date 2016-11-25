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
#include <set>



#include "testing.h"
#include "main.h"

using zsLib::CSTR;
using zsLib::ULONG;

struct XMLWarningInfo
{
  zsLib::UINT mWarningType;
  CSTR mSearchStr;
  ULONG mRow;
  ULONG mColumn;
};

struct XMLResultInfo
{
  CSTR mInput;
  CSTR mOutputXML;
  CSTR mOutputJSON;
  ULONG  mTabSize;
  CSTR *mSingleLineElements;
  bool mCaseSensativeElements;
  bool mCaseSensativeAttributes;
  bool mReparseMayHaveWarnings;
  XMLWarningInfo **mWarnings;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML1Input()
{
  static const char *gXML1Input =
    "<?xml version=\"1.0\" encoding=\"ISO8859-1\" ?>\n"
    "<note>\n"
    "  <!--this is a test comment-->\n"
    "  <%--what is this 1%>\n"
    "  <!what is this 2>\n"
    "  <$what is this 3$>\n"
    "  <to test=\"hello\" test2=\"goodbye\">Tove</to>\n"
    "  <from>Jani</from>\n"
    "  <heading>Reminder</heading>\n"
    "  <body>Don't forget me this weekend!</body>\n"
    "  <cdatatest><![CDATA[your momma wears <army boots>]]></cdatatest>\n"
    "</note>\n";
  return gXML1Input;
}

static const char *get_gXML1Output()
{
  static const char *gXML1Output = get_gXML1Input();
  return gXML1Output;
}

static const char *get_gJSON1Output()
{
    static const char *gJSON1Output =
    "{"
    "\"#text\":\"\\n\\n\","
    "\"note\":{"
        "\"#text\":\"\\n  \\n  \\n  \\n  \\n  \\n  \\n  \\n  \\n  \\n\","
        "\"to\":{"
          "\"$test\":\"hello\","
          "\"$test2\":\"goodbye\","
          "\"#text\":\"Tove\""
        "},"
        "\"from\":\"Jani\","
        "\"heading\":\"Reminder\","
        "\"body\":\"Don't forget me this weekend!\","
        "\"cdatatest\":\"your momma wears <army boots>\""
      "}"
    "}"
  ;
  return gJSON1Output;
}

static XMLWarningInfo **get_gXMLWarnings1Array()
{
  static XMLWarningInfo *gXMLWarnings1Array[] = { NULL };
  return gXMLWarnings1Array;
}

static XMLResultInfo &get_gXMLResults1()
{
  static XMLResultInfo gXMLResults1 =
  {
    get_gXML1Input(),
    get_gXML1Output(),
    get_gJSON1Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings1Array()
  };
  return gXMLResults1;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML2Input()
{
  static const char *gXML2Input =
  "\n"
  "<test>contents</test>\n"
  "  \t\t <!--- this is a non-closed comment -- >\n"
  "<test2>contents2</test2>\n";
  return gXML2Input;
}

static const char *get_gXML2Output()
{
  static const char *gXML2Output =
  "\n"
  "<test>contents</test>\n"
  "  \t\t <!--- this is a non-closed comment -- >\n"
  "<test2>contents2</test2>\n"
  "-->";
  return gXML2Output;
}

static const char *get_gJSON2Output()
{
  static const char *gJSON2Output =
  "{\"#text\":\"\\n\\n  \\t\\t \",\"test\":\"contents\"}";
  return gJSON2Output;
}

static XMLWarningInfo *get_gXMLWarnings2_1()
{
  static XMLWarningInfo gXMLWarnings2_1[] =
  {
    {0,"<!---", 3, 18},
    {
      zsLib::XML::ParserWarningType_NoEndCommentFound,
      "Comment was not closed properly\n"
      "@ row=3 column=18: <!--- this is a non-closed com", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings2_1;
}

static XMLWarningInfo **get_gXMLWarnings2Array()
{
  static XMLWarningInfo *gXMLWarnings2Array[] =
  {
    get_gXMLWarnings2_1(),
    NULL
  };
  return gXMLWarnings2Array;
}

static XMLResultInfo &get_gXMLResults2()
{
  static XMLResultInfo gXMLResults2 =
  {
    get_gXML2Input(),
    get_gXML2Output(),
    get_gJSON2Output(),
    8,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings2Array()
  };
  return gXMLResults2;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML3Input()
{
  static const char *gXML3Input =
  "<?xml +bogus=\"1\" x=\"1\"?>\n"
  "</bogus>\n"
  "<test>contents</test>\n"
  "<outer><inner></inner></outer>\n"
  "<outer><inner1></bogus></></outer>\n"
  "<outer><inner2 /></outer>\n"
  "<outer><inner3></outer>\n"
  "<outer><inner4></></></>\n"
  "<outer><inner5></outer></inner5>\n"
  "\n";
  return gXML3Input;
}

static const char *get_gXML3Output()
{
  static const char *gXML3Output =
  "<?xml x=\"1\" ?>\n"
  "\n"
  "<test>contents</test>\n"
  "<outer><inner /></outer>\n"
  "<outer><inner1 /></outer>\n"
  "<outer><inner2 /></outer>\n"
  "<outer><inner3 /></outer>\n"
  "<outer><inner4 /></outer>\n"
  "<outer><inner5 /></outer>\n"
  "\n";
  return gXML3Output;
}

static const char *get_gJSON3Output()
{
  static const char *gJSON3Output =
  "{"
    "\"#text\":\"\\n\\n\\n\\n\\n\\n\\n\\n\\n\\n\","
    "\"test\":\"contents\","
    "\"outer\":"
    "["
              "{\"inner\":\"\"},"
              "{\"inner1\":\"\"},"
              "{\"inner2\":\"\"},"
              "{\"inner3\":\"\"},"
              "{\"inner4\":\"\"},"
              "{\"inner5\":\"\"}"
              "]"
  "}";
  return gJSON3Output;
}

static XMLWarningInfo *get_gXMLWarnings3_1()
{
  static XMLWarningInfo gXMLWarnings3_1[] =
  {
    {0,"+bogus", 1, 7},
    {0,"<?xml", 1, 1},
    {
      zsLib::XML::ParserWarningType_IllegalAttributeName,
      "Illegal attribute name found\n"
      "@ row=1 column=7: +bogus=\"1\" x=\"1\"?> </bogus> <t\n"
      "@ row=1 column=1: <?xml +bogus=\"1\" x=\"1\"?> </bog", 0, 0},
    {0,NULL, 0, 0}
  };

  return gXMLWarnings3_1;
}

static XMLWarningInfo *get_gXMLWarnings3_2()
{
  static XMLWarningInfo gXMLWarnings3_2[] =
  {
    {0,"</bogus>", 2, 1},
    {
      zsLib::XML::ParserWarningType_MismatchedEndTag,
      "Element end tag mismatched\n"
      "@ row=2 column=1: </bogus> <test>contents</test>", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings3_2;
}

static XMLWarningInfo *get_gXMLWarnings3_3()
{
  static XMLWarningInfo gXMLWarnings3_3[] =
  {
    {0,"</bogus></>", 5, 16},
    {0,"<inner1>", 5, 8},
    {0,"<outer><inner1>", 5, 1},
    {
      zsLib::XML::ParserWarningType_MismatchedEndTag,
      "Element end tag mismatched\n"
      "@ row=5 column=16: </bogus></></outer> <outer><in\n"
      "@ row=5 column=8: <inner1></bogus></></outer> <o\n"
      "@ row=5 column=1: <outer><inner1></bogus></></ou", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings3_3;
}

static XMLWarningInfo *get_gXMLWarnings3_4()
{
  static XMLWarningInfo gXMLWarnings3_4[] =
  {
    {0,"</>\n", 8, 22},
    {
      zsLib::XML::ParserWarningType_MismatchedEndTag,
      "Element end tag mismatched\n"
      "@ row=8 column=22: </> <outer><inner5></outer></i", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings3_4;
}

static XMLWarningInfo *get_gXMLWarnings3_5()
{
  static XMLWarningInfo gXMLWarnings3_5[] =
  {
    {0,"</inner5>", 9, 24},
    {
      zsLib::XML::ParserWarningType_MismatchedEndTag,
      "Element end tag mismatched\n"
      "@ row=9 column=24: </inner5>", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings3_5;
}

static XMLWarningInfo **get_gXMLWarnings3Array()
{
  static XMLWarningInfo *gXMLWarnings3Array[] =
  {
    get_gXMLWarnings3_1(),
    get_gXMLWarnings3_2(),
    get_gXMLWarnings3_3(),
    get_gXMLWarnings3_4(),
    get_gXMLWarnings3_5(),
    NULL
  };
  return gXMLWarnings3Array;
}

static XMLResultInfo &get_gXMLResults3()
{
  static XMLResultInfo gXMLResults3 =
  {
    get_gXML3Input(),
    get_gXML3Output(),
    get_gJSON3Output(),
    3,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings3Array()
  };
  return gXMLResults3;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML4Input()
{
  static const char *gXML4Input =
  "\n"
  "<test>contents</test>\n"
  "<?xml unclosed_declaration illegal{}name\n";
  return gXML4Input;
}

static const char *get_gXML4Output()
{
  static const char *gXML4Output =
  "\n"
  "<test>contents</test>\n"
  "<?xml unclosed_declaration ?>";
  return gXML4Output;
}

static const char *get_gJSON4Output()
{
  static const char *gJSON4Output =
  "{\"#text\":\"\\n\\n\",\"test\":\"contents\"}";
  return gJSON4Output;
}

static XMLWarningInfo *get_gXMLWarnings4_1()
{
  static XMLWarningInfo gXMLWarnings4_1[] =
  {
    {0,"unclosed_declaration", 3, 7},
    {0,"<?xml", 3, 1},
    {
      zsLib::XML::ParserWarningType_AttributeWithoutValue,
      "Attribute found that does not have a value\n"
      "@ row=3 column=7: unclosed_declaration illegal{}\n"
      "@ row=3 column=1: <?xml unclosed_declaration ill", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings4_1;
}

static XMLWarningInfo *get_gXMLWarnings4_2()
{
  static XMLWarningInfo gXMLWarnings4_2[] =
  {
    {0,"{}name", 3, 35},
    {0,"illegal{}", 3, 28},
    {0,"<?xml", 3, 1},
    {
      zsLib::XML::ParserWarningType_IllegalAttributeName,
      "Illegal attribute name found\n"
      "@ row=3 column=35: {}name\n"
      "@ row=3 column=28: illegal{}name\n"
      "@ row=3 column=1: <?xml unclosed_declaration ill", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings4_2;
}

static XMLWarningInfo *get_gXMLWarnings4_3()
{
  static XMLWarningInfo gXMLWarnings4_3[] =
  {
    {0,"<?xml", 3, 1},
    {
      zsLib::XML::ParserWarningType_NoEndDeclarationFound,
      "Declation \"<?xml ...\" was found but closing \"?>\" was not found\n"
      "@ row=3 column=1: <?xml unclosed_declaration ill", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings4_3;
}

static XMLWarningInfo **get_gXMLWarnings4Array()
{
  static XMLWarningInfo *gXMLWarnings4Array[] =
  {
    get_gXMLWarnings4_1(),
    get_gXMLWarnings4_2(),
    get_gXMLWarnings4_3(),
    NULL
  };
  return gXMLWarnings4Array;
}

static XMLResultInfo &get_gXMLResults4()
{
  static XMLResultInfo gXMLResults4 =
  {
    get_gXML4Input(),
    get_gXML4Output(),
    get_gJSON4Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings4Array()
  };
  return gXMLResults4;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML5Input()
{
  static const char *gXML5Input =
  "<?xml no_value_found = ?>\n"
  "<test illegal{}name=\"1\" dup=value1 dup=value2 ></test bogus>\n"
  "<?xml dup=value1 dup=\"value2\" no_end_quote_found=\'haha";
  return gXML5Input;
}

static const char *get_gXML5Output()
{
  static const char *gXML5Output =
  "<?xml ?>\n"
  "<test dup=\"value2\" />\n"
  "<?xml dup=\"value2\" ?>";
  return gXML5Output;
}

static const char *get_gJSON5Output()
{
  static const char *gJSON5Output =
  "{"
    "\"#text\":\"\\n\\n\","
    "\"test\":{\"$dup\":\"value2\"}"
  "}";
  return gJSON5Output;
}

static XMLWarningInfo *get_gXMLWarnings5_1()
{
  static XMLWarningInfo gXMLWarnings5_1[] =
  {
    {0,"?>", 1, 24},
    {0,"no_value_found", 1, 7},
    {0,"<?xml", 1, 1},
    {
      zsLib::XML::ParserWarningType_AttributeValueNotFound,
      "Attribute is missing value\n"
      "@ row=1 column=24: ?> <test illegal{}name=\"1\" dup\n"
      "@ row=1 column=7: no_value_found = ?> <test ille\n"
      "@ row=1 column=1: <?xml no_value_found = ?> <tes", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings5_1;
}

static XMLWarningInfo *get_gXMLWarnings5_2()
{
  static XMLWarningInfo gXMLWarnings5_2[] =
  {
    {0,"{}name", 2, 14},
    {0,"illegal{}name", 2, 7},
    {0,"<test", 2, 1},
    {
      zsLib::XML::ParserWarningType_IllegalAttributeName,
      "Illegal attribute name found\n"
      "@ row=2 column=14: {}name=\"1\" dup=value1 dup=valu\n"
      "@ row=2 column=7: illegal{}name=\"1\" dup=value1 d\n"
      "@ row=2 column=1: <test illegal{}name=\"1\" dup=va", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings5_2;
}

static XMLWarningInfo *get_gXMLWarnings5_3()
{
  static XMLWarningInfo gXMLWarnings5_3[] =
  {
    {0,"dup=value2", 2, 36},
    {0,"<test", 2, 1},
    {
      zsLib::XML::ParserWarningType_DuplicateAttribute,
      "An attribute on an element was duplicated\n"
      "@ row=2 column=36: dup=value2 ></test bogus> <?xm\n"
      "@ row=2 column=1: <test illegal{}name=\"1\" dup=va", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings5_3;
}

static XMLWarningInfo *get_gXMLWarnings5_4()
{
  static XMLWarningInfo gXMLWarnings5_4[] =
  {
    {0,"bogus>", 2, 55},
    {0,"<test", 2, 1},
    {
      zsLib::XML::ParserWarningType_ContentAfterCloseElementName,
      "Content found after \"</name \" in closing of element\n"
      "@ row=2 column=55: bogus> <?xml dup=value1 dup=\"v\n"
      "@ row=2 column=1: <test illegal{}name=\"1\" dup=va", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings5_4;
}

static XMLWarningInfo *get_gXMLWarnings5_5()
{
  static XMLWarningInfo gXMLWarnings5_5[] =
  {
    {0,"<?xml dup", 3, 1},
    {
      zsLib::XML::ParserWarningType_DuplicateAttribute,
      "An attribute on an element was duplicated\n"
      "@ row=3 column=1: <?xml dup=value1 dup=\"value2\"", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings5_5;
}

static XMLWarningInfo *get_gXMLWarnings5_6()
{
  static XMLWarningInfo gXMLWarnings5_6[] =
  {
    {0,"\'", 3, 50},
    {0,"no_end_quote_found", 3, 31},
    {0,"<?xml dup=value1", 3, 1},
    {
      zsLib::XML::ParserWarningType_AttributeValueMissingEndQuote,
      "Attribute value is missing the end quote\n"
      "@ row=3 column=50: \'haha\n"
      "@ row=3 column=31: no_end_quote_found=\'haha\n"
      "@ row=3 column=1: <?xml dup=value1 dup=\"value2\"", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings5_6;
}

static XMLWarningInfo *get_gXMLWarnings5_7()
{
  static XMLWarningInfo gXMLWarnings5_7[] =
  {
    {0,"<?xml dup=value1", 3, 1},
    {
      zsLib::XML::ParserWarningType_NoEndDeclarationFound,
      "Declation \"<?xml ...\" was found but closing \"?>\" was not found\n"
      "@ row=3 column=1: <?xml dup=value1 dup=\"value2\"", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings5_7;
}

static XMLWarningInfo **get_gXMLWarnings5Array()
{
  static XMLWarningInfo *gXMLWarnings5Array[] =
  {
    get_gXMLWarnings5_1(),
    get_gXMLWarnings5_2(),
    get_gXMLWarnings5_3(),
    get_gXMLWarnings5_4(),
    get_gXMLWarnings5_5(),
    get_gXMLWarnings5_6(),
    get_gXMLWarnings5_7(),
    NULL
  };
  return gXMLWarnings5Array;
}

static XMLResultInfo &get_gXMLResults5()
{
  static XMLResultInfo gXMLResults5 =
  {
    get_gXML5Input(),
    get_gXML5Output(),
    get_gJSON5Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings5Array()
  };
  return gXMLResults5;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML6Input()
{
  static const char *gXML6Input =
  "<test>contents</test>\n"
  "<whatever /bogus>\n"
  "<tag";
  return gXML6Input;
}

static const char *get_gXML6Output()
{
  static const char *gXML6Output =
  "<test>contents</test>\n"
  "<whatever />\n"
  "<tag />";
  return gXML6Output;
}

static const char *get_gJSON6Output()
{
  static const char *gJSON6Output =
  "{"
    "\"#text\":\"\\n\\n\","
    "\"test\":\"contents\","
    "\"whatever\":\"\","
    "\"tag\":\"\""
  "}";
  return gJSON6Output;
}

static XMLWarningInfo *get_gXMLWarnings6_1()
{
  static XMLWarningInfo gXMLWarnings6_1[] =
  {
    {0,"bogus>", 2, 12},
    {0,"<whatever", 2, 1},
    {
      zsLib::XML::ParserWarningType_ContentAfterCloseSlashInElement,
      "Content found after closing \'/\' in element\n"
      "@ row=2 column=12: bogus> <tag\n"
      "@ row=2 column=1: <whatever /bogus> <tag", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings6_1;
}

static XMLWarningInfo *get_gXMLWarnings6_2()
{
  static XMLWarningInfo gXMLWarnings6_2[] =
  {
    {0,"<tag", 3, 1},
    {
      zsLib::XML::ParserWarningType_NoEndBracketFound,
      "The closing \'>\' tag was not found\n"
      "@ row=3 column=1: <tag", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings6_2;
}

static XMLWarningInfo *get_gXMLWarnings6_3()
{
  static XMLWarningInfo gXMLWarnings6_3[] =
  {
    {0,"<tag", 3, 1},
    {
      zsLib::XML::ParserWarningType_NoEndTagFound,
      "Start element found but no \"</>\" end tag found\n"
      "@ row=3 column=1: <tag", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings6_3;
}

static XMLWarningInfo **get_gXMLWarnings6Array()
{
  static XMLWarningInfo *gXMLWarnings6Array[] =
  {
    get_gXMLWarnings6_1(),
    get_gXMLWarnings6_2(),
    get_gXMLWarnings6_3(),
    NULL
  };
  return gXMLWarnings6Array;
}

static XMLResultInfo &get_gXMLResults6()
{
  static XMLResultInfo gXMLResults6 =
  {
    get_gXML6Input(),
    get_gXML6Output(),
    get_gJSON6Output(),
    8,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings6Array()
  };
  return gXMLResults6;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML7Input()
{
  static const char *gXML7Input =
  "<test>contents</test>\n"
  "<tag>whatever</tag \n";
  return gXML7Input;
}

static const char *get_gXML7Output()
{
  static const char *gXML7Output =
  "<test>contents</test>\n"
  "<tag>whatever</tag>";
  return gXML7Output;
}

static const char *get_gJSON7Output()
{
  static const char *gJSON7Output =
  "{"
    "\"#text\":\"\\n\","
    "\"test\":\"contents\","
    "\"tag\":\"whatever\""
  "}";
  return gJSON7Output;
}

static XMLWarningInfo *get_gXMLWarnings7_1()
{
  static XMLWarningInfo gXMLWarnings7_1[] =
  {
    {0,"</tag", 2, 14},
    {0,"<tag>whatever", 2, 1},
    {
      zsLib::XML::ParserWarningType_NoEndBracketFound,
      "The closing \'>\' tag was not found\n"
      "@ row=2 column=14: </tag\n"
      "@ row=2 column=1: <tag>whatever</tag", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings7_1;
}

static XMLWarningInfo **get_gXMLWarnings7Array()
{
  static XMLWarningInfo *gXMLWarnings7Array[] =
  {
    get_gXMLWarnings7_1(),
    NULL
  };
  return gXMLWarnings7Array;
}

static XMLResultInfo &get_gXMLResults7()
{
  static XMLResultInfo gXMLResults7 =
  {
    get_gXML7Input(),
    get_gXML7Output(),
    get_gJSON7Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings7Array()
  };
  return gXMLResults7;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML8Input()
{
  static const char *gXML8Input =
  "<test>contents</test>\n"
  "<tag>whatever</ \n";
  return gXML8Input;
}

static const char *get_gXML8Output()
{
  static const char *gXML8Output =
  "<test>contents</test>\n"
  "<tag>whatever</tag>";
  return gXML8Output;
}

static const char *get_gJSON8Output()
{
  static const char *gJSON8Output =
  "{"
    "\"#text\":\"\\n\","
    "\"test\":\"contents\","
    "\"tag\":\"whatever\""
  "}";
  return gJSON8Output;
}

static XMLWarningInfo *get_gXMLWarnings8_1()
{
  static XMLWarningInfo gXMLWarnings8_1[] =
  {
    {0,"</ ", 2, 14},
    {0,"<tag>whatever", 2, 1},
    {
      zsLib::XML::ParserWarningType_NoEndBracketFound,
      "The closing \'>\' tag was not found\n"
      "@ row=2 column=14: </\n"
      "@ row=2 column=1: <tag>whatever</", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings8_1;
}

static XMLWarningInfo **get_gXMLWarnings8Array()
{
  static XMLWarningInfo *gXMLWarnings8Array[] =
  {
    get_gXMLWarnings8_1(),
    NULL
  };
  return gXMLWarnings8Array;
}

static XMLResultInfo &get_gXMLResults8()
{
  static XMLResultInfo gXMLResults8 =
  {
    get_gXML8Input(),
    get_gXML8Output(),
    get_gJSON8Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings8Array()
  };
  return gXMLResults8;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML9Input()
{
  static const char *gXML9Input =
  "<test>contents</test>\n"
  "<tag>whatever</ {\n";
  return gXML9Input;
}

static const char *get_gXML9Output()
{
  static const char *gXML9Output =
  "<test>contents</test>\n"
  "<tag>whatever</tag>";
  return gXML9Output;
}

static const char *get_gJSON9Output()
{
  static const char *gJSON9Output =
  "{"
    "\"#text\":\"\\n\","
    "\"test\":\"contents\","
    "\"tag\":\"whatever\""
  "}";
  return gJSON9Output;
}

static XMLWarningInfo *get_gXMLWarnings9_1()
{
  static XMLWarningInfo gXMLWarnings9_1[] =
  {
    {0,"</ {", 2, 14},
    {0,"<tag>whatever", 2, 1},
    {
      zsLib::XML::ParserWarningType_MismatchedEndTag,
      "Element end tag mismatched\n"
      "@ row=2 column=14: </ {\n"
      "@ row=2 column=1: <tag>whatever</ {", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings9_1;
}

static XMLWarningInfo *get_gXMLWarnings9_2()
{
  static XMLWarningInfo gXMLWarnings9_2[] =
  {
    {0,"<tag>whatever", 2, 1},
    {
      zsLib::XML::ParserWarningType_NoEndTagFound,
      "Start element found but no \"</>\" end tag found\n"
      "@ row=2 column=1: <tag>whatever</ {", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings9_2;
}

static XMLWarningInfo **get_gXMLWarnings9Array()
{
  static XMLWarningInfo *gXMLWarnings9Array[] =
  {
    get_gXMLWarnings9_1(),
    get_gXMLWarnings9_2(),
    NULL
  };
  return gXMLWarnings9Array;
}

static XMLResultInfo &get_gXMLResults9()
{
  static XMLResultInfo gXMLResults9 =
  {
    get_gXML9Input(),
    get_gXML9Output(),
    get_gJSON9Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings9Array()
  };
  return gXMLResults9;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML10Input()
{
  static const char *gXML10Input =
  "<test>contents</test>\n"
  "<!bogus";
  return gXML10Input;
}

static const char *get_gXML10Output()
{
  static const char *gXML10Output =
  "<test>contents</test>\n"
  "<!bogus>";
  return gXML10Output;
}

static const char *get_gJSON10Output()
{
  static const char *gJSON10Output =
  "{\"#text\":\"\\n\",\"test\":\"contents\"}";
  return gJSON10Output;
}

static XMLWarningInfo *get_gXMLWarnings10_1()
{
  static XMLWarningInfo gXMLWarnings10_1[] =
  {
    {0,"<!bogus", 2, 1},
    {
      zsLib::XML::ParserWarningType_NoEndUnknownTagFound,
      "Open \'<\' was found but no closing \'>\' was found\n"
      "@ row=2 column=1: <!bogus", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings10_1;
}

static XMLWarningInfo **get_gXMLWarnings10Array()
{
  static XMLWarningInfo *gXMLWarnings10Array[] =
  {
    get_gXMLWarnings10_1(),
    NULL
  };
  return gXMLWarnings10Array;
}

static XMLResultInfo &get_gXMLResults10()
{
  static XMLResultInfo gXMLResults10 =
  {
    get_gXML10Input(),
    get_gXML10Output(),
    get_gJSON10Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings10Array()
  };
  return gXMLResults10;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML11Input()
{
  static const char *gXML11Input =
  "<test>contents</test>\n"
  "<BR><Br><br></br>\n"
  "<HR /><hr>\n"
  "<input Dup=alpha DUP=beta dup=/hello.php>\n"
  "<input hi=/hello.php />";
  return gXML11Input;
}

static const char *get_gXML11Output()
{
  static const char *gXML11Output =
  "<test>contents</test>\n"
  "<BR /><Br /><br />\n"
  "<HR /><hr />\n"
  "<input dup=\"/hello.php\" />\n"
  "<input hi=\"/hello.php\" />";
  return gXML11Output;
}

static const char *get_gJSON11Output()
{
  static const char *gJSON11Output =
  "{"
    "\"#text\":\"\\n\\n\\n\\n\","
    "\"test\":\"contents\","
    "\"BR\":["
           "\"\","
           "\"\","
           "\"\""
           "],"
    "\"HR\":["
           "\"\","
           "\"\""
           "],"
    "\"input\":["
              "{\"$dup\":\"/hello.php\"},"
              "{\"$hi\":\"/hello.php\"}"
              "]"
  "}";
  return gJSON11Output;
}

static XMLWarningInfo *get_gXMLWarnings11_1()
{
  static XMLWarningInfo gXMLWarnings11_1[] =
  {
    {0,"</br>", 2, 13},
    {
      zsLib::XML::ParserWarningType_MismatchedEndTag,
      "Element end tag mismatched\n"
      "@ row=2 column=13: </br> <HR /><hr> <input Dup=al", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings11_1;
}

static XMLWarningInfo *get_gXMLWarnings11_2()
{
  static XMLWarningInfo gXMLWarnings11_2[] =
  {
    {0,"DUP=beta", 4, 18},
    {0,"<input Dup=alpha", 4, 1},
    {
      zsLib::XML::ParserWarningType_DuplicateAttribute,
      "An attribute on an element was duplicated\n"
      "@ row=4 column=18: DUP=beta dup=/hello.php> <inpu\n"
      "@ row=4 column=1: <input Dup=alpha DUP=beta dup=", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings11_2;
}

static XMLWarningInfo *get_gXMLWarnings11_3()
{
  static XMLWarningInfo gXMLWarnings11_3[] =
  {
    {0,"dup=/hello.php", 4, 27},
    {0,"<input Dup=alpha", 4, 1},
    {
      zsLib::XML::ParserWarningType_DuplicateAttribute,
      "An attribute on an element was duplicated\n"
      "@ row=4 column=27: dup=/hello.php> <input hi=/hel\n"
      "@ row=4 column=1: <input Dup=alpha DUP=beta dup=", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings11_3;
}

static XMLWarningInfo **get_gXMLWarnings11Array()
{
  static XMLWarningInfo *gXMLWarnings11Array[] =
  {
    get_gXMLWarnings11_1(),
    get_gXMLWarnings11_2(),
    get_gXMLWarnings11_3(),
    NULL
  };
  return gXMLWarnings11Array;
}

static CSTR *get_gSingleLineElements11()
{
  static CSTR gSingleLineElements11[] =
  {
    "hr",
    "br",
    "INPUT",
    NULL
  };
  return gSingleLineElements11;
}

static XMLResultInfo &get_gXMLResults11()
{
  static XMLResultInfo gXMLResults11 =
  {
    get_gXML11Input(),
    get_gXML11Output(),
    get_gJSON11Output(),
    0,
    get_gSingleLineElements11(),
    false,
    false,
    false,
    get_gXMLWarnings11Array()
  };
  return gXMLResults11;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML12Input()
{
  static const char *gXML12Input =
  "<img>test\n"
  "<Img>test</Img>"
  "<IMG +bogus value1=1 />";
  return gXML12Input;
}

static const char *get_gXML12Output()
{
  static const char *gXML12Output =
  "<img />test\n"
  "<Img>test</Img>"
  "<IMG value1=\"1\" />";
  return gXML12Output;
}

static const char *get_gJSON12Output()
{
  static const char *gJSON12Output =
  "{"
    "\"#text\":\"test\\n\","
    "\"img\":\"\","
    "\"Img\":\"test\","
    "\"IMG\":{\"$value1\":\"1\"}"
  "}";
  return gJSON12Output;
}

static XMLWarningInfo *get_gXMLWarnings12_1()
{
  static XMLWarningInfo gXMLWarnings12_1[] =
  {
    {0,"+bogus", 2, 21},
    {0,"<IMG", 2, 16},
    {
      zsLib::XML::ParserWarningType_IllegalAttributeName,
      "Illegal attribute name found\n"
      "@ row=2 column=21: +bogus value1=1 />\n"
      "@ row=2 column=16: <IMG +bogus value1=1 />", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings12_1;
}

static XMLWarningInfo **get_gXMLWarnings12Array()
{
  static XMLWarningInfo *gXMLWarnings12Array[] =
  {
    get_gXMLWarnings12_1(),
    NULL
  };
  return gXMLWarnings12Array;
}

static CSTR *get_gSingleLineElements12()
{
  static CSTR gSingleLineElements12[] =
  {
    "img",
    NULL
  };
  return gSingleLineElements12;
}

static XMLResultInfo &get_gXMLResults12()
{
  static XMLResultInfo gXMLResults12 =
  {
    get_gXML12Input(),
    get_gXML12Output(),
    get_gJSON12Output(),
    0,
    get_gSingleLineElements12(),
    true,
    true,
    false,
    get_gXMLWarnings12Array()
  };
  return gXMLResults12;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gXML13Input()
{
  static const char *gXML13Input =
  "<?xml test1=\"test\" />\n"
  "<?xml text2=\"test\" ?>\n";
  return gXML13Input;
}

static const char *get_gXML13Output()
{
  static const char *gXML13Output =
  "<?xml test1=\"test\" ?>\n"
  "<?xml text2=\"test\" ?>\n";
  return gXML13Output;
}

static const char *get_gJSON13Output()
{
  static const char *gJSON13Output =
  "{\"#text\":\"\\n\\n\"}";
  return gJSON13Output;
}

static XMLWarningInfo *get_gXMLWarnings13_1()
{
  static XMLWarningInfo gXMLWarnings13_1[] =
  {
    {0,"/>", 1, 20},
    {0,"<?", 1, 1},
    {
      zsLib::XML::ParserWarningType_NotProperEndDeclaration,
      "Declation \"<?xml ...\" was found but found \">\" instead of closing \"?>\"\n"
      "@ row=1 column=20: /> <?xml text2=\"test\" ?>\n"
      "@ row=1 column=1: <?xml test1=\"test\" /> <?xml te", 0, 0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings13_1;
}

static XMLWarningInfo **get_gXMLWarnings13Array()
{
  static XMLWarningInfo *gXMLWarnings13Array[] =
  {
    get_gXMLWarnings13_1(),
    NULL
  };
  return gXMLWarnings13Array;
}

static XMLResultInfo &get_gXMLResults13()
{
  static XMLResultInfo gXMLResults13 =
  {
    get_gXML13Input(),
    get_gXML13Output(),
    get_gJSON13Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings13Array()
  };
  return gXMLResults13;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gJSON14Input()
{
  static const char *gJSON14Input =
  "   { \n"
    "\t\"outer\" : {\n"
      "\t\t\"test\" \t:\t \"hello\\n\\tand\\ngoodbye\"\f\r\n"
    " }\n"
  "} \n";
  return gJSON14Input;
}

static const char *get_gXML14Output()
{
  static const char *gXML14Output =
  "<outer>"
  "<test>hello\n\tand\ngoodbye</test>"
  "</outer>"
  ;
  return gXML14Output;
}

static const char *get_gJSON14Output()
{
  static const char *gJSON14Output =
  "{"
    "\"outer\":"
      "{"
        "\"test\":\"hello\\n\\tand\\ngoodbye\""
      "}"
  "}";
  return gJSON14Output;
}

static XMLWarningInfo **get_gXMLWarnings14Array()
{
  static XMLWarningInfo *gXMLWarnings14Array[] = {NULL};
  return gXMLWarnings14Array;
}

static XMLResultInfo &get_gXMLResults14()
{
  static XMLResultInfo gXMLResults14 =
  {
    get_gJSON14Input(),
    get_gXML14Output(),
    get_gJSON14Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings14Array()
  };
  return gXMLResults14;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gJSON15Input()
{
  static const char *gJSON15Input =
  "   { \n"
  "\t\"outer\" : {\n"
  "\t\t\"test\" \t:\t \"hello\\n\\tand\\ngoodbye\"\f\r\n,\n"
  "\t\t\"foos\":{\n"
      "  \"foo\" : [ \"far\\u000afig\\u000Anewton\" , \n"
                    "\"mars \\u00E4 attacks\"\n"
                  " ] "
        " } \n"
  " }\n"
  "}\n.\n";
  return gJSON15Input;
}

static const char *get_gXML15Output()
{
  static const char *gXML15Output =
  "<outer>"
    "<test>hello\n\tand\ngoodbye</test>"
    "<foos>"
      "<foo>far\nfig\nnewton</foo>"
      "<foo>mars \xc3\xa4 attacks</foo>"
    "</foos>"
  "</outer>"
  ;
  return gXML15Output;
}

static const char *get_gJSON15Output()
{
  static const char *gJSON15Output =
  "{"
    "\"outer\":"
      "{"
        "\"test\":\"hello\\n\\tand\\ngoodbye\","
        "\"foos\":"
          "{"
            "\"foo\":["
              "\"far\\u000afig\\u000Anewton\","
              "\"mars \\u00E4 attacks\""
            "]"
          "}"
      "}"
  "}";
  return gJSON15Output;
}

static XMLWarningInfo *get_gXMLWarnings15_1()
{
  static XMLWarningInfo gXMLWarnings15_1[] =
  {
    {0,".", 11, 1},
    {0,"{", 1, 4},
    {
      zsLib::XML::ParserWarningType_DataFoundAfterFinalObjectClose,
      "Found data after final object close\n"
      "@ row=11 column=1: .\n"
      "@ row=1 column=4: {   \"outer\" : {   \"test\"  :  \""
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings15_1;
}

static XMLWarningInfo **get_gXMLWarnings15Array()
{
  static XMLWarningInfo *gXMLWarnings15Array[] =
  {
    get_gXMLWarnings15_1(),
    NULL
  };
  return gXMLWarnings15Array;
}

static XMLResultInfo &get_gXMLResults15()
{
  static XMLResultInfo gXMLResults15 =
  {
    get_gJSON15Input(),
    get_gXML15Output(),
    get_gJSON15Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings15Array()
  };
  return gXMLResults15;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gJSON16Input()
{
  static const char *gJSON16Input =
  "{"
    "\"outer\":{"
      "\"inner\":{\"foo\":\"bar\",do\"\"too\\z\"}"
    ""
  "";
  return gJSON16Input;
}

static const char *get_gXML16Output()
{
  static const char *gXML16Output =
  "<outer>"
    "<inner>"
      "<foo>bar</foo>"
      "<do>too\\z</do>"
    "</inner>"
  "</outer>"
  ;
  return gXML16Output;
}

static const char *get_gJSON16Output()
{
  static const char *gJSON16Output =
  "{\"outer\":{\"inner\":{\"foo\":\"bar\",\"do\":\"too\\z\"}}}"
  ;
  return gJSON16Output;
}


static XMLWarningInfo *get_gXMLWarnings16_1()
{
  static XMLWarningInfo gXMLWarnings16_1[] =
  {
    {0,"do", 1, 32},
    {0,"\"inner", 1, 11},
    {0,"\"outer", 1, 2},
    {0,"{\"outer", 1, 1},
    {
      zsLib::XML::ParserWarningType_MissingStringQuotes,
      "String quotes (\") were expected but not found\n"
      "@ row=1 column=32: do\"\"too\\z\"}\n"
      "@ row=1 column=11: \"inner\":{\"foo\":\"bar\",do\"\"too\\z\n"
      "@ row=1 column=2: \"outer\":{\"inner\":{\"foo\":\"bar\",\n"
      "@ row=1 column=1: {\"outer\":{\"inner\":{\"foo\":\"bar\""
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings16_1;
}

static XMLWarningInfo *get_gXMLWarnings16_2()
{
  static XMLWarningInfo gXMLWarnings16_2[] =
  {
    {0,"\"too", 1, 35},
    {0,"do", 1, 32},
    {0,"\"inner", 1, 11},
    {0,"\"outer", 1, 2},
    {0,"{\"outer", 1, 1},
    {
      zsLib::XML::ParserWarningType_MissingColonBetweenStringAndValue,
      "Missing \":\" between string : value\n"
      "@ row=1 column=35: \"too\\z\"}\n"
      "@ row=1 column=32: do\"\"too\\z\"}\n"
      "@ row=1 column=11: \"inner\":{\"foo\":\"bar\",do\"\"too\\z\n"
      "@ row=1 column=2: \"outer\":{\"inner\":{\"foo\":\"bar\",\n"
      "@ row=1 column=1: {\"outer\":{\"inner\":{\"foo\":\"bar\""
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings16_2;
}

static XMLWarningInfo *get_gXMLWarnings16_3()
{
  static XMLWarningInfo gXMLWarnings16_3[] =
  {
    {0,"\\z", 1, 39},
    {0,"\"too", 1, 35},
    {0,"do", 1, 32},
    {0,"\"inner", 1, 11},
    {0,"\"outer", 1, 2},
    {0,"{\"outer", 1, 1},
    {
      zsLib::XML::ParserWarningType_InvalidEscapeSequence,
      "Invalid escape sequence\n"
      "@ row=1 column=39: \\z\"}\n"
      "@ row=1 column=35: \"too\\z\"}\n"
      "@ row=1 column=32: do\"\"too\\z\"}\n"
      "@ row=1 column=11: \"inner\":{\"foo\":\"bar\",do\"\"too\\z\n"
      "@ row=1 column=2: \"outer\":{\"inner\":{\"foo\":\"bar\",\n"
      "@ row=1 column=1: {\"outer\":{\"inner\":{\"foo\":\"bar\""
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings16_3;
}

static XMLWarningInfo *get_gXMLWarnings16_4()
{
  static XMLWarningInfo gXMLWarnings16_4[] =
  {
    {0,"{\"inner", 1, 10},
    {0,"\"outer", 1, 2},
    {0,"{\"outer", 1, 1},
    {
      zsLib::XML::ParserWarningType_MissingObjectClose,
      "Missing object \"}\" close\n"
      "@ row=1 column=10: {\"inner\":{\"foo\":\"bar\",do\"\"too\\\n"
      "@ row=1 column=2: \"outer\":{\"inner\":{\"foo\":\"bar\",\n"
      "@ row=1 column=1: {\"outer\":{\"inner\":{\"foo\":\"bar\""
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings16_4;
}

static XMLWarningInfo *get_gXMLWarnings16_5()
{
  static XMLWarningInfo gXMLWarnings16_5[] =
  {
    {0,"", 1, 43},
    {0,"\"outer", 1, 2},
    {0,"{\"outer", 1, 1},
    {
      zsLib::XML::ParserWarningType_MustCloseRootObject,
      "Expected to close root object with \"}\"\n"
      "@ row=1 column=43: \n"
      "@ row=1 column=2: \"outer\":{\"inner\":{\"foo\":\"bar\",\n"
      "@ row=1 column=1: {\"outer\":{\"inner\":{\"foo\":\"bar\""
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings16_5;
}

static XMLWarningInfo **get_gXMLWarnings16Array()
{
  static XMLWarningInfo *gXMLWarnings16Array[] =
  {
    get_gXMLWarnings16_1(),
    get_gXMLWarnings16_2(),
    get_gXMLWarnings16_3(),
    get_gXMLWarnings16_4(),
    get_gXMLWarnings16_5(),
    NULL
  };
  return gXMLWarnings16Array;
}


static XMLResultInfo &get_gXMLResults16()
{
  static XMLResultInfo gXMLResults16 =
  {
    get_gJSON16Input(),
    get_gXML16Output(),
    get_gJSON16Output(),
    0,
    NULL,
    true,
    true,
    true,
    get_gXMLWarnings16Array()
  };
  return gXMLResults16;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gJSON17Input()
{
  static const char *gJSON17Input =
  "{\n"
    "\"foo\":\"test \\uc00p done\",\n"
    "\"a\":-1.0e+5,\n"
    "\"b\":-0.0e-5,\n"
    "\"c\":0.1e+5,\n"
    "\"d\":9998337000.1e+549329432,\n"
    "\"e\":-9998337000.1,\n"
    "\"f\":-9998337000,\n"
    "\"g\":9998337000,\n"
    "\"h\":9998337000.,\n"
    "\"i\":9998337000.0e+,\n"
    "\"j\":00.11,\n"
    "\"k\":9998337000.0e4,\n"
    "\"l\":-9.0e.0\n"
  "}";
  return gJSON17Input;
}

static const char *get_gXML17Output()
{
  static const char *gXML17Output =
  "<foo>test \\uc00p done</foo>"
  "<a>-1.0e+5</a>"
  "<b>-0.0e-5</b>"
  "<c>0.1e+5</c>"
  "<d>9998337000.1e+549329432</d>"
  "<e>-9998337000.1</e>"
  "<f>-9998337000</f>"
  "<g>9998337000</g>"
  "<h>9998337000.</h>"
  "<i>9998337000.0e+</i>"
  "<j>00.11</j>"
  "<k>9998337000.0e4</k>"
  "<l>-9.0e</l>"
  ;
  return gXML17Output;
}

static const char *get_gJSON17Output()
{
  static const char *gJSON17Output =
  "{"
  "\"foo\":\"test \\uc00p done\","
  "\"a\":-1.0e+5,"
  "\"b\":-0.0e-5,"
  "\"c\":0.1e+5,"
  "\"d\":9998337000.1e+549329432,"
  "\"e\":-9998337000.1,"
  "\"f\":-9998337000,"
  "\"g\":9998337000,"
  "\"h\":9998337000.,"
  "\"i\":9998337000.0e+,"
  "\"j\":00.11,"
  "\"k\":9998337000.0e4,"
  "\"l\":-9.0e"
  "}"
  ;
  return gJSON17Output;
}


static XMLWarningInfo *get_gXMLWarnings17_1()
{
  static XMLWarningInfo gXMLWarnings17_1[] =
  {
    {0,"\\uc00p", 2, 13},
    {0,"\"test", 2, 7},
    {0,"\"foo", 2, 1},
    {0,"{", 1, 1},
    {
      zsLib::XML::ParserWarningType_InvalidUnicodeEscapeSequence,
      "Invalid unicode escape sequence\n"
      "@ row=2 column=13: \\uc00p done\", \"a\":-1.0e+5, \"b\"\n"
      "@ row=2 column=7: \"test \\uc00p done\", \"a\":-1.0e+\n"
      "@ row=2 column=1: \"foo\":\"test \\uc00p done\", \"a\":\n"
      "@ row=1 column=1: { \"foo\":\"test \\uc00p done\", \"a"
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings17_1;
}

static XMLWarningInfo *get_gXMLWarnings17_2()
{
  static XMLWarningInfo gXMLWarnings17_2[] =
  {
    {0,",\n\"i\"", 10, 16},
    {0,"9998337000.,\n\"i\"", 10, 5},
    {0,"\"h\"", 10, 1},
    {0,"{\n\"foo", 1, 1},
    {
      zsLib::XML::ParserWarningType_IllegalNumberSequence,
      "Illegal number sequence\n"
      "@ row=10 column=16: , \"i\":9998337000.0e+, \"j\":00.1\n"
      "@ row=10 column=5: 9998337000., \"i\":9998337000.0e\n"
      "@ row=10 column=1: \"h\":9998337000., \"i\":999833700\n"
      "@ row=1 column=1: { \"foo\":\"test \\uc00p done\", \"a"
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings17_2;
}

static XMLWarningInfo *get_gXMLWarnings17_3()
{
  static XMLWarningInfo gXMLWarnings17_3[] =
  {
    {0,",\n\"j\"", 11, 19},
    {0,"9998337000.0e+,\n\"j\"", 11, 5},
    {0,"\"i\":9998337000.0e+", 11, 1},
    {0,"{\n\"foo", 1, 1},
    {
      zsLib::XML::ParserWarningType_IllegalNumberSequence,
      "Illegal number sequence\n"
      "@ row=11 column=19: , \"j\":00.11, \"k\":9998337000.0e\n"
      "@ row=11 column=5: 9998337000.0e+, \"j\":00.11, \"k\"\n"
      "@ row=11 column=1: \"i\":9998337000.0e+, \"j\":00.11,\n"
      "@ row=1 column=1: { \"foo\":\"test \\uc00p done\", \"a"
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings17_3;
}

static XMLWarningInfo *get_gXMLWarnings17_4()
{
  static XMLWarningInfo gXMLWarnings17_4[] =
  {
    {0,"0.11,\n\"k\"", 12, 6},
    {0,"00.11,\n\"k\"", 12, 5},
    {0,"\"j\":00.11", 12, 1},
    {0,"{\n\"foo", 1, 1},
    {
      zsLib::XML::ParserWarningType_IllegalNumberSequence,
      "Illegal number sequence\n"
      "@ row=12 column=6: 0.11, \"k\":9998337000.0e4, \"l\":\n"
      "@ row=12 column=5: 00.11, \"k\":9998337000.0e4, \"l\"\n"
      "@ row=12 column=1: \"j\":00.11, \"k\":9998337000.0e4,\n"
      "@ row=1 column=1: { \"foo\":\"test \\uc00p done\", \"a"
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings17_4;
}

static XMLWarningInfo *get_gXMLWarnings17_5()
{
  static XMLWarningInfo gXMLWarnings17_5[] =
  {
    {0,".0\n}", 14, 10},
    {0,"-9.0e.0", 14, 5},
    {0,"\"l\"", 14, 1},
    {0,"{\n\"foo", 1, 1},
    {
      zsLib::XML::ParserWarningType_IllegalNumberSequence,
      "Illegal number sequence\n"
      "@ row=14 column=10: .0 }\n"
      "@ row=14 column=5: -9.0e.0 }\n"
      "@ row=14 column=1: \"l\":-9.0e.0 }\n"
      "@ row=1 column=1: { \"foo\":\"test \\uc00p done\", \"a"
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings17_5;
}

static XMLWarningInfo *get_gXMLWarnings17_6()
{
  static XMLWarningInfo gXMLWarnings17_6[] =
  {
    {0,".0\n}", 14, 10},
    {0,"{\n\"foo", 1, 1},
    {
      zsLib::XML::ParserWarningType_MissingStringQuotes,
      "String quotes (\") were expected but not found\n"
      "@ row=14 column=10: .0 }\n"
      "@ row=1 column=1: { \"foo\":\"test \\uc00p done\", \"a"
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings17_6;
}

static XMLWarningInfo *get_gXMLWarnings17_7()
{
  static XMLWarningInfo gXMLWarnings17_7[] =
  {
    {0,"", 15, 2},
    {0,".0\n}", 14, 10},
    {0,"{\n\"foo", 1, 1},
    {
      zsLib::XML::ParserWarningType_MissingStringQuotes,
      "String quotes (\") were expected but not found\n"
      "@ row=15 column=2: \n"
      "@ row=14 column=10: .0 }\n"
      "@ row=1 column=1: { \"foo\":\"test \\uc00p done\", \"a"
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings17_7;
}

static XMLWarningInfo *get_gXMLWarnings17_8()
{
  static XMLWarningInfo gXMLWarnings17_8[] =
  {
    {0,"", 15, 2},
    {0,"{\n\"foo", 1, 1},
    {
      zsLib::XML::ParserWarningType_MustCloseRootObject,
      "Expected to close root object with \"}\"\n"
      "@ row=15 column=2: \n"
      "@ row=1 column=1: { \"foo\":\"test \\uc00p done\", \"a"
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings17_8;
}

static XMLWarningInfo **get_gXMLWarnings17Array()
{
  static XMLWarningInfo *gXMLWarnings17Array[] =
  {
    get_gXMLWarnings17_1(),
    get_gXMLWarnings17_2(),
    get_gXMLWarnings17_3(),
    get_gXMLWarnings17_4(),
    get_gXMLWarnings17_5(),
    get_gXMLWarnings17_6(),
    get_gXMLWarnings17_7(),
    get_gXMLWarnings17_8(),
    NULL
  };
  return gXMLWarnings17Array;
}

static XMLResultInfo &get_gXMLResults17()
{
  static XMLResultInfo gXMLResults17 =
  {
    get_gJSON17Input(),
    get_gXML17Output(),
    get_gJSON17Output(),
    0,
    NULL,
    true,
    true,
    true,
    get_gXMLWarnings17Array()
  };
  return gXMLResults17;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gJSON18Input()
{
  static const char *gJSON18Input =
  "{\n"
    "\"foo\":\"bar\",\n"
    "\"#text\":\"hello\",\n"
    "\"inner\":{\"$\":\"attribute_no_name\"},\n"
    "\"$root\":\"attribute_root\",\n"
    "\"\":\"missing\",\n"
    "\"illegal\":.\n"
  "}";
  return gJSON18Input;
}

static const char *get_gXML18Output()
{
  static const char *gXML18Output =
  "<foo>bar</foo>hello<inner />"
  ;
  return gXML18Output;
}

static const char *get_gJSON18Output()
{
  static const char *gJSON18Output =
  "{"
    "\"#text\":\"hello\","
    "\"foo\":\"bar\","
    "\"inner\":\"\""
  "}"
  ;
  return gJSON18Output;
}

static XMLWarningInfo *get_gXMLWarnings18_1()
{
  static XMLWarningInfo gXMLWarnings18_1[] =
  {
    {0,"\"$\"", 4, 10},
    {0,"\"inner\"", 4, 1},
    {0,"{", 1, 1},
    {
      zsLib::XML::ParserWarningType_AttributePrefixWithoutName,
      "Attribute prefix found but no name for attribute found\n"
      "@ row=4 column=10: \"$\":\"attribute_no_name\"}, \"$ro\n"
      "@ row=4 column=1: \"inner\":{\"$\":\"attribute_no_nam\n"
      "@ row=1 column=1: { \"foo\":\"bar\", \"#text\":\"hello\""
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings18_1;
}

static XMLWarningInfo *get_gXMLWarnings18_2()
{
  static XMLWarningInfo gXMLWarnings18_2[] =
  {
    {0,"\"$root\"", 5, 1},
    {0,"{", 1, 1},
    {
      zsLib::XML::ParserWarningType_AttributePrefixAtRoot,
      "Attribute prefix found at document root\n"
      "@ row=5 column=1: \"$root\":\"attribute_root\", \"\":\"\n"
      "@ row=1 column=1: { \"foo\":\"bar\", \"#text\":\"hello\""
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings18_2;
}

static XMLWarningInfo *get_gXMLWarnings18_3()
{
  static XMLWarningInfo gXMLWarnings18_3[] =
  {
    {0,"\"\":\"missing\"", 6, 1},
    {0,"{", 1, 1},
    {
      zsLib::XML::ParserWarningType_MissingPairString,
      "Empty \"string\" found in pair string : value\n"
      "@ row=6 column=1: \"\":\"missing\", \"illegal\":. }\n"
      "@ row=1 column=1: { \"foo\":\"bar\", \"#text\":\"hello\""
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings18_3;
}

static XMLWarningInfo *get_gXMLWarnings18_4()
{
  static XMLWarningInfo gXMLWarnings18_4[] =
  {
    {0,".\n}", 7, 11},
    {0,"\"illegal\"", 7, 1},
    {0,"{", 1, 1},
    {
      zsLib::XML::ParserWarningType_IllegalValue,
      "Illegal value found in pair string : value\n"
      "@ row=7 column=11: . }\n"
      "@ row=7 column=1: \"illegal\":. }\n"
      "@ row=1 column=1: { \"foo\":\"bar\", \"#text\":\"hello\""
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings18_4;
}


static XMLWarningInfo **get_gXMLWarnings18Array()
{
  static XMLWarningInfo *gXMLWarnings18Array[] =
  {
    get_gXMLWarnings18_1(),
    get_gXMLWarnings18_2(),
    get_gXMLWarnings18_3(),
    get_gXMLWarnings18_4(),
    NULL
  };
  return gXMLWarnings18Array;
}

static XMLResultInfo &get_gXMLResults18()
{
  static XMLResultInfo gXMLResults18 =
  {
    get_gJSON18Input(),
    get_gXML18Output(),
    get_gJSON18Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings18Array()
  };
  return gXMLResults18;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gJSON19Input()
{
  static const char *gJSON19Input =
  "{\n"
    "\"foo\":[1,2,,3]\n"
  "}";
  return gJSON19Input;
}

static const char *get_gXML19Output()
{
  static const char *gXML19Output =
  "<foo>1</foo><foo>2</foo><foo /><foo>3</foo>"
  ;
  return gXML19Output;
}

static const char *get_gJSON19Output()
{
  static const char *gJSON19Output =
  "{\"foo\":[1,2,\"\",3]}"
  ;
  return gJSON19Output;
}

static XMLWarningInfo *get_gXMLWarnings19_1()
{
  static XMLWarningInfo gXMLWarnings19_1[] =
  {
    {0,",3", 2, 12},
    {0,"\"foo\"", 2, 1},
    {0,"{", 1, 1},
    {
      zsLib::XML::ParserWarningType_UnexpectedComma,
      "Parser did not expect a \",\" comma\n"
      "@ row=2 column=12: ,3] }\n"
      "@ row=2 column=1: \"foo\":[1,2,,3] }\n"
      "@ row=1 column=1: { \"foo\":[1,2,,3] }"
      ,0,0},
    {0,NULL, 0, 0}
  };
  return gXMLWarnings19_1;
}


static XMLWarningInfo **get_gXMLWarnings19Array()
{
  static XMLWarningInfo *gXMLWarnings19Array[] =
  {
    get_gXMLWarnings19_1(),
    NULL
  };
  return gXMLWarnings19Array;
}

static XMLResultInfo &get_gXMLResults19()
{
  static XMLResultInfo gXMLResults19 =
  {
    get_gJSON19Input(),
    get_gXML19Output(),
    get_gJSON19Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings19Array()
  };
  return gXMLResults19;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static const char *get_gJSON20Input()
{
  static const char *gJSON20Input =
  "[\n"
  "\"hello\",\n"
  "{ \"goodbye\": \"foo\" }\n"
  "]";
  return gJSON20Input;
}

static const char *get_gXML20Output()
{
  static const char *gXML20Output =
  "<unknown>hello</unknown><unknown><goodbye>foo</goodbye></unknown>";
  return gXML20Output;
}

static const char *get_gJSON20Output()
{
  static const char *gJSON20Output =
  "[\"hello\",{\"goodbye\":\"foo\"}]"
  ;
  return gJSON20Output;
}

static XMLWarningInfo **get_gXMLWarnings20Array()
{
  static XMLWarningInfo *gXMLWarnings20Array[] =
  {NULL};
  return gXMLWarnings20Array;
}

static XMLResultInfo &get_gXMLResults20()
{
  static XMLResultInfo gXMLResults20 =
  {
    get_gJSON20Input(),
    get_gXML20Output(),
    get_gJSON20Output(),
    0,
    NULL,
    true,
    true,
    false,
    get_gXMLWarnings20Array()
  };
  return gXMLResults20;
}


class TestXML
{
public:
  typedef std::set<zsLib::XML::Node *> FoundNodesSet;

  TestXML()
  {
    parse(get_gXMLResults1());
    parse(get_gXMLResults2());
    parse(get_gXMLResults3());
    parse(get_gXMLResults4());
    parse(get_gXMLResults5());
    parse(get_gXMLResults6());
    parse(get_gXMLResults7());
    parse(get_gXMLResults8());
    parse(get_gXMLResults9());
    parse(get_gXMLResults10());
    parse(get_gXMLResults11());
    parse(get_gXMLResults12());
    parse(get_gXMLResults13());
    parse(get_gXMLResults14());
    parse(get_gXMLResults15());
    parse(get_gXMLResults16());
    parse(get_gXMLResults17());
    parse(get_gXMLResults18());
    parse(get_gXMLResults19());
    parse(get_gXMLResults20());
    generate();
    parserPosTest();
    {int i = 0; ++i;}
  }

  void parse(XMLResultInfo &results)
  {
    zsLib::XML::ParserPtr parser = ::zsLib::XML::Parser::createAutoDetectParser("#text",'$');

    ULONG tabSize = results.mTabSize;
    if (0 != tabSize)
      parser->setTabSize(results.mTabSize);

    zsLib::XML::Parser::NoChildrenElementList list;

    if (NULL != results.mSingleLineElements)
    {
      for (ULONG loop = 0; NULL != results.mSingleLineElements[loop]; ++loop)
      {
        list.push_back(results.mSingleLineElements[loop]);
      }
    }

    parser->setNoChildrenElements(list);

    zsLib::XML::DocumentPtr document = parser->parse(
                                                     results.mInput,
                                                     results.mCaseSensativeElements,
                                                     results.mCaseSensativeAttributes
                                                     );

    zsLib::XML::DocumentPtr cloneDocument = (document->clone())->toDocument();

    zsLib::XML::GeneratorPtr generatorXML = zsLib::XML::Generator::createXMLGenerator();
    zsLib::XML::GeneratorPtr generatorJSON = zsLib::XML::Generator::createJSONGenerator("#text",'$');
    zsLib::XML::GeneratorPtr reGeneratorJSON = zsLib::XML::Generator::createJSONGenerator("#text",'$');
    zsLib::XML::GeneratorPtr generatorJSONPretty = zsLib::XML::Generator::createJSONGenerator(zsLib::XML::Generator::JSONWriteFlag_PrettyPrint, "#text", '$');

    size_t lengthXML = 0;
    std::unique_ptr<char[]> outputXML = generatorXML->write(cloneDocument, &lengthXML);

    size_t lengthJSON = 0;
    std::unique_ptr<char[]> outputJSON = generatorJSON->write(cloneDocument, &lengthJSON);

    size_t lengthJSONPretty = 0;
    std::unique_ptr<char[]> outputJSONPretty = generatorJSONPretty->write(cloneDocument, &lengthJSONPretty);

    TESTING_CHECK(lengthXML == strlen(outputXML.get()))
    TESTING_CHECK(lengthJSON == strlen(outputJSON.get()))
    TESTING_CHECK(lengthJSONPretty == strlen(outputJSONPretty.get()))

    TESTING_CHECK(0 == strcmp(results.mOutputXML, outputXML.get()))
    TESTING_CHECK(0 == strcmp(results.mOutputJSON, outputJSON.get()))

    TESTING_STDOUT() << "--- SOR:(input) ---\n" << results.mInput << "\n--- EOR ---\n";
    TESTING_STDOUT() << "--- SOR:XML(final) ---\n" << outputXML.get() << "\n" << lengthXML << " (calculated) -vs- " << strlen(outputXML.get()) << " (actual)\n--- EOR:XML ---\n";
    if (results.mOutputJSON) {
      TESTING_STDOUT() << "--- SOR:JSON(compare) ---\n" << results.mOutputJSON << "\n--- EOR:JSON(compare) ---\n";
    }
    TESTING_STDOUT() << "--- SOR:JSON(result) ---\n" << outputJSON.get() << "\n" << lengthJSON << " (calculated) -vs- " << strlen(outputJSON.get()) << " (actual)\n--- EOR:JSON(result) ---\n";
    TESTING_STDOUT() << "--- SOR:JSON(pretty) ---\n" << outputJSONPretty.get() << "\n" << lengthJSONPretty << " (calculated) -vs- " << strlen(outputJSONPretty.get()) << " (actual)\n--- EOR:JSON(pretty) ---\n";

    // parse document from the JSON and then regenerate output again - results must match
    zsLib::XML::ParserPtr reParser = ::zsLib::XML::Parser::createAutoDetectParser("#text",'$');
    reParser->setNoChildrenElements(list);
    zsLib::XML::DocumentPtr reDocument = reParser->parse(
                                                         outputJSON.get(),
                                                         results.mCaseSensativeElements,
                                                         results.mCaseSensativeAttributes
                                                         );

    size_t reLengthJSON = 0;
    std::unique_ptr<char[]> reOutputJSON = reGeneratorJSON->write(reDocument, &reLengthJSON);
    TESTING_CHECK(lengthJSON == reLengthJSON)
    TESTING_CHECK(reLengthJSON == strlen(reOutputJSON.get()))
    TESTING_CHECK(0 == strcmp(results.mOutputJSON, reOutputJSON.get()))

    if (!results.mReparseMayHaveWarnings) {
      const zsLib::XML::Parser::Warnings &shouldBeEmptyWarnings = reParser->getWarnings();
      TESTING_CHECK(0 == shouldBeEmptyWarnings.size())
      if (0 != shouldBeEmptyWarnings.size()) {
        for (zsLib::XML::Parser::Warnings::const_iterator iter = shouldBeEmptyWarnings.begin(); iter != shouldBeEmptyWarnings.end(); ++iter)
        {
          const zsLib::XML::ParserWarning &actualWarning = (*iter);
          zsLib::String warningText = actualWarning.getAsString();
          TESTING_STDOUT() << "--- SOR:WARNING(unexpected) -- \n" << warningText << "\n--- EOR:WARNING(unexpected) ---\n";
        }
      }
    }

    TESTING_STDOUT() << "--- SOR:re-JSON(result) ---\n" << reOutputJSON.get() << "\n" << reLengthJSON << " (calculated) -vs- " << strlen(reOutputJSON.get()) << " (actual)\n--- EOR:re-JSON(result) ---\n";


    // original document, cloned and re-document must both validate
    treeWalkChecker(document);
    treeWalkChecker(cloneDocument);
    treeWalkChecker(reDocument);

    ULONG loop = 0;
    const zsLib::XML::Parser::Warnings &actualWarnings = parser->getWarnings();

    if (NULL == results.mWarnings) {
      TESTING_CHECK(actualWarnings.size() < 1)
    }

    for (zsLib::XML::Parser::Warnings::const_iterator iter = actualWarnings.begin(); iter != actualWarnings.end(); ++iter, ++loop)
    {
      XMLWarningInfo *resultWarning = (results.mWarnings ? (results.mWarnings[loop]) : NULL);
      if (NULL == resultWarning)
      {
        // found more warnings than results
        TESTING_CHECK(NULL == "more actual warnings than expected")

        for (; iter != actualWarnings.end(); ++iter)
        {
          const zsLib::XML::ParserWarning &actualWarning = (*iter);
          zsLib::String warningText = actualWarning.getAsString();
          TESTING_STDOUT() << "--- SOR:WARNING(found but not expected) -- \n" << warningText << "\n--- EOR:WARNING(found but not expected) ---\n";
        }
        break;
      }

      const zsLib::XML::ParserWarning &actualWarning = (*iter);
      zsLib::String warningText = actualWarning.getAsString();

      TESTING_STDOUT() << "--- SOR:WARNING(actual) -- \n" << warningText << "\n--- EOR:WARNING(actual) ---\n";

      ULONG loop2 = 0;
      for (zsLib::XML::ParserWarning::ParserStack::const_reverse_iterator iter2 = actualWarning.mStack.rbegin(); iter2 != actualWarning.mStack.rend(); ++iter2, ++loop2)
      {
        const zsLib::XML::ParserPos &pos = (*iter2).mPos;
        if (NULL == resultWarning[loop2+1].mSearchStr)
        {
          TESTING_CHECK(NULL == "found too many warning positions")
          break;
        }

        XMLWarningInfo &resultWarningDetail = resultWarning[loop2];

        const char *found = NULL;

        if (*resultWarningDetail.mSearchStr) {
          found = strstr(results.mInput, resultWarningDetail.mSearchStr);
        } else {
          found = results.mInput + strlen(results.mInput);
        }

        TESTING_CHECK(pos.mPos == found)
        TESTING_CHECK(pos.mRow == resultWarningDetail.mRow)
        TESTING_CHECK(pos.mColumn == resultWarningDetail.mColumn)
      }

      TESTING_CHECK(actualWarning.mWarningType == resultWarning[loop2].mWarningType)

      TESTING_CHECK(0 == strcmp(warningText, resultWarning[loop2].mSearchStr))
      TESTING_STDOUT() << "--- SOR:WARNING(compare) -- \n" << resultWarning[loop2].mSearchStr << "\n--- EOR:WARNING(compare) ---\n";
    }

    // make sure there aren't more warnings expected than found
    TESTING_CHECK(NULL == (results.mWarnings ? results.mWarnings[loop] : NULL))
  }

  void treeWalkChecker(zsLib::XML::NodePtr inNode)
  {
    FoundNodesSet foundNodes;
    treeWalkChecker(inNode, foundNodes);
  }

  void treeWalkChecker(zsLib::XML::NodePtr inNode, FoundNodesSet &foundNodes)
  {
    // make sure this node hasn't been seen before
    FoundNodesSet::iterator iter = foundNodes.find(inNode.get());
    TESTING_CHECK(iter == foundNodes.end())
    foundNodes.insert(inNode.get());

    zsLib::XML::AttributePtr firstAttribute;
    zsLib::XML::AttributePtr lastAttribute;

    // check specialized aspects of each node
    switch (inNode->getNodeType())
    {
      case zsLib::XML::Node::NodeType::Document:
      {
        // this cannot have a parent
        TESTING_CHECK(!(inNode->getParent()))
        TESTING_CHECK(inNode->getFirstSibling()->isDocument())
        TESTING_CHECK(inNode->getLastSibling()->isDocument())
        break;
      }
      case zsLib::XML::Node::NodeType::Element:
      {
        // must have a parent
        TESTING_CHECK(inNode->getParent())

        // parent must be another element or document
        TESTING_CHECK((inNode->getParent()->isElement()) ||
                  (inNode->getParent()->isDocument()))

        firstAttribute = inNode->toElement()->getFirstAttribute();
        lastAttribute = inNode->toElement()->getLastAttribute();
        break;
      }
      case zsLib::XML::Node::NodeType::Attribute:
      {
        // must have a parent
        TESTING_CHECK(inNode->getParent())

        // parents has to be an element or a declaration
        TESTING_CHECK((inNode->getParent()->isElement()) ||
                  (inNode->getParent()->isDeclaration()))

        // cannot have children
        TESTING_CHECK(!(inNode->getFirstChild()))
        TESTING_CHECK(!(inNode->getLastChild()))

        // check if previous or next are also attributes - they must be
        if (inNode->getPreviousSibling()) {
          TESTING_CHECK(inNode->getPreviousSibling()->isAttribute())
        }
        if (inNode->getNextSibling()) {
          TESTING_CHECK(inNode->getNextSibling()->isAttribute())
        }

        if (inNode->getParent()->isElement())
        {
          TESTING_CHECK(inNode->getFirstSibling() == inNode->getParent()->toElement()->getFirstAttribute())
          TESTING_CHECK(inNode->getLastSibling() == inNode->getParent()->toElement()->getLastAttribute())
        }
        else
        {
          TESTING_CHECK(inNode->getParent()->isDeclaration())
          TESTING_CHECK(inNode->getFirstSibling() == inNode->getParent()->toDeclaration()->getFirstAttribute())
          TESTING_CHECK(inNode->getLastSibling() == inNode->getParent()->toDeclaration()->getLastAttribute())
        }
        break;
      }
      case zsLib::XML::Node::NodeType::Text:
      {
        // must have a parent
        TESTING_CHECK(inNode->getParent())

        // parent must be another element or document
        TESTING_CHECK((inNode->getParent()->isElement()) ||
                  (inNode->getParent()->isDocument()))

        // cannot have children
        TESTING_CHECK(!(inNode->getFirstChild()))
        TESTING_CHECK(!(inNode->getLastChild()))
        break;
      }
      case zsLib::XML::Node::NodeType::Comment:
      {
        // must have a parent
        TESTING_CHECK(inNode->getParent())

        // parent must be another element or document
        TESTING_CHECK((inNode->getParent()->isElement()) ||
                  (inNode->getParent()->isDocument()))

        // cannot have children
        TESTING_CHECK(!(inNode->getFirstChild()))
        TESTING_CHECK(!(inNode->getLastChild()))
        break;
      }
      case zsLib::XML::Node::NodeType::Declaration:
      {
        // must have a parent
        TESTING_CHECK(inNode->getParent())

        // parent must be another element or document
        TESTING_CHECK((inNode->getParent()->isElement()) ||
                  (inNode->getParent()->isDocument()))

        // cannot have children
        TESTING_CHECK(!(inNode->getFirstChild()))
        TESTING_CHECK(!(inNode->getLastChild()))

        firstAttribute = inNode->toDeclaration()->getFirstAttribute();
        lastAttribute = inNode->toDeclaration()->getLastAttribute();
        break;
      }
      case zsLib::XML::Node::NodeType::Unknown:
      {
        // must have a parent
        TESTING_CHECK(inNode->getParent())

        // parent must be another element or document
        TESTING_CHECK((inNode->getParent()->isElement()) ||
                  (inNode->getParent()->isDocument()))

        // cannot have children
        TESTING_CHECK(!(inNode->getFirstChild()))
        TESTING_CHECK(!(inNode->getLastChild()))
        break;
      }
      default:
      {
        // what is this?
        TESTING_CHECK(false)
      }
    }

    // this node's previous's next must be this node
    if (inNode->getPreviousSibling())
    {
      TESTING_CHECK(inNode->getPreviousSibling()->getNextSibling().get() == inNode.get())
      if (!inNode->isAttribute())
      {
        TESTING_CHECK(inNode->getFirstSibling() == inNode->getParent()->getFirstChild())
      }
    }

    // this node's next's previous must be this node
    if (inNode->getNextSibling())
    {
      TESTING_CHECK(inNode->getNextSibling()->getPreviousSibling().get() == inNode.get())
      if (!inNode->isAttribute())
      {
        TESTING_CHECK(inNode->getLastSibling() == inNode->getParent()->getLastChild())
      }
    }

    checkChildren(inNode, firstAttribute, lastAttribute, foundNodes);
    checkChildren(inNode, inNode->getFirstChild(), inNode->getLastChild(), foundNodes);
  }

  void checkChildren(zsLib::XML::NodePtr inNode, zsLib::XML::NodePtr inFirstChild, zsLib::XML::NodePtr inLastChild, FoundNodesSet &foundNodes)
  {
    // if this does not have a first child then it cannot have a last child
    if (!inFirstChild) {
      TESTING_CHECK(!inLastChild)
    }

      // if this does not have a last child then it cannot have a first child
    if (!inLastChild) {
      TESTING_CHECK(!inFirstChild)
    }

    bool foundFirstChild = false;
    bool foundLastChild = false;
    zsLib::XML::NodePtr child = inFirstChild;
    while (child)
    {
      bool isFirstChild = false;
      bool isLastChild = false;

      // must have a last child, since a first child was found
      TESTING_CHECK(inLastChild)

      // if this is the first child, make sure it does not have a previous
      if (child.get() == inFirstChild.get())
      {
        isFirstChild = foundFirstChild = true;
        TESTING_CHECK(!(child->getPreviousSibling()))
      }
      if (child.get() == inLastChild.get())
      {
        isLastChild = foundLastChild = true;
        TESTING_CHECK(!(child->getNextSibling()))
      }

      // if this child doesn't have a previous or next then must point to the first or last child respectively
      if (!(child->getPreviousSibling())) {
        TESTING_CHECK(inFirstChild.get() == child.get())
      }

      if (!(child->getNextSibling())) {
        TESTING_CHECK(inLastChild.get() == child.get())
      }

        // if the first node equals last node then the child node must not have a previous or next
        if (inFirstChild.get() == inLastChild.get())
        {
          // cannot have a previous or next sibling
          TESTING_CHECK(!(child->getPreviousSibling()))
          TESTING_CHECK(!(child->getNextSibling()))
        }
        else
        {
          // first child must have a next and last child must have a previous
          TESTING_CHECK(inFirstChild->getNextSibling())
          TESTING_CHECK(inLastChild->getPreviousSibling())
        }

      // if its not the first child, must have a previous sibling
      if (!isFirstChild) {
        TESTING_CHECK(child->getPreviousSibling())
      }
      // if its not the last child, must have a next sibling
      if (!isLastChild) {
        TESTING_CHECK(child->getNextSibling())
      }

      // parent must be this node
      TESTING_CHECK(child->getParent().get() == inNode.get())
      treeWalkChecker(child, foundNodes);

      child = child->getNextSibling();
    }
    if (foundFirstChild) {
      TESTING_CHECK(foundLastChild)
    }
    if (foundLastChild) {
      TESTING_CHECK(foundFirstChild)
    }
  }

  void generate()
  {
    zsLib::XML::AttributePtr attribute7(::zsLib::XML::Attribute::create());
    bool caught = false;

    static CSTR gOutput =
    "<?xml value12=\"12\" value2=\"2\" value1=\"1\" value3=\"foo4\" value11=\"11\" value10=\"10\" ?>\n"
    "insert previous which is first sibling\n"
    "<test value13=\"13\" value5=\"5\" value9=\"9\" value6=\"6\" value8=\"8\"><!--comment1-->text</test>\n"
    "insert previous sibling\n"
    "insert next sibling\n"
    "\n"
    "insert next which is last sibling\n"
    "<text> this  &lt;is&gt; \n"
    "a &#96;test&#96; of &nbsp;the <wild> text </wild>\telement\n"
    "\n"
    "</text>\n"
    "<encoding value1=\"&quot;&apos;\" value2=\'\"\' value3=\"\'\"><![CDATA[<cdata test & result>]]></encoding><!unknown!>";

    zsLib::XML::DocumentPtr document(::zsLib::XML::Document::create());

    treeWalkChecker(document);

    zsLib::XML::ElementPtr element1(::zsLib::XML::Element::create());
    element1->setValue(zsLib::String("test"));
    document->adoptAsFirstChild(element1);

    treeWalkChecker(document);

    zsLib::XML::TextPtr textEOL1(::zsLib::XML::Text::create());
    textEOL1->setValue(zsLib::String("\n"));
    element1->adoptAsNextSibling(textEOL1);

    treeWalkChecker(document);

    zsLib::XML::TextPtr text1(::zsLib::XML::Text::create());
    text1->setValue(zsLib::String("text"));
    element1->adoptAsFirstChild(text1);

    treeWalkChecker(document);

    zsLib::XML::TextPtr text2(::zsLib::XML::Text::create());
    text2->setValue(zsLib::String("\n"));
    document->adoptAsLastChild(text2);

    treeWalkChecker(document);

    zsLib::XML::TextPtr text3(::zsLib::XML::Text::create());
    text3->setValue(zsLib::String("insert previous sibling\n"));
    text2->adoptAsPreviousSibling(text3);

    treeWalkChecker(document);

    zsLib::XML::TextPtr text4(::zsLib::XML::Text::create());
    text4->setValue(zsLib::String("insert next sibling\n"));
    text3->adoptAsNextSibling(text4);

    treeWalkChecker(document);

    zsLib::XML::TextPtr text5(::zsLib::XML::Text::create());
    text5->setValue(zsLib::String("insert next which is last sibling\n"));
    text2->adoptAsNextSibling(text5);

    treeWalkChecker(document);

    zsLib::XML::TextPtr text6(::zsLib::XML::Text::create());
    text6->setValue(zsLib::String("insert previous which is first sibling\n"));
    element1->adoptAsPreviousSibling(text6);

    treeWalkChecker(document);

    zsLib::XML::DeclarationPtr declaration1(::zsLib::XML::Declaration::create());
    declaration1->setAttribute(zsLib::String("value1"), zsLib::String("1"));
    document->adoptAsFirstChild(declaration1);

    TESTING_CHECK(0 == strcmp(declaration1->getAttributeValue(zsLib::String("value1")), "1"))
    TESTING_CHECK(0 == strcmp(declaration1->getAttributeValue(zsLib::String("whatever")), ""))

    // normally wouldn't do this after adding to the document, but this is a test after all
    TESTING_CHECK(document->isAttributeNameIsCaseSensative())
    TESTING_CHECK(0 == strcmp(declaration1->getAttributeValue(zsLib::String("VALUE1")), ""))
    document->setAttributeNameIsCaseSensative(false);
    TESTING_CHECK(!document->isAttributeNameIsCaseSensative())
    TESTING_CHECK(0 == strcmp(declaration1->getAttributeValue(zsLib::String("VALUE1")), "1"))
    document->setAttributeNameIsCaseSensative(true);

    zsLib::XML::TextPtr bogusText(::zsLib::XML::Text::create());
    bogusText->setValue(zsLib::String("bogus"));

    caught = false;
    try {(declaration1->toNode())->adoptAsFirstChild(bogusText);} catch(...) {caught = true;}
    TESTING_CHECK(caught)

    caught = false;
    try {(declaration1->toNode())->adoptAsLastChild(bogusText);} catch(...) {caught = true;}
    TESTING_CHECK(caught)

    treeWalkChecker(document);

    zsLib::XML::TextPtr textEOL2(::zsLib::XML::Text::create());
    textEOL2->setValue(zsLib::String("\n"));
    declaration1->adoptAsNextSibling(textEOL2);

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute2(::zsLib::XML::Attribute::create());
    attribute2->setName(zsLib::String("value2"));
    attribute2->setValue(zsLib::String("2"));
    declaration1->adoptAsFirstChild(attribute2);

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute3(::zsLib::XML::Attribute::create());
    attribute3->setName(zsLib::String("value4"));   // whoops, wrong name
    attribute3->setValue(zsLib::String("3"));
    declaration1->adoptAsFirstChild(attribute3);
    attribute3->setName(zsLib::String("value3"));

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute4(::zsLib::XML::Attribute::create());
    attribute4->setName(zsLib::String("foo4"));   // whoops, wrong name
    attribute4->setValue(zsLib::String("foo4"));
    declaration1->adoptAsFirstChild(attribute4);
    attribute4->setName(zsLib::String("value3"));

    TESTING_CHECK(attribute2 == attribute4->getFirstSibling())
    TESTING_CHECK(attribute4 == attribute2->getLastSibling())

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute5(::zsLib::XML::Attribute::create());
    attribute5->setName(zsLib::String("foo5"));   // whoops, wrong name
    attribute5->setValue(zsLib::String("5"));
    element1->adoptAsLastChild(attribute5);
    attribute5->setName(zsLib::String("value5"));

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute6(::zsLib::XML::Attribute::create());
    attribute6->setName(zsLib::String("value6"));
    attribute6->setValue(zsLib::String("6"));
    TESTING_CHECK(zsLib::XML::NodePtr() == attribute6->getFirstSibling()) // haven't added yet so shouldn't have any
    TESTING_CHECK(zsLib::XML::NodePtr() == attribute6->getLastSibling()) // haven't added yet so shouldn't have any
    element1->adoptAsLastChild(attribute6);

    TESTING_CHECK(attribute5 == attribute6->getFirstSibling())
    TESTING_CHECK(attribute6 == attribute5->getLastSibling())

    treeWalkChecker(document);

    caught = false;
    try {(attribute6->toNode())->adoptAsFirstChild(attribute7);} catch(...) {caught = true;}
    TESTING_CHECK(caught)
    caught = false;
    try {(attribute6->toNode())->adoptAsLastChild(attribute7);} catch(...) {caught = true;}
    TESTING_CHECK(caught)

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute8(::zsLib::XML::Attribute::create());
    attribute8->setName(zsLib::String("value8"));
    attribute8->setValue(zsLib::String("8"));
    attribute6->adoptAsNextSibling(attribute8);

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute9(::zsLib::XML::Attribute::create());
    attribute9->setName(zsLib::String("value9"));
    attribute9->setValue(zsLib::String("9"));
    attribute6->adoptAsPreviousSibling(attribute9);
    TESTING_CHECK(0 == strcmp(attribute9->getValue(), "9"))

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute10(::zsLib::XML::Attribute::create());
    attribute10->setName(zsLib::String("value10"));
    attribute10->setValue(zsLib::String("10"));
    attribute4->adoptAsNextSibling(attribute10);

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute11(::zsLib::XML::Attribute::create());
    attribute11->setName(zsLib::String("value11"));
    attribute11->setValue(zsLib::String("11"));
    attribute4->adoptAsNextSibling(attribute11);

    treeWalkChecker(document);

    zsLib::XML::TextPtr text7(::zsLib::XML::Text::create());
    text7->setValue(zsLib::String("text7"));

    caught = false;
    try {attribute4->adoptAsNextSibling(text7);} catch(...) {caught = true;}
    TESTING_CHECK(caught)

    treeWalkChecker(document);

    caught = false;
    try {attribute4->adoptAsPreviousSibling(text7);} catch(...) {caught = true;}
    TESTING_CHECK(caught)

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute12(::zsLib::XML::Attribute::create());
    attribute12->setName(zsLib::String("value12"));
    attribute12->setValue(zsLib::String("12"));
    declaration1->getFirstAttribute()->adoptAsPreviousSibling(attribute12);

    treeWalkChecker(document);

    zsLib::XML::AttributePtr attribute13(::zsLib::XML::Attribute::create());
    attribute13->setName(zsLib::String("value13"));
    attribute13->setValue(zsLib::String("13"));
    element1->getFirstAttribute()->adoptAsPreviousSibling(attribute13);

    treeWalkChecker(document);

    zsLib::XML::CommentPtr comment1(::zsLib::XML::Comment::create());
    comment1->setValue(zsLib::String("comment1"));
    element1->adoptAsFirstChild(comment1);
    TESTING_CHECK(0 == strcmp(comment1->getValue(), "comment1"))

    treeWalkChecker(document);

    zsLib::XML::ElementPtr element2(::zsLib::XML::Element::create());
    element2->setValue(zsLib::String("text"));
    document->adoptAsLastChild(element2);

    treeWalkChecker(document);

    zsLib::XML::TextPtr text8(::zsLib::XML::Text::create());
    text8->setValue(zsLib::String(" this  &lt;is&gt; \n" "a &#96;test&#96; of &nbsp;the "));
    element2->adoptAsLastChild(text8);

    treeWalkChecker(document);

    zsLib::XML::ElementPtr element3(::zsLib::XML::Element::create());
    element3->setValue(zsLib::String("wild"));
    element2->adoptAsLastChild(element3);

    treeWalkChecker(document);

    zsLib::XML::TextPtr text9(::zsLib::XML::Text::create());
    text9->setValue(zsLib::String(" text "));
    element3->adoptAsLastChild(text9);

    zsLib::XML::TextPtr text10(::zsLib::XML::Text::create());
    text10->setValue(zsLib::String("\telement\n\n"));
    element3->adoptAsNextSibling(text10);

    treeWalkChecker(document);

    //"<text> this  &lt;is&gt; \n"
    //"a &#96;test&#96; of &nbsp;the <wild> text </wild>\telement\n"
    //"\n"
    //"</text>";

    zsLib::String result1 = element2->getText(false, true);
    TESTING_CHECK(0 == strcmp(result1, " this  &lt;is&gt; \n" "a &#96;test&#96; of &nbsp;the " " text " "\telement\n" "\n"))

    zsLib::String result2 = element2->getText(false, false);
    TESTING_CHECK(0 == strcmp(result2, " this  &lt;is&gt; \n" "a &#96;test&#96; of &nbsp;the " "\telement\n" "\n"))

    zsLib::String result3 = element2->getText(true, true);
    TESTING_CHECK(0 == strcmp(result3, "this &lt;is&gt; " "a &#96;test&#96; of &nbsp;the " "text " "element"))

    zsLib::String result4 = element2->getText(true, false);
    TESTING_CHECK(0 == strcmp(result4, "this &lt;is&gt; " "a &#96;test&#96; of &nbsp;the " "element"))

    result1 = zsLib::XML::Parser::convertFromEntities(result1);
    TESTING_EQUAL(result1, " this  <is> \n" "a `test` of  the " " text " "\telement\n" "\n")

    result1 = zsLib::XML::Parser::convertFromEntities(zsLib::String("&#x23;&#x4c;O&#x4c;&nbsp;&bogus;&#ab;&#xyz;&+;&#195;"));
    result2 = "#LOL &bogus;&#ab;&#xyz;&+;\xC3\x83";
    TESTING_EQUAL(result1, result2)

    result1 = zsLib::XML::Parser::makeTextEntitySafe(zsLib::String("&<>;  \"\'"));
    result2 = "&amp;&lt;&gt;;  \"\'";
    TESTING_CHECK(0 == strcmp(result1, result2))

    result1 = zsLib::XML::Parser::makeAttributeEntitySafe(zsLib::String(zsLib::String("&<>;  \"\'")));
    result2 = "&amp;&lt;&gt;;  &quot;&apos;";
    TESTING_CHECK(0 == strcmp(result1, result2))

    result1 = zsLib::XML::Parser::makeAttributeEntitySafe(zsLib::String("&<>;  \""));
    result2 = "&amp;&lt;&gt;;  \"";
    TESTING_CHECK(0 == strcmp(result1, result2))

    result1 = zsLib::XML::Parser::makeAttributeEntitySafe(zsLib::String("&<>;  \'"));
    result2 = "&amp;&lt;&gt;;  \'";
    TESTING_CHECK(0 == strcmp(result1, result2))

    zsLib::XML::TextPtr text11(::zsLib::XML::Text::create());
    text11->setValue(zsLib::String("\n"));
    element2->adoptAsNextSibling(text11);

    treeWalkChecker(document);

    zsLib::XML::ElementPtr element4(::zsLib::XML::Element::create());
    element4->setValue(zsLib::String("encoding"));
    element4->setAttribute(zsLib::String("value1"), zsLib::XML::Parser::makeAttributeEntitySafe(zsLib::String("\"\'")));
    element4->setAttribute(zsLib::String("value2"), zsLib::XML::Parser::makeAttributeEntitySafe(zsLib::String("\"")));
    element4->setAttribute(zsLib::String("value3"), zsLib::XML::Parser::makeAttributeEntitySafe(zsLib::String("\'")));
    text11->adoptAsNextSibling(element4);

    treeWalkChecker(document);

    zsLib::XML::TextPtr text12(::zsLib::XML::Text::create());
    text12->setValue(zsLib::String("<cdata test & result>"), zsLib::XML::Text::Format_CDATA);
    text12->setOutputFormat(zsLib::XML::Text::Format_CDATA);
    TESTING_CHECK(zsLib::XML::Text::Format_CDATA == text12->getOutputFormat())

    zsLib::String cdataResult = text12->getValueInFormat(zsLib::XML::Text::Format_EntityEncoded);
    TESTING_CHECK(0 == strcmp(cdataResult, "&lt;cdata test &amp; result&gt;"))

    element4->adoptAsFirstChild(text12);

    treeWalkChecker(document);

    zsLib::XML::UnknownPtr unknown1(::zsLib::XML::Unknown::create());
    unknown1->setValue(zsLib::String("!unknown!"));
    TESTING_CHECK(0 == strcmp(unknown1->getValue(), "!unknown!"))
    caught = false;
    try {(unknown1->toNode())->adoptAsFirstChild(attribute7);} catch(...) {caught = true;}
    TESTING_CHECK(caught)
    caught = false;
    try {(unknown1->toNode())->adoptAsLastChild(attribute7);} catch(...) {caught = true;}
    TESTING_CHECK(caught)
    element4->adoptAsNextSibling(unknown1);
    treeWalkChecker(document);

    /*
     "<?xml value12=\"12\" value2=\"2\" value1=\"1\" value3=\"foo4\" value11=\"11\" value10=\"10\" ?>\n"
     "insert previous which is first sibling\n"
     "<test value13=\"13\" value5=\"5\" value9=\"9\" value6=\"6\" value8=\"8\"><!--comment1-->text</test>\n"
     "insert previous sibling\n"
     "insert next sibling\n"
     "\n"
     "insert next which is last sibling\n"
     "<text> this  &lt;is&gt; \n"
     "a &#96;test&#96; of &nbsp;the <wild> text </wild>\telement\n"
     "\n"
     "</text>\n"
     "<encoding value1=\"&quot;&apos;\" value2=\'\"\' value3=\"\'\"><![CDATA[<cdata test & result>]]></encoding><!unknown!>";
     */

    // check attributes
    TESTING_CHECK(0 == strcmp(element4->getAttributeValue(zsLib::String("value1")), "&quot;&apos;"))
    TESTING_CHECK(0 == strcmp(element4->getAttributeValue(zsLib::String("bogus")), ""))

    // check element walker
    TESTING_CHECK(element4->findAttribute(zsLib::String("value3"))->getParentElement().get() == element4.get())
    TESTING_CHECK(!(document->getParentElement()))
    TESTING_CHECK(!(document->getFirstSiblingElement()))
    TESTING_CHECK(!(document->getLastSiblingElement()))
    TESTING_CHECK(!(document->getPreviousSiblingElement()))
    TESTING_CHECK(!(document->getNextSiblingElement()))
    TESTING_CHECK(document->getFirstChildElement() == element1)
    TESTING_CHECK(document->getFirstChildElement()->getLastSiblingElement() == element4)
    TESTING_CHECK(document->getFirstChildElement()->getNextSiblingElement() == element2)
    TESTING_CHECK(!(element1->getFirstChildElement()))
    TESTING_CHECK(!(element1->getLastChildElement()))
    TESTING_CHECK(document->getLastChildElement() == element4)
    TESTING_CHECK(document->getLastChildElement()->getFirstSiblingElement() == element1)
    TESTING_CHECK(document->getLastChildElement()->getPreviousSiblingElement() == element2)

    TESTING_CHECK(document->findFirstChildElement(zsLib::String("test")) == element1)
    TESTING_CHECK(document->findFirstChildElement(zsLib::String("text")) == element2)
    TESTING_CHECK(!(document->findFirstChildElement(zsLib::String("wild"))))
    TESTING_CHECK(document->findFirstChildElement(zsLib::String("text"))->findFirstChildElement(zsLib::String("wild")) == element3)
    TESTING_CHECK(document->findFirstChildElement(zsLib::String("encoding")) == element4)

    //test case
    TESTING_CHECK(document->isElementNameIsCaseSensative())
    TESTING_CHECK(!(document->findFirstChildElement(zsLib::String("TEST"))))

    // normally don't change case sensativity after a document is formed but this is a special case
    document->setElementNameIsCaseSensative(false);
    TESTING_CHECK(document->findFirstChildElement(zsLib::String("TEST")) == element1)
    document->setElementNameIsCaseSensative(true);

    zsLib::XML::GeneratorPtr generator = zsLib::XML::Generator::createXMLGenerator();

    size_t length = 0;
    std::unique_ptr<char[]> output = generator->write(document, &length);

    TESTING_CHECK(length == strlen(output.get()))

    // TESTING_STDOUT() << "--- SOR ---\n" << output << "--- EOR ---\n";

    TESTING_CHECK(0 == strcmp(gOutput, output.get()))

    document->clear();
    TESTING_CHECK(zsLib::XML::NodePtr() == document->getLastChild())

    {int i = 0; ++i;}
  }

  void parserPosTestConst(const zsLib::XML::ParserPos &constPos)
  {
    zsLib::XML::ParserPos temp1(++constPos);
    TESTING_CHECK(temp1.isString("OL"))
    zsLib::XML::ParserPos temp2(--constPos);
    TESTING_CHECK(temp2.isString(" EOL"))
    size_t length1 = (size_t)(temp1 - temp2);
    TESTING_CHECK(2 == length1)

    // move the end to the EOF
    temp1.setEOF();
    length1 = (size_t)(temp1 - temp2);
    TESTING_CHECK(strlen(" EOL") == length1)

    TESTING_CHECK(0 == strcmp((CSTR)temp2, " EOL"))

    // reverse the positions
    zsLib::XML::ParserPos temp1a(temp2);
    zsLib::XML::ParserPos temp2a(temp1);
    size_t length1a = temp1a - temp2a;
    TESTING_CHECK((-1*strlen(" EOL")) == length1a)

    temp1a.setEOF();
    temp2a.setEOF();
    length1a = temp1a - temp2a;
    TESTING_CHECK(0 == length1a)
    TESTING_CHECK(NULL != ((CSTR)temp1a))
    if (NULL != ((CSTR)temp1a))
    {
      TESTING_CHECK(0 == strcmp((CSTR)temp1a, ""))
    }

    // cause it to go the physical
    --temp2a;
    ++temp2a;
    length1a = temp1a - temp2a;
    TESTING_CHECK(0 == length1a)
  }

  void parserPosTest()
  {
    static CSTR gParse =
    "<test>this</test> DOS EOL\r\n"
    "whatever dude!\r"
    "WAS MAC EOL";

    zsLib::XML::ParserPtr parser = zsLib::XML::Parser::createXMLParser();

    zsLib::XML::DocumentPtr document = parser->parse(gParse);
    zsLib::XML::ParserPos pos(parser, document);
    TESTING_CHECK(pos.isSOF())
    TESTING_CHECK(pos.isString("<test>"))

    pos.setEOF();
    TESTING_CHECK(pos.isEOF())

    pos -= strlen("EOL");
    TESTING_CHECK(pos.isString("EOL"))
    parserPosTestConst(pos);

    pos.setSOF();
    TESTING_CHECK(pos.isSOF())

    ++pos;

    TESTING_CHECK(pos.isString("test>this"))

    --pos;
    --pos;   // try and force the pos to be before the start of the parse blob

    // that attempt should fail...
    TESTING_CHECK(pos.isString("<test>"))
    TESTING_CHECK(1 == pos.mRow)
    TESTING_CHECK(1 == pos.mColumn)

    // advance to EOL
    pos += (size_t)(strstr(gParse, "\r\n") - gParse);
    TESTING_CHECK(pos.isString("\r\n"))
    TESTING_CHECK(1 == pos.mRow)
    TESTING_CHECK(pos.mColumn == ((size_t)(strstr(gParse, "\r\n") - gParse))+1)

    // advance passed the \r which should force the column to 1
    ++pos;
    TESTING_CHECK(pos.isString("\n"))
    TESTING_CHECK(1 == pos.mRow)
    TESTING_CHECK(pos.mColumn == ((size_t)(strstr(gParse, "\r\n") - gParse))+1)

    ++pos;
    TESTING_CHECK(pos.isString("whatever"))
    TESTING_CHECK(2 == pos.mRow)
    TESTING_CHECK(1 == pos.mColumn)

    --pos;
    TESTING_CHECK(pos.isString("\n"))
    TESTING_CHECK(1 == pos.mRow)
    TESTING_CHECK(pos.mColumn == ((size_t)(strstr(gParse, "\r\n") - gParse))+1)

    ++pos;
    pos += strlen("whatever dude!");
    TESTING_CHECK(pos.isString("\r"))
    TESTING_CHECK(2 == pos.mRow)
    TESTING_CHECK(pos.mColumn == ((ULONG)strlen("whatever dude!")+1))

    ++pos;
    TESTING_CHECK(pos.isString("WAS MAC EOL"))
    TESTING_CHECK(3 == pos.mRow)
    TESTING_CHECK(1 == pos.mColumn)

    --pos;
    TESTING_CHECK(pos.isString("\r"))
    TESTING_CHECK(2 == pos.mRow)
    TESTING_CHECK(pos.mColumn == ((ULONG)strlen("whatever dude!")+1))

    pos.setSOF();
    pos += 3;
    zsLib::XML::ParserPos pos2(pos - 2);
    TESTING_CHECK(pos2.isString("test>this"))

    zsLib::XML::ParserPos pos3(pos - strlen("es"));
    TESTING_CHECK(pos3.isString("test>this"))

    {int i = 0; ++i;}
  }
};

void testXML()
{
  if (!ZSLIB_TEST_XML) return;

  TestXML test;
}
