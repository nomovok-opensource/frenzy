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

#include <set>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <boost/bind.hpp>

#include "treeconstructor.hpp"
#include "htmlnames.hpp"
#include "dom/element.hpp"
#include "dom/text.hpp"
#include "dom/pointers.hpp"
#include "util/stringlist.hpp"

using namespace frenzy::html;

namespace
{
  // TODO: MathML and SVG names not in the list
  const frenzy::stringlist elemscope =
    frenzy::stringlist(applet) + caption + html + table + td + th + marquee + object;
  const frenzy::stringlist listscope =
    elemscope + ol + ul;
  const frenzy::stringlist buttonscope =
    elemscope + button;
  const frenzy::stringlist tablescope =
    frenzy::stringlist(html) + table;

  bool has_element_in_specific_scope(const std::vector<frenzy::dom::Elementp>& stack, const frenzy::stringlist& scope, frenzy::ustring name)
  {
    for (std::vector<frenzy::dom::Elementp>::const_reverse_iterator it = stack.rbegin();
	 it != stack.rend();
	 ++it)
    {
      frenzy::ustring t = (*it)->get_localName();
      if (t == name)
	return true;
      
      if (scope.contains(t))
	return false;
    }
    
    throw std::logic_error("Should not be reached: No html element in stack");
  }

  bool has_element_in_specific_scope(const std::vector<frenzy::dom::Elementp>& stack, const frenzy::stringlist& scope, frenzy::stringlist names)
  {
    for (std::vector<frenzy::dom::Elementp>::const_reverse_iterator it = stack.rbegin();
	 it != stack.rend();
	 ++it)
    {
      frenzy::ustring t = (*it)->get_localName();
      if (names.contains(t))
	return true;
      
      if (scope.contains(t))
	return false;
    }
    
    throw std::logic_error("Should not be reached: No html element in stack");
  }

  // TODO: MathML and SVG names
  const frenzy::stringlist specialnames =
    frenzy::stringlist(address) + applet + area + article + aside + base + basefont +
    bgsound + blockquote + body + br + button + caption + center + col + colgroup +
    command + dd + details + dir + frenzy::html::div + dl + dt + embed + fieldset + figcaption +
    figure + footer + form + frame + frameset + h1 + h2 + h3 + h4 + h5 + h6 +
    head + header + hgroup + hr + frenzy::html::html + iframe + img + input + isindex + li +
    frenzy::html::link + listing + marquee + menu + meta + nav + noembed + noframes + noscript +
    object + ol + p + param + plaintext + pre + script + section + frenzy::html::select + source +
    style + summary + table + tbody + td + textarea + tfoot + th + thead + title +
    tr + track + ul + wbr + xmp;
}

frenzy::parser::treeconstructor::treeconstructor(frenzy::dom::Documentp document)
  : doc(document)
  , tok(NULL)
  , frameset_ok(true)
  , ignore_next_lf(false)
  , force_foster_parent(false)
  , stop(false)
  , state(STATE_INITIAL)
{
}

void
frenzy::parser::treeconstructor::couple_tokenizer(frenzy::parser::htmltokenizer* tokenizer)
{
  tok = tokenizer;
  tok->attach_destination(boost::bind(&treeconstructor::process_token, this, _1));
}

frenzy::dom::Documentp
frenzy::parser::treeconstructor::document()
{
  return doc;
}

bool
frenzy::parser::treeconstructor::stopped() const
{
  return stop;
}

frenzy::dom::Elementp
frenzy::parser::treeconstructor::current_node()
{
  return open_elements.back();
}

bool
frenzy::parser::treeconstructor::open_elements_contains(frenzy::dom::Elementp elem) const
{
  return std::find(open_elements.begin(), open_elements.end(), elem) != open_elements.end();
}

void
frenzy::parser::treeconstructor::remove_from_open_elements(frenzy::dom::Elementp elem)
{
  std::vector<dom::Elementp>::iterator it = std::find(open_elements.begin(), open_elements.end(), elem);
  if (it != open_elements.end())
  {
    open_elements.erase(it);
  }
}

void
frenzy::parser::treeconstructor::clear_open_elements_to_context(frenzy::stringlist ctx)
{
  while (!ctx.contains(current_node()->get_localName()))
  {
    open_elements.pop_back();
  }
}

void
frenzy::parser::treeconstructor::foster_parent(frenzy::dom::Nodep elem)
{
  std::vector<dom::Elementp>::reverse_iterator lasttable;
  for (lasttable = open_elements.rbegin();
       lasttable != open_elements.rend();
       ++lasttable)
  {
    if ((*lasttable)->get_localName() == table)
      break;
  }

  if (lasttable == open_elements.rend())
  {
    assert(!open_elements.empty());
    open_elements[0]->appendChild(elem);
    return;
  }

  if (dom::Nodep parent = (*lasttable)->get_parentNode())
  {
    if (parent->get_nodeType() == dom::Node::ELEMENT_NODE)
    {
      parent->insertBefore(elem, *lasttable);
      return;
    }
  }

  ++lasttable;
  (*lasttable)->appendChild(elem);
}

void
frenzy::parser::treeconstructor::foster_parent(frenzy::uchar u)
{
  std::vector<dom::Elementp>::reverse_iterator lasttable;
  for (lasttable = open_elements.rbegin();
       lasttable != open_elements.rend();
       ++lasttable)
  {
    if ((*lasttable)->get_localName() == table)
      break;
  }

  if (lasttable == open_elements.rend())
  {
    assert(!open_elements.empty());
    append_character(u, open_elements[0]);
    return;
  }

  if (dom::Nodep parent = (*lasttable)->get_parentNode())
  {
    if (parent->get_nodeType() == dom::Node::ELEMENT_NODE)
    {
      if (dom::Nodep prevchild = (*lasttable)->get_previousSibling())
      {
	if (prevchild->get_nodeType() == dom::Node::TEXT_NODE)
	{
	  dom_cast<dom::Text>(prevchild)->appendData(u);
	  return;
	}
      }
      dom::Textp t = doc->createTextNode(u);
      parent->insertBefore(t, *lasttable);
      return;
    }
  }

  ++lasttable;
  append_character(u, *lasttable);
}

frenzy::dom::Elementp
frenzy::parser::treeconstructor::insert_element_for(frenzy::parser::token t)
{
  dom::Elementp ret = create_element_for(t);

  insert_node(ret);
  open_elements.push_back(ret);

  // TODO: Handle form owner stuff
  
  return ret;
}

void
frenzy::parser::treeconstructor::insert_node(frenzy::dom::Nodep node)
{
  static const stringlist tagnames =
    stringlist(table) + tbody + tfoot + thead + tr;
  if (force_foster_parent && tagnames.contains(current_node()->get_localName()))
  {
    return foster_parent(node);
  }

  current_node()->appendChild(node);
}

frenzy::dom::Elementp
frenzy::parser::treeconstructor::create_element_for(frenzy::parser::token t)
{
  assert(t.type == TOKEN_START_TAG);

  dom::Elementp ret = doc->createElement(t.tagname);
  for (std::map<ustring, ustring>::const_iterator it = t.attributes.begin();
       it != t.attributes.end();
       ++it)
  {
    ret->setAttribute(it->first, it->second);
  }

  return ret;
}

void
frenzy::parser::treeconstructor::insert_character(frenzy::uchar u)
{
  static const stringlist tagnames =
    stringlist(table) + tbody + tfoot + thead + tr;
  if (force_foster_parent && tagnames.contains(current_node()->get_localName()))
  {
    return foster_parent(u);
  }

  append_character(u, current_node());
}

void
frenzy::parser::treeconstructor::append_character(frenzy::uchar u, frenzy::dom::Elementp elem)
{
  if (elem->hasChildNodes())
  {
    dom::Nodep lastchild = elem->get_lastChild();
    if (lastchild->get_nodeType() == dom::Node::TEXT_NODE)
    {
      dom_cast<dom::Text>(lastchild)->appendData(u);
      return;
    }
  }

  dom::Textp t = doc->createTextNode(u);
  elem->appendChild(t);
}

void
frenzy::parser::treeconstructor::push_active_formatting(frenzy::dom::Elementp elem, frenzy::parser::token t)
{
  active_formatting_list.push_back(active_formatting(elem, t));
}

void
frenzy::parser::treeconstructor::push_active_formatting_marker()
{
  active_formatting_list.push_back(active_formatting());
}

