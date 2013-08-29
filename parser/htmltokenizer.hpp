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

#ifndef FRENZY_HTMLTOKENIZER_HPP
#define FRENZY_HTMLTOKENIZER_HPP

#include <string>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "util/unicode.hpp"
#include "token.hpp"

namespace frenzy
{
  /*
   * Parser stages are constructed once _per_bytestream_. A
   * constructed parser stage contains parser state that needs to be
   * refreshed.
   *
   * Note that Javascript document.open() also requires new parser
   * stage objects.
   *
   * Parser stage interface: pass_*() takes a sequence of input
   * datums. Any completed results are stored and can be fetched with
   * complete_*s(), unless a destination is attached using
   * attach_destination(). The parameter to attach_destination is a
   * function that takes a sequence of result datums, except for
   * htmltokenizer, for which the parameter is a single
   * token. Incomplete input is buffered. Passing an empty sequence
   * passes "end-of-file".
   *
   * The attach_destination() function is intended to be the primary
   * interface. complete_*s() cannot represent end-of-file.
   */

  namespace parser
  {
    /*
     * HTML5 8.2.4 "Tokenization"
     *
     * Third step of parsing is constructing tokens from the input
     * character stream.
     *
     * The tokenizer sends tokens to be handled immediately after
     * constructing (if a destination is attached, as it should be in
     * a complete pipeline).  This is because the tree construction
     * stage can affect the state of tokenization and can insert
     * additional characters into the stream. For example, the
     * 'script' element will result in scripts executing which can
     * modify the input stream or the tree being constructed.
     *
     * Note that on end-of-file, htmltokenizer will not pass an empty
     * sequence of tokens, like the stage before, because there is an
     * end-of-file token for that purpose.
     */
    struct htmltokenizer : private boost::noncopyable
    {
      htmltokenizer();

      void pass_characters(const urope& input);

      typedef std::vector<token> tokensequence_t;

      tokensequence_t complete_tokens();
      void attach_destination(boost::function<void (const token&)> dest);

    private:
      tokensequence_t completed_items;
      boost::function<void (const token&)> destination;

      void emit(const token& t);
      // Emits the 'incomplete' member below
      void emit();

      urope buffer;
      uchar current_input;
      uchar next_input() const;
      uchar peek_input(size_t index) const;
      uchar consume();
      void consume(size_t howmany);
      void rewind(uchar u);
      bool can_consume() const;
      bool can_consume(size_t howmany) const;

      // Peek at the buffer, match against an ASCII string
      // Must check can_consume first
      bool peek_match(std::string str) const;
      // Peek at the buffer, match against an ASCII string case insensitive
      // Must check can_consume first
      // Must give the input string as upper case, and contain upper case characters only
      // (This is only really meant to test for DOCTYPE, PUBLIC and SYSTEM)
      bool peek_match_caseless(std::string str) const;
      // Match the temporary_buffer against an ASCII string.
      // No size checks a priori necessary, mismatching size returns false.
      bool temporary_match(std::string str) const;
      // Peek at the buffer, at most 'howmany' characters, returns
      // true if eof can be found
      bool peek_eof(size_t howmany) const;

      token incomplete;
      uchar additional; // For character reference parsing. If U+0000, considered unset.
      urope temporary_buffer;
      ustring last_start_tag_name;
      
      // Tries to consume a character. Returns false if there's not enough input buffered.
      bool try_consume_character_reference(urope& chars, bool inattribute);
      // Try to consume a hex number string. Returns false if there's not enough input buffered.
      // Must have checked that the buffer contains a valid hex number string.
      bool try_consume_hex_number(uchar& u);
      // Tries to consume a dec number string. Returns false if there's not enough input buffered.
      // Must have checked that the buffer contains a valid dec number string.
      bool try_consume_number(uchar& u);
      // Filters a character according to 8.2.4.69
      static uchar filter_numeric_reference(uchar in);

