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

#include <cassert>

#include "htmltokenizer.hpp"
#include "htmlentitysearcher.hpp"

namespace
{
  /*
   * Internally we represent end-of-file character with
   * U+FFFFFFFF. The UTF8 decoder doesn't ever emit that, so it should
   * be okay.
   */

  const frenzy::uchar eof = 0xFFFFFFFF;

  bool is_ucase_ascii(frenzy::uchar u)
  {
    return u >= 0x41 && u <= 0x5A;
  }

  bool is_lcase_ascii(frenzy::uchar u)
  {
    return u >= 0x61 && u <= 0x7A;
  }

  bool is_ascii_digit(frenzy::uchar u)
  {
    return u >= 0x30 && u <= 0x39;
  }

  bool is_ascii_hex_digit(frenzy::uchar u)
  {
    return is_ascii_digit(u) || (u >= 0x41 && u <= 0x46) || (u >= 0x61 && u <= 0x66);
  }
}

frenzy::parser::htmltokenizer::htmltokenizer()
  : incomplete(token::make_end_of_file()) // value unused before replacing
  , state(STATE_DATA)
{

}

void
frenzy::parser::htmltokenizer::pass_characters(const frenzy::urope& input)
{
  buffer.append(input);
  if (input.empty())
    buffer.push_back(eof);

  while (call_state())
  {
    // Nothing
  }
}

frenzy::parser::htmltokenizer::tokensequence_t
frenzy::parser::htmltokenizer::complete_tokens()
{
  tokensequence_t ret;
  ret.swap(completed_items);
  return ret;
}

void
frenzy::parser::htmltokenizer::attach_destination(boost::function<void (const token&)> dest)
{
  destination = dest;

  for (tokensequence_t::const_iterator it = completed_items.begin();
       it != completed_items.end();
       ++it)
  {
    destination(*it);
  }

  completed_items.clear();
}

void
frenzy::parser::htmltokenizer::emit(const frenzy::parser::token& t)
{
  if (destination)
  {
    destination(t);
  }
  else
  {
    completed_items.push_back(t);
  }
}

void
frenzy::parser::htmltokenizer::emit()
{
  incomplete.finish();

  if (incomplete.type == TOKEN_START_TAG)
    last_start_tag_name = incomplete.tagname;

  if (destination)
  {
    destination(incomplete);
  }
  else
  {
    completed_items.push_back(incomplete);
  }
}

frenzy::uchar
frenzy::parser::htmltokenizer::next_input() const
{
  return buffer[0];
}

frenzy::uchar
frenzy::parser::htmltokenizer::peek_input(size_t index) const
{
  return buffer[index];
}

frenzy::uchar
frenzy::parser::htmltokenizer::consume()
{
  current_input = buffer[0];
  buffer.pop_front();
  return current_input;
}

void
frenzy::parser::htmltokenizer::consume(size_t howmany)
{
  buffer.pop_front(howmany);
}

void
frenzy::parser::htmltokenizer::rewind(frenzy::uchar u)
{
  buffer.push_front(u);
}

bool
frenzy::parser::htmltokenizer::can_consume() const
{
  return !buffer.empty();
}

bool
frenzy::parser::htmltokenizer::can_consume(size_t howmany) const
{
  return buffer.size_at_least(howmany);
}

bool
frenzy::parser::htmltokenizer::peek_match(std::string str) const
{
  for (size_t i = 0; i < str.size(); ++i)
  {
    uchar u = str[i];
    if (buffer[i] != u)
      return false;
  }
  
  return true;
}

bool
frenzy::parser::htmltokenizer::peek_match_caseless(std::string str) const
{
  for (size_t i = 0; i < str.size(); ++i)
  {
    uchar u = str[i];
    if (buffer[i] != u && buffer[i] != u + 0x20)
      return false;
  }
  
  return true;
}

bool
frenzy::parser::htmltokenizer::temporary_match(std::string str) const
{
  if (temporary_buffer.size() != str.size())
    return false;

  for (size_t i = 0; i < str.size(); ++i)
  {
    uchar u = str[i];
    if (buffer[i] != u)
      return false;
  }
  
  return true;
}

bool
frenzy::parser::htmltokenizer::peek_eof(size_t howmany) const
{
  if (buffer.size() < howmany)
    howmany = buffer.size();

  for (size_t i = 0; i < howmany; ++i)
  {
    if (buffer[i] == eof)
      return true;
  }

  return false;
}

bool
frenzy::parser::htmltokenizer::try_consume_character_reference(frenzy::urope& chars, bool inattribute)
{
  if (!can_consume())
    return false;

  uchar next = consume();

  if (additional != 0x00 && next == additional)
  {
    rewind(next);
    return true;
  }

  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
  case 0x3C: // <
  case 0x26: // Ampersand
  case eof:
    rewind(next);
    return true;
  case 0x23: // #
    if (!can_consume())
    {
      rewind(next);
      return false;
    }
    if (next_input() == 0x78 || next_input() == 0x58) // x or X
    {
      uchar x = consume();

      if (!can_consume())
      {
	rewind(x);
	rewind(next);
	return false;
      }

      if (is_ascii_hex_digit(next_input()))
      {
        uchar u = 0x00;
        bool success = try_consume_hex_number(u);
        if (!success)
        {
          rewind(x);
          rewind(next);
          return false;
        }

        chars.push_back(u);
        return true;
      }
      else
      {
        rewind(x);
        rewind(next);
        // TODO: Should produce a parse error
        return true;
      }
    }
    else
    {
      if (is_ascii_digit(next_input()))
      {
        uchar u = 0x00;
        bool success = try_consume_number(u);
        if (!success)
        {
          rewind(next);
          return false;
        }

        chars.push_back(u);
        return true;
      }
      else
      {
        rewind(next);
        // TODO: Should produce a parse error
        return true;
      }
    }
  default:
    {
      htmlentitysearcher s(next);
      size_t idx = 0;
      while (s.need_more_input())
      {
        if (!can_consume(idx + 1))
        {
          // More input needed here
          rewind(next);
          return false;
        }

        s.next(peek_input(idx++));
      }

      rewind(next);

      const htmlentity* res = s.result();
      if (!res)
      {
        // TODO: Should produce a parse error if the buffer contains an ampersand, a sequence
        // of ascii digits or letters, and a semicolon.
        return true;
      }

      if (inattribute && res->name[res->len - 1] != 0x3B) // Semicolon
      {
        if (!can_consume(res->len + 1))
          return false;

        uchar after = peek_input(res->len);
        // Match for U+003D (=) or ascii digits or ascii letters
        if (after != 0x3D && !is_lcase_ascii(after) && !is_ucase_ascii(after))
        {
          // Nothing is to be consumed or returned in this case
          return true;
        }
      }

      // TODO: Should produce a parse error if the last character in
      // the match is not a semicolon.

      chars.push_back(res->codepoint1);
      if (res->codepoint2)
        chars.push_back(res->codepoint2);
      
      consume(res->len);
      return true;
    }
  }
}

