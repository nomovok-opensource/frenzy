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

#include <stdexcept>
#include <boost/bind.hpp>

#include "chardecoder.hpp"

frenzy::parser::charset_decoder::~charset_decoder()
{
}

frenzy::urope
frenzy::parser::charset_decoder::complete_characters()
{
  urope ret;
  ret.splice_back(completed_items);

  return ret;
}

void
frenzy::parser::charset_decoder::attach_destination(boost::function<void (const urope&)> dest)
{
  destination = dest;
  if (!completed_items.empty())
  {
    destination(completed_items);
    completed_items.clear();
  }
}

void
frenzy::parser::charset_decoder::completed(frenzy::urope& items)
{
  if (destination)
  {
    destination(items);
  }
  else
  {
    completed_items.splice_back(items);
  }
}

frenzy::parser::utf8_decoder::utf8_decoder()
  : state(UTF8_BEGIN)
{
}

void
frenzy::parser::utf8_decoder::pass_bytes(const bytestring& input)
{
  std::for_each(input.begin(), input.end(), boost::bind(&utf8_decoder::process_one, this, _1));

  if (input.empty() && state == UTF8_CONTINUATION)
  {
    // Special eof handling for incomplete sequence
    result_buffer.push_back(0xFFFD);
    completed(result_buffer);
    result_buffer.clear();
  }

  completed(result_buffer);
  result_buffer.clear();
}

void
frenzy::parser::utf8_decoder::process_one(byte b)
{
  switch (state)
  {
  case UTF8_BEGIN:
    if ((b & 0x80) == 0x00)
    {
      // Single byte code point
      result_buffer.push_back(b);
      return;
    }
    
    if ((b & 0xE0) == 0xC0)
    {
      // Two-byte code point
      state = UTF8_CONTINUATION;
      bytes_left = 1;
      multibytesize = 2;
      incomplete = (b & 0x1F) << 6;
      return;
    }

    if ((b & 0xF0) == 0xE0)
    {
      // Three-byte code point
      state = UTF8_CONTINUATION;
      bytes_left = 2;
      multibytesize = 3;
      incomplete = (b & 0x0F) << 12;
      return;
    }

    if ((b & 0xF8) == 0xF0)
    {
      // Four-byte code point
      state = UTF8_CONTINUATION;
      bytes_left = 3;
      multibytesize = 4;
      incomplete = (b & 0x07) << 18;
      return;
    }

    if ((b & 0xFC) == 0xF8)
    {
      // Five-byte code point
      state = UTF8_CONTINUATION;
      bytes_left = 4;
      multibytesize = 5;
      incomplete = 0; // value will just be discarded
      return;
    }

    if ((b & 0xFE) == 0xFC)
    {
      // Six-byte code point
      state = UTF8_CONTINUATION;
      bytes_left = 5;
      multibytesize = 6;
      incomplete = 0; // value will just be discarded
      return;
    }
    
    // If we reach this point, the byte was a continuation byte or
    // FE or FF, all of them invalid for this state.
    // Insert U+FFFD for it.
    result_buffer.push_back(0xFFFD);
    return;

  case UTF8_CONTINUATION:
    if ((b & 0xC0) == 0x80)
    {
      --bytes_left;
      incomplete |= (b & 0x3F) << (bytes_left * 6);

      if (bytes_left == 0)
      {
        bool valid = true;
        // Characters above U+10FFFF are invalid
        valid = valid && incomplete <= 0x10FFFF;
        // Characters in range U+D800 to U+DFFF are invalid
        valid = valid && (incomplete < 0xD800 || incomplete > 0xDFFF);

        // Overlong sequences are invalid
        uchar minvalid = 0;
        uchar maxvalid = 0;
        switch (multibytesize)
        {
        case 2:
          minvalid = 0x0080;
          maxvalid = 0x07FF;
          break;
        case 3:
          minvalid = 0x0800;
          maxvalid = 0xFFFF;
          break;
        case 4:
          minvalid = 0x10000;
          maxvalid = 0x1FFFFF;
        }
        valid = valid && (incomplete >= minvalid && incomplete <= maxvalid);

        valid = valid && (multibytesize < 5);

        if (valid)
          result_buffer.push_back(incomplete);
        else
          result_buffer.push_back(0xFFFD);
        state = UTF8_BEGIN;
      }
      return;
    }

    // Not a continuation byte. Insert a U+FFFD and re-process this byte.
    state = UTF8_BEGIN;
    result_buffer.push_back(0xFFFD);
    process_one(b);
    return;
  default:
    // This shouldn't happen
    throw std::logic_error("Decoder state was not one of valid values");
  }
}
