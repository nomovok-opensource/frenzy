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

#ifndef FRENZY_HTMLPARSER_HPP
#define FRENZY_HTMLPARSER_HPP

#include "dom/pointers.hpp"
#include "chardecoder.hpp"
#include "input_preprocessor.hpp"
#include "htmltokenizer.hpp"
#include "treeconstructor.hpp"

namespace frenzy
{
  /*
   * htmlparser is the main interface for parsing HTML bytestreams
   * into DOM trees. A htmlparser is created with an empty Document
   * object, and bytes are passed to it with pass_bytes(). An empty
   * string of bytes passes an end-of-stream message, and is
   * equivalent to calling pass_eof().
  */

  // TODO: Interface for fragment parsing.

  struct htmlparser
  {
    htmlparser(dom::Documentp doc);

    void pass_bytes(bytestring str);
    void pass_eof();

    // Returns true if the parser has finished working.
    bool stopped() const;

  private:
    parser::utf8_decoder dec;
    parser::input_preprocessor proc;
    parser::htmltokenizer tok;
    parser::treeconstructor tree;
  };
}

#endif
