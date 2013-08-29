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

#include <limits>

#include "util/unicode.hpp"
#include "test_helpers.hpp"

using namespace frenzy;

BOOST_TEST_DONT_PRINT_LOG_VALUE(frenzy::ustring::iterator)
BOOST_TEST_DONT_PRINT_LOG_VALUE(frenzy::ustring::const_iterator)
BOOST_TEST_DONT_PRINT_LOG_VALUE(frenzy::urope::const_iterator)

namespace
{
  void identity_tests(ustring& str)
  {
    const ustring& ref = str;

    BOOST_CHECK_EQUAL(str.begin(), ref.begin());
    BOOST_CHECK_EQUAL(str.end(), ref.end());
    
    BOOST_CHECK(str == str);
    BOOST_CHECK(!(str != str));
    BOOST_CHECK(!(str < str));
  }

  void identity_tests(urope& str)
  {
    BOOST_CHECK(str == str);
    BOOST_CHECK(!(str != str));
  }

  void verify_empty(urope& u)
  {
    BOOST_CHECK(u.empty());
    BOOST_CHECK_EQUAL(u.size(), 0);
    BOOST_CHECK(!u.size_at_least(1));
    BOOST_CHECK(u.size_at_least(0));
    BOOST_CHECK_EQUAL(u.begin(), u.end());

    // And for kicks
    BOOST_CHECK_EQUAL(u.begin(), u.begin());
    BOOST_CHECK_EQUAL(u.end(), u.end());
  }

  void verify_size(urope& u, size_t expected)
  {
    BOOST_CHECK_EQUAL(u.size(), expected);
    BOOST_CHECK(u.size_at_least(expected));
    BOOST_CHECK(!u.size_at_least(expected + 1));
    BOOST_CHECK_EQUAL(std::distance(u.begin(), u.end()), expected);
  }

  void verify_contents_iterating(urope& u, std::string expected)
  {
    BOOST_REQUIRE_EQUAL(u.size(), expected.size());

    size_t i = 0;
    for (urope::const_iterator it = u.begin(),
	   otherit = u.begin(),
	   begin = u.begin();
	 it != u.end();
	 ++it, ++otherit)
    {
      if (it != u.begin())
      {
	BOOST_CHECK_NE(it, begin);
      }
      BOOST_CHECK_EQUAL(*it, expected[i++]);
      BOOST_CHECK_EQUAL(it, otherit);
    }

    urope::const_iterator rev = u.end();
    urope::const_iterator otherrev = u.end();
    urope::const_iterator revbegin = u.begin();
    do
    {
      BOOST_CHECK_NE(rev, revbegin);
      --rev;
      --otherrev;
      BOOST_CHECK_EQUAL(*rev, expected[--i]);
      BOOST_CHECK_EQUAL(rev, otherrev);
    } while (rev != u.begin());
  }

  void verify_contents_indexing(urope& u, std::string expected)
  {
    BOOST_REQUIRE_EQUAL(u.size(), expected.size());

    for (size_t i = 0; i < u.size(); ++i)
    {
      BOOST_CHECK_EQUAL(u[i], expected[i]);
    }
  }

  void verify_contents(urope& u, std::string expected)
  {
    verify_contents_iterating(u, expected);
    verify_contents_indexing(u, expected);
  }
}

BOOST_AUTO_TEST_SUITE(unicode_tests)

BOOST_AUTO_TEST_CASE(uchar_size)
{
  // Could also be tested with BOOST_STATIC_ASSERT
  BOOST_CHECK_EQUAL(sizeof(uchar), 4);
  BOOST_REQUIRE(std::numeric_limits<uchar>::is_specialized);
  BOOST_CHECK(!std::numeric_limits<uchar>::is_signed);
}

BOOST_AUTO_TEST_SUITE(ustring_tests)

BOOST_AUTO_TEST_CASE(empty)
{
  ustring str;
  const ustring& ref = str;

  BOOST_CHECK(str.empty());
  BOOST_CHECK_EQUAL(str.size(), 0);
  BOOST_CHECK_EQUAL(str.begin(), str.end());
  BOOST_CHECK_EQUAL(ref.begin(), ref.end());

  identity_tests(str);
}

BOOST_AUTO_TEST_CASE(from_str_literal)
{
  ustring str("hello world");
  const ustring& ref = str;

  BOOST_CHECK(!str.empty());
  BOOST_CHECK_EQUAL(str.size(), 11);
  BOOST_CHECK_NE(str.begin(), str.end());
  BOOST_CHECK_NE(ref.begin(), ref.end());

  identity_tests(str);
}

BOOST_AUTO_TEST_CASE(from_std_string)
{
  std::string hello = "hello world";
  ustring str(hello);
  const ustring& ref = str;

  BOOST_CHECK(!str.empty());
  BOOST_CHECK_EQUAL(str.size(), 11);
  BOOST_CHECK_NE(str.begin(), str.end());
  BOOST_CHECK_NE(ref.begin(), ref.end());

  identity_tests(str);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(urope_tests)

BOOST_AUTO_TEST_CASE(empty)
{
  urope str;

  verify_empty(str);

  identity_tests(str);
}

BOOST_AUTO_TEST_CASE(from_ustring)
{
  urope str("hello world");

  BOOST_CHECK(!str.empty());
  BOOST_CHECK_NE(str.begin(), str.end());
  verify_size(str, 11);
  verify_contents(str, "hello world");
}

BOOST_AUTO_TEST_CASE(splicing)
{
  urope hello("hello ");
  urope world("world");

  hello.splice_back(world);

  verify_empty(world);

  urope helloworld("hello world");
  BOOST_CHECK_EQUAL(hello, helloworld);
  verify_size(hello, 11);
  verify_contents(hello, "hello world");
}

BOOST_AUTO_TEST_CASE(chunk_deletion)
{
  urope hello("h");
  urope world("ello world");

  hello.splice_back(world);
  
  hello.pop_front();

  verify_size(hello, 10);
  verify_contents(hello, "ello world");
}

BOOST_AUTO_TEST_CASE(popping_to_empty)
{
  urope hello("hi");

  hello.pop_front();
  hello.pop_front();

  verify_empty(hello);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