bool
frenzy::parser::htmltokenizer::try_consume_hex_number(uchar& u)
{
  uchar res = 0x00;
  size_t idx = 0;

  while (true)
  {
    if (!can_consume(idx + 1))
      return false;
    uchar next = peek_input(idx++);
    uchar num = 0;
    if (next >= 0x61 && next <= 0x66)
      num = 10 + next - 0x61;
    else if (next >= 0x41 && next <= 0x46)
      num = 10 + next - 0x41;
    else if (next >= 0x30 && next <= 0x39)
      num = next - 0x30;
    else
    {
      // Stop here
      if (next == 0x3B) // ;
        ++idx;
      // else { TODO: Should produce a parse error }

      u = filter_numeric_reference(res);
      consume(idx - 1);
      return true;
    }

    res *= 16;
    res += num;
  }
}

bool
frenzy::parser::htmltokenizer::try_consume_number(uchar& u)
{
  uchar res = 0x00;
  size_t idx = 0;

  while (true)
  {
    if (!can_consume(idx + 1))
      return false;
    uchar next = peek_input(idx++);
    uchar num = 0;
    if (next >= 0x30 && next <= 0x39)
      num = next - 0x30;
    else
    {
      // Stop here
      if (next == 0x3B) // ;
        ++idx;
      // else { TODO: Should produce a parse error }

      u = filter_numeric_reference(res);
      consume(idx - 1);
      return true;
    }

    res *= 10;
    res += num;
  }
}

namespace
{
  frenzy::uchar filtering_table[] = {
    0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, // 80-87
    0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x008D, 0x017D, 0x008F, // 88-8F
    0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, // 90-97
    0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178  // 98-9F
  };
}

frenzy::uchar
frenzy::parser::htmltokenizer::filter_numeric_reference(uchar in)
{
  // TODO: All these values checked should produce a parse error
  if (in == 0x00)
    return 0xFFFD;
  if (in == 0x0D)
    return 0x0D;
  if (in >= 0x80 && in <= 0x9F)
    return filtering_table[in - 0x80];
  // All the above should produce a parse error

  return in;
}

void
frenzy::parser::htmltokenizer::change_state(frenzy::parser::htmltokenizer::tokenizestate s)
{
  state = s;
}

