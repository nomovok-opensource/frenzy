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

#ifndef FRENZY_TREECONSTRUCTOR_HPP
#define FRENZY_TREECONSTRUCTOR_HPP

#include <map>
#include <vector>

#include "util/unicode.hpp"
#include "util/stringlist.hpp"
#include "dom/document.hpp"
#include "token.hpp"
#include "htmltokenizer.hpp"

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
   * The tree constructor stage interface is different from the
   * previous stages. The tree constructor requires a coupled
   * htmltokenizer for input, because tree construction needs to
   * sometimes modify the tokenizer state. Also the result of tree
   * construction is not an item that is passed around, but instead
   * the tree constructor takes a Document object to modify when
   * constructed.
   */

  namespace parser
  {
    /*
     * HTML5 8.2.5 "Tree construction"
     *
     * Fourth and last step of parsing is constructing the DOM tree
     * from the tokens.
     */
    struct treeconstructor : private boost::noncopyable
    {
      treeconstructor(dom::Documentp document);

      // TODO: Rething interface, really want to take tokenizer by a naked pointer?
      // Calls attach_destination on the tokenizer
      void couple_tokenizer(htmltokenizer* tokenizer);

      // TODO: Rethink interface on this
      dom::Documentp document();

      // Returns true when parsing has stopped.
      bool stopped() const;

    private:
      dom::Documentp doc;
      htmltokenizer* tok;

      std::vector<dom::Elementp> open_elements;
      dom::Elementp current_node();
      bool open_elements_contains(dom::Elementp elem) const;
      void remove_from_open_elements(dom::Elementp elem);
      // Pop elements until a particular context
      void clear_open_elements_to_context(stringlist ctx);
      // Insert the given node to the proper 'foster parent' in the
      // proper position according to HTML5 8.2.5.3
      void foster_parent(dom::Nodep elem);
      // Insert the character to the proper 'foster parent' in the
      // proper position
      void foster_parent(uchar u);
      dom::Elementp current_form;
      dom::Elementp head_element;
      std::vector<token> pending_table_characters;
      bool frameset_ok;
      bool ignore_next_lf;
      bool force_foster_parent;
      bool stop;

      // As per 8.2.5.1 insert an element to the proper place (current
      // node or foster parent) and returns the created element
      dom::Elementp insert_element_for(token t);
      // Insert the given node to the proper place (current node or foster parent)
      void insert_node(dom::Nodep node);
      // Creates an appropriate element
      dom::Elementp create_element_for(token t);
      // Inserts the character to the proper element (current element
      // or foster parent)
      void insert_character(uchar u);
      void append_character(uchar u, dom::Elementp elem);

      // Object in the list of "active formatting elements", represents either an element or a list marker
      struct active_formatting
      {
	// Construct as an element.
	active_formatting(dom::Elementp elem, token t)
	  : elem(elem)
	  , t(t)
	{
	  assert(t.type == TOKEN_START_TAG);
	}

	// Construct as a marker
	active_formatting()
	  : elem()
	  , t(token::make_end_of_file())
	{}

	// True if this object is a list marker
	bool is_marker() const
	{
	  return !elem;
	}

	dom::Elementp element() const
	{
	  return elem;
	}

	token gettoken() const
	{
	  return t;
	}

      private:
	dom::Elementp elem;
	token t;
      };

      std::vector<active_formatting> active_formatting_list;

      // Push either an element or a list marker to the list of active
      // formatting elements.
      void push_active_formatting(dom::Elementp elem, token t);
      void push_active_formatting_marker();
      // Reconstruct the active formatting list and insert any needed
      // new elements to the document according to HTML5 8.2.3.3
      void reconstruct_active_formatting();
      void clear_active_formatting_up_to_last_marker();
      // Returns the element that's in the active formatting list
      // after the last marker (or beginning of list if no markers
      // present). Returns a NULL handle if not found.
      dom::Elementp find_active_formatting_after_last_marker(ustring name);
      // Remove the given element from the list of active formatting
      // elements.
      void remove_from_active_formatting(dom::Elementp elem);

      // Pop elements from the stack of open elements as per HTML5 8.2.5.2
      bool needs_implied_end_tag(ustring name) const;
      void generate_implied_end_tags();
      void generate_implied_end_tags_except(ustring name);

      bool has_element_in_scope(stringlist names) const;
      bool has_element_in_scope(ustring name) const;
      bool has_element_in_button_scope(ustring name) const;
      bool has_element_in_list_scope(ustring name) const;
      bool has_element_in_table_scope(ustring name) const;
      bool has_element_in_select_scope(ustring name) const;

      static bool is_special(ustring name);

      enum parserstate
      {
	STATE_INITIAL, // 8.2.5.4.1
	STATE_BEFORE_HTML, // 8.2.5.4.2
	STATE_BEFORE_HEAD, // 8.2.5.4.3
	STATE_IN_HEAD, // 8.2.5.4.4
	STATE_IN_HEAD_NOSCRIPT, // 8.2.5.4.5
	STATE_AFTER_HEAD, // 8.2.5.4.6
	STATE_IN_BODY, // 8.2.5.4.7
	STATE_TEXT, // 8.2.5.4.8
	STATE_IN_TABLE, // 8.2.5.4.9
	STATE_IN_TABLE_TEXT, // 8.2.5.4.10
	STATE_IN_CAPTION, // 8.2.5.4.11
	STATE_IN_COLUMN_GROUP, // 8.2.5.4.12
	STATE_IN_TABLE_BODY, // 8.2.5.4.13
	STATE_IN_ROW, // 8.2.5.4.14
	STATE_IN_CELL, // 8.2.5.4.15
	STATE_IN_SELECT, // 8.2.5.4.16
	STATE_IN_SELECT_IN_TABLE, // 8.2.5.4.17
	STATE_AFTER_BODY, // 8.2.5.4.18
	STATE_IN_FRAMESET, // 8.2.5.4.19
	STATE_AFTER_FRAMESET, // 8.2.5.4.20
	STATE_AFTER_AFTER_BODY, // 8.2.5.4.21
	STATE_AFTER_AFTER_FRAMESET // 8.2.5.4.22
      } state, origstate;

      // Change the parser state according to HTML5 8.2.3.1
      void reset_insertion_mode();
      static std::map<ustring, parserstate> get_reset_mapping();

      void process_token(const token& t);

      void state_initial(const token& t);
      void state_before_html(const token& t);
      void state_before_head(const token& t);
      void state_in_head(const token& t);
      void state_in_head_noscript(const token& t);
      void state_after_head(const token& t);
      void state_in_body(const token& t);
      void state_text(const token& t);
      void state_in_table(const token& t);
      void state_in_table_text(const token& t);
      void state_in_caption(const token& t);
      void state_in_column_group(const token& t);
      void state_in_table_body(const token& t);
      void state_in_row(const token& t);
      void state_in_cell(const token& t);
      void state_in_select(const token& t);
      void state_in_select_in_table(const token& t);
      void state_after_body(const token& t);
      void state_in_frameset(const token& t);
      void state_after_frameset(const token& t);
      void state_after_after_body(const token& t);
      void state_after_after_frameset(const token& t);
    };
  }
}

#endif
