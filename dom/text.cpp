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

#include "text.hpp"
#include "document.hpp"
#include "exception.hpp"
#include "graphics.hpp"

frenzy::dom::CharacterData::~CharacterData()
{
}

boost::optional<frenzy::ustring>
frenzy::dom::CharacterData::get_nodeValue() const
{
  return get_data();
}

void
frenzy::dom::CharacterData::set_nodeValue(frenzy::ustring str)
{
  // TODO: Mutation records
  datastr = str;
}

frenzy::ustring
frenzy::dom::CharacterData::get_data() const
{
  return datastr;
}

void
frenzy::dom::CharacterData::set_data(frenzy::ustring data)
{
  datastr = data;
}

size_t
frenzy::dom::CharacterData::get_length() const
{
  return datastr.size();
}

frenzy::ustring
frenzy::dom::CharacterData::substringData(int offset, int count)
{
  verify_values(offset, count);

  return datastr.substr(offset, count);
}

void
frenzy::dom::CharacterData::appendData(frenzy::ustring data)
{
  datastr.append(data);
}

void
frenzy::dom::CharacterData::insertData(int offset, frenzy::ustring data)
{
  verify_values(offset, 0);

  ustring newdata = datastr.substr(0, offset);
  newdata.append(data);
  newdata.append(datastr.substr(offset, datastr.size() - offset));

  datastr = newdata;
}

void
frenzy::dom::CharacterData::deleteData(int offset, int count)
{
  // replaceData will do offset and count checking
  replaceData(offset, count, ustring());
}

void
frenzy::dom::CharacterData::replaceData(int offset, int count, frenzy::ustring data)
{
  verify_values(offset, count);

  if (static_cast<size_t>(count) > datastr.size() - offset)
    count = datastr.size() - offset;

  ustring newdata = datastr.substr(0, offset);
  newdata.append(data);
  newdata.append(datastr.substr(offset + count, datastr.size() - offset - count));

  datastr = newdata;
}

frenzy::dom::CharacterData::CharacterData(frenzy::ustring data)
  : datastr(data)
{
}

void
frenzy::dom::CharacterData::verify_values(int offset, int count)
{
  if (offset < 0 || count < 0 || static_cast<size_t>(offset) > datastr.size())
    throw DOMException(DOMException::INDEX_SIZE_ERR);
}

frenzy::dom::Node::nodeType
frenzy::dom::Text::get_nodeType() const
{
  return Node::TEXT_NODE;
}

frenzy::ustring
frenzy::dom::Text::get_nodeName() const
{
  return "#text";
}

frenzy::dom::Nodep
frenzy::dom::Text::cloneNode(bool deep) const
{
  Textp ret = Text::create(get_data());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::Textp
frenzy::dom::Text::splitText(int offset)
{
  size_t len = get_length();
  
  // Needed checks for offset are performed by substringData and
  // deleteData.
  ustring rest = substringData(offset, len - offset);
  deleteData(offset, len - offset);

  Textp ret = get_ownerDocument()->createTextNode(rest);
  get_parentNode()->insertBefore(ret, get_nextSibling());
  return ret;
}

frenzy::dom::Textp
frenzy::dom::Text::create(frenzy::ustring data)
{
  return Textp(new Text(data));
}

void
frenzy::dom::Text::drop_graphics()
{
  drawntext.reset();
}

frenzy::vec2
frenzy::dom::Text::layout(frenzy::vec maxwidth)
{
  ensure_graphics_object();
  
  drawntext->data(get_data());
  return drawntext->autowrap(maxwidth);
}

frenzy::dom::Text::Text(frenzy::ustring data)
  : CharacterData(data)
{
}

void
frenzy::dom::Text::ensure_graphics_object()
{
  if (drawntext)
    return;

  boost::shared_ptr<graphics::factory> fact = get_ownerDocument()->graphicsfactory();
  assert(fact);

  boost::shared_ptr<graphics::translation> ownpos = get_position();
  assert(ownpos);

  drawntext = fact->create_text(ownpos);
}

frenzy::dom::Node::nodeType
frenzy::dom::Comment::get_nodeType() const
{
  return Node::COMMENT_NODE;
}

frenzy::ustring
frenzy::dom::Comment::get_nodeName() const
{
  return "#comment";
}

frenzy::dom::Nodep
frenzy::dom::Comment::cloneNode(bool deep) const
{
  Commentp ret = Comment::create(get_data());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::Commentp
frenzy::dom::Comment::create(frenzy::ustring data)
{
  return Commentp(new Comment(data));
}

frenzy::dom::Comment::Comment(frenzy::ustring data)
  : CharacterData(data)
{
}
