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

#ifndef FRENZY_GRAPHICS_HPP
#define FRENZY_GRAPHICS_HPP

// TODO: Possibly move to a directory called `graphics'.

#include <boost/shared_ptr.hpp>

#include "util/vector.hpp"
#include "util/unicode.hpp"

namespace frenzy
{
  namespace graphics
  {
    // A translation `node'.
    struct translation
    {
      virtual ~translation() = 0;

      // Position, relative to screen origin or another translation.
      virtual void position(vec2 pos) = 0;
      virtual vec2 position() = 0;
      // Set the translation this object is relative to
      virtual void relative_to(boost::shared_ptr<translation> t) = 0;
      virtual boost::shared_ptr<translation> relative_to() = 0;
    };

    // TODO: Font

    struct text
    {
      virtual ~text() = 0;

      // Sets the text data.
      virtual void data(ustring str) = 0;

      // Autowrap the text using the given width. Returns the size
      // (width and height) of the drawn text in pixels.
      // TODO: Line size constraints for the first line in particular.
      virtual vec2 autowrap(vec width) = 0;
    };

    struct factory
    {
      virtual ~factory() = 0;

      // The width of the page.
      // TODO: Signal when this changes
      virtual vec page_width() = 0;

      // Creation functions
      virtual boost::shared_ptr<translation> create_translation() = 0;
      virtual boost::shared_ptr<text> create_text(boost::shared_ptr<translation> position) = 0;
    };
  }
}

#endif
