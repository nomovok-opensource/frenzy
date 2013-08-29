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

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <boost/bind.hpp>

#include "parser/chardecoder.hpp"
#include "test_helpers.hpp"

using namespace frenzy;
using namespace frenzy::parser;
using namespace frenzy::test_helpers;

namespace
{
  // Helper function for testing decoding with full strings
  void decode_test(std::string utf8_input, std::string expected_output,
                   uchar exprepl_from = 0, uchar exprepl_to = 0,
                   uchar second_from = 0, uchar second_to = 0)
  {
    utf8_decoder dec;
    utf8_decoder attdec;
    utf8_decoder pdec;
    utf8_decoder attpdec;

    destination dest;
    destination pdest;

    attdec.attach_destination(boost::bind(&destination::receive, &dest, _1));
    attpdec.attach_destination(boost::bind(&destination::receive, &pdest, _1));
    
    bytestring input = bstr(utf8_input);
    urope expected = ustr(expected_output, exprepl_from, exprepl_to,
			  second_from, second_to);
    
    // Pass as a whole
    dec.pass_bytes(input);
    attdec.pass_bytes(input);
    // Pass a single byte at a time
    for (bytestring::const_iterator it = input.begin();
	 it != input.end();
	 ++it)
    {
      bytestring partinput(1, *it);
      pdec.pass_bytes(partinput);
      attpdec.pass_bytes(partinput);
    }

    // And pass eof
    bytestring eof;
    dec.pass_bytes(eof);
    attdec.pass_bytes(eof);
    pdec.pass_bytes(eof);
    attpdec.pass_bytes(eof);
    
    urope received = dec.complete_characters();
    urope preceived = pdec.complete_characters();
    
    BOOST_CHECK_EQUAL(received, expected);
    BOOST_CHECK_EQUAL(dest.items, expected);
    BOOST_CHECK_EQUAL(preceived, expected);
    BOOST_CHECK_EQUAL(pdest.items, expected);
    
    BOOST_CHECK(dest.ended);
    BOOST_CHECK(pdest.ended);
  }
}

BOOST_AUTO_TEST_SUITE(utf8_decoder_tests)

BOOST_AUTO_TEST_CASE(basic_input)
{
  std::string teststr = "abcdefghijklmnopqrstuvwxyz1234567890";

  decode_test(teststr, teststr);
}

BOOST_AUTO_TEST_CASE(two_byte_characters)
{
  std::string teststr = "k\xC3\xA4nkk\xC3\xA4r\xC3\xA4nkk\xC3\xA4";
  std::string expstr = "k,nkk,r,nkk,";

  decode_test(teststr, expstr, ',', 0xE4);
}

BOOST_AUTO_TEST_CASE(three_byte_characters)
{
  std::string teststr = "3 \xE2\x88\x88 integers";
  std::string expstr = "3 , integers";

  decode_test(teststr, expstr, ',', 0x2208);
}

BOOST_AUTO_TEST_CASE(four_byte_characters)
{
  std::string teststr = "to speak tirelessly: \xF0\xA0\xB9\xB7";
  std::string expstr = "to speak tirelessly: ,";

  decode_test(teststr, expstr, ',', 0x20E77);
}

