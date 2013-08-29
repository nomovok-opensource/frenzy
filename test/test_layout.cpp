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

#include "dom/document.hpp"
#include "dom/text.hpp"
#include "dom/htmlelement.hpp"
#include "test_helpers.hpp"

using namespace frenzy;
using namespace frenzy::dom;
using namespace frenzy::test_helpers;

namespace
{
}

#error This file needs updating for the latest layout code API.

BOOST_AUTO_TEST_SUITE(layout_tests)

BOOST_AUTO_TEST_CASE(simple_text)
{
  Documentp doc = Document::create();

  HTMLElementp html = dom_cast<HTMLElement>(doc->createElement("html"));
  BOOST_REQUIRE(html);
  doc->appendChild(html);

  {
    vec2 htmlpos = html->get_position();
    BOOST_CHECK_EQUAL(htmlpos, vec2(0, 0));
    vec2 htmlsize = html->get_size();
    BOOST_CHECK_EQUAL(htmlsize, vec2(0, 0));
  }

  HTMLElementp p = dom_cast<HTMLElement>(doc->createElement("p"));
  BOOST_REQUIRE(p);
  html->appendChild(p);

  {
    vec2 htmlpos = html->get_position();
    BOOST_CHECK_EQUAL(htmlpos, vec2(0, 0));
    vec2 htmlsize = html->get_size();
    BOOST_CHECK_EQUAL(htmlsize, vec2(0, 0));

    vec2 ppos = p->get_position();
    BOOST_CHECK_EQUAL(ppos, vec2(0, 0));
    vec2 psize = p->get_size();
    BOOST_CHECK_EQUAL(psize, vec2(0, 0));
  }

  Textp text = doc->createTextNode("Hello world");
  p->appendChild(text);

  {
    vec2 htmlpos = html->get_position();
    BOOST_CHECK_EQUAL(htmlpos, vec2(0, 0));
    vec2 htmlsize = html->get_size();
    BOOST_CHECK_NE(htmlsize, vec2(0, 0));

    vec2 ppos = p->get_position();
    BOOST_CHECK_EQUAL(ppos, vec2(0, 0));
    vec2 psize = p->get_size();
    BOOST_CHECK_NE(psize, vec2(0, 0));
  }
}

BOOST_AUTO_TEST_CASE(two_element_text)
{
  Documentp doc = Document::create();

  HTMLElementp html = dom_cast<HTMLElement>(doc->createElement("html"));
  BOOST_REQUIRE(html);
  doc->appendChild(html);

  {
    vec2 htmlpos = html->get_position();
    BOOST_CHECK_EQUAL(htmlpos, vec2(0, 0));
    vec2 htmlsize = html->get_size();
    BOOST_CHECK_EQUAL(htmlsize, vec2(0, 0));
  }

  HTMLElementp p = dom_cast<HTMLElement>(doc->createElement("p"));
  BOOST_REQUIRE(p);
  html->appendChild(p);

  {
    vec2 htmlpos = html->get_position();
    BOOST_CHECK_EQUAL(htmlpos, vec2(0, 0));
    vec2 htmlsize = html->get_size();
    BOOST_CHECK_EQUAL(htmlsize, vec2(0, 0));

    vec2 ppos = p->get_position();
    BOOST_CHECK_EQUAL(ppos, vec2(0, 0));
    vec2 psize = p->get_size();
    BOOST_CHECK_EQUAL(psize, vec2(0, 0));
  }

  Textp text = doc->createTextNode("Hello world");
  p->appendChild(text);

  {
    vec2 htmlpos = html->get_position();
    BOOST_CHECK_EQUAL(htmlpos, vec2(0, 0));
    vec2 htmlsize = html->get_size();
    BOOST_CHECK_NE(htmlsize, vec2(0, 0));

    vec2 ppos = p->get_position();
    BOOST_CHECK_EQUAL(ppos, vec2(0, 0));
    vec2 psize = p->get_size();
    BOOST_CHECK_NE(psize, vec2(0, 0));
  }

  HTMLElementp p2 = dom_cast<HTMLElement>(doc->createElement("p"));
  BOOST_REQUIRE(p2);
  html->appendChild(p2);

  {
    vec2 htmlpos = html->get_position();
    BOOST_CHECK_EQUAL(htmlpos, vec2(0, 0));
    vec2 htmlsize = html->get_size();
    BOOST_CHECK_NE(htmlsize, vec2(0, 0));

    vec2 ppos = p->get_position();
    BOOST_CHECK_EQUAL(ppos, vec2(0, 0));
    vec2 psize = p->get_size();
    BOOST_CHECK_NE(psize, vec2(0, 0));

    vec2 p2pos = p2->get_position();
    BOOST_CHECK_NE(p2pos, vec2(0, 0));
    BOOST_CHECK_EQUAL(p2pos.x, 0);
    vec2 p2size = p2->get_size();
    BOOST_CHECK_EQUAL(p2size, vec2(0, 0));
  }
}

BOOST_AUTO_TEST_SUITE_END()