bool
frenzy::parser::htmltokenizer::call_state()
{
  switch (state)
  {
  case STATE_DATA:
    return state_data();
  case STATE_CHARACTER_REFERENCE_IN_DATA:
    return state_character_reference_in_data();
  case STATE_RCDATA:
    return state_rcdata();
  case STATE_CHARACTER_REFERENCE_IN_RCDATA:
    return state_character_reference_in_rcdata();
  case STATE_RAWTEXT:
    return state_rawtext();
  case STATE_SCRIPT_DATA:
    return state_script_data();
  case STATE_PLAINTEXT:
    return state_plaintext();
  case STATE_TAG_OPEN:
    return state_tag_open();
  case STATE_END_TAG_OPEN:
    return state_end_tag_open();
  case STATE_TAG_NAME:
    return state_tag_name();
  case STATE_RCDATA_LESS_THAN_SIGN:
    return state_rcdata_less_than_sign();
  case STATE_RCDATA_END_TAG_OPEN:
    return state_rcdata_end_tag_open();
  case STATE_RCDATA_END_TAG_NAME:
    return state_rcdata_end_tag_name();
  case STATE_RAWTEXT_LESS_THAN_SIGN:
    return state_rawtext_less_than_sign();
  case STATE_RAWTEXT_END_TAG_OPEN:
    return state_rawtext_end_tag_open();
  case STATE_RAWTEXT_END_TAG_NAME:
    return state_rawtext_end_tag_name();
  case STATE_SCRIPT_DATA_LESS_THAN_SIGN:
    return state_script_data_less_than_sign();
  case STATE_SCRIPT_DATA_END_TAG_OPEN:
    return state_script_data_end_tag_open();
  case STATE_SCRIPT_DATA_END_TAG_NAME:
    return state_script_data_end_tag_name();
  case STATE_SCRIPT_DATA_ESCAPE_START:
    return state_script_data_escape_start();
  case STATE_SCRIPT_DATA_ESCAPE_START_DASH:
    return state_script_data_escape_start_dash();
  case STATE_SCRIPT_DATA_ESCAPED:
    return state_script_data_escaped();
  case STATE_SCRIPT_DATA_ESCAPED_DASH:
    return state_script_data_escaped_dash();
  case STATE_SCRIPT_DATA_ESCAPED_DASH_DASH:
    return state_script_data_escaped_dash_dash();
  case STATE_SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN:
    return state_script_data_escaped_less_than_sign();
  case STATE_SCRIPT_DATA_ESCAPED_END_TAG_OPEN:
    return state_script_data_escaped_end_tag_open();
  case STATE_SCRIPT_DATA_ESCAPED_END_TAG_NAME:
    return state_script_data_escaped_end_tag_name();
  case STATE_SCRIPT_DATA_DOUBLE_ESCAPE_START:
    return state_script_data_double_escape_start();
  case STATE_SCRIPT_DATA_DOUBLE_ESCAPED:
    return state_script_data_double_escaped();
  case STATE_SCRIPT_DATA_DOUBLE_ESCAPED_DASH:
    return state_script_data_double_escaped_dash();
  case STATE_SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH:
    return state_script_data_double_escaped_dash_dash();
  case STATE_SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN:
    return state_script_data_double_escaped_less_than_sign();
  case STATE_SCRIPT_DATA_DOUBLE_ESCAPE_END:
    return state_script_data_double_escape_end();
  case STATE_BEFORE_ATTRIBUTE_NAME:
    return state_before_attribute_name();
  case STATE_ATTRIBUTE_NAME:
    return state_attribute_name();
  case STATE_AFTER_ATTRIBUTE_NAME:
    return state_after_attribute_name();
  case STATE_BEFORE_ATTRIBUTE_VALUE:
    return state_before_attribute_value();
  case STATE_ATTRIBUTE_VALUE_DOUBLE_QUOTED:
    return state_attribute_value_double_quoted();
  case STATE_ATTRIBUTE_VALUE_SINGLE_QUOTED:
    return state_attribute_value_single_quoted();
  case STATE_ATTRIBUTE_VALUE_UNQUOTED:
    return state_attribute_value_unquoted();
  case STATE_CHARACTER_REFERENCE_IN_ATTRIBUTE_VALUE:
    return state_character_reference_in_attribute_value();
  case STATE_AFTER_ATTRIBUTE_VALUE_QUOTED:
    return state_after_attribute_value_quoted();
  case STATE_SELF_CLOSING_START_TAG:
    return state_self_closing_start_tag();
  case STATE_BOGUS_COMMENT:
    return state_bogus_comment();
  case STATE_MARKUP_DECLARATION_OPEN:
    return state_markup_declaration_open();
  case STATE_COMMENT_START:
    return state_comment_start();
  case STATE_COMMENT_START_DASH:
    return state_comment_start_dash();
  case STATE_COMMENT:
    return state_comment();
  case STATE_COMMENT_END_DASH:
    return state_comment_end_dash();
  case STATE_COMMENT_END:
    return state_comment_end();
  case STATE_COMMENT_END_BANG:
    return state_comment_end_bang();
  case STATE_DOCTYPE:
    return state_doctype();
  case STATE_BEFORE_DOCTYPE_NAME:
    return state_before_doctype_name();
  case STATE_DOCTYPE_NAME:
    return state_doctype_name();
  case STATE_AFTER_DOCTYPE_NAME:
    return state_after_doctype_name();
  case STATE_AFTER_DOCTYPE_PUBLIC_KEYWORD:
    return state_after_doctype_public_keyword();
  case STATE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER:
    return state_before_doctype_public_identifier();
  case STATE_DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED:
    return state_doctype_public_identifier_double_quoted();
  case STATE_DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED:
    return state_doctype_public_identifier_single_quoted();
  case STATE_AFTER_DOCTYPE_PUBLIC_IDENTIFIER:
    return state_after_doctype_public_identifier();
  case STATE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS:
    return state_between_doctype_public_and_system_identifiers();
  case STATE_AFTER_DOCTYPE_SYSTEM_KEYWORD:
    return state_after_doctype_system_keyword();
  case STATE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER:
    return state_before_doctype_system_identifier();
  case STATE_DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED:
    return state_doctype_system_identifier_double_quoted();
  case STATE_DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED:
    return state_doctype_system_identifier_single_quoted();
  case STATE_AFTER_DOCTYPE_SYSTEM_IDENTIFIER:
    return state_after_doctype_system_identifier();
  case STATE_BOGUS_DOCTYPE:
    return state_bogus_doctype();
  case STATE_CDATA_SECTION:
    return state_cdata_section();
  default:
    // Should not be reached
    throw std::logic_error("HTML Tokenizer state unknown");
  }
  
  return false;
}

