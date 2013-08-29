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

#ifndef FRENZY_NODE_HPP
#define FRENZY_NODE_HPP

#include <vector>
#include <list>
#include <boost/optional.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/signals2/signal.hpp>

#include "util/unicode.hpp"
#include "util/vector.hpp"
#include "event.hpp"
#include "pointers.hpp"
#include "graphics.hpp"

namespace frenzy
{
  namespace dom
  {
    struct Node : EventTarget, boost::enable_shared_from_this<Node>
    {
      virtual ~Node() = 0;

      //
      // DOM4
      //

      enum nodeType
      {
	ELEMENT_NODE = 1,
	ATTRIBUTE_NODE = 2, // historical
	TEXT_NODE = 3,
	CDATA_SECTION_NODE = 4, // historical
	ENTITY_REFERENCE_NODE = 5, // historical
	ENTITY_NODE = 6, // historical
	PROCESSING_INSTRUCTION_NODE = 7,
	COMMENT_NODE = 8,
	DOCUMENT_NODE = 9,
	DOCUMENT_TYPE_NODE = 10,
	DOCUMENT_FRAGMENT_NODE = 11,
	NOTATION_NODE = 12 // historical
      };

      virtual nodeType get_nodeType() const = 0;
      virtual ustring get_nodeName() const = 0;

      boost::optional<ustring> get_baseURI() const;

      Documentp get_ownerDocument() const;
      Nodep get_parentNode() const;
      Elementp get_parentElement() const;
      bool hasChildNodes() const;
      NodeListp get_childNodes();
      Nodep get_firstChild() const;
      Nodep get_lastChild() const;
      Nodep get_previousSibling() const;
      Nodep get_nextSibling() const;

      virtual boost::optional<ustring> get_nodeValue() const;
      virtual void set_nodeValue(ustring str);
      boost::optional<ustring> get_textContent() const;
      void set_textContent();
      Nodep insertBefore(Nodep node, Nodep child);
      Nodep appendChild(Nodep node);
      Nodep replaceChild(Nodep node, Nodep child);
      Nodep removeChild(Nodep child);
      void normalize();

      virtual Nodep cloneNode(bool deep = true) const = 0;
      bool isEqualNode(Nodep node) const;

      enum documentPosition
      {
	DOCUMENT_POSITION_DISCONNECTED = 0x01,
	DOCUMENT_POSITION_PRECEDING = 0x02,
	DOCUMENT_POSITION_FOLLOWING = 0x04,
	DOCUMENT_POSITION_CONTAINS = 0x08,
	DOCUMENT_POSITION_CONTAINED_BY = 0x10,
	DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC = 0x20
      };

      documentPosition compareDocumentPosition(Nodep other) const;
      bool contains(Nodep other) const;

      boost::optional<ustring> lookupPrefix(boost::optional<ustring> nspace) const;
      boost::optional<ustring> lookupNamespaceURI(boost::optional<ustring> prefix) const;
      bool isDefaultNamespace(boost::optional<ustring> nspace) const;

      //
      // DOM1
      //
      
      virtual NamedNodeMapp get_attributes();

      //
      // Implementation details
      //

      Nodep get_nextInTreeOrder(Nodep root);

      void recursive_set_ownerdocument(Documentp doc);

      //
      // Graphics
      //

      // Mark this node and its children as dirty
      void recursive_mark_dirty();
      // Mark this node and its parent all the way to the root as dirty
      void mark_dirty();
      // Query dirtiness. layout() is supposed to recurse down to
      // children only if they are dirty.
      bool is_dirty() const;
      
      // Called when the node is supposed to drop all its graphics items.
      virtual void drop_graphics();
      // Calls the above function and recurses to children.
      void recursive_drop_graphics();

      // Lays out the node's children and returns the size of this
      // node, in pixels. The width is restricted to the given
      // size. If the returned width exceeds the allotted maximum,
      // this function can be called again (the parent attempting to
      // position this node to a lower line). If the node's owner
      // document has a graphics provider factory, the node is also
      // supposed to set up its graphics items.
      virtual vec2 layout(vec maxwidth);