    public:
      enum tokenizestate
      {
        STATE_DATA, // 8.2.4.1
        STATE_CHARACTER_REFERENCE_IN_DATA, // 8.2.4.2
        STATE_RCDATA, // 8.2.4.3
        STATE_CHARACTER_REFERENCE_IN_RCDATA, // 8.2.4.4
        STATE_RAWTEXT, // 8.2.4.5
        STATE_SCRIPT_DATA, // 8.2.4.6
        STATE_PLAINTEXT, // 8.2.4.7
        STATE_TAG_OPEN, // 8.2.4.8
        STATE_END_TAG_OPEN, // 8.2.4.9
        STATE_TAG_NAME, // 8.2.4.10
        STATE_RCDATA_LESS_THAN_SIGN, // 8.2.4.11
        STATE_RCDATA_END_TAG_OPEN, // 8.2.4.12
        STATE_RCDATA_END_TAG_NAME, // 8.2.4.13
        STATE_RAWTEXT_LESS_THAN_SIGN, // 8.2.4.14
        STATE_RAWTEXT_END_TAG_OPEN, // 8.2.4.15
        STATE_RAWTEXT_END_TAG_NAME, // 8.2.4.16
        STATE_SCRIPT_DATA_LESS_THAN_SIGN, // 8.2.4.17
        STATE_SCRIPT_DATA_END_TAG_OPEN, // 8.2.4.18
        STATE_SCRIPT_DATA_END_TAG_NAME, // 8.2.4.19
        STATE_SCRIPT_DATA_ESCAPE_START, // 8.2.4.20
        STATE_SCRIPT_DATA_ESCAPE_START_DASH, // 8.2.4.21
        STATE_SCRIPT_DATA_ESCAPED, // 8.2.4.22
        STATE_SCRIPT_DATA_ESCAPED_DASH, // 8.2.4.23
        STATE_SCRIPT_DATA_ESCAPED_DASH_DASH, // 8.2.4.24
        STATE_SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN, // 8.2.4.25
        STATE_SCRIPT_DATA_ESCAPED_END_TAG_OPEN, // 8.2.4.26
        STATE_SCRIPT_DATA_ESCAPED_END_TAG_NAME, // 8.2.4.27
        STATE_SCRIPT_DATA_DOUBLE_ESCAPE_START, // 8.2.4.28
        STATE_SCRIPT_DATA_DOUBLE_ESCAPED, // 8.2.4.29
        STATE_SCRIPT_DATA_DOUBLE_ESCAPED_DASH, // 8.2.4.30
        STATE_SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH, // 8.2.4.31
        STATE_SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN, // 8.2.4.32
        STATE_SCRIPT_DATA_DOUBLE_ESCAPE_END, // 8.2.4.33
        STATE_BEFORE_ATTRIBUTE_NAME, // 8.2.4.34
        STATE_ATTRIBUTE_NAME, // 8.2.4.35
        STATE_AFTER_ATTRIBUTE_NAME, // 8.2.4.36
        STATE_BEFORE_ATTRIBUTE_VALUE, // 8.2.4.37
        STATE_ATTRIBUTE_VALUE_DOUBLE_QUOTED, // 8.2.4.38
        STATE_ATTRIBUTE_VALUE_SINGLE_QUOTED, // 8.2.4.39
        STATE_ATTRIBUTE_VALUE_UNQUOTED, // 8.2.4.40
        STATE_CHARACTER_REFERENCE_IN_ATTRIBUTE_VALUE, // 8.2.4.41
        STATE_AFTER_ATTRIBUTE_VALUE_QUOTED, // 8.2.4.42
        STATE_SELF_CLOSING_START_TAG, // 8.2.4.43
        STATE_BOGUS_COMMENT, // 8.2.4.44
        STATE_MARKUP_DECLARATION_OPEN, // 8.2.4.45
        STATE_COMMENT_START, // 8.2.4.46
        STATE_COMMENT_START_DASH, // 8.2.4.47
        STATE_COMMENT, // 8.2.4.48
        STATE_COMMENT_END_DASH, // 8.2.4.49
        STATE_COMMENT_END, // 8.2.4.50
        STATE_COMMENT_END_BANG, // 8.2.4.51
        STATE_DOCTYPE, // 8.2.4.52
        STATE_BEFORE_DOCTYPE_NAME, // 8.2.4.53
        STATE_DOCTYPE_NAME, // 8.2.4.54
        STATE_AFTER_DOCTYPE_NAME, // 8.2.4.55
        STATE_AFTER_DOCTYPE_PUBLIC_KEYWORD, // 8.2.4.56
        STATE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER, // 8.2.4.57
        STATE_DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED, // 8.2.4.58
        STATE_DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED, // 8.2.4.59
        STATE_AFTER_DOCTYPE_PUBLIC_IDENTIFIER, // 8.2.4.60
        STATE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS, // 8.2.4.61
        STATE_AFTER_DOCTYPE_SYSTEM_KEYWORD, // 8.2.4.62
        STATE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER, // 8.2.4.63
        STATE_DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED, // 8.2.4.64
        STATE_DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED, // 8.2.4.65
        STATE_AFTER_DOCTYPE_SYSTEM_IDENTIFIER, // 8.2.4.66
        STATE_BOGUS_DOCTYPE, // 8.2.4.67
        STATE_CDATA_SECTION // 8.2.4.68
      };