void
frenzy::parser::treeconstructor::reconstruct_active_formatting()
{
  // HTML5 8.2.3.3

  if (active_formatting_list.empty())
    return;

  std::vector<active_formatting>::iterator entry = active_formatting_list.end();
  --entry;

  if (entry->is_marker() || open_elements_contains(entry->element()))
    return;

  while (true)
  {
    if (entry == active_formatting_list.begin())
      break;

    --entry;

    if (entry->is_marker() || open_elements_contains(entry->element()))
    {
      ++entry;
      break;
    }
  }

  for (; entry != active_formatting_list.end(); ++entry)
  {
    token t = entry->gettoken();
    dom::Elementp elem = insert_element_for(t);
    *entry = active_formatting(elem, t);
  }
}

void
frenzy::parser::treeconstructor::clear_active_formatting_up_to_last_marker()
{
  while (!active_formatting_list.empty())
  {
    bool was_marker = active_formatting_list.back().is_marker();
    active_formatting_list.pop_back();
    if (was_marker)
      return;
  }
}

frenzy::dom::Elementp
frenzy::parser::treeconstructor::find_active_formatting_after_last_marker(frenzy::ustring name)
{
  for (std::vector<active_formatting>::const_reverse_iterator it = active_formatting_list.rbegin();
       it != active_formatting_list.rend();
       ++it)
  {
    if (it->is_marker())
      return dom::Elementp();

    if (it->element()->get_localName() == name)
      return it->element();
  }

  return dom::Elementp();
}

void
frenzy::parser::treeconstructor::remove_from_active_formatting(frenzy::dom::Elementp elem)
{
  for (std::vector<active_formatting>::iterator it = active_formatting_list.begin();
       it != active_formatting_list.end();
       ++it)
  {
    if (it->element() == elem)
    {
      active_formatting_list.erase(it);
      return;
    }
  }
}

bool
frenzy::parser::treeconstructor::needs_implied_end_tag(frenzy::ustring name) const
{
  // HTML5 8.2.5.2
  static const stringlist list =
    stringlist(dd) + dt + li + option + optgroup + p + rp + rt;

  return list.contains(name);
}

void
frenzy::parser::treeconstructor::generate_implied_end_tags()
{
  while (needs_implied_end_tag(current_node()->get_localName()))
  {
    open_elements.pop_back();
  }
}

void
frenzy::parser::treeconstructor::generate_implied_end_tags_except(frenzy::ustring name)
{
  while (current_node()->get_localName() != name && needs_implied_end_tag(current_node()->get_localName()))
  {
    open_elements.pop_back();
  }
}

bool
frenzy::parser::treeconstructor::has_element_in_scope(frenzy::stringlist names) const
{
  return has_element_in_specific_scope(open_elements, elemscope, names);
}

bool
frenzy::parser::treeconstructor::has_element_in_scope(frenzy::ustring name) const
{
  return has_element_in_specific_scope(open_elements, elemscope, name);
}

bool
frenzy::parser::treeconstructor::has_element_in_button_scope(frenzy::ustring name) const
{
  return has_element_in_specific_scope(open_elements, buttonscope, name);
}

bool
frenzy::parser::treeconstructor::has_element_in_list_scope(frenzy::ustring name) const
{
  return has_element_in_specific_scope(open_elements, listscope, name);
}

bool
frenzy::parser::treeconstructor::has_element_in_table_scope(frenzy::ustring name) const
{
  return has_element_in_specific_scope(open_elements, tablescope, name);
}

bool
frenzy::parser::treeconstructor::has_element_in_select_scope(frenzy::ustring name) const
{
  for (std::vector<frenzy::dom::Elementp>::const_reverse_iterator it = open_elements.rbegin();
       it != open_elements.rend();
       ++it)
  {
    frenzy::ustring t = (*it)->get_localName();
    if (t == name)
      return true;
    
    if (t != optgroup && t != option)
      return false;
  }
  
  throw std::logic_error("Should not be reached: No html element in stack");
}

bool
frenzy::parser::treeconstructor::is_special(frenzy::ustring name)
{
  return specialnames.contains(name);
}

void
frenzy::parser::treeconstructor::reset_insertion_mode()
{
  // TODO: This needs to be the proper member variable when fragment
  // parsing is implemented
  dom::Elementp mockcontext;

  bool last = false;

  std::vector<dom::Elementp>::const_iterator it = open_elements.end();
  --it;

  while (true)
  {
    dom::Elementp node = *it;

    if (it == open_elements.begin())
    {
      last = true;
      node = mockcontext;
    }
    
    ustring name = node->get_localName();

    // A special case
    static const stringlist tdth = stringlist(td) + th;
    if (!last && tdth.contains(name))
    {
      state = STATE_IN_CELL;
      return;
    }
    
    static const std::map<ustring, parserstate> mapping = get_reset_mapping();
    
    std::map<ustring, parserstate>::const_iterator it = mapping.find(name);

    if (it != mapping.end())
    {
      state = it->second;
      return;
    }

    if (last)
    {
      state = STATE_IN_BODY;
      return;
    }

    --it;
  }
}

std::map<frenzy::ustring, frenzy::parser::treeconstructor::parserstate>
frenzy::parser::treeconstructor::get_reset_mapping()
{
  std::map<frenzy::ustring, frenzy::parser::treeconstructor::parserstate> ret;

  ret[html::select] = STATE_IN_SELECT;
  ret[tr] = STATE_IN_ROW;
  ret[tbody] = STATE_IN_TABLE_BODY;
  ret[thead] = STATE_IN_TABLE_BODY;
  ret[tfoot] = STATE_IN_TABLE_BODY;
  ret[caption] = STATE_IN_CAPTION;
  ret[colgroup] = STATE_IN_COLUMN_GROUP;
  ret[table] = STATE_IN_TABLE;
  ret[head] = STATE_IN_BODY; // Not STATE_IN_HEAD
  ret[body] = STATE_IN_BODY;
  ret[frameset] = STATE_IN_FRAMESET;
  ret[html::html] = STATE_BEFORE_HEAD;

  return ret;
}

void
frenzy::parser::treeconstructor::process_token(const frenzy::parser::token& t)
{
  if (stop)
    return;

  bool was_ignore = ignore_next_lf;
  ignore_next_lf = false;

  if (was_ignore && t.type == TOKEN_CHARACTER && t.character == 0x0A) // LF
    return;

  if (open_elements.empty() // no current node
      || true // At the moment all elements are in the HTML namespace. See below.
      /* TODO:
	 || current node . namespaceURI == HTMLNameSpace
	 || (current node . is_mathml_text_integration_point && t.type == start tag && t.name != mglyph && t.name != malignmark)
	 || (current node . is_mathml_text_integration_point && t.type == character token)
	 || (current node . type == annotation-xml && current_node . namespaceURI == mathmlNameSpace && t.type == start_tag && t.tagname == svg)
	 || (current node . is_html_integration_point && t.type == start tag)
	 || (current node . is_html_integration_point && t.type == character token)
      */
      || t.type == TOKEN_END_OF_FILE)
  {
    switch (state)
    {
    case STATE_INITIAL:
      return state_initial(t);
    case STATE_BEFORE_HTML:
      return state_before_html(t);
    case STATE_BEFORE_HEAD:
      return state_before_head(t);
    case STATE_IN_HEAD:
      return state_in_head(t);
    case STATE_IN_HEAD_NOSCRIPT:
      return state_in_head_noscript(t);
    case STATE_AFTER_HEAD:
      return state_after_head(t);
    case STATE_IN_BODY:
      return state_in_body(t);
    case STATE_TEXT:
      return state_text(t);
    case STATE_IN_TABLE:
      return state_in_table(t);
    case STATE_IN_TABLE_TEXT:
      return state_in_table_text(t);
    case STATE_IN_CAPTION:
      return state_in_caption(t);
    case STATE_IN_COLUMN_GROUP:
      return state_in_column_group(t);
    case STATE_IN_TABLE_BODY:
      return state_in_table_body(t);
    case STATE_IN_ROW:
      return state_in_row(t);
    case STATE_IN_CELL:
      return state_in_cell(t);
    case STATE_IN_SELECT:
      return state_in_select(t);
    case STATE_IN_SELECT_IN_TABLE:
      return state_in_select_in_table(t);
    case STATE_AFTER_BODY:
      return state_after_body(t);
    case STATE_IN_FRAMESET:
      return state_in_frameset(t);
    case STATE_AFTER_FRAMESET:
      return state_after_frameset(t);
    case STATE_AFTER_AFTER_BODY:
      return state_after_after_body(t);
    case STATE_AFTER_AFTER_FRAMESET:
      return state_after_after_frameset(t);
    default:
      // Both assert and throw so the error is not ignored in release
      // builds
      assert(false && "Should not be reached");
      throw std::logic_error("Tree constructor state unknown");
    }
  }
  else
  {
    // TODO: Process according to the rules for parsing tokens in foreign content (svg, mathml, etc.), 8.2.5.5
  }
}