      // Called by the default implementation of the above with the
      // size determined by layout(). The purpose of this function is
      // to set up graphics items for supplementary things like
      // borders and background. For other content, layout() will need
      // to be implemented.
      virtual void setup_graphics(vec2 size);

      enum positiontype
      {
	POSITION_ABSOLUTE,
	POSITION_FLOAT,
	POSITION_INLINE,
	POSITION_BLOCK
      };

      // How this node is positioned. Absolute-positioned nodes are
      // placed relative to the page, float-positioned nodes move
      // around funnily, and inline/block-positioned are positioned
      // left-to-right top-to-bottom like English text. Note:
      // Relative-positioned nodes return POSITION_FLOW/POSITION_BLOCK
      // as normal, and just place their graphics items accordingly.
      virtual positiontype get_positiontype() const;

      // Retrieve the position object for this node. If one doesn't
      // exist already, it is created using the owner document's
      // graphics provider. If it isn't set, NULL is returned.
      virtual boost::shared_ptr<graphics::translation> get_position();

      // Sets this node's position relative to the position object
      // passed as a parameter. This call is ignored if the position
      // type is POSITION_ABSOLUTE.
      virtual void set_position(boost::shared_ptr<graphics::translation> relative, vec2 pos);

      // DOM modification hooks

    private:
      typedef boost::signals2::signal<void(Nodep)> modified_signal;

      // Called after this node is inserted as a child of another node.
      modified_signal inserted_to;
      // Called after this node is removed from its parent.
      modified_signal removed_from;
      // Called after a child is removed from `this'.
      modified_signal child_removed;
      // Called after a child is added to `this'.
      modified_signal child_added;

    public:
      typedef modified_signal::slot_type modified_slot;

      boost::signals2::connection on_inserted_to(const modified_slot& slot);
      boost::signals2::connection on_removed_from(const modified_slot& slot);
      boost::signals2::connection on_child_removed(const modified_slot& slot);
      boost::signals2::connection on_child_added(const modified_slot& slot);


    protected:
      Node();

      // All cloneNode() implementations call this. Implemented if the
      // class holds any data of its own. Any implementation must call
      // the base class's copyTo() as well.
      virtual void copyTo(dom::Nodep n, bool deep) const;

      typedef std::list<Nodep> children_t;
      children_t children;
      Nodewp parent;

      boost::shared_ptr<graphics::translation> position;

    private:
      Documentwp ownerdocument;
      Nodewp prev_sibling;
      Nodewp next_sibling;
      bool dirty;

      Nodep insertBefore(Nodep node, Nodep child, bool suppress_observers);
      void removeChild(Nodep child, bool suppress_observers);

      // Verifies valid parent-child relationship, throws appropriate
      // DOMException when necessary. ignorechild, if not null, will
      // be ignored from the child list of `this'. The purpose of that
      // is to support checks for replaceChild().
      void verify_can_insert(Nodep node, Nodep before, Nodep ignorechild = Nodep());
    };

    enum node_traversal
    {
      TRAVERSE_CHILDREN,
      TRAVERSE_TREE // Tree-order, aka depth-first
    };

    typedef boost::function<bool(Nodep)> node_filter;
    node_filter type_filter(Node::nodeType type);
    bool always(frenzy::dom::Nodep);

    struct NodeList
    {
      Nodep item(size_t index) const;
      size_t get_length() const;

      // Create a list that matches all nodes
      static NodeListp create(Nodep root, node_traversal trav);
      // Create a list that matches all nodes of given type
      static NodeListp create(Nodep root, node_traversal trav, Node::nodeType type);
      // Create a list that matches all nodes that match the filter
      static NodeListp create(Nodep root, node_traversal trav, node_filter filter);

    private:
      NodeList(Nodep root, node_traversal trav, node_filter filter);

      Nodep get_first() const;
      Nodep get_next(Nodep n) const;
      Nodep get_next_nofilter(Nodep n) const;

      Nodep root;
      node_traversal trav;
      node_filter filter;
    };
  }
}

#endif
