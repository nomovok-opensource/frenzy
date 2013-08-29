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

#include <cassert>
#include <algorithm>
#include <stdexcept>

#include "node.hpp"
#include "document.hpp"
#include "element.hpp"
#include "text.hpp"
#include "exception.hpp"

frenzy::dom::Node::~Node()
{
}


frenzy::dom::Documentp
frenzy::dom::Node::get_ownerDocument() const
{
  return ownerdocument.lock();
}

frenzy::dom::Nodep
frenzy::dom::Node::get_parentNode() const
{
  return parent.lock();
}

bool
frenzy::dom::Node::hasChildNodes() const
{
  return !children.empty();
}

frenzy::dom::NodeListp
frenzy::dom::Node::get_childNodes()
{
  return NodeList::create(shared_from_this(), TRAVERSE_CHILDREN);
}

frenzy::dom::Nodep
frenzy::dom::Node::get_firstChild() const
{
  if (hasChildNodes())
    return children.front();

  return Nodep();
}

frenzy::dom::Nodep
frenzy::dom::Node::get_lastChild() const
{
  if (hasChildNodes())
    return children.back();
  
  return Nodep();
}

frenzy::dom::Nodep
frenzy::dom::Node::get_previousSibling() const
{
  return prev_sibling.lock();
}

frenzy::dom::Nodep
frenzy::dom::Node::get_nextSibling() const
{
  return next_sibling.lock();
}

boost::optional<frenzy::ustring>
frenzy::dom::Node::get_nodeValue() const
{
  return boost::optional<frenzy::ustring>();
}

void
frenzy::dom::Node::set_nodeValue(frenzy::ustring)
{
  // Do nothing
}

frenzy::dom::Nodep
frenzy::dom::Node::insertBefore(frenzy::dom::Nodep node,
				frenzy::dom::Nodep child)
{
  verify_can_insert(node, child);

  Nodep refchild = child;
  if (refchild == node)
  {
    refchild = node->get_nextSibling();
  }

  // ownerDocument attribute is defined to be null if `this' is a
  // Document. The "node document" in that case is defined to be
  // `this'.
  Documentp doc = get_ownerDocument();
  if (get_nodeType() == DOCUMENT_NODE)
  {
    doc = dom_cast<Document>(shared_from_this());
  }
  assert(doc);
  doc->adoptNode(node);

  // TODO: Suppress_observers flag handling
  insertBefore(node, refchild, false);

  return node;
}

frenzy::dom::Nodep
frenzy::dom::Node::appendChild(frenzy::dom::Nodep node)
{
  return insertBefore(node, Nodep());
}

frenzy::dom::Nodep
frenzy::dom::Node::replaceChild(frenzy::dom::Nodep node,
				frenzy::dom::Nodep child)
{
  if (!child || child->get_parentNode() != shared_from_this())
  {
    throw DOMException(DOMException::NOT_FOUND_ERR);
  }

  Nodep refchild = child->get_nextSibling();

  verify_can_insert(node, refchild, child);

  if (refchild == node)
  {
    refchild = node->get_nextSibling();
  }

  get_ownerDocument()->adoptNode(node);

  child->get_parentNode()->removeChild(child, true);
  
  insertBefore(node, refchild, true);

  // TODO: Mutation record

  // TODO: "node is removed" and "node is inserted" according to HTML
  // spec at this stage
  
  return child;
}

frenzy::dom::Nodep
frenzy::dom::Node::removeChild(frenzy::dom::Nodep child)
{
  if (!child || child->get_parentNode() != shared_from_this())
  {
    throw DOMException(DOMException::NOT_FOUND_ERR);
  }

  removeChild(child, false);

  return child;
}

