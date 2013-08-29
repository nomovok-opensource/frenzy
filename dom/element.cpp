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

#include "element.hpp"
#include "document.hpp"
#include "text.hpp"
#include "exception.hpp"

frenzy::dom::Node::nodeType
frenzy::dom::Element::get_nodeType() const
{
  return Node::ELEMENT_NODE;
}

frenzy::ustring
frenzy::dom::Element::get_nodeName() const
{
  return get_tagName();
}

frenzy::ustring
frenzy::dom::Element::get_localName() const
{
  return localname;
}

frenzy::ustring
frenzy::dom::Element::get_tagName() const
{
  // TODO: If namespace prefix is not null, prepend namespace:
  ustring ret = get_localName();

  // TODO: If element in HTML namespace and node document in HTML namespace:
  ret = ascii_toupper(ret);

  return ret;
}

boost::optional<frenzy::ustring>
frenzy::dom::Element::getAttribute(frenzy::ustring name) const
{
  attributes_t::const_iterator it = attributes.find(name);
  if (it == attributes.end())
  {
    // DOM4 says return null, DOM3 says empty string.
    return ustring();
  }

  return it->second->get_value();
}

void
frenzy::dom::Element::setAttribute(frenzy::ustring name, frenzy::ustring value)
{
  attributes_t::const_iterator it = attributes.find(name);
  if (it == attributes.end())
  {
    Attrp newattr = get_ownerDocument()->createAttribute(name);
    newattr->set_ownerElement(shared_from_this());
    newattr->set_value(value);
    attributes.insert(std::make_pair(name, newattr));
    return;
  }

  it->second->set_value(value);  
}

void
frenzy::dom::Element::removeAttribute(frenzy::ustring name)
{
  attributes_t::iterator it = attributes.find(name);
  if (it != attributes.end())
    attributes.erase(it);
}

bool
frenzy::dom::Element::hasAttribute(frenzy::ustring name) const
{
  return attributes.find(name) != attributes.end();
}

frenzy::dom::Attrp
frenzy::dom::Element::getAttributeNode(frenzy::ustring name)
{
  attributes_t::iterator it = attributes.find(name);
  if (it == attributes.end())
    return Attrp();

  return it->second;
}

frenzy::dom::Attrp
frenzy::dom::Element::setAttributeNode(frenzy::dom::Attrp newAttr)
{
  if (!newAttr)
  {
    return newAttr;
  }

  if (newAttr->get_ownerDocument() != get_ownerDocument())
    throw DOMException(DOMException::WRONG_DOCUMENT_ERR);

  Elementp owner = newAttr->get_ownerElement();
  if (owner && owner != shared_from_this())
    throw DOMException(DOMException::INUSE_ATTRIBUTE_ERR);

  ustring name = newAttr->get_name();

  Attrp ret;
  attributes_t::iterator it = attributes.find(name);
  if (it != attributes.end())
  {
    ret = it->second;
    attributes.erase(it);
  }

  attributes.insert(std::make_pair(name, newAttr));
  newAttr->set_ownerElement(shared_from_this());

  return ret;
}

frenzy::dom::Attrp
frenzy::dom::Element::removeAttributeNode(frenzy::dom::Attrp oldAttr)
{
  if (!oldAttr || !hasAttribute(oldAttr->get_name()))
    throw DOMException(DOMException::NOT_FOUND_ERR);

  if (oldAttr->get_ownerElement() != shared_from_this())
    throw DOMException(DOMException::NOT_FOUND_ERR);

  removeAttribute(oldAttr->get_name());

  return oldAttr;
}

// TODO: Refactor to somehow combine Element::getElementsByTagName and
// Document::getElementsByTagName, and localnamematch
namespace
{
  struct localnamematch
  {
    localnamematch(frenzy::ustring localname)
      : localname(localname)
    {}

    bool operator()(frenzy::dom::Nodep n)
    {
      if (n->get_nodeType() != frenzy::dom::Node::ELEMENT_NODE)
	return false;

      return frenzy::dom_cast<frenzy::dom::Element>(n)->get_localName() == localname;
    }

  private:
    frenzy::ustring localname;
  };
}

frenzy::dom::NodeListp
frenzy::dom::Element::getElementsByTagName(ustring name)
{
  // Special case
  if (name == "*")
    return NodeList::create(shared_from_this(), TRAVERSE_TREE, type_filter(Node::ELEMENT_NODE));
  
  return NodeList::create(shared_from_this(), TRAVERSE_TREE, localnamematch(name));
}

frenzy::dom::NamedNodeMapp
frenzy::dom::Element::get_attributes()
{
  return NamedNodeMap::create(shared_from_this());
}