void
frenzy::parser::treeconstructor::state_initial(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      // Ignore the token
      return;
    default:
      break;
    }
    break;
  case TOKEN_COMMENT:
    doc->appendChild(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Parse errors depending on the doctype token contents
    // TODO: Quirks-mode for the document based on the token contents
    // TODO: Creation of a DocumentType node, appending it to document,
    // setting it to the document's 'doctype' attribute
    state = STATE_BEFORE_HTML;
    return;
  default:
    break;
  }

  // TODO: If document is not an iframe srcdoc document, should
  // produce a parse error and set the document to quirks mode.
  state = STATE_BEFORE_HTML;
  return process_token(t);
}

void
frenzy::parser::treeconstructor::state_before_html(const token& t)
{
  switch (t.type)
  {
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_COMMENT:
    doc->appendChild(doc->createComment(t.comment));
    return;
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      // Ignore the token
      return;
    default:
      break;
    }
    break;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
    {
      dom::Elementp elem = create_element_for(t);
      doc->appendChild(elem);
      open_elements.push_back(elem);
      // TODO: Run the application cache selection here, as per 5.7.5
      // depending on the element's manifest attribute
      state = STATE_BEFORE_HEAD;
      return;
    }
    break;
  case TOKEN_END_TAG:
    {
      static const stringlist tagnames = stringlist(head) + body + html::html + br;
      if (tagnames.contains(t.tagname))
      {
	break;
      }
    }

    // TODO: Should produce a parse error
    // Ignore the token
    return;
  default:
    break;
  }

  dom::Elementp elem = create_element_for(token::make_start_tag(html::html));
  doc->appendChild(elem);
  open_elements.push_back(elem);
  // TODO: Application cache selection with no manifest
  state = STATE_BEFORE_HEAD;
  process_token(t);
}

void
frenzy::parser::treeconstructor::state_before_head(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      // Ignore the token
      return;
    default:
      break;
    }
    break;
  case TOKEN_COMMENT:
    insert_node(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
      return state_in_body(t);

    if (t.tagname == head)
    {
      dom::Elementp h = insert_element_for(t);
      head_element = h;
      state = STATE_IN_HEAD;
      return;
    }
    
    break;
  case TOKEN_END_TAG:
    {
      static const stringlist tagnames = stringlist(head) + body + html::html + br;
      if (tagnames.contains(t.tagname))
      {
	break;
      }
    }

    // TODO: Should produce a parse error
    // Ignore the token
    return;
  default:
    break;
  }

  // Process an implied <head>, then reprocess t
  process_token(token::make_start_tag(head));
  process_token(t);
}

void
frenzy::parser::treeconstructor::state_in_head(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      insert_character(t.character);
      return;
    default:
      break;
    }
    break;
  case TOKEN_COMMENT:
    insert_node(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
      return state_in_body(t);

    {
      static const stringlist tagnames = stringlist(base) + basefont + bgsound + command + html::link;
      if (tagnames.contains(t.tagname))
      {
	insert_element_for(t);
	open_elements.pop_back();
	// TODO: Acknowledge self-closing flag
	return;
      }
    }

    if (t.tagname == meta)
    {
      insert_element_for(t);
      open_elements.pop_back();
      // TODO: Acknowledge self-closing flag
      // TODO: Change character encoding if confidence tentative and appropriate attributes are set
      return;
    }

    if (t.tagname == title)
    {
      // HTML5 8.2.5.1, generic RCDATA element parsing
      insert_element_for(t);
      tok->change_state(htmltokenizer::STATE_RCDATA);
      origstate = state;
      state = STATE_TEXT;
      return;
    }
    
    if ((t.tagname == noscript && true /* scripting flag enabled */)
	|| t.tagname == noframes
	|| t.tagname == style)
    {
      // HTML5 8.2.5.1, generic raw text element parsing
      insert_element_for(t);
      tok->change_state(htmltokenizer::STATE_RAWTEXT);
      origstate = state;
      state = STATE_TEXT;
      return;
    }

    if (t.tagname == noscript && false /* scripting flag disabled */)
    {
      insert_element_for(t);
      state = STATE_IN_HEAD_NOSCRIPT;
      return;
    }

    if (t.tagname == script)
    {
      dom::Elementp scr = create_element_for(t);
      // TODO: Mark scr as parser-inserted and set its force-async to false
      // TODO: If parser created for fragment parsing, mark scr as already-started
      current_node()->appendChild(scr);
      open_elements.push_back(scr);
      tok->change_state(htmltokenizer::STATE_SCRIPT_DATA);
      origstate = state;
      state = STATE_TEXT;
      return;
    }

    if (t.tagname == head)
    {
      // TODO: Should produce a parse error
      // Ignore the token
      return;
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == head)
    {
      open_elements.pop_back();
      state = STATE_AFTER_HEAD;
      return;
    }

    if (t.tagname == body
	|| t.tagname == html::html
	|| t.tagname == br)
    {
      break;
    }

    // TODO: Should produce a parse error
    // Ignore the token
    return;
  default:
    break;
  }

  // Process an implied </head>, then reprocess t
  process_token(token::make_end_tag(head));
  process_token(t);
}

void
frenzy::parser::treeconstructor::state_in_head_noscript(const token& t)
{
  switch (t.type)
  {
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
    {
      return state_in_body(t);
    }

    {
      static const stringlist tagnames =
	stringlist(basefont) + bgsound + html::link + meta + noframes + style;
      if (tagnames.contains(t.tagname))
      {
	return state_in_head(t);
      }
    }

    if (t.tagname == head || t.tagname == noscript)
    {
      // TODO: Should produce a parse error
      // Ignore the token
      return;
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == noscript)
    {
      assert(current_node()->get_localName() == noscript);

      open_elements.pop_back();

      assert(current_node()->get_localName() == head);

      state = STATE_IN_HEAD;
      return;
    }

    if (t.tagname == br)
    {
      break;
    }

    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      return state_in_head(t);
    default:
      break;
    }
    break;
  case TOKEN_COMMENT:
    return state_in_head(t);
  default:
    break;
  }

  // TODO: Should produce a parse error
  // Process implied </noscript>
  process_token(token::make_end_tag(noscript));
  process_token(t);
}

void
frenzy::parser::treeconstructor::state_after_head(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      insert_character(t.character);
      return;
    default:
      break;
    }
    break;
  case TOKEN_COMMENT:
    insert_node(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
      return state_in_body(t);

    if (t.tagname == body)
    {
      insert_element_for(t);
      frameset_ok = false;
      state = STATE_IN_BODY;
      return;
    }

    if (t.tagname == frameset)
    {
      insert_element_for(t);
      state = STATE_IN_FRAMESET;
      return;
    }

    {
      static const stringlist tagnames =
	stringlist(base) + basefont + bgsound + html::link +
	meta + noframes + script + style + title;
      if (tagnames.contains(t.tagname))
      {
	// TODO: Should produce a parse error
	open_elements.push_back(head_element);
	state_in_head(t);
	remove_from_open_elements(head_element);
	return;
      }
    }

    if (t.tagname == head)
    {
      // TODO: Should produce a parse error
      // Ignore the token
      return;
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == body
	|| t.tagname == html::html
	|| t.tagname == br)
    {
      break;
    }

    // TODO: Should produce a parse error
    // Ignore the token
    return;
  default:
    break;
  }

  // Process an implied <body>
  process_token(token::make_start_tag(body));
  frameset_ok = true;
  process_token(t);
}