void
frenzy::dom::Node::normalize()
{
  if (get_nodeType() == ELEMENT_NODE)
  {
    Elementp thiselem = dom_cast<Element>(shared_from_this());
    thiselem->normalizeAttributes();
  }

  for (children_t::const_iterator it = children.begin();
       it != children.end();
       ++it)
  {
    dom::Nodep n = *it;
    nodeType type = n->get_nodeType();
    
    if (type == TEXT_NODE)
    {
      dom::Textp t = dom_cast<Text>(n);
      if (t->get_length() == 0)
      {
	removeChild(n);
	return normalize();
      }
      
      if (dom::Nodep sib = n->get_nextSibling())
      {
	if (sib->get_nodeType() == TEXT_NODE)
	{
	  t->replaceData(t->get_length(), 0, dom_cast<Text>(sib)->get_data());
	  removeChild(sib);
	  return normalize();
	}
      }
    }
    else
    {
      n->normalize();
    }
  }
}

frenzy::dom::NamedNodeMapp
frenzy::dom::Node::get_attributes()
{
  return NamedNodeMapp();
}

frenzy::dom::Nodep
frenzy::dom::Node::get_nextInTreeOrder(frenzy::dom::Nodep root)
{
  if (Nodep child = get_firstChild())
    return child;

  for (Nodep n = shared_from_this(); n != root; n = n->get_parentNode())
  {
    if (Nodep sib = n->get_nextSibling())
      return sib;
  }

  return Nodep();
}

void
frenzy::dom::Node::recursive_set_ownerdocument(frenzy::dom::Documentp doc)
{
  ownerdocument = doc;

  for (children_t::iterator it = children.begin();
       it != children.end();
       ++it)
  {
    (*it)->recursive_set_ownerdocument(doc);
  }
}

void
frenzy::dom::Node::recursive_mark_dirty()
{
  dirty = true;

  for (children_t::iterator it = children.begin();
       it != children.end();
       ++it)
  {
    (*it)->recursive_mark_dirty();
  }
}

void
frenzy::dom::Node::mark_dirty()
{
  dirty = true;
  
  if (Nodep n = get_parentNode())
  {
    n->mark_dirty();
  }
}

bool
frenzy::dom::Node::is_dirty() const
{
  return dirty;
}

void
frenzy::dom::Node::drop_graphics()
{
  position.reset();
}

void
frenzy::dom::Node::recursive_drop_graphics()
{
  drop_graphics();

  for (children_t::iterator it = children.begin();
       it != children.end();
       ++it)
  {
    (*it)->recursive_drop_graphics();
  }
}

frenzy::vec2
frenzy::dom::Node::layout(frenzy::vec maxwidth)
{
  vec2 size = vec2(0, 0);

  vec xroom = maxwidth;

  vec2 childpos(0, 0);
  vec nextline(0);

  for (children_t::iterator it = children.begin();
       it != children.end();
       ++it)
  {
    positiontype postype = (*it)->get_positiontype();
    if (postype == POSITION_ABSOLUTE || postype == POSITION_FLOAT)
    {
      // TODO: Implement these
      continue;
    }
    
    vec2 childsize = (*it)->layout(xroom);
    if (!fits(childsize.x, xroom) && nextline != 0)
    {
      xroom = maxwidth;
      childpos = vec2(0, nextline);
      childsize = (*it)->layout(xroom);
      // We'll leave it there even if it doesn't fit
    }

    // Note: If you follow the recursion direction, you see that the
    // child's actual (relative) position is set before this node's
    // (relative) position is known.
    boost::shared_ptr<graphics::translation> thispos = get_position();
    // Document should only call layout() when there is a graphics provider.
    assert(thispos);
    (*it)->set_position(thispos, childpos);
    
    size.grow(childpos + childsize);
    if (nextline < childpos.y + childsize.y)
    {
      nextline = childpos.y + childsize.y;
    }

    if (postype == POSITION_INLINE)
    {
      childpos.x += childsize.x;
      xroom -= childsize.x;
    }
    else
    {
      childpos.x = 0;
      childpos.y = nextline;
      xroom = maxwidth;
    }
  }

  setup_graphics(size);

  return size;
}

