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

#ifndef FRENZY_UNICODE_HPP
#define FRENZY_UNICODE_HPP

#include <list>
#include <vector>
#include <deque>
#include <string>
#include <boost/cstdint.hpp>

/*
 * Unicode characters and strings and their handling.
 */
namespace frenzy
{
  // A single unicode code point
  typedef uint32_t uchar;

  // A unicode character string
  struct ustring
  {
    ustring()
    {}

    // All characters copied to the ustring as is, as code point values
    ustring(const char* str)
    {
      while (uchar x = *str++)
	data.push_back(x);
    }
    ustring(std::string str)
      : data(str.begin(), str.end())
    {}

    // Single uchar
    ustring(uchar u)
    : data(1, u)
    {}

    void push_back(uchar u)
    {
      data.push_back(u);
    }
    void pop_back()
    {
      data.pop_back();
    }
    void append(const ustring& other)
    {
      data.insert(data.end(), other.data.begin(), other.data.end());
    }

    void clear()
    {
      data.clear();
    }

    bool empty() const
    {
      return data.empty();
    }
    size_t size() const
    {
      return data.size();
    }

    uchar& operator[](size_t idx)
    {
      return data[idx];
    }

    const uchar& operator[](size_t idx) const
    {
      return data[idx];
    }

    // Returns a substring. Offset and count are clamped to the size
    // of `this'.
    ustring substr(size_t offset, size_t count) const
    {
      if (offset > data.size())
	offset = data.size();

      if (count > data.size() - offset)
	count = data.size() - offset;

      std::vector<uchar> newdata(data.begin() + offset, data.begin() + offset + count);
      return ustring(newdata);
    }

    bool equal(const ustring& other) const
    {
      return data == other.data;
    }

    bool operator<(const ustring& other) const
    {
      return data < other.data;
    }

    typedef std::vector<uchar>::iterator iterator;
    typedef std::vector<uchar>::const_iterator const_iterator;

    iterator begin()
    {
      return data.begin();
    }

    const_iterator begin() const
    {
      return data.begin();
    }

    iterator end()
    {
      return data.end();
    }

    const_iterator end() const
    {
      return data.end();
    }

  private:
    std::vector<uchar> data;

    ustring(const std::vector<uchar> data)
      : data(data)
    {}
  };

  inline bool operator==(const ustring& one, const ustring& two)
  {
    return one.equal(two);
  }

  inline bool operator!=(const ustring& one, const ustring& two)
  {
    return !one.equal(two);
  }

  inline ustring ascii_toupper(ustring str)
  {
    ustring ret;
    
    for (ustring::const_iterator it = str.begin();
	 it != str.end();
	 ++it)
    {
      uchar x = *it;
      
      if (x >= 0x61 && x <= 0x7A)
	x -= 0x20;
      
      ret.append(x);
    }
    
    return ret;
  }
  
  // urope is similar to ustring, but is more gentle towards
  // modification.  Especially it supports splice_back to move the
  // data from another urope entirely, without copying. Only the
  // parsers should be using this type, ustring is for general purpose
  // strings.
  struct urope
  {
    typedef uchar value_type;

    urope();
    urope(const ustring& str);

    // Moves the data from 'other' to the end of this
    void splice_back(urope& other);
    // Copies the data from 'other' to the end of this
    void append(const urope& other);

    void push_back(uchar u);
    void push_front(uchar u);
    void pop_front();
    void pop_front(size_t howmany);
    void clear();

    bool empty() const;
    size_t size() const;

    // Like size() but aborts as soon as possible
    bool size_at_least(size_t howmany) const;

    struct const_iterator;

    const_iterator begin() const;
    const_iterator end() const;

    uchar& operator[](size_t idx);
    const uchar& operator[](size_t idx) const;

    bool operator==(const urope& other) const;
    bool operator!=(const urope& other) const;

  private:
    typedef std::deque<uchar> destring;
    typedef std::list<destring> data_t;
    data_t data;
  };

  struct urope::const_iterator
  {
    typedef ptrdiff_t difference_type;
    typedef uchar value_type;
    typedef const uchar* pointer;
    typedef const uchar& reference;
    typedef std::bidirectional_iterator_tag iterator_category;

    const_iterator(std::list<std::deque<uchar> >::const_iterator it,
		   std::list<std::deque<uchar> >::const_iterator end);

    urope::const_iterator& operator++();
    urope::const_iterator operator++(int);
    urope::const_iterator& operator--();
    urope::const_iterator operator--(int);

    reference operator*() const;

    bool operator==(urope::const_iterator other) const;
    bool operator!=(urope::const_iterator other) const;

  private:
    void move_next();
    void move_prev();

    std::list<std::deque<uchar> >::const_iterator iter;
    std::list<std::deque<uchar> >::const_iterator end;
    std::deque<uchar>::const_iterator chiter;
  };
}

#endif
