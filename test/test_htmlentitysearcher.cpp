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

#include "parser/htmlentitysearcher.hpp"
#include "test_helpers.hpp"

using namespace frenzy;
using namespace frenzy::parser;

namespace
{
  /*
   * Do a search test that's assumed to succeed and results in given
   * codepoints.
   */
  void search_test(std::string input, uchar cp1, uchar cp2 = 0)
  {
    htmlentitysearcher s(input[0]);

    for (size_t i = 1; i < input.size() && s.need_more_input(); ++i)
    {
      s.next(input[i]);
    }

    const htmlentity* result = s.result();
    BOOST_REQUIRE(result != NULL);

    BOOST_CHECK_EQUAL(result->codepoint1, cp1);
    BOOST_CHECK_EQUAL(result->codepoint2, cp2);
  }

  /*
   * Search test like above, but one that's assumed to require more
   * input.
   */
  void search_test_short(std::string input)
  {
    htmlentitysearcher s(input[0]);

    for (size_t i = 1; i < input.size(); ++i)
    {
      s.next(input[i]);
    }

    BOOST_CHECK(s.need_more_input());
  }

  /*
   * Search test like above, but one that's assumed to not find an
   * entity.
   */
  void search_test_notfound(std::string input)
  {
    htmlentitysearcher s(input[0]);

    for (size_t i = 1; i < input.size(); ++i)
    {
      s.next(input[i]);
    }

    BOOST_CHECK(!s.need_more_input());
    BOOST_CHECK(s.result() == NULL);
  }
}

BOOST_AUTO_TEST_SUITE(htmlentitysearcher_tests)

BOOST_AUTO_TEST_CASE(entitydb_correctchars)
{
  for (uchar x = 'a'; x <= 'z'; ++x)
  {
    const htmlentity* begin = htmlentity_begin(x);
    const htmlentity* end = htmlentity_end(x);

    BOOST_REQUIRE(begin != NULL);
    BOOST_REQUIRE(end != NULL);
    BOOST_CHECK(begin < end);

    for (; begin <= end; ++begin)
    {
      BOOST_CHECK_EQUAL(begin->name[0], x);
    }
  }

  for (uchar x = 'A'; x <= 'Z'; ++x)
  {
    const htmlentity* begin = htmlentity_begin(x);
    const htmlentity* end = htmlentity_end(x);

    BOOST_REQUIRE(begin != NULL);
    BOOST_REQUIRE(end != NULL);
    BOOST_CHECK(begin < end);

    for (; begin <= end; ++begin)
    {
      BOOST_CHECK_EQUAL(begin->name[0], x);
    }
  }
}

BOOST_AUTO_TEST_CASE(simple_lookup)
{
  search_test("amp;", '&');
}

BOOST_AUTO_TEST_CASE(simple_lookup_too_short)
{
  search_test_short("am");
}

BOOST_AUTO_TEST_CASE(simple_lookup_not_found)
{
  search_test_notfound("pig;");
}

BOOST_AUTO_TEST_CASE(lookup_result_two_codepoints)
{
  search_test("acE;", 0x223E, 0x0333);
}

BOOST_AUTO_TEST_CASE(missing_semicolon)
{
  search_test("ampfoobar", '&');
}

BOOST_AUTO_TEST_CASE(boundary_cases)
{
  search_test("Aacute;", 0xC1);
  search_test("Aacute", 0xC1);
  search_test("aacute;", 0xE1);
  search_test("aacute", 0xE1);

  search_test("Zscr;", 0x1D4B5);
  search_test("zwnj;", 0x200C);
}

BOOST_AUTO_TEST_SUITE_END()
