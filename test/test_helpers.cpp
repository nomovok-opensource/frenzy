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

#include <fstream>
#include <stdexcept>
#include <iterator>

#include <boost/bind.hpp>

#include "dom/document.hpp"
#include "dom/element.hpp"
#include "dom/text.hpp"
#include "parser/htmlparser.hpp"
#include "test_helpers.hpp"

frenzy::dom::Documentp
frenzy::test_helpers::load(std::string filename)
{
  using namespace parser;

  std::ifstream input(filename.c_str());
  if (!input || !input.is_open())
  {
    throw std::runtime_error("Error opening " + filename);
  }

  dom::Documentp doc = dom::Document::create();

  htmlparser parser(doc);

  bytestring inputstring((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

  parser.pass_bytes(inputstring);
  parser.pass_eof();

  // The loading must succeed
  BOOST_REQUIRE(parser.stopped());

  return doc;
}

bool
frenzy::test_helpers::ascii_case_insensitive_equals(frenzy::ustring one, frenzy::ustring two)
{
  // We're only dealing with _ascii_ case insensitive, so toupper for
  // both leads to correct results.
  // Also BOOST_CHECK_EQUAL the values to get better test failure output.
  ustring oneup = ascii_toupper(one);
  ustring twoup = ascii_toupper(two);
  BOOST_CHECK_EQUAL(oneup, twoup);
  return oneup == twoup;
}

std::vector<frenzy::ustring>
frenzy::test_helpers::ascii_toupper(const std::vector<frenzy::ustring>& v)
{
  std::vector<ustring> ret;
  
  for (std::vector<ustring>::const_iterator it = v.begin();
       it != v.end();
       ++it)
  {
    ret.push_back(ascii_toupper(*it));
  }
  
  return ret;
}

frenzy::ustring
frenzy::test_helpers::utf8dec(std::string str)
{
  bytestring bs(str.begin(), str.end());

  parser::utf8_decoder dec;

  dec.pass_bytes(bs);

  urope r = dec.complete_characters();
  ustring ret;
  for (urope::const_iterator it = r.begin();
       it != r.end();
       ++it)
  {
    ret.push_back(*it);
  }

  return ret;
}

frenzy::test_helpers::mocknode::mocknode(frenzy::dom::Node::nodeType type)
  : type(type)
{
}

frenzy::test_helpers::attr::attr()
{}

frenzy::test_helpers::attr::attr(std::string key, std::string value)
{
  attributes.insert(std::make_pair(key, value));
}

frenzy::test_helpers::mocknode
frenzy::test_helpers::operator+(frenzy::test_helpers::mocknode one,
				frenzy::test_helpers::mocknode two)
{
  one.children.push_back(two);
  return one;
}

frenzy::test_helpers::attr
frenzy::test_helpers::operator+(frenzy::test_helpers::attr one,
				frenzy::test_helpers::attr two)
{
  one.attributes.insert(two.attributes.begin(), two.attributes.end());
  return one;
}

frenzy::test_helpers::mocknode
frenzy::test_helpers::elem(std::string name, frenzy::test_helpers::attr attributes)
{
  mocknode ret(dom::Node::ELEMENT_NODE);
  ret.name = name;
  ret.attributes = attributes.attributes;
  return ret;
}

frenzy::test_helpers::mocknode
frenzy::test_helpers::txt(std::string text)
{
  mocknode ret(dom::Node::TEXT_NODE);
  ret.name = text;
  return ret;
}

namespace
{
  void assert_element_data(frenzy::dom::Elementp elem, frenzy::test_helpers::mocknode expected)
  {
    frenzy::ustring xptag(expected.name);
    BOOST_CHECK_EQUAL(xptag, elem->get_localName());
    
    BOOST_CHECK_EQUAL(elem->get_attributes()->get_length(), expected.attributes.size());
    for (std::map<std::string, std::string>::iterator it = expected.attributes.begin();
	 it != expected.attributes.end();
	 ++it)
    {
      frenzy::ustring value = elem->getAttribute(it->first).get_value_or("");
      frenzy::ustring expected = it->second;
      BOOST_CHECK_EQUAL(value, expected);
    }
  }
  
  void assert_text_data(frenzy::dom::Textp t, frenzy::test_helpers::mocknode expected)
  {
    frenzy::ustring data(expected.name);
    BOOST_CHECK_EQUAL(data, t->get_data());
  }
}

void
frenzy::test_helpers::assert_node_and_children(frenzy::dom::Nodep n,
					       frenzy::test_helpers::mocknode expected,
					       size_t depth)
{
  BOOST_CHECK_EQUAL(n->get_nodeType(), expected.type);
  
  if (n->get_nodeType() == expected.type)
  {
    switch (n->get_nodeType())
    {
    case dom::Node::ELEMENT_NODE:
      assert_element_data(dom_cast<dom::Element>(n), expected);
      break;
    case dom::Node::TEXT_NODE:
      assert_text_data(dom_cast<dom::Text>(n), expected);
      break;
    default:
      BOOST_REQUIRE_MESSAGE(1 == 0, "Test for this node type not implemented");
      break;
    }
  }
  
  dom::NodeListp nl = n->get_childNodes();
  BOOST_REQUIRE_MESSAGE(nl->get_length() == expected.children.size(), "Depth " << depth << ": childcount " << nl->get_length() << ", expected " << expected.children.size());
  for (size_t i = 0; i < nl->get_length(); ++i)
  {
    dom::Nodep c = nl->item(i);
    assert_node_and_children(c, expected.children[i], depth + 1);
  }
}