void
frenzy::dom::Node::setup_graphics(frenzy::vec2)
{
}

frenzy::dom::Node::positiontype
frenzy::dom::Node::get_positiontype() const
{
  return POSITION_INLINE;
}

boost::shared_ptr<frenzy::graphics::translation>
frenzy::dom::Node::get_position()
{
  if (!position)
  {
    boost::shared_ptr<graphics::factory> fact = get_ownerDocument()->graphicsfactory();
    if (fact)
    {
      position = fact->create_translation();
    }
  }

  return position;
}

void
frenzy::dom::Node::set_position(boost::shared_ptr<frenzy::graphics::translation> relative,
				frenzy::vec2 pos)
{
  if (get_positiontype() == POSITION_ABSOLUTE)
    return;

  boost::shared_ptr<graphics::translation> ownpos = get_position();
  assert(ownpos);

  ownpos->relative_to(relative);
  ownpos->position(pos);
}

frenzy::dom::Node::Node()
  : dirty(true)
{
}

void
frenzy::dom::Node::copyTo(frenzy::dom::Nodep n, bool deep) const
{
  n->ownerdocument = ownerdocument;

  if (deep)
  {
    for (children_t::const_iterator it = children.begin();
	 it != children.end();
	 ++it)
    {
      n->appendChild((*it)->cloneNode(deep));
    }
  }
}

frenzy::dom::Nodep
frenzy::dom::Node::insertBefore(frenzy::dom::Nodep node,
				frenzy::dom::Nodep child,
				bool suppress_observers)
{
  // TODO: Handle all created Range objects as per DOM4 5.2.1

  if (!suppress_observers)
  {
    // TODO: Mutation records as per DOM4 5.2.1
  }

  children_t toinsert;

  if (node->get_nodeType() == DOCUMENT_FRAGMENT_NODE)
  {
    toinsert = node->children;
  }
  else
  {
    toinsert.push_back(node);
  }

  children_t::iterator it = children.end();
  if (child)
  {
    it = std::find(children.begin(), children.end(), child);
    assert(it != children.end());
  }

  Nodep prev;
  Nodep next;
  if (it != children.begin())
  {
    children_t::iterator previt = it;
    --previt;
    prev = *previt;
  }
  if (it != children.end())
  {
    next = *it;
  }

  for (children_t::const_iterator insiter = toinsert.begin();
       insiter != toinsert.end();
       ++insiter)
  {
    if (prev)
    {
      prev->next_sibling = *insiter;
    }
    if (next)
    {
      next->prev_sibling = *insiter;
    }
    (*insiter)->next_sibling = next;
    (*insiter)->prev_sibling = prev;
    (*insiter)->parent = shared_from_this();

    children.insert(it, *insiter);

    prev = *insiter;

    (*insiter)->inserted_to(shared_from_this());
    child_added(*insiter);

    if (!suppress_observers)
    {
      // TODO: Run "node is inserted" according to HTML spec at this
      // stage
    }
  }

  return node;
}

void
frenzy::dom::Node::removeChild(frenzy::dom::Nodep child,
			       bool suppress_observers)
{
  // TODO: Handle all created Range objects as per DOM4 5.2.1

  if (!suppress_observers)
  {
    // TODO: Mutation records
  }

  // TODO: Registered observers chain handling to ancestors

  children_t::iterator it = std::find(children.begin(), children.end(), child);
  assert(it != children.end());

  Nodep prev;
  Nodep next;
  if (it != children.begin())
  {
    children_t::iterator previt = it;
    --previt;
    prev = *previt;
  }
  if (it != children.end())
  {
    children_t::iterator nextit = it;
    ++nextit;
    if (nextit != children.end())
    {
      next = *nextit;
    }
  }

  if (prev)
  {
    prev->next_sibling = next;
  }
  if (next)
  {
    next->prev_sibling = prev;
  }

  children.erase(it);

  child->parent.reset();

  if (!suppress_observers)
  {
    // TODO: Run "node is removed" as per HTML spec at this stage
  }

  child->removed_from(shared_from_this());
  child_removed(child);
}

