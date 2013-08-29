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

#ifndef FRENZY_HTMLENTITYSEARCHER_HPP
#define FRENZY_HTMLENTITYSEARCHER_HPP

#include "htmlentitydb.hpp"

namespace frenzy
{
  namespace parser
  {
    struct htmlentitysearcher
    {
      htmlentitysearcher(uchar first);

      void next(uchar u);
      bool need_more_input() const;
      
      // Value of NULL represents no match
      const htmlentity* result() const;

    private:
      const htmlentity* begin;
      const htmlentity* end;
      const htmlentity* last_match;
      size_t len;

      const htmlentity* next_begin(uchar next) const;
      const htmlentity* next_end(uchar next) const;
      int compare(const htmlentity* e, uchar next) const;
    };
  }
}

#endif
