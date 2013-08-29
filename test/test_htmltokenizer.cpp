/* 
 * Copyright 2013 by Nomovok Ltd.
 * 
 * Contact: info@nomovok.com
 * 
 * This Source Code Form is subject to the
 * terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with
 * this file, You can obtain one at
 * http://mozilla.org/MPL/2.0/.
 * 
 */

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <sstream>
#include <iomanip>
#include <boost/bind.hpp>

#include "parser/htmltokenizer.hpp"
#include "test_helpers.hpp"

using namespace frenzy;
using namespace frenzy::parser;
using namespace frenzy::test_helpers;

namespace
{
  // Test-specific string serialization of a HTML token

  std::string tokentypestr(const token& t)
  {
    switch (t.type)
    {
    case TOKEN_DOCTYPE:
      return "d";
    case TOKEN_START_TAG:
      return "s";
    case TOKEN_END_TAG:
      return "e";
    case TOKEN_COMMENT:
      return "//";
    case TOKEN_CHARACTER:
      return "c";
    case TOKEN_END_OF_FILE:
      return "eof";
    default:
      return "unknown";
    }
  }

  std::string chartocodepoint(uchar u)
  {
    std::ostringstream out;
    if (std::isprint(u))
      out << static_cast<char>(u);
    else
      out << "U+" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << static_cast<size_t>(u);
    return out.str();
  }

  std::string ustrtostr(ustring s)
  {
    std::string ret;
    for (ustring::const_iterator it = s.begin();
         it != s.end();
         ++it)
    {
      ret += static_cast<char>(*it);
    }
    return ret;
  }

  std::string tokendatastr(const token& t)
  {
    std::string ret;

    switch (t.type)
    {
    case TOKEN_DOCTYPE:
      {
        std::string delim = "";
        if (t.doctype_name)
        {
          ret += delim + "name=" + ustrtostr(*t.doctype_name);
          delim = " ";
        }
        if (t.public_identifier)
        {
          ret += delim + "pid=" + ustrtostr(*t.public_identifier);
          delim = " ";
        }
        if (t.system_identifier)
        {
          ret += delim + "sid=" + ustrtostr(*t.system_identifier);
          delim = " ";
        }
        if (t.force_quirks)
        {
          ret += delim + "force_quirks";
          delim = " ";
        }
        break;
      }
    case TOKEN_START_TAG:
      ret += ustrtostr(t.tagname);
      for (std::map<ustring, ustring>::const_iterator it = t.attributes.begin();
           it != t.attributes.end();
           ++it)
      {
        ret += " " + ustrtostr(it->first) + "=" + ustrtostr(it->second);
      }
      if (t.self_closing)
      {
        ret += " /";
      }
      break;
    case TOKEN_END_TAG:
      ret += ustrtostr(t.tagname);
      break;
    case TOKEN_COMMENT:
      ret += ustrtostr(t.comment);
      break;
    case TOKEN_CHARACTER:
      ret += chartocodepoint(t.character);
      break;
    case TOKEN_END_OF_FILE:
      break;
    default:
      ret += "unknown";
      break;
    }

    if (!ret.empty())
    {
      ret = " " + ret;
    }

    return ret;
  }

  std::string serializetoken(const token& t)
  {
    return "<#" + tokentypestr(t) + tokendatastr(t) + "#>";
  }

  std::string serializetokens(const htmltokenizer::tokensequence_t& d)
  {
    std::string ret;
    for (htmltokenizer::tokensequence_t::const_iterator it = d.begin();
         it != d.end();
         ++it)
    {
      ret += serializetoken(*it);
    }
    return ret;
  }

  typedef destinationT<token> tokdestination;

  // Helper function for testing tokenizer with full strings.
  // The expected output is a test-specific string serialization of tokens.
  // See serializetoken, above.
  void tokenize_test(std::string tokeninput, std::string expected)
  {
    htmltokenizer html;
    htmltokenizer atthtml;
    htmltokenizer phtml;
    htmltokenizer attphtml;

    tokdestination dest;
    tokdestination pdest;

    atthtml.attach_destination(boost::bind(&tokdestination::receive, &dest, _1));
    attphtml.attach_destination(boost::bind(&tokdestination::receive, &pdest, _1));
    
    urope input(tokeninput);
    
    // Pass as a whole
    html.pass_characters(input);
    atthtml.pass_characters(input);
    // Pass a single item at a time
    for (urope::const_iterator it = input.begin();
	 it != input.end();
	 ++it)
    {
      urope partinput(*it);
      phtml.pass_characters(partinput);
      attphtml.pass_characters(partinput);
    }
    
    // And pass eof
    urope eof;
    html.pass_characters(eof);
    atthtml.pass_characters(eof);
    phtml.pass_characters(eof);
    attphtml.pass_characters(eof);

    htmltokenizer::tokensequence_t received = html.complete_tokens();
    htmltokenizer::tokensequence_t preceived = phtml.complete_tokens();

    std::string rectokenstr = serializetokens(received);
    std::string atttokenstr = serializetokens(dest.items);
    std::string prectokenstr = serializetokens(preceived);
    std::string attptokenstr = serializetokens(pdest.items);

    BOOST_CHECK_EQUAL(rectokenstr, expected);
    BOOST_CHECK_EQUAL(atttokenstr, expected);
    BOOST_CHECK_EQUAL(prectokenstr, expected);
    BOOST_CHECK_EQUAL(attptokenstr, expected);

    // dest.ended & co. not checked because htmltokenizer doesn't
    // signal end-of-file that way.  It sends an end-of-file token
    // instead, and below test functions handle it themselves.
  }

