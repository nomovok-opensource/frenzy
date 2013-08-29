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

#include "htmlentitysearcher.hpp"

namespace
{
  using frenzy::parser::htmlentity;
  const htmlentity* split(const htmlentity* left, const htmlentity* right)
  {
    return &left[(right - left) / 2];
  }
}

frenzy::parser::htmlentitysearcher::htmlentitysearcher(frenzy::uchar first)
  : begin(htmlentity_begin(first))
  , end(htmlentity_end(first))
  , last_match(NULL)
  , len(1)
{
  if (!begin || !end)
  {
    begin = NULL;
    end = NULL;
  }

  if (begin && begin->len == len)
    last_match = begin;
}

void
frenzy::parser::htmlentitysearcher::next(uchar u)
{
  if (!begin || !end)
    return;

  begin = next_begin(u);
  end = next_end(u);

  if (begin == end && compare(begin, u) != 0)
  {
    begin = NULL;
    end = NULL;
    return;
  }

  ++len;

  if (begin->len == len)
    last_match = begin;
}

bool
frenzy::parser::htmlentitysearcher::need_more_input() const
{
  return begin != end || (begin && begin->len > len);
}

const frenzy::parser::htmlentity*
frenzy::parser::htmlentitysearcher::result() const
{
  return last_match;
}

const frenzy::parser::htmlentity*
frenzy::parser::htmlentitysearcher::next_begin(frenzy::uchar next) const
{
  const htmlentity* left = begin;
  const htmlentity* right = end;

  if (left == right)
    return left;

  {
    int k = compare(left, next);
    if (k == 0)
      return left;
    if (k > 0)
      return right;
  }

  while (left + 1 < right)
  {
    const htmlentity* half = split(left, right);
    int k = compare(half, next);
    if (k < 0)
      left = half;
    else
      right = half;
  }

  return right;
}

const frenzy::parser::htmlentity*
frenzy::parser::htmlentitysearcher::next_end(frenzy::uchar next) const
{
  const htmlentity* left = begin;
  const htmlentity* right = end;

  if (left == right)
    return left;

  {
    int k = compare(right, next);
    if (k == 0)
      return right;
    if (k < 0)
      return left;
  }

  while (left + 1 < right)
  {
    const htmlentity* half = split(left, right);
    int k = compare(half, next);
    if (k > 0)
      right = half;
    else
      left = half;
  }

  return left;
}

int
frenzy::parser::htmlentitysearcher::compare(const frenzy::parser::htmlentity* e, frenzy::uchar next) const
{
  if (e->len < len + 1)
    return -1;

  return e->name[len] - next;
}
