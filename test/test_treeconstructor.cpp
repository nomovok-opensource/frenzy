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

#include <map>
#include <string>
#include <vector>

#include <boost/bind.hpp>

#include "parser/treeconstructor.hpp"
#include "dom/node.hpp"
#include "dom/element.hpp"
#include "dom/text.hpp"
#include "dom/document.hpp"
#include "test_helpers.hpp"

using namespace frenzy;
using namespace frenzy::parser;
using namespace frenzy::dom;
using namespace frenzy::test_helpers;

namespace
{
  // Helper function for testing tree construction
  void construct_test(std::string inputstr, mocknode expected)
  {
    Documentp doc(Document::create());

    BOOST_TEST_CHECKPOINT("Constructing the parser stages");

    htmltokenizer tok;
    treeconstructor tree(doc);

    BOOST_TEST_CHECKPOINT("Coupling the tokenizer and tree constructor");

    tree.couple_tokenizer(&tok);
    BOOST_CHECK(!tree.stopped());

    BOOST_TEST_CHECKPOINT("Passing input");

    urope input(inputstr);
    tok.pass_characters(input);
    tok.pass_characters(urope());

    BOOST_CHECK(tree.stopped());

    BOOST_CHECK_EQUAL(tree.document(), doc);

    Elementp root = doc->get_documentElement();
    BOOST_REQUIRE_MESSAGE(root, "document doesn't have root element");

    assert_node_and_children(root, expected);
  }
}

BOOST_AUTO_TEST_SUITE(meta_mocknode_tests)