bool
frenzy::parser::htmltokenizer::state_data()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  
  switch (next)
  {
  case 0x26: // Ampersand
    state = STATE_CHARACTER_REFERENCE_IN_DATA;
    return true;
  case 0x3C: // <
    state = STATE_TAG_OPEN;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    emit(token::make_character(next));
    return true;
  case eof:
    emit(token::make_end_of_file());
    return true;
  default:
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_character_reference_in_data()
{
  additional = 0x00;
  prevstate = STATE_DATA;
  urope chars;
  bool success = try_consume_character_reference(chars, false);
  if (!success)
    return false;

  if (chars.empty())
    emit(token::make_character(0x26));

  for (urope::const_iterator it = chars.begin();
       it != chars.end();
       ++it)
  {
    emit(token::make_character(*it));
  }

  state = STATE_DATA;
  return true;
}

bool
frenzy::parser::htmltokenizer::state_rcdata()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x26: // Ampersand
    state = STATE_CHARACTER_REFERENCE_IN_RCDATA;
    return true;
  case 0x3C: // <
    state = STATE_RCDATA_LESS_THAN_SIGN;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    emit(token::make_end_of_file());
    return true;
  default:
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_character_reference_in_rcdata()
{
  additional = 0x00;
  prevstate = STATE_DATA; // Won't be used
  urope chars;
  bool success = try_consume_character_reference(chars, false);
  if (!success)
    return false;

  if (chars.empty())
    emit(token::make_character(0x26));

  for (urope::const_iterator it = chars.begin();
       it != chars.end();
       ++it)
  {
    emit(token::make_character(*it));
  }

  state = STATE_RCDATA;
  return true;
}

bool
frenzy::parser::htmltokenizer::state_rawtext()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x3C: // <
    state = STATE_RAWTEXT_LESS_THAN_SIGN;
    return true;
  case 0x00: // NUL
    // Todo: Should produce a parse error
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    emit(token::make_end_of_file());
    return true;
  default:
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_script_data()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x3C: // <
    state = STATE_SCRIPT_DATA_LESS_THAN_SIGN;
    return true;
  case 0x00: // NUL
    // Todo: Should produce a parse error
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    emit(token::make_end_of_file());
    return true;
  default:
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_plaintext()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x00: // NUL
    // Todo: Should produce a parse error
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    emit(token::make_end_of_file());
    return true;
  default:
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_tag_open()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x21: // Exclamation mark
    state = STATE_MARKUP_DECLARATION_OPEN;
    return true;
  case 0x2F: // Slash
    state = STATE_END_TAG_OPEN;
    return true;
  case 0x3F: // Question mark
    // TODO: Should produce a parse error
    state = STATE_BOGUS_COMMENT;
    incomplete = token::make_comment();
    rewind(next);
    return true;
  default:
    break;
  }

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    incomplete = token::make_start_tag(next);
    state = STATE_TAG_NAME;
    return true;
  }

  // Anything else
  // TODO: Should produce a parse error
  state = STATE_DATA;
  emit(token::make_character(0x3C)); // <
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_end_tag_open()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  if (next == 0x3E) // >
  {
    // TODO: Should produce a parse error
    state = STATE_DATA;
    return true;
  }

  if (next == eof)
  {
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit(token::make_character(0x3C)); // <
    emit(token::make_character(0x2F)); // /
    rewind(next);
    return true;
  }

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    incomplete = token::make_end_tag(next);
    state = STATE_TAG_NAME;
    return true;
  }
  else
  {
    // TODO: Should produce a parse error
    state = STATE_BOGUS_COMMENT;
    incomplete = token::make_comment();
    rewind(orignext);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_tag_name()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    state = STATE_BEFORE_ATTRIBUTE_NAME;
    return true;
  case 0x2F: // Slash
    state = STATE_SELF_CLOSING_START_TAG;
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.tagname.push_back(0xFFFD);
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    // Lowercase if necessary
    if (is_ucase_ascii(next))
      next += 0x20;

    incomplete.tagname.push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_rcdata_less_than_sign()
{
  if (!can_consume())
    return false;

  uchar next = consume();

  if (next == 0x2F) // Slash
  {
    temporary_buffer.clear();
    state = STATE_RCDATA_END_TAG_OPEN;
    return true;
  }

  state = STATE_RCDATA;
  emit(token::make_character(0x3C)); // <
  rewind(next);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_rcdata_end_tag_open()
{
  if (!can_consume())
    return false;

  uchar next = consume();

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    incomplete = token::make_end_tag(next);
    temporary_buffer.push_back(orignext);
    state = STATE_RCDATA_END_TAG_NAME;
    return true;
  }

  state = STATE_RCDATA;
  emit(token::make_character(0x3C)); // <
  emit(token::make_character(0x2F)); // /
  rewind(next);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_rcdata_end_tag_name()
{
  assert(incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_BEFORE_ATTRIBUTE_NAME;
      return true;
    }
    break;
  case 0x2F: // Slash
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_SELF_CLOSING_START_TAG;
      return true;
    }
    break;
  case 0x3E: // >
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_DATA;
      emit();
      return true;
    }
    break;
  default:
    break;
  }

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    incomplete.tagname.push_back(next);
    temporary_buffer.push_back(orignext);
    return true;
  }

  state = STATE_RCDATA;
  emit(token::make_character(0x3C)); // <
  emit(token::make_character(0x2F)); // Slash
  for (urope::const_iterator it = temporary_buffer.begin();
       it != temporary_buffer.end();
       ++it)
  {
    emit(token::make_character(*it));
  }
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_rawtext_less_than_sign()
{
  if (!can_consume())
    return false;

  uchar next = consume();

  if (next == 0x2F)
  {
    temporary_buffer.clear();
    state = STATE_RAWTEXT_END_TAG_OPEN;
    return true;
  }

  state = STATE_RAWTEXT;
  emit(token::make_character(0x3C)); // <
  rewind(next);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_rawtext_end_tag_open()
{
  if (!can_consume())
    return false;

  uchar next = consume();

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    incomplete = token::make_end_tag(next);
    temporary_buffer.push_back(orignext);
    state = STATE_RAWTEXT_END_TAG_NAME;
    return true;
  }

  state = STATE_RAWTEXT;
  emit(token::make_character(0x3C)); // <
  emit(token::make_character(0x2F)); // Slash
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_rawtext_end_tag_name()
{
  assert(incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_BEFORE_ATTRIBUTE_NAME;
      return true;
    }
    break;
  case 0x2F: // Slash
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_SELF_CLOSING_START_TAG;
      return true;
    }
    break;
  case 0x3E: // >
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_DATA;
      emit();
      return true;
    }
    break;
  default:
    break;
  }

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    incomplete.tagname.push_back(next);
    temporary_buffer.push_back(orignext);
    return true;
  }

  state = STATE_RAWTEXT;
  emit(token::make_character(0x3C)); // <
  emit(token::make_character(0x2F)); // Slash
  for (urope::const_iterator it = temporary_buffer.begin();
       it != temporary_buffer.end();
       ++it)
  {
    emit(token::make_character(*it));
  }
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_script_data_less_than_sign()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2F: // Slash
    temporary_buffer.clear();
    state = STATE_SCRIPT_DATA_END_TAG_OPEN;
    return true;
  case 0x21: // Exclamation mark
    state = STATE_SCRIPT_DATA_ESCAPE_START;
    emit(token::make_character(0x3C)); // <
    emit(token::make_character(0x21)); // Exclamation mark
    return true;
  default:
    state = STATE_SCRIPT_DATA;
    emit(token::make_character(0x3C)); // <
    rewind(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_script_data_end_tag_open()
{
  if (!can_consume())
    return false;

  uchar next = consume();

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    incomplete = token::make_end_tag(next);
    temporary_buffer.push_back(orignext);
    state = STATE_SCRIPT_DATA_END_TAG_NAME;
    return true;
  }

  state = STATE_SCRIPT_DATA;
  emit(token::make_character(0x3C)); // <
  emit(token::make_character(0x2F)); // Slash
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_script_data_end_tag_name()
{
  assert(incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_BEFORE_ATTRIBUTE_NAME;
      return true;
    }
    break;
  case 0x2F: // Slash
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_SELF_CLOSING_START_TAG;
      return true;
    }
    break;
  case 0x3E: // >
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_DATA;
      emit();
      return true;
    }
    break;
  default:
    break;
  }

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    incomplete.tagname.push_back(next);
    temporary_buffer.push_back(orignext);
    return true;
  }

  state = STATE_SCRIPT_DATA;
  emit(token::make_character(0x3C)); // <
  emit(token::make_character(0x2F)); // Slash
  for (urope::const_iterator it = temporary_buffer.begin();
       it != temporary_buffer.end();
       ++it)
  {
    emit(token::make_character(*it));
  }
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_script_data_escape_start()
{
  if (!can_consume())
    return false;

  uchar next = consume();

  if (next == 0x2D)
  {
    state = STATE_SCRIPT_DATA_ESCAPE_START_DASH;
    emit(token::make_character(0x2D)); // -
    return true;
  }

  state = STATE_SCRIPT_DATA;
  rewind(next);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_script_data_escape_start_dash()
{
  if (!can_consume())
    return false;

  uchar next = consume();

  if (next == 0x2D)
  {
    state = STATE_SCRIPT_DATA_ESCAPED_DASH_DASH;
    emit(token::make_character(0x2D)); // -
    return true;
  }

  state = STATE_SCRIPT_DATA;
  rewind(next);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_script_data_escaped()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    state = STATE_SCRIPT_DATA_ESCAPED_DASH;
    emit(token::make_character(0x2D)); // -
    return true;
  case 0x3C: // <
    state = STATE_SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_script_data_escaped_dash()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    state = STATE_SCRIPT_DATA_ESCAPED_DASH_DASH;
    emit(token::make_character(0x2D)); // -
    return true;
  case 0x3C: // <
    state = STATE_SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    state = STATE_SCRIPT_DATA_ESCAPED;
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    state = STATE_SCRIPT_DATA_ESCAPED;
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_script_data_escaped_dash_dash()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    emit(token::make_character(0x2D)); // -
    return true;
  case 0x3C: // <
    state = STATE_SCRIPT_DATA_ESCAPED_LESS_THAN_SIGN;
    return true;
  case 0x3E: // >
    state = STATE_SCRIPT_DATA;
    emit(token::make_character(0x3E)); // >
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    state = STATE_SCRIPT_DATA_ESCAPED;
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    state = STATE_SCRIPT_DATA_ESCAPED;
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_script_data_escaped_less_than_sign()
{
  if (!can_consume())
    return false;

  uchar next = consume();

  if (next == 0x2F) // Slash
  {
    temporary_buffer.clear();
    state = STATE_SCRIPT_DATA_ESCAPED_END_TAG_OPEN;
    return true;
  }

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    temporary_buffer.clear();
    temporary_buffer.push_back(next);
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPE_START;
    emit(token::make_character(0x3C)); // <
    emit(token::make_character(orignext));
    return true;
  }

  state = STATE_SCRIPT_DATA_ESCAPED;
  emit(token::make_character(0x3C)); // <
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_script_data_escaped_end_tag_open()
{
  if (!can_consume())
    return false;

  uchar next = consume();

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    incomplete = token::make_end_tag(next);
    temporary_buffer.push_back(orignext);
    state = STATE_SCRIPT_DATA_ESCAPED_END_TAG_NAME;
    return true;
  }

  state = STATE_SCRIPT_DATA_ESCAPED;
  emit(token::make_character(0x3C)); // <
  emit(token::make_character(0x2F)); // Slash
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_script_data_escaped_end_tag_name()
{
  assert(incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_BEFORE_ATTRIBUTE_NAME;
      return true;
    }
    break;
  case 0x2F: // Slash
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_SELF_CLOSING_START_TAG;
      return true;
    }
    break;
  case 0x3E: // >
    if (incomplete.tagname == last_start_tag_name)
    {
      state = STATE_DATA;
      emit();
      return true;
    }
    break;
  default:
    break;
  }

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    incomplete.tagname.push_back(next);
    temporary_buffer.push_back(orignext);
    return true;
  }

  state = STATE_SCRIPT_DATA_ESCAPED;
  emit(token::make_character(0x3C)); // <
  emit(token::make_character(0x2F)); // Slash
  for (urope::const_iterator it = temporary_buffer.begin();
       it != temporary_buffer.end();
       ++it)
  {
    emit(token::make_character(*it));
  }
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_script_data_double_escape_start()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
  case 0x2F: // Slash
  case 0x3E: // >
    if (temporary_match("script"))
      state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED;
    else
      state = STATE_SCRIPT_DATA_ESCAPED;
    emit(token::make_character(next));
    return true;
  default:
    break;
  }

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    temporary_buffer.push_back(next);
    emit(token::make_character(orignext));
    return true;
  }

  state = STATE_SCRIPT_DATA_ESCAPED;
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_script_data_double_escaped()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED_DASH;
    emit(token::make_character(0x2D)); // -
    return true;
  case 0x3C: // <
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN;
    emit(token::make_character(0x3C)); // <
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_script_data_double_escaped_dash()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED_DASH_DASH;
    emit(token::make_character(0x2D)); // -
    return true;
  case 0x3C: // <
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN;
    emit(token::make_character(0x3C)); // <
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED;
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED;
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_script_data_double_escaped_dash_dash()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    emit(token::make_character(0x2D)); // -
    return true;
  case 0x3C: // <
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED_LESS_THAN_SIGN;
    emit(token::make_character(0x3C)); // <
    return true;
  case 0x3E: // >
    state = STATE_SCRIPT_DATA;
    emit(token::make_character(0x3E)); // >
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED;
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED;
    emit(token::make_character(next));
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_script_data_double_escaped_less_than_sign()
{
  if (!can_consume())
    return false;

  uchar next = consume();

  if (next == 0x2F) // Slash
  {
    temporary_buffer.clear();
    state = STATE_SCRIPT_DATA_DOUBLE_ESCAPE_END;
    emit(token::make_character(0x2F)); // Slash
    return true;
  }

  state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED;
  rewind(next);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_script_data_double_escape_end()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
  case 0x2F: // Slash
  case 0x3E: // >
    if (temporary_match("script"))
      state = STATE_SCRIPT_DATA_ESCAPED;
    else
      state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED;
    emit(token::make_character(next));
    return true;
  default:
    break;
  }

  uchar orignext = next;
  if (is_ucase_ascii(next))
    next += 0x20;

  if (is_lcase_ascii(next))
  {
    temporary_buffer.push_back(next);
    emit(token::make_character(orignext));
    return true;
  }

  state = STATE_SCRIPT_DATA_DOUBLE_ESCAPED;
  rewind(orignext);
  return true;
}

