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

#include <algorithm>

#include "input_preprocessor.hpp"

frenzy::parser::input_preprocessor::input_preprocessor()
  : beginning(true)
  , prev_was_cr(false)
{
}

void
frenzy::parser::input_preprocessor::pass_characters(const frenzy::urope& input)
{
  std::for_each(input.begin(), input.end(), boost::bind(&input_preprocessor::process_one, this, _1));
  completed(result_buffer);
  result_buffer.clear();
}

frenzy::urope
frenzy::parser::input_preprocessor::complete_characters()
{
  urope ret;
  ret.splice_back(completed_items);

  return ret;
}

void
frenzy::parser::input_preprocessor::attach_destination(boost::function<void (const urope&)> dest)
{
  destination = dest;
  if (!completed_items.empty())
  {
    destination(completed_items);
    completed_items.clear();
  }
}

void
frenzy::parser::input_preprocessor::process_one(uchar uc)
{
  bool was_begin = beginning;
  beginning = false;

  if (was_begin && uc == 0xFEFF)
  {
    // Leading BOM, skip it
    return;
  }

  if (prev_was_cr && uc == 0x000A)
  {
    // Ignore LF
    prev_was_cr = false;
    return;
  }

  prev_was_cr = (uc == 0x000D);

  if (uc == 0x000D)
  {
    uc = 0x000A;
  }

  result_buffer.push_back(uc);
}

void
frenzy::parser::input_preprocessor::completed(frenzy::urope& items)
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
