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

#ifndef FRENZY_TOKEN_HPP
#define FRENZY_TOKEN_HPP

#include <map>
#include <boost/optional.hpp>

#include "util/unicode.hpp"

namespace frenzy
{
  namespace parser
  {
    /*
     * Only a single class for all token types exist. Token type is an
     * enum field.
     */
    enum token_type
    {
      TOKEN_DOCTYPE,
      TOKEN_START_TAG,
      TOKEN_END_TAG,
      TOKEN_COMMENT,
      TOKEN_CHARACTER,
      TOKEN_END_OF_FILE
    };

    struct token
    {
      token_type type;

      // For doctype
      // boost::optional used because state "missing" must be
      // different from empty string as data
      boost::optional<ustring> doctype_name;
      boost::optional<ustring> public_identifier;
      boost::optional<ustring> system_identifier;
      bool force_quirks;

      // For start/end tag
      ustring tagname;
      bool self_closing;
      std::map<ustring, ustring> attributes;
      // Filled by tokenizer
      std::pair<ustring, ustring> incompleteattr;

      // For comment
      ustring comment;

      // For character
      uchar character;

      // Named constructors
      static token make_doctype();
      static token make_doctype(uchar first);
      static token make_start_tag(uchar first);
      static token make_start_tag(ustring str);
      static token make_end_tag(uchar first);
      static token make_end_tag(ustring str);
      static token make_comment();
      static token make_character(uchar c);
      static token make_end_of_file();

      // Tokenizer calls this to finish incomplete tokens
      void finish();
      // Tokenizer calls this when creating attributes for tags
      void start_new_attribute(uchar first);

    private:
      // Constructors are private. Use the named constructors above.
      token(token_type type);
    };
  }
}

#endif