frenzy::dom::Attrp
frenzy::dom::Element::getAttributeNodeByIndex(size_t index)
{
  if (index >= attributes.size())
    return Attrp();

  attributes_t::const_iterator iter = attributes.begin();
  for (size_t i = 0; i < index; ++i, ++iter)
  {
  }

  return getAttributeNode(iter->first);
}

size_t
frenzy::dom::Element::getAttributeSize() const
{
  return attributes.size();
}

void
frenzy::dom::Element::normalizeAttributes()
{
  for (attributes_t::iterator it = attributes.begin();
       it != attributes.end();
       ++it)
  {
    it->second->normalize();
  }
}

void
frenzy::dom::Element::copyTo(frenzy::dom::Nodep n, bool deep) const
{
  frenzy::dom::Elementp elem = dom_cast<Element>(n);
  assert(elem);

  elem->attributes = attributes;

  Node::copyTo(n, deep);
}

frenzy::dom::Element::Element(frenzy::ustring localname)
  : localname(localname)
{
}

frenzy::dom::Elementp
frenzy::dom::Element::shared_from_this()
{
  return boost::static_pointer_cast<Element>(Node::shared_from_this());
}

frenzy::dom::Nodep
frenzy::dom::NamedNodeMap::getNamedItem(frenzy::ustring name) const
{
  return elem->getAttributeNode(name);
}

frenzy::dom::Nodep
frenzy::dom::NamedNodeMap::setNamedItem(frenzy::dom::Nodep n)
{
  if (!n || n->get_nodeType() != Node::ATTRIBUTE_NODE)
  {
    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
  }

  Attrp a = dom_cast<Attr>(n);

  return elem->setAttributeNode(a);
}

frenzy::dom::Nodep
frenzy::dom::NamedNodeMap::removeNamedItem(frenzy::ustring name)
{
  Attrp a = elem->getAttributeNode(name);
  if (!a)
  {
    throw DOMException(DOMException::NOT_FOUND_ERR);
  }

  return elem->removeAttributeNode(a);
}

frenzy::dom::Nodep
frenzy::dom::NamedNodeMap::item(size_t index) const
{
  return elem->getAttributeNodeByIndex(index);
}

size_t
frenzy::dom::NamedNodeMap::get_length() const
{
  return elem->getAttributeSize();
}

frenzy::dom::NamedNodeMapp
frenzy::dom::NamedNodeMap::create(frenzy::dom::Elementp elem)
{
  return NamedNodeMapp(new NamedNodeMap(elem));
}

frenzy::dom::NamedNodeMap::NamedNodeMap(frenzy::dom::Elementp elem)
  : elem(elem)
{
}

frenzy::dom::Node::nodeType
frenzy::dom::Attr::get_nodeType() const
{
  return Node::ATTRIBUTE_NODE;
}

frenzy::ustring
frenzy::dom::Attr::get_nodeName() const
{
  return get_name();
}

boost::optional<frenzy::ustring>
frenzy::dom::Attr::get_nodeValue() const
{
  return get_value();
}

void
frenzy::dom::Attr::set_nodeValue(frenzy::ustring str)
{
  set_value(str);
}

frenzy::dom::Nodep
frenzy::dom::Attr::cloneNode(bool deep) const
{
  Attrp ret = Attr::create(name);
  copyTo(ret, deep);
  return ret;
}

frenzy::ustring
frenzy::dom::Attr::get_name() const
{
  return name;
}

bool
frenzy::dom::Attr::get_specified() const
{
  // TODO: Should return false when this attribute exists as a default
  // value according to the document schema, and isn't explicitly set.
  return true;
}

frenzy::ustring
frenzy::dom::Attr::get_value() const
{
  ustring ret;

  for (children_t::const_iterator it = children.begin();
       it != children.end();
       ++it)
  {
    Textp t = dom_cast<Text>(*it);
    assert(t);

    ret.append(t->get_data());
  }

  return ret;
}

void
frenzy::dom::Attr::set_value(frenzy::ustring str)
{
  while (!children.empty())
  {
    removeChild(children.back());
  }

  Textp t = get_ownerDocument()->createTextNode(str);
  appendChild(t);
}

frenzy::dom::Elementp
frenzy::dom::Attr::get_ownerElement() const
{
  return elem.lock();
}

void
frenzy::dom::Attr::set_ownerElement(frenzy::dom::Elementp e)
{
  elem = e;
}

frenzy::dom::Attrp
frenzy::dom::Attr::create(frenzy::ustring name)
{
  return Attrp(new Attr(name));
}

void
frenzy::dom::Attr::copyTo(dom::Nodep n, bool) const
{
  Attrp a = dom_cast<Attr>(n);
  assert(a);

  a->elem = elem;

  // Always copy deeply
  Node::copyTo(n, true);
}

frenzy::dom::Attr::Attr(frenzy::ustring name)
  : name(name)
{
}
