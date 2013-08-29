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

#include "token.hpp"

frenzy::parser::token
frenzy::parser::token::make_doctype()
{
  token t(TOKEN_DOCTYPE);
  
  return t;
}

frenzy::parser::token
frenzy::parser::token::make_doctype(frenzy::uchar first)
{
  token t(TOKEN_DOCTYPE);
  t.doctype_name = ustring();
  t.doctype_name->push_back(first);
  
  return t;
}

frenzy::parser::token
frenzy::parser::token::make_start_tag(frenzy::uchar first)
{
  token t(TOKEN_START_TAG);
  t.tagname.push_back(first);

  return t;
}

frenzy::parser::token
frenzy::parser::token::make_start_tag(frenzy::ustring str)
{
  token t(TOKEN_START_TAG);
  t.tagname = str;

  return t;
}

frenzy::parser::token
frenzy::parser::token::make_end_tag(frenzy::uchar first)
{
  token t(TOKEN_END_TAG);
  t.tagname.push_back(first);
  
  return t;
}

frenzy::parser::token
frenzy::parser::token::make_end_tag(frenzy::ustring str)
{
  token t(TOKEN_END_TAG);
  t.tagname = str;
  
  return t;
}

frenzy::parser::token
frenzy::parser::token::make_comment()
{
  token t(TOKEN_COMMENT);

  return t;
}

frenzy::parser::token
frenzy::parser::token::make_character(frenzy::uchar c)
{
  token t(TOKEN_CHARACTER);
  t.character = c;

  return t;
}

frenzy::parser::token
frenzy::parser::token::make_end_of_file()
{
  token t(TOKEN_END_OF_FILE);
  
  return t;
}

void
frenzy::parser::token::finish()
{
  if (type == TOKEN_START_TAG && !incompleteattr.first.empty())
  {
    attributes.insert(incompleteattr);
    incompleteattr = std::make_pair(ustring(), ustring());
  }
}

void
frenzy::parser::token::start_new_attribute(frenzy::uchar first)
{
  if (!incompleteattr.first.empty())
  {
    attributes.insert(incompleteattr);
    incompleteattr = std::make_pair(ustring(), ustring());
  }

  incompleteattr.first.push_back(first);    
}

frenzy::parser::token::token(frenzy::parser::token_type type)
  : type(type)
  , force_quirks(false)
  , self_closing(false)
{
}
