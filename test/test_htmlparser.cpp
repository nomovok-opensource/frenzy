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

#include "parser/htmlparser.hpp"
#include "dom/document.hpp"
#include "dom/element.hpp"
#include "test_helpers.hpp"

using namespace frenzy;
using namespace frenzy::dom;
using namespace frenzy::test_helpers;

namespace
{
  // Helper function for testing the parser
  void parser_test(std::string inputstr, mocknode expected)
  {
    Documentp doc(Document::create());

    BOOST_TEST_CHECKPOINT("Constructing the parser");

    htmlparser parser(doc);

    BOOST_TEST_CHECKPOINT("Passing input");

    bytestring input(bstr(inputstr));
    parser.pass_bytes(input);
    parser.pass_eof();

    BOOST_CHECK(parser.stopped());

    Elementp root = doc->get_documentElement();
    BOOST_REQUIRE_MESSAGE(root, "document doesn't have root element");

    assert_node_and_children(root, expected);
  }
}

BOOST_AUTO_TEST_SUITE(htmlparser_tests)

BOOST_AUTO_TEST_CASE(simple_tree)
{
  parser_test("<html><head></head><body></body></html>",
	      elem("html")
	      + elem("head")
	      + elem("body"));
}

BOOST_AUTO_TEST_CASE(with_text)
{
  parser_test("<html><head></head><body><h1>Hello world</h1></body></html>",
	      elem("html")
	      + elem("head")
	      + (elem("body")
		 + (elem("h1")
		    + txt("Hello world"))));
}

BOOST_AUTO_TEST_SUITE_END()
