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

#ifndef FRENZY_CHARDECODER_HPP
#define FRENZY_CHARDECODER_HPP

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

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

  typedef unsigned char byte;
  typedef std::basic_string<byte> bytestring;
  
  namespace parser
  {
    /*
     * HTML5 8.2.2 "The input byte stream"
     *
     * First step of parsing is decoding a bytestream to Unicode code
     * points.  8.2.2.1 etc. describe the character encoding sniffing
     * mechanisms.
     *
     * TODO: Implement encoding sniffing. At this point we just assume
     * UTF-8.  The interface currently is as follows: Construct a
     * utf8_decoder, attach it to the parser pipeline. In an actual
     * implementation we need to implement 8.2.2.1 specified
     * confidences and decoder selection.
     */

    /*
     * A charset_decoder takes a string of bytes and passes on uchars.
     * Incomplete characters are buffered and passed when more bytes
     * are passed to it later.
     */
    struct charset_decoder : private boost::noncopyable
    {
      virtual ~charset_decoder() = 0;

      virtual void pass_bytes(const bytestring& input) = 0;

      urope complete_characters();
      void attach_destination(boost::function<void (const urope&)> dest);

    protected:
      void completed(urope& items);

    private:
      urope completed_items;
      boost::function<void (const urope&)> destination;
    };

    /*
     * HTML5 2.4 UTF-8
     *
     * The specification gives precise instructions for UTF-8 decoding, mostly for
     * the special error handling required. This is the reason for a custom UTF-8
     * decoder.
     */
    struct utf8_decoder : charset_decoder
    {
      utf8_decoder();

      void pass_bytes(const bytestring& input);

    private:
      void process_one(byte b);
      urope result_buffer;

      enum
      {
        UTF8_BEGIN,
        UTF8_CONTINUATION
      } state;
      uchar incomplete;
      size_t bytes_left;
      unsigned char multibytesize;
    };
  }
}

#endif
