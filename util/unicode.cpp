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

#include <stdexcept>

#include "unicode.hpp"

frenzy::urope::urope()
{
}

frenzy::urope::urope(const frenzy::ustring& str)
{
  destring d(str.begin(), str.end());
  data.push_back(d);
}

void
frenzy::urope::splice_back(frenzy::urope& other)
{
  data.splice(data.end(), other.data);
}

void
frenzy::urope::append(const frenzy::urope& other)
{
  std::copy(other.data.begin(), other.data.end(), std::back_inserter(data));
}

void
frenzy::urope::push_back(frenzy::uchar u)
{
  if (data.empty())
    data.push_back(destring());

  data.back().push_back(u);
}

void
frenzy::urope::push_front(frenzy::uchar u)
{
  if (data.empty())
    data.push_back(destring());

  data.front().push_front(u);
}

void
frenzy::urope::pop_front()
{
  data.front().pop_front();

  if (data.front().empty())
    data.pop_front();
}

void
frenzy::urope::pop_front(size_t howmany)
{
  for (size_t i = 0; i < howmany; ++i)
  {
    pop_front();
  }
}

void
frenzy::urope::clear()
{
  data.clear();
}

bool
frenzy::urope::empty() const
{
  return data.empty();
}

size_t
frenzy::urope::size() const
{
  size_t s = 0;
  for (data_t::const_iterator it = data.begin();
       it != data.end();
       ++it)
  {
    s += it->size();
  }

  return s;
}

bool
frenzy::urope::size_at_least(size_t howmany) const
{
  size_t s = 0;
  for (data_t::const_iterator it = data.begin();
       it != data.end();
       ++it)
  {
    s += it->size();

    if (s >= howmany)
      return true;
  }

  return s >= howmany;
}

frenzy::urope::const_iterator
frenzy::urope::begin() const
{
  if (data.empty())
    return end();

  return const_iterator(data.begin(), data.end());
}

frenzy::urope::const_iterator
frenzy::urope::end() const
{
  return const_iterator(data.end(), data.end());
}

frenzy::uchar&
frenzy::urope::operator[](size_t idx)
{
  for (data_t::iterator it = data.begin();
       it != data.end();
       ++it)
  {
    if (it->size() > idx)
      return (*it)[idx];

    idx -= it->size();
  }

  throw std::logic_error("Subscript out of bounds");
}

const frenzy::uchar&
frenzy::urope::operator[](size_t idx) const
{
  for (data_t::const_iterator it = data.begin();
       it != data.end();
       ++it)
  {
    if (it->size() > idx)
      return (*it)[idx];

    idx -= it->size();
  }

  throw std::logic_error("Subscript out of bounds");
}

bool
frenzy::urope::operator==(const frenzy::urope& other) const
{
  if (size() != other.size())
    return false;

  for (const_iterator tit = begin(), oit = other.begin();
       tit != end();
       ++tit, ++oit)
  {
    if (*tit != *oit)
      return false;
  }

  return true;
}

bool
frenzy::urope::operator!=(const frenzy::urope& other) const
{
  return !(*this == other);
}

frenzy::urope::const_iterator::const_iterator(std::list<std::deque<frenzy::uchar> >::const_iterator it,
					      std::list<std::deque<frenzy::uchar> >::const_iterator end)
  : iter(it)
  , end(end)
{
  if (iter != end)
    chiter = iter->begin();
}

frenzy::urope::const_iterator&
frenzy::urope::const_iterator::operator++()
{
  move_next();
  return *this;
}

frenzy::urope::const_iterator
frenzy::urope::const_iterator::operator++(int)
{
  const_iterator tmp = *this;
  move_next();
  return tmp;
}

frenzy::urope::const_iterator&
frenzy::urope::const_iterator::operator--()
{
  move_prev();
  return *this;
}

frenzy::urope::const_iterator
frenzy::urope::const_iterator::operator--(int)
{
  const_iterator tmp = *this;
  move_prev();
  return tmp;
}

frenzy::urope::const_iterator::reference
frenzy::urope::const_iterator::operator*() const
{
  return *chiter;
}

bool
frenzy::urope::const_iterator::operator==(frenzy::urope::const_iterator other) const
{
  if (iter == end)
    return iter == other.iter;

  return iter == other.iter && chiter == other.chiter;
}

bool
frenzy::urope::const_iterator::operator!=(frenzy::urope::const_iterator other) const
{
  return !(*this == other);
}

void
frenzy::urope::const_iterator::move_next()
{
  ++chiter;
  if (chiter == iter->end())
  {
    ++iter;
    if (iter != end)
      chiter = iter->begin();
  }
}

void
frenzy::urope::const_iterator::move_prev()
{
  if (iter == end || chiter == iter->begin())
  {
    --iter;
    chiter = iter->end();
  }
  --chiter;
}
