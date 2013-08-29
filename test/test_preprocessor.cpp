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

#include "parser/input_preprocessor.hpp"
#include "test_helpers.hpp"

using namespace frenzy;
using namespace frenzy::parser;
using namespace frenzy::test_helpers;

namespace
{
  // Helper function for testing preprocessing with full strings.
  // Both preinput and expected_output get modified by replacing 'from'
  // with 'to', and 'second_from' with 'second_to'.
  void preprocess_test(std::string preinput, std::string expected_output,
                       uchar from = 0, uchar to = 0,
                       uchar second_from = 0, uchar second_to = 0)
  {
    input_preprocessor proc;
    input_preprocessor attproc;
    input_preprocessor pproc;
    input_preprocessor attpproc;

    destination dest;
    destination pdest;

    attproc.attach_destination(boost::bind(&destination::receive, &dest, _1));
    attpproc.attach_destination(boost::bind(&destination::receive, &pdest, _1));
    
    urope input = ustr(preinput, from, to, second_from, second_to);
    urope expected = ustr(expected_output, from, to, second_from, second_to);
    
    // Pass as a whole
    proc.pass_characters(input);
    attproc.pass_characters(input);
    // Pass a single item at a time
    for (urope::const_iterator it = input.begin();
	 it != input.end();
	 ++it)
    {
      urope partinput(*it);
      pproc.pass_characters(partinput);
      attpproc.pass_characters(partinput);
    }

    // And pass eof
    urope eof;
    proc.pass_characters(eof);
    attproc.pass_characters(eof);
    pproc.pass_characters(eof);
    attpproc.pass_characters(eof);

    urope received = proc.complete_characters();
    urope preceived = pproc.complete_characters();
        
    BOOST_CHECK_EQUAL(received, expected);
    BOOST_CHECK_EQUAL(dest.items, expected);
    BOOST_CHECK_EQUAL(preceived, expected);
    BOOST_CHECK_EQUAL(pdest.items, expected);

    BOOST_CHECK(dest.ended);
    BOOST_CHECK(pdest.ended);
  }
}

BOOST_AUTO_TEST_SUITE(input_preprocessor_tests)

BOOST_AUTO_TEST_CASE(skip_leading_bom)
{
  // One leading U+FEFF BYTE ORDER MARK character must be ignored if
  // any are present in the input stream.
  std::string teststr = ",hello";
  std::string expstr = "hello";

  preprocess_test(teststr, expstr, ',', 0xFEFF);
}

BOOST_AUTO_TEST_CASE(skip_only_one_bom)
{
  std::string teststr = ",,hello";
  std::string expstr = ",hello";

  preprocess_test(teststr, expstr, ',', 0xFEFF);
}

BOOST_AUTO_TEST_CASE(skip_bom_only_if_leading)
{
  std::string teststr = "hel,lo";
  std::string expstr = "hel,lo";

  preprocess_test(teststr, expstr, ',', 0xFEFF);
}

BOOST_AUTO_TEST_CASE(newline_conversion)
{
  // CR (U+000D) must be converted to LF (U+000A), and LF characters that
  // immediately follow a CR must be ignored.
  std::string teststr = "hello\r\nworld\rfoo\nbar\r\r\n\nquz";
  std::string expstr = "hello\nworld\nfoo\nbar\n\n\nquz";

  preprocess_test(teststr, expstr);
}

BOOST_AUTO_TEST_CASE(newline_conversion_with_multiple_pass_calls)
{
  // As above, CRLF must be converted to LF, even if that happens with
  // multiple invocations of pass_characters.
  urope cr('\r');
  urope lf('\n');
  std::string expstr = "\n";
  urope expected(expstr);
  
  input_preprocessor proc;
  input_preprocessor attproc;
  destination dest;
  attproc.attach_destination(boost::bind(&destination::receive, &dest, _1));

  proc.pass_characters(cr);
  attproc.pass_characters(cr);

  // The CR characters should have already been emitted as LF
  urope received = proc.complete_characters();
  BOOST_CHECK_EQUAL(received, expected);
  BOOST_CHECK_EQUAL(dest.items, expected);

  bool dest_had_lf = (dest.items == expected);

  proc.pass_characters(lf);
  attproc.pass_characters(lf);

  // The LF characters that follow should be ignored
  urope morereceived = proc.complete_characters();
  BOOST_CHECK(morereceived.empty());
  BOOST_CHECK_EQUAL(dest.items, expected);
  BOOST_CHECK(dest_had_lf);
}

BOOST_AUTO_TEST_SUITE_END()
