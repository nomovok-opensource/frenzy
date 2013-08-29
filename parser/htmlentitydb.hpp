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

#ifndef FRENZY_HTMLENTITYDB_HPP
#define FRENZY_HTMLENTITYDB_HPP

#include "util/unicode.hpp"

namespace frenzy
{
  namespace parser
  {
    struct htmlentity
    {
      const uchar* name;
      size_t len;

      uchar codepoint1;
      uchar codepoint2;
    };

    // The first entity in an array of entities that begin with the
    // given character.
    const htmlentity* htmlentity_begin(uchar u);
    // The last entity in an array of entities that begin
    // with the given character.
    const htmlentity* htmlentity_end(uchar u);
  }
}

#endif