BOOST_AUTO_TEST_CASE(single_element)
{
  mocknode e = elem("html");

  BOOST_CHECK_EQUAL(e.type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(e.name, "html");
  BOOST_CHECK_EQUAL(e.children.size(), 0);

  mocknode t = txt("hello");
  BOOST_CHECK_EQUAL(t.type, Node::TEXT_NODE);
  BOOST_CHECK_EQUAL(t.name, "hello");
  BOOST_CHECK_EQUAL(t.children.size(), 0);
}

BOOST_AUTO_TEST_CASE(single_child)
{
  mocknode x =
    elem("html")
    + elem("head");

  BOOST_CHECK_EQUAL(x.type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.name, "html");
  BOOST_REQUIRE_EQUAL(x.children.size(), 1);
  
  BOOST_CHECK_EQUAL(x.children[0].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[0].name, "head");
  BOOST_REQUIRE_EQUAL(x.children[0].children.size(), 0);
}

BOOST_AUTO_TEST_CASE(multiple_children)
{
  mocknode x =
    elem("html")
    + elem("head")
    + elem("body");

  BOOST_CHECK_EQUAL(x.type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.name, "html");
  BOOST_REQUIRE_EQUAL(x.children.size(), 2);
  
  BOOST_CHECK_EQUAL(x.children[0].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[0].name, "head");
  BOOST_REQUIRE_EQUAL(x.children[0].children.size(), 0);

  BOOST_CHECK_EQUAL(x.children[1].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[1].name, "body");
  BOOST_REQUIRE_EQUAL(x.children[1].children.size(), 0);
}

BOOST_AUTO_TEST_CASE(single_child_tree)
{
  mocknode x =
    elem("html")
    + (elem("head")
       + elem("title"));

  BOOST_CHECK_EQUAL(x.type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.name, "html");
  BOOST_REQUIRE_EQUAL(x.children.size(), 1);
  
  BOOST_CHECK_EQUAL(x.children[0].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[0].name, "head");
  BOOST_REQUIRE_EQUAL(x.children[0].children.size(), 1);
  
  BOOST_CHECK_EQUAL(x.children[0].children[0].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[0].children[0].name, "title");
  BOOST_REQUIRE_EQUAL(x.children[0].children[0].children.size(), 0);
}

BOOST_AUTO_TEST_CASE(complex_tree)
{
  mocknode x =
    elem("html")
    + (elem("head")
       + elem("title"))
    + (elem("body")
       + elem("p"));

  BOOST_CHECK_EQUAL(x.type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.name, "html");
  BOOST_REQUIRE_EQUAL(x.children.size(), 2);
  
  BOOST_CHECK_EQUAL(x.children[0].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[0].name, "head");
  BOOST_REQUIRE_EQUAL(x.children[0].children.size(), 1);
  
  BOOST_CHECK_EQUAL(x.children[0].children[0].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[0].children[0].name, "title");
  BOOST_REQUIRE_EQUAL(x.children[0].children[0].children.size(), 0);

  BOOST_CHECK_EQUAL(x.children[1].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[1].name, "body");
  BOOST_REQUIRE_EQUAL(x.children[1].children.size(), 1);

  BOOST_CHECK_EQUAL(x.children[1].children[0].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[1].children[0].name, "p");
  BOOST_REQUIRE_EQUAL(x.children[1].children[0].children.size(), 0);
}

BOOST_AUTO_TEST_CASE(combining_attributes)
{
  attr x = attr("foo", "bar") + attr("quz", "bleh") + attr("qwerty", "asdf");

  BOOST_CHECK_EQUAL(x.attributes.size(), 3);
  BOOST_CHECK_EQUAL(x.attributes["foo"], "bar");
  BOOST_CHECK_EQUAL(x.attributes["quz"], "bleh");
  BOOST_CHECK_EQUAL(x.attributes["qwerty"], "asdf");
}

BOOST_AUTO_TEST_CASE(attributes_in_elements)
{
  mocknode x =
    elem("html")
    + (elem("head")
       + elem("title"))
    + (elem("body")
       + elem("p", attr("class", "x") + attr("style", "color: black")));

  BOOST_CHECK_EQUAL(x.type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.name, "html");
  BOOST_CHECK(x.attributes.empty());
  BOOST_REQUIRE_EQUAL(x.children.size(), 2);
  
  BOOST_CHECK_EQUAL(x.children[0].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[0].name, "head");
  BOOST_CHECK(x.children[0].attributes.empty());
  BOOST_REQUIRE_EQUAL(x.children[0].children.size(), 1);
  
  BOOST_CHECK_EQUAL(x.children[0].children[0].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[0].children[0].name, "title");
  BOOST_CHECK(x.children[0].children[0].attributes.empty());
  BOOST_REQUIRE_EQUAL(x.children[0].children[0].children.size(), 0);

  BOOST_CHECK_EQUAL(x.children[1].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[1].name, "body");
  BOOST_CHECK(x.children[1].attributes.empty());
  BOOST_REQUIRE_EQUAL(x.children[1].children.size(), 1);

  BOOST_CHECK_EQUAL(x.children[1].children[0].type, Node::ELEMENT_NODE);
  BOOST_CHECK_EQUAL(x.children[1].children[0].name, "p");
  BOOST_REQUIRE_EQUAL(x.children[1].children[0].children.size(), 0);

  std::map<std::string, std::string>& attrs = x.children[1].children[0].attributes;
  BOOST_CHECK_EQUAL(attrs.size(), 2);
  BOOST_CHECK_EQUAL(attrs["class"], "x");
  BOOST_CHECK_EQUAL(attrs["style"], "color: black");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(treeconstructor_tests)

BOOST_AUTO_TEST_CASE(simple_tree)
{
  construct_test("<html><head></head><body></body></html>",
		 elem("html")
		 + elem("head")
		 + elem("body"));
}

BOOST_AUTO_TEST_CASE(with_text)
{
  construct_test("<html><head></head><body><h1>Hello world</h1></body></html>",
		 elem("html")
		 + elem("head")
		 + (elem("body")
		    + (elem("h1")
		       + txt("Hello world"))));
}

BOOST_AUTO_TEST_CASE(simple_attributes)
{
  construct_test("<html><head></head><body>"
		 "<p class='x'><a href=\"http://www.google.com/\">google</a></p>"
		 "</body></html>",
		 elem("html")
		 + elem("head")
		 + (elem("body")
		    + (elem("p", attr("class", "x"))
		       + (elem("a", attr("href", "http://www.google.com/"))
			  + txt("google")))));
}

BOOST_AUTO_TEST_CASE(simple_table)
{
  construct_test("<html><head></head><body>"
		 "<table><tr><td>aa</td></tr></table>"
		 "</body></html>",
		 elem("html")
		 + elem("head")
		 + (elem("body")
		    + (elem("table")
		       + (elem("tbody")
			  + (elem("tr")
			     + (elem("td")
				+ txt("aa")))))));
}

BOOST_AUTO_TEST_CASE(title_element)
{
  construct_test("<html><head>"
		 "<title>Simple title</title>"
		 "</head><body>"
		 "<p>1</p>"
		 "</body></html>",
		 elem("html")
		 + (elem("head")
		    + (elem("title")
		       + txt("Simple title")))
		 + (elem("body")
		    + (elem("p")
		       + txt("1"))));		 
}

BOOST_AUTO_TEST_CASE(misnested_nonblock_tags)
{
  // Example straight from HTML5 8.2.8.1
  construct_test("<html><head></head><body>"
		 "<p>1<b>2<i>3</b>4</i>5</p>"
		 "</body></html>",
		 elem("html")
		 + elem("head")
		 + (elem("body")
		    + (elem("p")
		       + txt("1")
		       + (elem("b")
			  + txt("2")
			  + (elem("i")
			     + txt("3")))
		       + (elem("i")
			  + txt("4"))
		       + txt("5"))));
}

BOOST_AUTO_TEST_CASE(misnested_block_tags)
{
  // Example straight from HTML5 8.2.8.2
  construct_test("<html><head></head><body>"
		 "<b>1<p>2</b>3</p>"
		 "</body></html>",
		 elem("html")
		 + elem("head")
		 + (elem("body")
		    +(elem("b")
		      + txt("1"))
		    + (elem("p")
		       + (elem("b")
			  + txt("2"))
		       + txt("3"))));
}

BOOST_AUTO_TEST_CASE(table_foster_parenting)
{
  // Example straight from HTML5 8.2.8.3
  construct_test("<html><head></head><body>"
		 "<table><b><tr><td>aaa</td></tr>bbb</table>ccc"
		 "</body></html>",
		 elem("html")
		 + elem("head")
		 + (elem("body")
		    + elem("b")
		    + (elem("b")
		       + txt("bbb"))
		    + (elem("table")
		       + (elem("tbody")
			  + (elem("tr")
			     + (elem("td")
				+ txt("aaa")))))
		    + (elem("b")
		       + txt("ccc"))));
}

BOOST_AUTO_TEST_CASE(text_node_combining_with_foster_parenting)
{
  // Examples straight from HTML5 8.2.5
  construct_test("<html><head></head><body>"
		 "A<table>B<tr>C</tr>D</table>"
		 "</body></html>",
		 elem("html")
		 + elem("head")
		 + (elem("body")
		    + txt("ABCD")
		    + (elem("table")
		       + (elem("tbody")
			  + elem("tr")))));
  
  construct_test("<html><head></head><body>"
		 "A<table><tr> B</tr> C</table>"
		 "</body></html>",
		 elem("html")
		 + elem("head")
		 + (elem("body")
		    + txt("A B C")
		    + (elem("table")
		       + (elem("tbody")
			  + elem("tr")))));
  
  construct_test("<html><head></head><body>"
		 "A<table><tr> B</tr> </em>C</table>"
		 "</body></html>",
		 elem("html")
		 + elem("head")
		 + (elem("body")
		    + txt("A BC")
		    + (elem("table")
		       + (elem("tbody")
			  + elem("tr")
			  + txt(" ")))));
}

BOOST_AUTO_TEST_SUITE_END()