bool
frenzy::parser::htmltokenizer::state_before_attribute_name()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    return true;
  case 0x2F: // Slash
    state = STATE_SELF_CLOSING_START_TAG;
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    emit(token::make_character(0xFFFD));
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  case 0x22: // "
  case 0x27: // '
  case 0x3C: // <
  case 0x3D: // =
    // TODO: Should produce a parse error
    // Flowing to default intended
  default:
    if (is_ucase_ascii(next))
      next += 0x20;

    incomplete.start_new_attribute(next);
    state = STATE_ATTRIBUTE_NAME;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_attribute_name()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    state = STATE_AFTER_ATTRIBUTE_NAME;
    return true;
  case 0x2F: // Slash
    state = STATE_SELF_CLOSING_START_TAG;
    return true;
  case 0x3D: // =
    state = STATE_BEFORE_ATTRIBUTE_VALUE;
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.incompleteattr.first.push_back(0xFFFD);
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  case 0x22: // "
  case 0x27: // '
  case 0x3C: // <
    // TODO: Should produce a parse error
    // Flowing to default intended
  default:
    if (is_ucase_ascii(next))
      next += 0x20;
    
    incomplete.incompleteattr.first.push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_after_attribute_name()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    // Ignore
    return true;
  case 0x2F: // Slash
    state = STATE_SELF_CLOSING_START_TAG;
    return true;
  case 0x3D: // =
    state = STATE_BEFORE_ATTRIBUTE_VALUE;
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.start_new_attribute(0xFFFD);
    state = STATE_ATTRIBUTE_NAME;
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  case 0x22: // "
  case 0x27: // '
  case 0x3C: // <
    // TODO: Should produce a parse error
    // Flow to default intended
  default:
    if (is_ucase_ascii(next))
      next += 0x20;
    
    incomplete.start_new_attribute(next);
    state = STATE_ATTRIBUTE_NAME;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_before_attribute_value()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    // Ignore
    return true;
  case 0x22: // "
    state = STATE_ATTRIBUTE_VALUE_DOUBLE_QUOTED;
    return true;
  case 0x26: // Ampersand
    state = STATE_ATTRIBUTE_VALUE_UNQUOTED;
    rewind(next); // This is to be reconsumed
    return true;
  case 0x27: // '
    state = STATE_ATTRIBUTE_VALUE_SINGLE_QUOTED;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    state = STATE_ATTRIBUTE_VALUE_UNQUOTED;
    incomplete.incompleteattr.second.push_back(0xFFFD);
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  case 0x3C: // <
  case 0x3D: // =
  case 0x60: // `
    // TODO: Should produce a parse error
    // Flow to default intended
  default:
    state = STATE_ATTRIBUTE_VALUE_UNQUOTED;
    incomplete.incompleteattr.second.push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_attribute_value_double_quoted()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x22: // "
    state = STATE_AFTER_ATTRIBUTE_VALUE_QUOTED;
    return true;
  case 0x26: // Ampersand
    state = STATE_CHARACTER_REFERENCE_IN_ATTRIBUTE_VALUE;
    additional = 0x22; // "
    prevstate = STATE_ATTRIBUTE_VALUE_DOUBLE_QUOTED;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.incompleteattr.second.push_back(0xFFFD);
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    incomplete.incompleteattr.second.push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_attribute_value_single_quoted()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x27: // '
    state = STATE_AFTER_ATTRIBUTE_VALUE_QUOTED;
    return true;
  case 0x26: // Ampersand
    state = STATE_CHARACTER_REFERENCE_IN_ATTRIBUTE_VALUE;
    additional = 0x27; // '
    prevstate = STATE_ATTRIBUTE_VALUE_SINGLE_QUOTED;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.incompleteattr.second.push_back(0xFFFD);
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    incomplete.incompleteattr.second.push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_attribute_value_unquoted()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    state = STATE_BEFORE_ATTRIBUTE_NAME;
    return true;
  case 0x26: // Ampersand
    state = STATE_CHARACTER_REFERENCE_IN_ATTRIBUTE_VALUE;
    additional = 0x3E; // >
    prevstate = STATE_ATTRIBUTE_VALUE_UNQUOTED;
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.incompleteattr.second.push_back(0xFFFD);
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  case 0x22: // "
  case 0x27: // '
  case 0x3C: // <
  case 0x3D: // =
  case 0x60: // `
    // TODO: Should produce a parse error
    // Flow to default intended
  default:
    incomplete.incompleteattr.second.push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_character_reference_in_attribute_value()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  urope chars;
  bool success = try_consume_character_reference(chars, true);
  if (!success)
    return false;

  if (chars.empty())
    incomplete.incompleteattr.second.push_back(0x26); // Ampersand

  for (urope::const_iterator it = chars.begin();
       it != chars.end();
       ++it)
  {
    incomplete.incompleteattr.second.push_back(*it);
  }

  state = prevstate;
  return true;
}

