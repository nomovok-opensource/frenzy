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

#include "vector.hpp"

frenzy::vec::vec()
  : n(0)
{
}

frenzy::vec::vec(int n)
  : n(n)
{
}

frenzy::vec&
frenzy::vec::operator+=(frenzy::vec v)
{
  n += v.n;
  return *this;
}

frenzy::vec&
frenzy::vec::operator-=(frenzy::vec v)
{
  n -= v.n;
  return *this;
}

frenzy::vec2::vec2()
{
}

frenzy::vec2::vec2(frenzy::vec x, frenzy::vec y)
  : x(x)
  , y(y)
{
}

void
frenzy::vec2::grow(frenzy::vec2 v)
{
  if (x < v.x)
    x = v.x;
  if (y < v.y)
    y = v.y;
}

bool
frenzy::operator<(frenzy::vec one, frenzy::vec two)
{
  return one.n < two.n;
}

bool
frenzy::operator>(frenzy::vec one, frenzy::vec two)
{
  return one.n > two.n;
}

bool
frenzy::operator<=(frenzy::vec one, frenzy::vec two)
{
  return one.n <= two.n;
}

bool
frenzy::operator>=(frenzy::vec one, frenzy::vec two)
{
  return one.n >= two.n;
}

bool
frenzy::operator==(frenzy::vec one, frenzy::vec two)
{
  return one.n == two.n;
}

bool
frenzy::operator!=(frenzy::vec one, frenzy::vec two)
{
  return one.n != two.n;
}

frenzy::vec
frenzy::operator+(frenzy::vec one, frenzy::vec two)
{
  return one += two;
}

frenzy::vec
frenzy::operator-(frenzy::vec one, frenzy::vec two)
{
  return one -= two;
}

bool
frenzy::operator==(frenzy::vec2 one, frenzy::vec2 two)
{
  return one.x == two.x && one.y == two.y;
}

bool
frenzy::operator!=(frenzy::vec2 one, frenzy::vec2 two)
{
  return !(one == two);
}

frenzy::vec2
frenzy::operator+(frenzy::vec2 one, frenzy::vec2 two)
{
  return vec2(one.x + two.x, one.y + two.y);
}

frenzy::vec2
frenzy::operator-(frenzy::vec2 one, frenzy::vec2 two)
{
  return vec2(one.x - two.x, one.y - two.y);
}

boost::optional<frenzy::vec>&
frenzy::operator+=(boost::optional<frenzy::vec>& one, frenzy::vec two)
{
  if (one)
  {
    *one += two;
  }

  return one;
}

boost::optional<frenzy::vec>&
frenzy::operator-=(boost::optional<frenzy::vec>& one, frenzy::vec two)
{
  if (one)
  {
    *one -= two;
  }

  return one;
}

bool
frenzy::fits(frenzy::vec size, boost::optional<frenzy::vec> room)
{
  if (!room)
    return true;

  return size <= *room;
}
