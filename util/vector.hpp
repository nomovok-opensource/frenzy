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

#ifndef FRENZY_VECTOR_HPP
#define FRENZY_VECTOR_HPP

#include <boost/optional.hpp>

namespace frenzy
{
  // Represents a 1D vector in pixels
  struct vec
  {
    vec();
    vec(int n);

    vec& operator+=(vec v);
    vec& operator-=(vec v);

    int n;
  };

  // Represents a 2D vector in pixels
  struct vec2
  {
    vec2();
    vec2(vec x, vec y);

    // Increases x and y if they're smaller than the given vec2
    void grow(vec2 v);

    vec x;
    vec y;
  };

  // Represents a position and size in pixels
  struct box2
  {
  };

  // Should be self-explanatory
  bool operator<(vec one, vec two);
  bool operator>(vec one, vec two);
  bool operator<=(vec one, vec two);
  bool operator>=(vec one, vec two);
  bool operator==(vec one, vec two);
  bool operator!=(vec one, vec two);

  vec operator+(vec one, vec two);
  vec operator-(vec one, vec two);

  bool operator==(vec2 one, vec2 two);
  bool operator!=(vec2 one, vec2 two);

  vec2 operator+(vec2 one, vec2 two);
  vec2 operator-(vec2 one, vec2 two);

  boost::optional<vec>& operator+=(boost::optional<vec>& one, vec two);
  boost::optional<vec>& operator-=(boost::optional<vec>& one, vec two);

  // Returns true if `size' is smaller or equal to `room'. If room is
  // null, returns true.
  bool fits(vec size, boost::optional<vec> room);
}

#endif