bool
frenzy::parser::htmltokenizer::state_after_attribute_value_quoted()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    state = STATE_BEFORE_ATTRIBUTE_NAME;
    return true;
  case 0x2F: // Slash
    state = STATE_SELF_CLOSING_START_TAG;
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    state = STATE_BEFORE_ATTRIBUTE_NAME;
    rewind(next); // To be reconsumed
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_self_closing_start_tag()
{
  assert(incomplete.type == TOKEN_START_TAG || incomplete.type == TOKEN_END_TAG);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x3E: // >
    incomplete.self_closing = true;
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    state = STATE_BEFORE_ATTRIBUTE_NAME;
    rewind(next); // To be reconsumed
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_bogus_comment()
{
  assert(incomplete.type == TOKEN_COMMENT);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x3E: // >
    emit();
    state = STATE_DATA;
    return true;
  case 0x00: // NUL
    incomplete.comment.push_back(0xFFFD);
    return true;
  case eof:
    emit();
    state = STATE_DATA;
    rewind(next);
    return true;
  default:
    incomplete.comment.push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_markup_declaration_open()
{
  // TODO: Refactor the multiples of execution flows leading to
  // STATE_BOGUS_COMMENT to just one

  if (peek_eof(2))
  {
    // TODO: Should produce a parse error
    state = STATE_BOGUS_COMMENT;
    incomplete = token::make_comment();
    return true;
  }

  if (!can_consume(2))
    return false;

  // Two - characters
  if (peek_match("--"))
  {
    consume(2);
    incomplete = token::make_comment();
    state = STATE_COMMENT_START;
    return true;
  }

  if (peek_eof(7))
  {
    // TODO: Should produce a parse error
    state = STATE_BOGUS_COMMENT;
    incomplete = token::make_comment();
    return true;
  }

  if (!can_consume(7))
    return false;

  if (peek_match_caseless("DOCTYPE"))
  {
    consume(7);
    state = STATE_DOCTYPE;
    return true;
  }

  /*
   * TODO: Check for current node in the tree construction, if it's
   * not an element in the HTML namespace and next seven characters
   * are [CDATA[, consume them and switch to STATE_CDATA_SECTION.
   */

  // TODO: Should produce a parse error
  state = STATE_BOGUS_COMMENT;
  incomplete = token::make_comment();
  return true;
}

bool
frenzy::parser::htmltokenizer::state_comment_start()
{
  assert(incomplete.type == TOKEN_COMMENT);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    state = STATE_COMMENT_START_DASH;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.comment.push_back(0xFFFD);
    state = STATE_COMMENT;
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit();
    rewind(next);
    return true;
  default:
    incomplete.comment.push_back(next);
    state = STATE_COMMENT;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_comment_start_dash()
{
  assert(incomplete.type == TOKEN_COMMENT);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    state = STATE_COMMENT_END;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0xFFFD);
    state = STATE_COMMENT;
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit();
    rewind(next);
    return true;
  default:
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(next);
    state = STATE_COMMENT;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_comment()
{
  assert(incomplete.type == TOKEN_COMMENT);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    state = STATE_COMMENT_END_DASH;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.comment.push_back(0xFFFD);
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit();
    rewind(next);
    return true;
  default:
    incomplete.comment.push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_comment_end_dash()
{
  assert(incomplete.type == TOKEN_COMMENT);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    state = STATE_COMMENT_END;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0xFFFD);
    state = STATE_COMMENT;
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit();
    rewind(next);
    return true;
  default:
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(next);
    state = STATE_COMMENT;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_comment_end()
{
  assert(incomplete.type == TOKEN_COMMENT);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0xFFFD);
    state = STATE_COMMENT;
    return true;
  case 0x21: // Exclamation mark
    // TODO: Should produce a parse error
    state = STATE_COMMENT_END_BANG;
    return true;
  case 0x2D: // -
    // TODO: Should produce a parse error
    incomplete.comment.push_back(0x2D);
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit();
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(next);
    state = STATE_COMMENT;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_comment_end_bang()
{
  assert(incomplete.type == TOKEN_COMMENT);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x2D: // -
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0x21);
    state = STATE_COMMENT_END_DASH;
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0x21);
    incomplete.comment.push_back(0xFFFD);
    state = STATE_COMMENT;
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit();
    rewind(next);
    return true;
  default:
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0x2D);
    incomplete.comment.push_back(0x21);
    incomplete.comment.push_back(next);
    state = STATE_COMMENT;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_doctype()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    state = STATE_BEFORE_DOCTYPE_NAME;
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete = token::make_doctype();
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    state = STATE_BEFORE_DOCTYPE_NAME;
    rewind(next); // To be reconsumed
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_before_doctype_name()
{
  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    // Ignore
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete = token::make_doctype(0xFFFD);
    state = STATE_DOCTYPE_NAME;
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    incomplete = token::make_doctype();
    incomplete.force_quirks = true;
    emit();
    state = STATE_DATA;
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete = token::make_doctype();
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    incomplete = token::make_doctype(next);
    state = STATE_DOCTYPE_NAME;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_doctype_name()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    state = STATE_AFTER_DOCTYPE_NAME;
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.doctype_name->push_back(0xFFFD);
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    if (is_ucase_ascii(next))
      next += 0x20;

    incomplete.doctype_name->push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_after_doctype_name()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    // Ignore
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    rewind(next);

    if (peek_eof(6))
    {
      // TODO: Should produce a parse error

      // TODO: Refactor this and the same code at the end of the
      // default case together

      consume();
      incomplete.force_quirks = true;
      state = STATE_BOGUS_DOCTYPE;
      return true;
    }

    if (!can_consume(6))
      return false;

    if (peek_match_caseless("PUBLIC"))
    {
      consume(6);
      state = STATE_AFTER_DOCTYPE_PUBLIC_KEYWORD;
      return true;
    }

    if (peek_match_caseless("SYSTEM"))
    {
      consume(6);
      state = STATE_AFTER_DOCTYPE_SYSTEM_KEYWORD;
      return true;
    }

    // TODO: If reached, should produce a parse error
    consume();
    incomplete.force_quirks = true;
    state = STATE_BOGUS_DOCTYPE;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_after_doctype_public_keyword()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    state = STATE_BEFORE_DOCTYPE_PUBLIC_IDENTIFIER;
    return true;
  case 0x22: // "
    // TODO: Should produce a parse error
    incomplete.public_identifier = ustring();
    state = STATE_DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED;
    return true;
  case 0x27: // '
    // TODO: Should produce a parse error
    incomplete.public_identifier = ustring();
    state = STATE_DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED;
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_BOGUS_DOCTYPE;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_before_doctype_public_identifier()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    // Ignore
    return true;
  case 0x22: // "
    incomplete.public_identifier = ustring();
    state = STATE_DOCTYPE_PUBLIC_IDENTIFIER_DOUBLE_QUOTED;
    return true;
  case 0x27: // '
    incomplete.public_identifier = ustring();
    state = STATE_DOCTYPE_PUBLIC_IDENTIFIER_SINGLE_QUOTED;
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_BOGUS_DOCTYPE;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_doctype_public_identifier_double_quoted()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x22: // "
    state = STATE_AFTER_DOCTYPE_PUBLIC_IDENTIFIER;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.public_identifier->push_back(0xFFFD);
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    incomplete.public_identifier->push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_doctype_public_identifier_single_quoted()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x27: // '
    state = STATE_AFTER_DOCTYPE_PUBLIC_IDENTIFIER;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.public_identifier->push_back(0xFFFD);
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    incomplete.public_identifier->push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_after_doctype_public_identifier()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    state = STATE_BETWEEN_DOCTYPE_PUBLIC_AND_SYSTEM_IDENTIFIERS;
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case 0x22: // "
    // TODO: Should produce a parse error
    incomplete.system_identifier = ustring();
    state = STATE_DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED;
    return true;
  case 0x27: // '
    // TODO: Should produce a parse error
    incomplete.system_identifier = ustring();
    state = STATE_DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED;
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_BOGUS_DOCTYPE;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_between_doctype_public_and_system_identifiers()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    // Ignore
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case 0x22: // "
    incomplete.system_identifier = ustring();
    state = STATE_DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED;
    return true;
  case 0x27: // '
    incomplete.system_identifier = ustring();
    state = STATE_DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED;
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_BOGUS_DOCTYPE;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_after_doctype_system_keyword()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    state = STATE_BEFORE_DOCTYPE_SYSTEM_IDENTIFIER;
    return true;
  case 0x22: // "
    // TODO: Should produce a parse error
    incomplete.system_identifier = ustring();
    state = STATE_DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED;
    return true;
  case 0x27: // '
    // TODO: Should produce a parse error
    incomplete.system_identifier = ustring();
    state = STATE_DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED;
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_BOGUS_DOCTYPE;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_before_doctype_system_identifier()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    // Ignore
    return true;
  case 0x22: // "
    incomplete.system_identifier = ustring();
    state = STATE_DOCTYPE_SYSTEM_IDENTIFIER_DOUBLE_QUOTED;
    return true;
  case 0x27: // '
    incomplete.system_identifier = ustring();
    state = STATE_DOCTYPE_SYSTEM_IDENTIFIER_SINGLE_QUOTED;
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_BOGUS_DOCTYPE;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_doctype_system_identifier_double_quoted()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x22: // "
    state = STATE_AFTER_DOCTYPE_SYSTEM_IDENTIFIER;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.system_identifier->push_back(0xFFFD);
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    incomplete.system_identifier->push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_doctype_system_identifier_single_quoted()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x27: // '
    state = STATE_AFTER_DOCTYPE_SYSTEM_IDENTIFIER;
    return true;
  case 0x00: // NUL
    // TODO: Should produce a parse error
    incomplete.system_identifier->push_back(0xFFFD);
    return true;
  case 0x3E: // >
    // TODO: Should produce a parse error
    incomplete.force_quirks = true;
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    incomplete.system_identifier->push_back(next);
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_after_doctype_system_identifier()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x09: // Tab
  case 0x0A: // LF
  case 0x0C: // FF
  case 0x20: // Space
    // Ignore
    return true;
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    incomplete.force_quirks = true;
    emit();
    rewind(next);
    return true;
  default:
    // TODO: Should produce a parse error
    // Note that this case doesn't set force_quirks
    state = STATE_BOGUS_DOCTYPE;
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_bogus_doctype()
{
  assert(incomplete.type == TOKEN_DOCTYPE);

  if (!can_consume())
    return false;

  uchar next = consume();
  switch (next)
  {
  case 0x3E: // >
    state = STATE_DATA;
    emit();
    return true;
  case eof:
    // TODO: Should produce a parse error
    state = STATE_DATA;
    emit();
    rewind(next);
    return true;
  default:
    // Ignore
    return true;
  }
}

bool
frenzy::parser::htmltokenizer::state_cdata_section()
{
  // TODO: Implement
  return false;
}
