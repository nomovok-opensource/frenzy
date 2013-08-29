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

#ifndef FRENZY_INPUT_PREPROCESSOR_HPP
#define FRENZY_INPUT_PREPROCESSOR_HPP

#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "util/unicode.hpp"

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
   * function that takes a sequence of result datums. Incomplete input
   * is buffered. Passing an empty sequence passes "end-of-file".
   *
   * The attach_destination() function is intended to be the primary
   * interface. complete_*s() cannot represent end-of-file.
   */

  namespace parser
  {
    /*
     * HTML5 8.2.2.4 "Preprocessing the input stream"
     *
     * Second stage of parsing is preprocessing the input stream. The
     * input stream preprocessor is also the stage where Javascript
     * can insert code points using document.write(). Handling this
     * properly requires relatively difficult care.
     */

    /*
     * There's only one way to preprocess the input, hence there's
     * only one class without virtual functions.
     */
    struct input_preprocessor : private boost::noncopyable
    {
      input_preprocessor();

      void pass_characters(const urope& input);

      urope complete_characters();
      void attach_destination(boost::function<void (const urope&)> dest);

    private:
      urope completed_items;
      boost::function<void (const urope&)> destination;

      bool beginning;
      bool prev_was_cr;

      void process_one(uchar uc);
      urope result_buffer;
      void completed(urope& items);
    };
  }
}

#endif
