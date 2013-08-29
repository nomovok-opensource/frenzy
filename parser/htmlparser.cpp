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

#include <boost/bind.hpp>

#include "htmlparser.hpp"

frenzy::htmlparser::htmlparser(frenzy::dom::Documentp doc)
  : tree(doc)
{
  dec.attach_destination(boost::bind(&parser::input_preprocessor::pass_characters, &proc, _1));
  proc.attach_destination(boost::bind(&parser::htmltokenizer::pass_characters, &tok, _1));
  tree.couple_tokenizer(&tok);
}

void
frenzy::htmlparser::pass_bytes(frenzy::bytestring str)
{
  dec.pass_bytes(str);
}

void
frenzy::htmlparser::pass_eof()
{
  dec.pass_bytes(bytestring());
}

bool
frenzy::htmlparser::stopped() const
{
  return tree.stopped();
}
