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

#ifndef FRENZY_STRINGLIST_HPP
#define FRENZY_STRINGLIST_HPP

#include <set>

#include "unicode.hpp"

namespace frenzy
{
  /*
   * A class that contains a list of ustrings. The main purpose is the
   * function .contains(n), which returns true if the list contains
   * `n'.
   */

  struct stringlist
  {
    stringlist(ustring str)
    {
      strings.insert(str);
    }

    bool contains(ustring str) const
    {
      return strings.find(str) != strings.end();
    }

    stringlist& add(ustring str)
    {
      strings.insert(str);
      return *this;
    }
    
  private:
    std::set<ustring> strings;
  };

  inline stringlist operator+(stringlist lhs, ustring str)
  {
    return lhs.add(str);
  }
}

#endif
