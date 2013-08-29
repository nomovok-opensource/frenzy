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

#include "util/vector.hpp"
#include "test_helpers.hpp"

using namespace frenzy;
using namespace frenzy::test_helpers;

namespace
{
}

BOOST_AUTO_TEST_SUITE(vector_tests)

BOOST_AUTO_TEST_CASE(creation)
{
  vec x(5);

  BOOST_CHECK_EQUAL(x, 5);

  vec2 v(1, 2);

  BOOST_CHECK_EQUAL(v.x, 1);
  BOOST_CHECK_EQUAL(v.y, 2);
}

BOOST_AUTO_TEST_CASE(default_initialization)
{
  vec x;
  vec2 v;

  BOOST_CHECK_EQUAL(x, 0);
  BOOST_CHECK_EQUAL(v.x, 0);
  BOOST_CHECK_EQUAL(v.y, 0);
}

BOOST_AUTO_TEST_CASE(comparisons)
{
  boost::optional<vec> x;
  vec y(5);

  BOOST_CHECK(fits(y, x));

  x = vec(4);

  BOOST_CHECK_LT(x, y);
  BOOST_CHECK_GT(y, x);
  BOOST_CHECK_LE(x, y);
  BOOST_CHECK_GE(y, x);
  BOOST_CHECK(!(x == y));
  BOOST_CHECK_NE(x, y);
}

BOOST_AUTO_TEST_CASE(vec2_comparisons)
{
  vec2 v1(1, 2);
  vec2 v2(2, 1);

  BOOST_CHECK_NE(v1, v2);
  BOOST_CHECK(!(v1 == v2));
}

BOOST_AUTO_TEST_SUITE_END()