void
frenzy::dom::Node::verify_can_insert(frenzy::dom::Nodep node,
				     frenzy::dom::Nodep before,
				     frenzy::dom::Nodep ignorechild)
{
  nodeType t = get_nodeType();
  nodeType ntype = node->get_nodeType();

  // Can have children in general
  if (t != DOCUMENT_NODE &&
      t != DOCUMENT_FRAGMENT_NODE &&
      t != ELEMENT_NODE &&
      t != ATTRIBUTE_NODE)
  {
    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
  }

  // Attribute nodes cannot be inserted anywhere
  if (ntype == ATTRIBUTE_NODE)
  {
    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
  }

  // Can't have ancestors as children
  for (Nodep check = shared_from_this(); check; check = check->get_parentNode())
  {
    if (node == check)
    {
      throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
    }
  }

  // If `before' non-null, it must be our child
  if (before && before->get_parentNode() != shared_from_this())
  {
    throw DOMException(DOMException::NOT_FOUND_ERR);
  }

  switch (t)
  {
  case DOCUMENT_NODE:
    {
      // Valid child node types
      if (ntype != DOCUMENT_FRAGMENT_NODE &&
	  ntype != DOCUMENT_TYPE_NODE &&
	  ntype != ELEMENT_NODE &&
	  ntype != PROCESSING_INSTRUCTION_NODE &&
	  ntype != COMMENT_NODE)
      {
	throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
      }
      
      bool haselem = false;
      bool nodehaselem = (ntype == ELEMENT_NODE);
      bool hasdoctype = false;
      bool nodehasdoctype = (ntype == DOCUMENT_TYPE_NODE);
      for (children_t::const_iterator it = children.begin();
	   it != children.end();
	   ++it)
      {
	if (*it == ignorechild)
	  continue;

	nodeType childtype = (*it)->get_nodeType();
	if (childtype == ELEMENT_NODE)
	{
	  assert(!haselem);
	  haselem = true;
	}
	else if (childtype == DOCUMENT_TYPE_NODE)
	{
	  assert(!hasdoctype);
	  hasdoctype = true;
	}
      }
      
      if (ntype == DOCUMENT_FRAGMENT_NODE)
      {
	for (children_t::const_iterator it = node->children.begin();
	     it != node->children.end();
	     ++it)
	{
	  nodeType fragchildt = (*it)->get_nodeType();
	  if (fragchildt == ELEMENT_NODE)
	  {
	    nodehaselem = true;
	  }
	  else if (fragchildt == DOCUMENT_TYPE_NODE)
	  {
	    nodehasdoctype = true;
	  }
	  
	  // Valid child node types
	  if (fragchildt != DOCUMENT_TYPE_NODE &&
	      fragchildt != ELEMENT_NODE &&
	      fragchildt != PROCESSING_INSTRUCTION_NODE &&
	      fragchildt != COMMENT_NODE)
	  {
	    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
	  }
	}
      }
	
      // Only one document type node child allowed
      if (hasdoctype && nodehasdoctype)
      {
	throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
      }
      
      // Only one element node child allowed
      if (haselem && nodehaselem)
      {
	throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
      }
      
      // Elements can't be added before a document type node, if any
      if (nodehaselem && hasdoctype)
      {
	for (Nodep follow = before; follow; follow = follow->get_nextSibling())
	{
	  if (follow->get_nodeType() == DOCUMENT_TYPE_NODE)
	  {
	    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
	  }
	}
      }

      // Document types can't be added after an element node, if any
      if (nodehasdoctype && haselem)
      {
	for (Nodep pre = before->get_previousSibling(); pre; pre = pre->get_previousSibling())
	{
	  if (pre->get_nodeType() == ELEMENT_NODE)
	  {
	    throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
	  }
	}
      }
    }
    break;
  case ATTRIBUTE_NODE:
    {
      if (ntype != DOCUMENT_FRAGMENT_NODE &&
	  ntype != TEXT_NODE)
      {
	throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
      }

      // Attribute nodes are deprecated in DOM4, and
      // WRONG_DOCUMENT_ERR is also deprecated in DOM4. As we're
      // dealing with a lower DOM level interface, error out as a
      // lower level DOM would.
      if (node->get_ownerDocument() != get_ownerDocument())
      {
	throw DOMException(DOMException::WRONG_DOCUMENT_ERR);
      }

      for (children_t::const_iterator it = node->children.begin();
	   it != node->children.end();
	   ++it)
      {
	if ((*it)->get_nodeType() != TEXT_NODE)
	{
	  throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
	}
      }
    }
    break;
  default:
    break;
  }	
}