      // Used by the tree constructor when appropriate
      void change_state(tokenizestate state);

    private:
      tokenizestate state, prevstate; // prevstate is used by character reference parser

      // Call the next appropriate state operation. Return value of false
      // means more input is needed to proceed.
      bool call_state();

      // State operation functions.
      bool state_data();
      bool state_character_reference_in_data();
      bool state_rcdata();
      bool state_character_reference_in_rcdata();
      bool state_rawtext();
      bool state_script_data();
      bool state_plaintext();
      bool state_tag_open();
      bool state_end_tag_open();
      bool state_tag_name();
      bool state_rcdata_less_than_sign();
      bool state_rcdata_end_tag_open();
      bool state_rcdata_end_tag_name();
      bool state_rawtext_less_than_sign();
      bool state_rawtext_end_tag_open();
      bool state_rawtext_end_tag_name();
      bool state_script_data_less_than_sign();
      bool state_script_data_end_tag_open();
      bool state_script_data_end_tag_name();
      bool state_script_data_escape_start();
      bool state_script_data_escape_start_dash();
      bool state_script_data_escaped();
      bool state_script_data_escaped_dash();
      bool state_script_data_escaped_dash_dash();
      bool state_script_data_escaped_less_than_sign();
      bool state_script_data_escaped_end_tag_open();
      bool state_script_data_escaped_end_tag_name();
      bool state_script_data_double_escape_start();
      bool state_script_data_double_escaped();
      bool state_script_data_double_escaped_dash();
      bool state_script_data_double_escaped_dash_dash();
      bool state_script_data_double_escaped_less_than_sign();
      bool state_script_data_double_escape_end();
      bool state_before_attribute_name();
      bool state_attribute_name();
      bool state_after_attribute_name();
      bool state_before_attribute_value();
      bool state_attribute_value_double_quoted();
      bool state_attribute_value_single_quoted();
      bool state_attribute_value_unquoted();
      bool state_character_reference_in_attribute_value();
      bool state_after_attribute_value_quoted();
      bool state_self_closing_start_tag();
      bool state_bogus_comment();
      bool state_markup_declaration_open();
      bool state_comment_start();
      bool state_comment_start_dash();
      bool state_comment();
      bool state_comment_end_dash();
      bool state_comment_end();
      bool state_comment_end_bang();
      bool state_doctype();
      bool state_before_doctype_name();
      bool state_doctype_name();
      bool state_after_doctype_name();
      bool state_after_doctype_public_keyword();
      bool state_before_doctype_public_identifier();
      bool state_doctype_public_identifier_double_quoted();
      bool state_doctype_public_identifier_single_quoted();
      bool state_after_doctype_public_identifier();
      bool state_between_doctype_public_and_system_identifiers();
      bool state_after_doctype_system_keyword();
      bool state_before_doctype_system_identifier();
      bool state_doctype_system_identifier_double_quoted();
      bool state_doctype_system_identifier_single_quoted();
      bool state_after_doctype_system_identifier();
      bool state_bogus_doctype();
      bool state_cdata_section();
    };
  }
}

#endif