BOOST_AUTO_TEST_CASE(range_fe_to_ff)
{
  // 0xFE and 0xFF must be replaced by a single U+FFFD REPLACEMENT CHARACTER
  std::string teststr = "\xFE test data \xFF";
  std::string expstr = ", test data ,";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(overlong_two_byte)
{
  // The whole overlong sequence must be replaced by a _single_ U+FFFD
  std::string teststr = "\xC0\xAF"; // This is U+002F SOLIDUS (aka a slash character, / )
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(overlong_three_byte)
{
  // The whole overlong sequence must be replaced by a _single_ U+FFFD
  std::string teststr = "\xE0\x80\xAF";
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(overlong_four_byte)
{
  // The whole overlong sequence must be replaced by a _single_ U+FFFD
  std::string teststr = "\xF0\x80\x80\xAF";
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(boundary_test_two_byte)
{
  // The highest code point that is represented properly using one less byte
  std::string teststr = "\xC1\xBF";
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(boundary_test_three_byte)
{
  // The highest code point that is represented properly using one less byte
  std::string teststr = "\xE0\x9F\xBF";
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(boundary_test_four_byte)
{
  // The highest code point that is represented properly using one less byte
  std::string teststr = "\xF0\x8F\xBF\xBF";
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(overlong_nul_two_byte)
{
  // Overlong representation of ASCII NUL character
  std::string teststr = "\xC0\x80";
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(overlong_nul_three_byte)
{
  // Overlong representation of ASCII NUL character
  std::string teststr = "\xE0\x80\x80";
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(overlong_nul_four_byte)
{
  // Overlong representation of ASCII NUL character
  std::string teststr = "\xF0\x80\x80\x80";
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(invalid_c0_c1_starter)
{
  // HTML5 2.4 "One byte in the range C0 to C1, followed by one byte in the range 80 to BF"
  // -> the whole matched sequence must be replaced by a _single_ U+FFFD
  for (int x = 0x80; x < 0xBF; ++x)
  {
    std::string teststr0 = "\xC0";
    std::string teststr1 = "\xC1";
    teststr0 += static_cast<char>(x);
    teststr1 += static_cast<char>(x);
    
    std::string expstr = ",";
    decode_test(teststr0, expstr, ',', 0xFFFD);
    decode_test(teststr1, expstr, ',', 0xFFFD);
  }
}

BOOST_AUTO_TEST_CASE(code_point_too_high)
{
  // HTML5 2.4 "One byte in the range F0 to F4, followed by three
  // bytes in the range 80 to BF that represent a code point above
  // U+10FFFF"
  // -> the whole matched sequence must be replaced by a _single_ U+FFFD
  // Personal observation: Only F4 starter byte can represent higher
  // than U+10FFFF o_O
  std::string teststr = "\xF4\x90\x80\x80"; // This would be U+110000
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(range_f5_to_f7)
{
  // HTML5 2.4 "One byte in the range F5 to F7, followed by three
  // bytes in the range 80 to BF
  // -> the whole matched sequence must be replaced by a _single_ U+FFFD
  // This is a valid four-byte UTF-8 encoding, but F5 to F7 are guaranteed
  // to go over U+10FFFF
  std::string teststr = "\xF5\x97\xA1\xA8"; // This would be U+157868
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);

}

BOOST_AUTO_TEST_CASE(five_byte_encoding)
{
  // Five byte encodings start with F8 to FB, and must be replaced
  // with a single U+FFFD
  std::string teststr = "\xFA\xAB\xAB\xAB\xAB";
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(six_byte_encoding)
{
  // Six byte encodings start with FC to FD, and must be replaced
  // with a single U+FFFD
  std::string teststr = "\xFC\xAB\xAB\xAB\xAB\xAB";
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(missing_continuation)
{
  // If a starting byte of a multibyte encoding (C0 to FD) is
  // not followed by a byte in the range 80 to BF (continuation),
  // the starter byte must be replaced with U+FFFD
  std::string teststr = "\xD2test data";
  std::string expstr = ",test data";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(missing_second_continuation)
{
  // Starting a 3-6 byte encoding, second continuation byte invalid.
  // The starter byte and the first continuation byte are replaced
  // with a single U+FFFD
  std::string teststr = "\xE7\xA2test data";
  std::string expstr = ",test data";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(missing_third_continuation)
{
  // Starting a 4-6 byte encoding, third continuation byte invalid.
  // The starter byte and the two continuation bytes are replaced
  // with a single U+FFFD
  std::string teststr = "\xF0\xA2\xA2test data";
  std::string expstr = ",test data";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(missing_fourth_continuation)
{
  // Starting a 5-6 byte encoding, fourth continuation byte invalid.
  // The starter byte and the three continuation bytes are replaced
  // with a single U+FFFD.
  // Note that this encoding would be invalid even if correct.
  // Testing for invalid 5-6 byte encodings are still performed.
  std::string teststr = "\xFA\xA2\xA2\xA2test data";
  std::string expstr = ",test data";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(missing_fifth_continuation)
{
  // Starting a 6 byte encoding, fifth continuation byte invalid.
  // The starter byte and the four continuation bytes are replaced
  // with a single U+FFFD.
  // Note that this encoding would be invalid even if correct.
  // Testing for invalid 6 byte encodings are still performed.
  std::string teststr = "\xFC\xA2\xA2\xA2\xA2test data";
  std::string expstr = ",test data";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(utf16_surrogates_invalid)
{
  // Any sequence that represents a code point in range
  // U+D800 to U+DFFF is replaced with a single U+FFFD
  std::string teststr = "\xED\xA0\x8F"; // U+D80F
  std::string expstr = ",";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(continuation_after_single_byte)
{
  // Every byte in range 80 to BF that follows a byte not in that range
  // is replaced with U+FFFD
  std::string teststr = "test\x91" "data"; // Split the string so compiler doesn't stun itself
  std::string expstr = "test,data";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_CASE(continuation_after_valid_multibyte)
{
  // Every byte in range 80 to BF that follows a byte that was
  // part of a complete UTF-8 sequence already is replaced
  // with U+FFFD
  std::string teststr = "v\xC3\xA4\x91\xC3\xA4ksy";
  std::string expstr = "v.,.ksy";

  decode_test(teststr, expstr, ',', 0xFFFD, '.', 0xE4);
}

BOOST_AUTO_TEST_CASE(bom_not_stripped)
{
  // HTML5 8.2.2
  // The leading U+FEFF BYTE ORDER MARK CHARACTER if present must not
  // be stripped by the byte decoder. It's stripped by the input
  // preprocessor
  std::string teststr = "\xEF\xBB\xBFhello";
  std::string expstr = ",hello";

  decode_test(teststr, expstr, ',', 0xFEFF);
}

BOOST_AUTO_TEST_CASE(early_eof)
{
  std::string teststr = "hello w\xD1";
  std::string expstr = "hello w,";

  decode_test(teststr, expstr, ',', 0xFFFD);
}

BOOST_AUTO_TEST_SUITE_END()