namespace
{
  struct sametype
  {
    sametype(frenzy::dom::Node::nodeType type)
      : type(type)
    {}

    bool operator()(frenzy::dom::Nodep n) const
    {
      return n->get_nodeType() == type;
    }
  
  private:
    frenzy::dom::Node::nodeType type;
  };
}

frenzy::dom::node_filter
frenzy::dom::type_filter(frenzy::dom::Node::nodeType type)
{
  return sametype(type);
}

bool
frenzy::dom::always(frenzy::dom::Nodep)
{
  return true;
}

frenzy::dom::Nodep
frenzy::dom::NodeList::item(size_t index) const
{
  size_t i = 0;

  Nodep ret = get_first();

  while (i++ < index)
  {
    ret = get_next(ret);
  }

  return ret;
}

size_t
frenzy::dom::NodeList::get_length() const
{
  size_t ret = 0;

  Nodep n = get_first();

  while (n)
  {
    ++ret;
    n = get_next(n);
  }

  return ret;
}

frenzy::dom::NodeListp
frenzy::dom::NodeList::create(frenzy::dom::Nodep root, frenzy::dom::node_traversal trav)
{
  return NodeListp(new NodeList(root, trav, &always));
}

frenzy::dom::NodeListp
frenzy::dom::NodeList::create(frenzy::dom::Nodep root, frenzy::dom::node_traversal trav, frenzy::dom::Node::nodeType type)
{
  return NodeListp(new NodeList(root, trav, type_filter(type)));
}

frenzy::dom::NodeListp
frenzy::dom::NodeList::create(frenzy::dom::Nodep root, frenzy::dom::node_traversal trav, frenzy::dom::node_filter filter)
{
  return NodeListp(new NodeList(root, trav, filter));
}

frenzy::dom::NodeList::NodeList(frenzy::dom::Nodep root, frenzy::dom::node_traversal trav, frenzy::dom::node_filter filter)
  : root(root)
  , trav(trav)
  , filter(filter)
{
}

frenzy::dom::Nodep
frenzy::dom::NodeList::get_first() const
{
  Nodep ret = root->get_firstChild();

  while (ret && !filter(ret))
  {
    ret = get_next(ret);
  }

  return ret;
}

frenzy::dom::Nodep
frenzy::dom::NodeList::get_next(frenzy::dom::Nodep n) const
{
  do
  {
    n = get_next_nofilter(n);
  } while (n && !filter(n));

  if (n == root)
    return Nodep();

  return n;
}

frenzy::dom::Nodep
frenzy::dom::NodeList::get_next_nofilter(frenzy::dom::Nodep n) const
{
  if (!n)
    return n;

  switch (trav)
  {
  case TRAVERSE_CHILDREN:
    return n->get_nextSibling();
  case TRAVERSE_TREE:
    return n->get_nextInTreeOrder(root);
  default:
    throw std::logic_error("Traversal mode not implemented");
  }
}