  // Helper function for sending input with eof sent too early.
  // Such input must send the eof token regardless of the parse error.
  void spurious_eof_test(std::string tokeninput)
  {
    htmltokenizer html;
    htmltokenizer atthtml;

    tokdestination dest;

    atthtml.attach_destination(boost::bind(&tokdestination::receive, &dest, _1));
    
    urope input(tokeninput);
    urope partinput;

    for (urope::const_iterator it = input.begin();
	 it != input.end();
	 ++it)
    {
      partinput.push_back(*it);

      if (it == input.begin())
	continue;

      htmltokenizer html;
      htmltokenizer atthtml;
      
      tokdestination dest;
      atthtml.attach_destination(boost::bind(&tokdestination::receive, &dest, _1));
      
      urope eof;

      html.pass_characters(partinput);
      atthtml.pass_characters(partinput);
      html.pass_characters(eof);
      atthtml.pass_characters(eof);

      htmltokenizer::tokensequence_t received = html.complete_tokens();

      BOOST_CHECK_MESSAGE(!received.empty(), "Received tokens empty for " + tokeninput);
      if (!received.empty())
      {
	BOOST_CHECK_EQUAL(received.back().type, TOKEN_END_OF_FILE);
      }
    }
  }
}

BOOST_AUTO_TEST_SUITE(htmltokenizer_tests)

BOOST_AUTO_TEST_CASE(character_tokens)
{
  std::string teststr = "qux";
  std::string expstr = "<#c q#><#c u#><#c x#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(tags)
{
  std::string teststr = "<html><head></head></html>";
  std::string expstr = "<#s html#><#s head#><#e head#><#e html#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(tag_with_text)
{
  std::string teststr = "<p>hello</p>";
  std::string expstr = "<#s p#><#c h#><#c e#><#c l#><#c l#><#c o#><#e p#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(tag_with_attributes)
{
  std::string teststr = "<p foo=bar quz=\"bleh\" hello='world' novalue>";
  std::string expstr = "<#s p foo=bar hello=world novalue= quz=bleh#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(tag_self_closing)
{
  std::string teststr = "<img />";
  std::string expstr = "<#s img /#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(basic_doctype_tag)
{
  std::string teststr = "<!DOCTYPE html>";
  std::string expstr = "<#d name=html#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(basic_doctype_tag_case_insensitive)
{
  std::string teststr = "<!DoCtYpE html>";
  std::string expstr = "<#d name=html#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(doctype_tag_with_identifiers)
{
  std::string teststr = "<!DOCTYPE html PUBLIC \"pub\" 'sys'>";
  std::string expstr = "<#d name=html pid=pub sid=sys#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(doctype_tag_with_identifiers_case_insensitive)
{
  std::string teststr = "<!DOCTYPE html puBliC \"pub\" 'sys'>";
  std::string expstr = "<#d name=html pid=pub sid=sys#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(doctype_tag_with_system_identifier)
{
  std::string teststr = "<!DOCTYPE html SYSTEM \"sys\">";
  std::string expstr = "<#d name=html sid=sys#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(doctype_tag_with_system_identifier_case_insensitive)
{
  std::string teststr = "<!DOCTYPE html SySTeM \"sys\">";
  std::string expstr = "<#d name=html sid=sys#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(comment)
{
  std::string teststr = "<!--hello world-->";
  std::string expstr = "<#// hello world#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(named_character_reference)
{
  std::string teststr = "foo&amp;bar";
  std::string expstr = "<#c f#><#c o#><#c o#><#c &#><#c b#><#c a#><#c r#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(numeric_character_reference)
{
  std::string teststr = "foo&#x26;&#38;bar";
  std::string expstr = "<#c f#><#c o#><#c o#><#c &#><#c &#><#c b#><#c a#><#c r#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(named_character_reference_in_attribute)
{
  std::string teststr = "<p foo='&amp;bar'>";
  std::string expstr = "<#s p foo=&bar#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(character_reference_in_attribute_not_a_reference)
{
  std::string teststr = "<p foo='bar&'>";
  std::string expstr = "<#s p foo=bar&#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(numeric_character_reference_in_attribute)
{
  std::string teststr = "<p foo='&#x26;&#38;bar'>";
  std::string expstr = "<#s p foo=&&bar#><#eof#>";

  tokenize_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(spurious_eof)
{
  // TODO: All these tests are supposed to produce a parse error.
  // That should be accounted for, and tested too.

  spurious_eof_test("<!DOCTYPE html>");
  spurious_eof_test("<!DOCTYPE html PUBLIC \"pub\" 'sys'>");
  spurious_eof_test("&amp;");
  spurious_eof_test("&#x26;");
  spurious_eof_test("&#38;");
  spurious_eof_test("<div>");
  spurious_eof_test("<div attr=\"value\">");
  spurious_eof_test("</div>");
  spurious_eof_test("simple text");
  spurious_eof_test("<!-- A comment -->");
}

BOOST_AUTO_TEST_SUITE_END()