void
frenzy::parser::treeconstructor::state_in_body(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x00: // Nul
      // TODO: Should produce a parse error
      // Ignore the token
      return;
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      reconstruct_active_formatting();
      insert_character(t.character);
      return;
    default:
      reconstruct_active_formatting();
      insert_character(t.character);
      frameset_ok = false;
      return;
    }
    break;
  case TOKEN_COMMENT:
    insert_node(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
    {
      // TODO: Should produce a parse error
      dom::Elementp top = open_elements.front();

      for (std::map<ustring, ustring>::const_iterator it = t.attributes.begin();
	   it != t.attributes.end();
	   ++it)
      {
	if (!top->hasAttribute(it->first))
	{
	  top->setAttribute(it->first, it->second);
	}
      }

      return;
    }

    {
      static const stringlist tagnames =
	stringlist(base) + basefont + bgsound + command +
	html::link + meta + noframes + script + style + title;
      if (tagnames.contains(t.tagname))
      {
	return state_in_head(t);
      }
    }

    if (t.tagname == body)
    {
      // TODO: Should produce a parse error
      if (open_elements.size() == 1 || open_elements[1]->get_localName() != body)
      {
	// Ignore the token
	return;
      }

      frameset_ok = false;

      dom::Elementp body = open_elements[1];
      for (std::map<ustring, ustring>::const_iterator it = t.attributes.begin();
	   it != t.attributes.end();
	   ++it)
      {
	if (!body->hasAttribute(it->first))
	{
	  body->setAttribute(it->first, it->second);
	}
      }
      
      return;
    }

    if (t.tagname == frameset)
    {
      // TODO: Should produce a parse error
      if (open_elements.size() == 1 || open_elements[1]->get_localName() != body)
      {
	// Ignore the token
	return;
      }

      if (!frameset_ok)
      {
	// Ignore the token
	return;
      }

      dom::Elementp body = open_elements[1];
      if (dom::Nodep p = body->get_parentNode())
      {
	p->removeChild(body);
      }

      open_elements.resize(1);
      insert_element_for(t);
      state = STATE_IN_FRAMESET;

      return;
    }

    {
      static const stringlist tagnames =
	stringlist(address) + article + aside + blockquote + center +
	details + dialog + dir + html::div + dl + fieldset + figcaption +
	figure + footer + header + hgroup + menu + nav + ol + p +
	section + summary + ul;
      if (tagnames.contains(t.tagname))
      {
	if (has_element_in_button_scope(p))
	{
	  // Process implied </p>
	  process_token(token::make_end_tag(p));
	}
	
	insert_element_for(t);
	return;
      }
    }

    {
      static const stringlist tagnames =
	stringlist(h1) + h2 + h3 + h4 + h5 + h6;
      if (tagnames.contains(t.tagname))
      {
	if (has_element_in_button_scope(p))
	{
	  // Process implied </p>
	  process_token(token::make_end_tag(p));
	}
	
	dom::Elementp current = current_node();
	ustring name = current->get_localName();
	if (tagnames.contains(name))
	{
	  // TODO: Should produce a parse error
	  open_elements.pop_back();
	}
	
	insert_element_for(t);
	return;
      }
    }

    if (t.tagname == pre
	|| t.tagname == listing)
    {
      if (has_element_in_button_scope(p))
      {
	// Process implied </p>
	process_token(token::make_end_tag(p));
      }
      
      insert_element_for(t);
      ignore_next_lf = true;
      
      return;
    }

    if (t.tagname == form)
    {
      if (current_form)
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
      
      if (has_element_in_button_scope(p))
      {
	// Process implied </p>
	process_token(token::make_end_tag(p));
      }

      dom::Elementp elem = insert_element_for(t);
      current_form = elem;
      return;
    }

    if (t.tagname == li)
    {
      frameset_ok = false;

      for (std::vector<dom::Elementp>::const_reverse_iterator it = open_elements.rbegin();
	   it != open_elements.rend();
	   ++it)
      {
	ustring name = (*it)->get_localName();
	
	if (name == t.tagname)
	{
	  // Process implied </li>
	  process_token(token::make_end_tag(t.tagname));
	  break;
	}
	
	static const stringlist exclude = stringlist(address) + html::div + p;
	if (is_special(name) && !exclude.contains(name))
	{
	  break;
	}
      }

      if (has_element_in_button_scope(p))
      {
	// Process implied </p>
	process_token(token::make_end_tag(p));
      }

      insert_element_for(t);
      return;
    }
    
    if (t.tagname == dd
	|| t.tagname == dt)
    {
      frameset_ok = false;
      
      for (std::vector<dom::Elementp>::const_reverse_iterator it = open_elements.rbegin();
	   it != open_elements.rend();
	   ++it)
      {
	ustring name = (*it)->get_localName();
	if (name == dd || name == dt)
	{
	  // Process implied end tag
	  process_token(token::make_end_tag(name));
	  break;
	}
	
	if (is_special(name) && name != address && name != html::div && name != p)
	{
	  break;
	}
      }
      
      if (has_element_in_button_scope(p))
      {
	// Process implied </p>
	process_token(token::make_end_tag(p));
      }
      
      insert_element_for(t);
      return;
    }

    if (t.tagname == plaintext)
    {
      if (has_element_in_button_scope(p))
      {
	// Process implied </p>
	process_token(token::make_end_tag(p));
      }

      insert_element_for(t);
      tok->change_state(htmltokenizer::STATE_PLAINTEXT);
      // Note: Only character tokens will be emitted from the
      // tokenizer from this point onwards, since there's no way to
      // switch out of PLAINTEXT state.
      return;
    }

    if (t.tagname == button)
    {
      if (has_element_in_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Process implied </button>
	process_token(token::make_end_tag(t.tagname));
	return process_token(t);
      }

      reconstruct_active_formatting();
      insert_element_for(t);
      frameset_ok = false;
      return;
    }

    if (t.tagname == a)
    {
      if (dom::Elementp elem = find_active_formatting_after_last_marker(t.tagname))
      {
	// TODO: Should produce a parse error
	// Process implied </a>
	process_token(token::make_end_tag(t.tagname));

	remove_from_active_formatting(elem);
	remove_from_open_elements(elem);
      }

      reconstruct_active_formatting();

      dom::Elementp elem = insert_element_for(t);
      push_active_formatting(elem, t);
      return;
    }

    {
      static const stringlist tagnames =
	stringlist(b) + big + code + em + font + i + s + small + strike + strong + tt + u;
      if (tagnames.contains(t.tagname))
      {
	reconstruct_active_formatting();
	
	dom::Elementp elem = insert_element_for(t);
	push_active_formatting(elem, t);
	
	return;
      }
    }

    if (t.tagname == nobr)
    {
      reconstruct_active_formatting();
      if (has_element_in_scope(nobr))
      {
	// TODO: Should produce a parse error
	// Process implied </nobr>
	process_token(token::make_end_tag(nobr));
	reconstruct_active_formatting();
      }

      dom::Elementp elem = insert_element_for(t);
      push_active_formatting(elem, t);
      return;
    }

    if (t.tagname == applet
	|| t.tagname == marquee
	|| t.tagname == object)
    {
      reconstruct_active_formatting();
      insert_element_for(t);
      push_active_formatting_marker();
      frameset_ok = false;
      return;
    }

    if (t.tagname == table)
    {
      // TODO: Do this only if document not in quirks mode
      if (has_element_in_button_scope(p))
      {
	// Process implied </p>
	process_token(token::make_end_tag(p));
      }

      insert_element_for(t);
      frameset_ok = false;
      state = STATE_IN_TABLE;
      return;
    }
    
    {
      static const stringlist tagnames = stringlist(area) + br + embed + img + keygen + wbr;
      if (tagnames.contains(t.tagname))
      {
	reconstruct_active_formatting();
	
	insert_element_for(t);
	open_elements.pop_back();
	
	// TODO: Acknowledge self-closing flag
	frameset_ok = false;
	return;
      }
    }

    if (t.tagname == input)
    {
      reconstruct_active_formatting();
      
      insert_element_for(t);
      open_elements.pop_back();

      // TODO: Acknowledge self-closing flag

      std::map<ustring, ustring>::const_iterator it = t.attributes.find(type);
      // TODO: hidden should be case-insensitive
      if (it == t.attributes.end() || it->second != hidden)
      {
	frameset_ok = false;
      }

      return;
    }
    
    if (t.tagname == param
	|| t.tagname == source
	|| t.tagname == track)
    {
      insert_element_for(t);
      open_elements.pop_back();
      // TODO: Acknowledge self-closing flag
      return;
    }

    if (t.tagname == hr)
    {
      if (has_element_in_button_scope(p))
      {
	// Process implied </p>
	process_token(token::make_end_tag(p));
      }

      insert_element_for(t);
      open_elements.pop_back();
      // TODO: Acknowledge self-closing flag
      frameset_ok = false;
      return;
    }
    
    if (t.tagname == image)
    {
      // TODO: Should produce a parse error
      token copy = t;
      copy.tagname = img;
      return process_token(copy);
      // The spec specifically says: Don't ask
    }
    
    if (t.tagname == isindex)
    {
      // TODO: Implement
      return;
    }
    
    if (t.tagname == textarea)
    {
      insert_element_for(t);
      ignore_next_lf = true;

      // HTML5 8.2.5.1, generic RCDATA element parsing
      tok->change_state(htmltokenizer::STATE_RCDATA);
      origstate = state;
      frameset_ok = false;
      state = STATE_TEXT;
      return;
    }

    if (t.tagname == xmp)
    {
      if (has_element_in_button_scope(p))
      {
	// Process implied </p>
	process_token(token::make_end_tag(p));
      }
      
      reconstruct_active_formatting();
      frameset_ok = false;

      // HTML5 8.2.5.1, generic raw text element parsing
      insert_element_for(t);
      tok->change_state(htmltokenizer::STATE_RAWTEXT);
      origstate = state;
      state = STATE_TEXT;
      return;
    }
    
    if (t.tagname == iframe)
    {
      frameset_ok = false;

      // HTML5 8.2.5.1, generic raw text element parsing
      insert_element_for(t);
      tok->change_state(htmltokenizer::STATE_RAWTEXT);
      origstate = state;
      state = STATE_TEXT;
      return;
    }
    
    if (t.tagname == noembed
	     || (t.tagname == noscript && true /* scripting flag enabled */))
    {
      // HTML5 8.2.5.1, generic raw text element parsing
      insert_element_for(t);
      tok->change_state(htmltokenizer::STATE_RAWTEXT);
      origstate = state;
      state = STATE_TEXT;
      return;
    }

    if (t.tagname == html::select)
    {
      reconstruct_active_formatting();
      insert_element_for(t);
      frameset_ok = false;
      switch (state)
      {
      case STATE_IN_TABLE:
      case STATE_IN_CAPTION:
      case STATE_IN_TABLE_BODY:
      case STATE_IN_ROW:
      case STATE_IN_CELL:
	state = STATE_IN_SELECT_IN_TABLE;
	break;
      default:
	state = STATE_IN_SELECT;
	break;
      }
      return;
    }
    
    if (t.tagname == optgroup
	|| t.tagname == option)
    {
      if (current_node()->get_localName() == option)
      {
	// Process implied </option>
	process_token(token::make_end_tag(option));
      }
      
      reconstruct_active_formatting();
      insert_element_for(t);
      return;
    }

    if (t.tagname == rp
	|| t.tagname == rt)
    {
      if (has_element_in_scope(ruby))
      {
	generate_implied_end_tags();
      }
      
      if (current_node()->get_localName() != ruby)
      {
	// TODO: Should produce a parse error
      }
      
      insert_element_for(t);
      return;
    }

    if (t.tagname == math)
    {
      // TODO: Implement
      return;
    }

    if (t.tagname == svg)
    {
      // TODO: Implement
      return;
    }

    {
      static const stringlist tagnames =
	stringlist(caption) + col + colgroup + frame + head +
	tbody + td + tfoot + th + thead + tr;
      if (tagnames.contains(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
    }

    reconstruct_active_formatting();
    insert_element_for(t);
    return;
  case TOKEN_END_TAG:
    if (t.tagname == body)
    {
      if (!has_element_in_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }

      // TODO: If the stack of open elements has a node that is not a
      // dd element, a dt element, an li element, an optgroup element,
      // an option element, a p element, an rp element, an rt element,
      // a tbody element, a td element, a tfoot element, a th element,
      // a thead element, a tr element, the body element, or the html
      // element, should produce a parse error.

      state = STATE_AFTER_BODY;
      return;
    }

    if (t.tagname == html::html)
    {
      // Process implied </body>
      parserstate oldstate = state;
      process_token(token::make_end_tag(body));
      if (oldstate != state)
      {
	// Wasn't ignored
	return process_token(t);
      }

      return;
    }

    {
      static const stringlist tagnames =
	stringlist(address) + article + aside + blockquote + button +
	center + details + dialog + dir + html::div + dl + fieldset +
	figcaption + figure + footer + header + hgroup + listing +
	menu + nav + ol + pre + section + summary + ul;
      if (tagnames.contains(t.tagname))
      {
	if (!has_element_in_scope(t.tagname))
	{
	  // TODO: Should produce a parse error
	  // Ignore the token
	  return;
	}
	
	generate_implied_end_tags();
	if (current_node()->get_localName() != t.tagname)
	{
	  // TODO: Should produce a parse error
	}
	
	while (current_node()->get_localName() != t.tagname)
	{
	  open_elements.pop_back();
	}
	open_elements.pop_back();
	
	return;
      }
    }

    if (t.tagname == form)
    {
      dom::Elementp node;
      node.swap(current_form);

      if (!node || !has_element_in_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }

      generate_implied_end_tags();
      if (current_node() != node)
      {
	// TODO: Should produce a parse error
      }
      
      remove_from_open_elements(node);
      
      return;
    }

    if (t.tagname == p)
    {
      if (!has_element_in_button_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Process implied <p>
	process_token(token::make_start_tag(t.tagname));
	return process_token(t);
      }

      generate_implied_end_tags_except(t.tagname);
      if (current_node()->get_localName() != t.tagname)
      {
	// TODO: Should produce a parse error
      }

      while (current_node()->get_localName() != t.tagname)
      {
	open_elements.pop_back();
      }
      open_elements.pop_back();

      return;
    }

    if (t.tagname == li)
    {
      if (!has_element_in_list_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }

      generate_implied_end_tags_except(t.tagname);

      if (current_node()->get_localName() != t.tagname)
      {
	// TODO: Should produce a parse error
      }

      while (current_node()->get_localName() != t.tagname)
      {
	open_elements.pop_back();
      }
      open_elements.pop_back();
      
      return;
    }
    
    if (t.tagname == dd
	|| t.tagname == dt)
    {
      if (!has_element_in_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
      
      generate_implied_end_tags_except(t.tagname);
      
      if (current_node()->get_localName() != t.tagname)
      {
	// TODO: Should produce a parse error
      }
      
      while (current_node()->get_localName() != t.tagname)
      {
	open_elements.pop_back();
      }
      open_elements.pop_back();
      
      return;
    }

    {
      static const stringlist tagnames =
	stringlist(h1) + h2 + h3 + h4 + h5 + h6;
      if (tagnames.contains(t.tagname))
      {
	if (!has_element_in_scope(tagnames))
	{
	  // TODO: Should produce a parse error
	  // Ignore the token
	  return;
	}

	generate_implied_end_tags();
	
	if (current_node()->get_localName() != t.tagname)
	{
	  // TODO: Should produce a parse error
	}
	
	while (current_node()->get_localName() != t.tagname)
	{
	  open_elements.pop_back();
	}
	open_elements.pop_back();
	
	return;
      }
    }

    {
      static const stringlist tagnames =
	stringlist(a) + b + big + code + em + font + i + nobr + s + small + strike + strong + tt + u;
      if (tagnames.contains(t.tagname))
      {
	// Magic number '8' is from the spec
	for (size_t i = 0; i < 8; ++i)
	{
	  dom::Elementp elem = find_active_formatting_after_last_marker(t.tagname);

	  if (!elem)
	  {
	    // TODO: Same code as "any other end tag". Possibly refactor.
	    for (std::vector<dom::Elementp>::const_reverse_iterator it = open_elements.rbegin();
		 it != open_elements.rend();
		 ++it)
	    {
	      ustring name = (*it)->get_localName();
	      if (name == t.tagname)
	      {
		generate_implied_end_tags_except(t.tagname);
		if (current_node()->get_localName() != t.tagname)
		{
		  // TODO: Should produce a parse error
		}
		
		while (current_node()->get_localName() != t.tagname)
		{
		  open_elements.pop_back();
		}
		open_elements.pop_back();
		return;
	      }
	      
	      if (is_special(name))
	      {
		// TODO: Should produce a parse error
		// Ignore the token and stop
		return;
	      }
	    }
	    return;
	  }

	  token elemtoken = token::make_end_of_file();
	  {
	    size_t k = 0;
	    for (; k < active_formatting_list.size(); ++k)
	    {
	      if (active_formatting_list[k].element() == elem)
		break;
	    }
	    assert(k < active_formatting_list.size());
	    elemtoken = active_formatting_list[k].gettoken();
	  }

	  size_t elempos = 0;
	  for (; elempos < open_elements.size(); ++elempos)
	  {
	    if (open_elements[elempos] == elem)
	      break;
	  }

	  if (elempos >= open_elements.size())
	  {
	    // TODO: Should produce a parse error
	    remove_from_active_formatting(elem);
	    return;
	  }

	  if (!has_element_in_scope(t.tagname))
	  {
	    // TODO: Should produce a parse error
	    // Ignore the token
	    return;
	  }

	  if (elem != current_node())
	  {
	    // TODO: Should produce a parse error
	  }

	  size_t furthestblock = elempos + 1;
	  for (; furthestblock < open_elements.size(); ++furthestblock)
	  {
	    if (is_special(open_elements[furthestblock]->get_localName()))
	      break;
	  }
	  if (furthestblock >= open_elements.size())
	  {
	    while (current_node() != elem)
	      open_elements.pop_back();
	    open_elements.pop_back();

	    remove_from_active_formatting(elem);
	    return;
	  }
	
	  assert(elempos > 0);
	  size_t commonancestor = elempos - 1;

	  size_t bookmark = 0;
	  for (; bookmark < active_formatting_list.size(); ++bookmark)
	  {
	    if (active_formatting_list[bookmark].element() == elem)
	      break;
	  }

	  size_t node, lastnode;
	  node = lastnode = furthestblock;

	  // Magic number 3 is from the spec
	  for (size_t inner = 0; inner < 3; ++inner)
	  {
	    assert(node > 0);
	    --node;
	    assert(node < open_elements.size());

	    size_t nodeinactive = 0;
	    for (; nodeinactive < active_formatting_list.size(); ++nodeinactive)
	    {
	      if (active_formatting_list[nodeinactive].element() == open_elements[node])
		break;
	    }

	    if (nodeinactive >= active_formatting_list.size())
	    {
	      open_elements.erase(open_elements.begin() + node - 1);
	      continue;
	    }
	    
	    if (node == elempos)
	    {
	      break;
	    }
	    
	    token newelemtoken = active_formatting_list[nodeinactive].gettoken();
	    dom::Elementp newelem = create_element_for(newelemtoken);
	    
	    active_formatting_list[nodeinactive] = active_formatting(newelem, newelemtoken);
	    open_elements[node] = newelem;
	    
	    if (lastnode == furthestblock)
	    {
	      bookmark = nodeinactive + 1;
	    }

	    assert(lastnode < open_elements.size());
	    open_elements[node]->appendChild(open_elements[lastnode]);
	    
	    lastnode = node;
	  }

	  assert(commonancestor < open_elements.size());
	  assert(lastnode < open_elements.size());
	  assert(furthestblock < open_elements.size());

	  static const stringlist tagnames =
	    stringlist(table) + tbody + tfoot + thead + tr;
	  if (tagnames.contains(open_elements[commonancestor]->get_localName()))
	  {
	    foster_parent(open_elements[lastnode]);
	  }
	  else
	  {
	    open_elements[commonancestor]->appendChild(open_elements[lastnode]);
	  }

	  dom::Elementp anothernewelem = create_element_for(elemtoken);

	  dom::NodeListp children = open_elements[furthestblock]->get_childNodes();
	  for (size_t i = 0; i < children->get_length(); ++i)
	  {
	    dom::Nodep child = children->item(i);
	    anothernewelem->appendChild(child);
	  }
	  open_elements[furthestblock]->appendChild(anothernewelem);

	  assert(bookmark <= active_formatting_list.size());

	  // TODO: Should this be "+ bookmark + 1"? Tests are not catching it.
	  active_formatting_list.insert(active_formatting_list.begin() + bookmark, active_formatting(anothernewelem, elemtoken));
	  remove_from_active_formatting(elem);

	  open_elements.insert(open_elements.begin() + furthestblock + 1, anothernewelem);
	  remove_from_open_elements(elem);
	}

	return;
      }
    }

    if (t.tagname == applet
	|| t.tagname == marquee
	|| t.tagname == object)
    {
      if (!has_element_in_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
      
      generate_implied_end_tags();
      
      if (current_node()->get_localName() != t.tagname)
      {
	// TODO: Should produce a parse error
      }
      
      while (current_node()->get_localName() != t.tagname)
      {
	open_elements.pop_back();
      }
      open_elements.pop_back();
      
      clear_active_formatting_up_to_last_marker();
      
      return;
    }

    if (t.tagname == br)
    {
      // TODO: Should produce a parse error
      // Process implied <br>
      process_token(token::make_start_tag(t.tagname));
      return;
    }

    for (std::vector<dom::Elementp>::const_reverse_iterator it = open_elements.rbegin();
	 it != open_elements.rend();
	 ++it)
    {
      ustring name = (*it)->get_localName();
      if (name == t.tagname)
      {
	generate_implied_end_tags_except(t.tagname);
	if (current_node()->get_localName() != t.tagname)
	{
	  // TODO: Should produce a parse error
	}
	
	while (current_node()->get_localName() != t.tagname)
	{
	  open_elements.pop_back();
	}
	open_elements.pop_back();
	return;
      }
      
      if (is_special(name))
      {
	// TODO: Should produce a parse error
	// Ignore the token and stop
	return;
      }
    }

    return;
  case TOKEN_END_OF_FILE:
    {
      static const stringlist tagnames =
	stringlist(dd) + dt + li + p + tbody + td + tfoot + th + thead + tr + body + html::html;

      for (std::vector<dom::Elementp>::const_iterator it = open_elements.begin();
	   it != open_elements.end();
	   ++it)
      {
	if (!tagnames.contains((*it)->get_localName()))
	{
	  // TODO: Should produce a parse error
	}
      }
    }

    stop = true;
    return;
  }

  assert(false && "Should not be reached");
}

void
frenzy::parser::treeconstructor::state_text(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    assert(t.character != 0x00); // Nul
    insert_character(t.character);
    return;
  case TOKEN_END_OF_FILE:
    // TODO: Should produce a parse error
    if (current_node()->get_localName() == script)
    {
      // TODO: Mark current_node() as 'already started'
    }

    open_elements.pop_back();
    state = origstate;
    return process_token(t);
  case TOKEN_END_TAG:
    if (t.tagname == script)
    {
      // TODO: Implement
      // This is where the script gets executed etc.
      open_elements.pop_back();
      state = origstate;
      return;
    }

    open_elements.pop_back();
    state = origstate;
    return;
  default:
    assert(false && "Should not be reached");
    throw std::logic_error("Tokenizer emitted a tag that shouldn't be possible");
  }
}

void
frenzy::parser::treeconstructor::state_in_table(const token& t)
{
  static const stringlist tablecontext = stringlist(table) + html::html;

  switch (t.type)
  {
  case TOKEN_CHARACTER:
    {
      ustring currentname = current_node()->get_localName();
      static const stringlist tagnames =
	stringlist(table) + tbody + tfoot + thead + tr;
      if (tagnames.contains(currentname))
      {
	pending_table_characters.clear();
	origstate = state;
	state = STATE_IN_TABLE_TEXT;
	return process_token(t);
      }
    }
    break;
  case TOKEN_COMMENT:
    insert_node(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == caption)
    {
      clear_open_elements_to_context(tablecontext);
      push_active_formatting_marker();
      insert_element_for(t);
      state = STATE_IN_CAPTION;
      return;
    }

    if (t.tagname == colgroup)
    {
      clear_open_elements_to_context(tablecontext);
      insert_element_for(t);
      state = STATE_IN_COLUMN_GROUP;
      return;
    }

    if (t.tagname == col)
    {
      // Process implied <colgroup>
      process_token(token::make_start_tag(colgroup));
      return process_token(t);
    }

    if (t.tagname == tbody
	|| t.tagname == tfoot
	|| t.tagname == thead)
    {
      clear_open_elements_to_context(tablecontext);
      insert_element_for(t);
      state = STATE_IN_TABLE_BODY;
      return;
    }

    if (t.tagname == td
	|| t.tagname == th
	|| t.tagname == tr)
    {
      // Process implied <tbody>
      process_token(token::make_start_tag(tbody));
      return process_token(t);
    }

    if (t.tagname == table)
    {
      // TODO: Should produce a parse error
      // Process implied </table>
      parserstate oldstate = state;
      process_token(token::make_end_tag(table));
      if (oldstate != state)
      {
	// Wasn't ignored
	process_token(t);
      }
      return;
    }

    if (t.tagname == style
	|| t.tagname == script)
    {
      return state_in_head(t);
    }

    if (t.tagname == input)
    {
      std::map<ustring, ustring>::const_iterator it = t.attributes.find(type);
      // TODO: hidden should be case-insensitive
      if (it == t.attributes.end() || it->second != hidden)
      {
	break;
      }

      // TODO: Should produce a parse error
      insert_element_for(t);
      open_elements.pop_back();
      // TODO: Acknowledge self-closing flag
      return;
    }

    if (t.tagname == form)
    {
      // TODO: Should produce a parse error
      if (current_form)
      {
	// Ignore the token
	return;
      }

      dom::Elementp elem = insert_element_for(t);
      current_form = elem;
      open_elements.pop_back();
      return;
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == table)
    {
      if (!has_element_in_table_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }

      while (current_node()->get_localName() != table)
      {
	open_elements.pop_back();
      }
      open_elements.pop_back();

      reset_insertion_mode();
      return;
    }

    {
      static const stringlist tagnames =
	stringlist(body) + caption + col + colgroup + html::html + tbody + td + tfoot + th + thead + tr;
      if (tagnames.contains(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
    }

    break;
  case TOKEN_END_OF_FILE:
    if (current_node()->get_localName() != html::html)
    {
      // TODO: Should produce a parse error
    }

    stop = true;
    return;
  default:
    break;
  }

  // TODO: Should produce a parse error
  force_foster_parent = true;
  state_in_body(t);
  force_foster_parent = false;
}

void
frenzy::parser::treeconstructor::state_in_table_text(const token& t)
{
  if (t.type == TOKEN_CHARACTER)
  {
    if (t.character == 0x00) // Nul
    {
      // TODO: Should produce a parse error
      // Ignore the token
      return;
    }

    pending_table_characters.push_back(t);
    return;
  }

  bool anynonspace = false;
  for (std::vector<token>::const_iterator it = pending_table_characters.begin();
       it != pending_table_characters.end();
       ++it)
  {
    switch (it->character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      break;
    default:
      anynonspace = true;
      break;
    }
  }

  if (anynonspace)
  {
    force_foster_parent = true;
    for (std::vector<token>::const_iterator it = pending_table_characters.begin();
	 it != pending_table_characters.end();
	 ++it)
    {
      state_in_body(*it);
    }
    force_foster_parent = false;
  }
  else
  {
    for (std::vector<token>::const_iterator it = pending_table_characters.begin();
	 it != pending_table_characters.end();
	 ++it)
    {
      insert_character(it->character);
    }
  }

  state = origstate;
  process_token(t);
}

void
frenzy::parser::treeconstructor::state_in_caption(const token& t)
{
  switch (t.type)
  {
  case TOKEN_END_TAG:
    if (t.tagname == caption)
    {
      if (!has_element_in_table_scope(caption))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }

      generate_implied_end_tags();
      
      if (current_node()->get_localName() != caption)
      {
	// TODO: Should produce a parse error
      }

      while (current_node()->get_localName() != caption)
      {
	open_elements.pop_back();
      }
      open_elements.pop_back();

      clear_active_formatting_up_to_last_marker();

      state = STATE_IN_TABLE;
      return;
    }

    if (t.tagname == table)
    {
      // TODO: Should produce a parse error
      // Process implied </caption>
      parserstate oldstate = state;
      process_token(token::make_end_tag(caption));
      if (oldstate != state)
      {
	// Wasn't ignored
	process_token(t);
      }
      return;
    }

    {
      static const stringlist tagnames =
	stringlist(body) + col + colgroup + tbody + td + tfoot + th + thead + tr;
      if (tagnames.contains(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
    }

    break;
  case TOKEN_START_TAG:
    {
      static const stringlist tagnames =
	stringlist(caption) + col + colgroup + tbody + td + tfoot + th + thead + tr;
      if (tagnames.contains(t.tagname))
      {
	// TODO: Should produce a parse error
	// Process implied </caption>
	parserstate oldstate = state;
	process_token(token::make_end_tag(caption));
	if (oldstate != state)
	{
	  // Wasn't ignored
	  process_token(t);
	}
	return;
      }
    }

    break;
  default:
    break;
  }

  state_in_body(t);
}

void
frenzy::parser::treeconstructor::state_in_column_group(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      insert_character(t.character);
      return;
    default:
      break;
    }
    break;
  case TOKEN_COMMENT:
    insert_node(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
    {
      return state_in_body(t);
    }

    if (t.tagname == col)
    {
      insert_element_for(t);
      open_elements.pop_back();
      // TODO: Acknowledge self-closing flag
      return;
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == colgroup)
    {
      if (current_node()->get_localName() == html::html)
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }

      assert(current_node()->get_localName() == colgroup);
      
      open_elements.pop_back();
      state = STATE_IN_TABLE;
      return;
    }

    if (t.tagname == col)
    {
      // TODO: Should produce a parse error
      // Ignore the token
      return;
    }

    break;
  case TOKEN_END_OF_FILE:
    if (current_node()->get_localName() == html::html)
    {
      stop = true;
      return;
    }

    break;
  default:
    break;
  }

  // Process implied </colgroup>
  parserstate oldstate = state;
  process_token(token::make_end_tag(colgroup));
  if (oldstate != state)
  {
    // Wasn't ignored
    process_token(t);
  }
}

void
frenzy::parser::treeconstructor::state_in_table_body(const token& t)
{
  static const stringlist tablebodycontext =
    stringlist(tbody) + tfoot + thead + html::html;

  switch (t.type)
  {
  case TOKEN_START_TAG:
    if (t.tagname == tr)
    {
      clear_open_elements_to_context(tablebodycontext);
      insert_element_for(t);
      state = STATE_IN_ROW;
      return;
    }

    if (t.tagname == th || t.tagname == td)
    {
      // TODO: Should produce a parse error
      // Process implied <tr>
      process_token(token::make_start_tag(tr));
      return process_token(t);
    }

    {
      static const stringlist tagnames =
	stringlist(caption) + col + colgroup + tbody + tfoot + thead;
      if (tagnames.contains(t.tagname))
      {
	if (!has_element_in_table_scope(tbody) &&
	    !has_element_in_table_scope(thead) &&
	    !has_element_in_table_scope(tfoot))
	{
	  // TODO: Should produce a parse error
	  // Ignore the token
	  return;
	}

	clear_open_elements_to_context(tablebodycontext);
	// Process implied end tag
	process_token(token::make_end_tag(current_node()->get_localName()));
	return process_token(t);
      }
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == tbody
	|| t.tagname == tfoot
	|| t.tagname == thead)
    {
      if (!has_element_in_table_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
      
      clear_open_elements_to_context(tablebodycontext);
      open_elements.pop_back();
      state = STATE_IN_TABLE;
      return;
    }

    if (t.tagname == table)
    {
      if (!has_element_in_table_scope(tbody) &&
	  !has_element_in_table_scope(thead) &&
	  !has_element_in_table_scope(tfoot))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
      
      clear_open_elements_to_context(tablebodycontext);
      // Process implied end tag
      process_token(token::make_end_tag(current_node()->get_localName()));
      return process_token(t);
    }

    {
      static const stringlist tagnames =
	stringlist(body) + caption + col + colgroup + html::html + td + th + tr;
      if (tagnames.contains(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
    }

    break;
  default:
    break;
  }

  state_in_table(t);
}

void
frenzy::parser::treeconstructor::state_in_row(const token& t)
{
  static const stringlist tablerowcontext =
    stringlist(tr) + html::html;

  switch (t.type)
  {
  case TOKEN_START_TAG:
    if (t.tagname == th || t.tagname == td)
    {
      clear_open_elements_to_context(tablerowcontext);
      insert_element_for(t);
      state = STATE_IN_CELL;
      push_active_formatting_marker();
      return;
    }

    {
      static const stringlist tagnames =
	stringlist(caption) + col + colgroup + tbody + tfoot + thead + tr;
      if (tagnames.contains(t.tagname))
      {
	// Process implied </tr>
	parserstate oldstate = state;
	process_token(token::make_end_tag(tr));
	if (oldstate != state)
	{
	  // Wasn't ignored
	  process_token(t);
	}
	return;
      }
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == tr)
    {
      if (!has_element_in_table_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }

      clear_open_elements_to_context(tablerowcontext);
      open_elements.pop_back();
      state = STATE_IN_TABLE_BODY;
      return;
    }

    if (t.tagname == table)
    {
      // Process implied </tr>
      parserstate oldstate = state;
      process_token(token::make_end_tag(tr));
      if (oldstate != state)
      {
	// Wasn't ignored
	process_token(t);
      }
      return;
    }

    if (t.tagname == tbody
	|| t.tagname == tfoot
	|| t.tagname == thead)
    {
      if (!has_element_in_table_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
      
      // Process implied </tr>
      process_token(token::make_end_tag(tr));
      return process_token(t);
    }

    {
      static const stringlist tagnames =
	stringlist(body) + caption + col + colgroup + html::html + td + th;
      if (tagnames.contains(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
    }
    break;
  default:
    break;
  }

  state_in_table(t);
}

void
frenzy::parser::treeconstructor::state_in_cell(const token& t)
{
  switch (t.type)
  {
  case TOKEN_START_TAG:
    {
      static const stringlist tagnames =
	stringlist(caption) + col + colgroup + tbody + td + tfoot + th + thead + tr;
      if (tagnames.contains(t.tagname))
      {
	if (!has_element_in_table_scope(td) &&
	    !has_element_in_table_scope(th))
	{
	  // TODO: Should produce a parse error
	  // Ignore the token
	  return;
	}

	// Close the cell
	if (has_element_in_table_scope(td))
	{
	  // Process implied </td>
	  process_token(token::make_end_tag(td));
	}
	else
	{
	  // Process implied </th>
	  process_token(token::make_end_tag(th));
	}

	return process_token(t);
      }
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == td || t.tagname == th)
    {
      if (!has_element_in_table_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }

      generate_implied_end_tags();
      
      if (current_node()->get_localName() != t.tagname)
      {
	// TODO: Should produce a parse error
      }
      
      while (current_node()->get_localName() != t.tagname)
      {
	open_elements.pop_back();
      }
      open_elements.pop_back();

      clear_active_formatting_up_to_last_marker();
      state = STATE_IN_ROW;
      return;
    }

    {
      static const stringlist tagnames =
	stringlist(body) + caption + col + colgroup + html::html;
      if (tagnames.contains(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
    }

    {
      static const stringlist tagnames =
	stringlist(table) + tbody + tfoot + thead + tr;
      if (tagnames.contains(t.tagname))
      {
	if (!has_element_in_table_scope(t.tagname))
	{
	  // TODO: Should produce a parse error
	  // Ignore the token
	  return;
	}

	// Close the cell
	if (has_element_in_table_scope(td))
	{
	  // Process implied </td>
	  process_token(token::make_end_tag(td));
	}
	else
	{
	  assert(has_element_in_table_scope(th));
	  // Process implied </th>
	  process_token(token::make_end_tag(th));
	}

	return process_token(t);
      }
    }

    break;
  default:
    break;
  }

  state_in_body(t);
}

void
frenzy::parser::treeconstructor::state_in_select(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    if (t.character == 0x00) // Nul
    {
      // TODO: Should produce a parse error
      // Ignore the token
      return;
    }

    insert_character(t.character);
    return;
  case TOKEN_COMMENT:
    insert_node(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
    {
      return state_in_body(t);
    }

    if (t.tagname == option)
    {
      if (current_node()->get_localName() == option)
      {
	// Process implied </option>
	process_token(token::make_end_tag(option));
      }

      insert_element_for(t);
      return;
    }

    if (t.tagname == optgroup)
    {
      ustring current = current_node()->get_localName();
      if (current == option)
      {
	// Process implied </option>
	process_token(token::make_end_tag(option));
      }
      else if (current == optgroup)
      {
	// Process implied </optgroup>
	process_token(token::make_end_tag(optgroup));
      }

      insert_element_for(t);
      return;
    }

    if (t.tagname == html::select)
    {
      // TODO: Should produce a parse error
      // Process </select> instead
      return process_token(token::make_end_tag(html::select));
    }

    if (t.tagname == input
	|| t.tagname == keygen
	|| t.tagname == textarea)
    {
      // TODO: Should produce a parse error
      if (!has_element_in_select_scope(html::select))
      {
	// Ignore the token
	return;
      }
      
      // Process implied </select>
      process_token(token::make_end_tag(html::select));
      return process_token(t);
    }

    if (t.tagname == script)
    {
      return state_in_head(t);
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == optgroup)
    {
      assert(open_elements.size() >= 2);
      if (open_elements[open_elements.size() - 1]->get_localName() == option
	  && open_elements[open_elements.size() - 2]->get_localName() == optgroup)
      {
	// Process implied </option>
	process_token(token::make_end_tag(option));
      }

      if (current_node()->get_localName() == optgroup)
      {
	open_elements.pop_back();
      }
      else
      {
	// TODO: Should produce a parse error
      }
      return;	
    }

    if (t.tagname == option)
    {
      if (current_node()->get_localName() == option)
      {
	open_elements.pop_back();
      }
      else
      {
	// TODO: Should produce a parse error
      }
      return;
    }

    if (t.tagname == html::select)
    {
      if (!has_element_in_select_scope(t.tagname))
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }

      while (current_node()->get_localName() != html::select)
      {
	open_elements.pop_back();
      }
      open_elements.pop_back();

      reset_insertion_mode();
      return;
    }

    break;
  case TOKEN_END_OF_FILE:
    if (current_node()->get_localName() != html::html)
    {
      // TODO: Should produce a parse error
    }

    stop = true;
    return;
  default:
    break;
  }

  // TODO: Should produce a parse error
  // Ignore the token
}

void
frenzy::parser::treeconstructor::state_in_select_in_table(const token& t)
{
  static const stringlist tagnames =
    stringlist(caption) + table + tbody + tfoot + thead + tr + td + th;

  if (t.type == TOKEN_START_TAG && tagnames.contains(t.tagname))
  {
    // TODO: Should produce a parse error
    // Process implied </select>
    process_token(token::make_end_tag(html::select));
    return process_token(t);
  }

  if (t.type == TOKEN_END_TAG && tagnames.contains(t.tagname))
  {
    // TODO: Should produce a parse error
    if (has_element_in_table_scope(t.tagname))
    {
      // Process implied </select>
      process_token(token::make_end_tag(html::select));
      return process_token(t);
    }

    // Ignore the token
    return;
  }

  state_in_select(t);
}

void
frenzy::parser::treeconstructor::state_after_body(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      return state_in_body(t);
    default:
      break;
    }
    break;
  case TOKEN_COMMENT:
    assert(!open_elements.empty());
    open_elements.front()->appendChild(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
    {
      return state_in_body(t);
    }
    break;
  case TOKEN_END_TAG:
    if (t.tagname == html::html)
    {
      // TODO: If the parser was created for fragment parsing, should
      // 1) produce a parse error 2) ignore the token instead of this.
      state = STATE_AFTER_AFTER_BODY;
      return;
    }
    break;
  case TOKEN_END_OF_FILE:
    stop = true;
    return;
  default:
    break;
  }

  // TODO: Should produce a parse error
  state = STATE_IN_BODY;
  process_token(t);
}

void
frenzy::parser::treeconstructor::state_in_frameset(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      insert_character(t.character);
      return;
    default:
      break;
    }
    break;
  case TOKEN_COMMENT:
    insert_node(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
    {
      return state_in_body(t);
    }

    if (t.tagname == frameset)
    {
      insert_element_for(t);
      return;
    }

    if (t.tagname == frame)
    {
      insert_element_for(t);
      open_elements.pop_back();
      // TODO: Acknowledge self-closing flag
      return;
    }

    if (t.tagname == noframes)
    {
      return state_in_head(t);
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == frameset)
    {
      if (current_node()->get_localName() == html::html)
      {
	// TODO: Should produce a parse error
	// Ignore the token
	return;
      }
      
      open_elements.pop_back();
      
      if (true // parser not created for fragment parsing
	  && current_node()->get_localName() != frameset)
      {
	state = STATE_AFTER_FRAMESET;
      }
      
      return;
    }

    break;
  case TOKEN_END_OF_FILE:
    if (current_node()->get_localName() != html::html)
    {
      // TODO: Should produce a parse error
    }

    stop = true;
    return;
  default:
    break;
  }

  // TODO: Should produce a parse error
  // Ignore the token
}

void
frenzy::parser::treeconstructor::state_after_frameset(const token& t)
{
  switch (t.type)
  {
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      insert_character(t.character);
      return;
    default:
      break;
    }
    break;
  case TOKEN_COMMENT:
    insert_node(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    // TODO: Should produce a parse error
    // Ignore the token
    return;
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
    {
      return state_in_body(t);
    }

    if (t.tagname == noframes)
    {
      return state_in_head(t);
    }

    break;
  case TOKEN_END_TAG:
    if (t.tagname == html::html)
    {
      state = STATE_AFTER_AFTER_FRAMESET;
      return;
    }

    break;
  case TOKEN_END_OF_FILE:
    stop = true;
    return;
  default:
    break;
  }

  // TODO: Should produce a parse error
  // Ignore the token
}

void
frenzy::parser::treeconstructor::state_after_after_body(const token& t)
{
  switch (t.type)
  {
  case TOKEN_COMMENT:
    doc->appendChild(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    return state_in_body(t);
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
    {
      return state_in_body(t);
    }
    break;
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      return state_in_body(t);
    default:
      break;
    }
    break;
  case TOKEN_END_OF_FILE:
    stop = true;
    return;
  default:
    break;
  }

  // TODO: Should produce a parse error
  state = STATE_IN_BODY;
  process_token(t);
}

void
frenzy::parser::treeconstructor::state_after_after_frameset(const token& t)
{
  switch (t.type)
  {
  case TOKEN_COMMENT:
    doc->appendChild(doc->createComment(t.comment));
    return;
  case TOKEN_DOCTYPE:
    return state_in_body(t);
  case TOKEN_START_TAG:
    if (t.tagname == html::html)
    {
      return state_in_body(t);
    }

    if (t.tagname == noframes)
    {
      return state_in_head(t);
    }

    break;
  case TOKEN_CHARACTER:
    switch (t.character)
    {
    case 0x09: // Tab
    case 0x0A: // LF
    case 0x0C: // FF
    case 0x0D: // CR
    case 0x20: // Space
      return state_in_body(t);
    default:
      break;
    }
    break;
  case TOKEN_END_OF_FILE:
    stop = true;
    return;
  default:
    break;
  }

  // TODO: Should produce a parse error
  // Ignore the token
}
