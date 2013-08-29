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

#ifndef FRENZY_ELEMENT_HPP
#define FRENZY_ELEMENT_HPP

#include <map>
#include <boost/optional.hpp>

#include "util/unicode.hpp"
#include "node.hpp"

namespace frenzy
{
  namespace dom
  {
    struct Element : Node
    {

      virtual Node::nodeType get_nodeType() const;
      virtual ustring get_nodeName() const;

      ustring get_localName() const;
      ustring get_tagName() const;

      boost::optional<ustring> getAttribute(ustring name) const;
      void setAttribute(ustring name, ustring value);
      void removeAttribute(ustring name);
      bool hasAttribute(ustring name) const;
      
      Attrp getAttributeNode(ustring name);
      Attrp setAttributeNode(Attrp newAttr);
      Attrp removeAttributeNode(Attrp oldAttr);

      NodeListp getElementsByTagName(ustring name);

      virtual NamedNodeMapp get_attributes();

      // Implementation details
      Attrp getAttributeNodeByIndex(size_t index);
      size_t getAttributeSize() const;

      void normalizeAttributes();

    protected:
      virtual void copyTo(dom::Nodep n, bool deep) const;

      Element(ustring localname);

    private:
      ustring localname;

      typedef std::map<ustring, Attrp> attributes_t;
      attributes_t attributes;

      Elementp shared_from_this();
    };

    struct HTMLCollection
    {
    };

    struct NamedNodeMap
    {
      Nodep getNamedItem(ustring name) const;
      Nodep setNamedItem(Nodep n);
      Nodep removeNamedItem(ustring name);
      Nodep item(size_t index) const;
      size_t get_length() const;

      // Attribute list of a given element
      static NamedNodeMapp create(Elementp elem);

    private:
      NamedNodeMap(Elementp elem);

      Elementp elem;
    };

    struct Attr : Node
    {
      virtual Node::nodeType get_nodeType() const;
      virtual ustring get_nodeName() const;
      virtual boost::optional<ustring> get_nodeValue() const;
      virtual void set_nodeValue(ustring str);
      virtual Nodep cloneNode(bool deep = true) const;

      ustring get_name() const;
      bool get_specified() const;
      ustring get_value() const;
      void set_value(ustring str);

      Elementp get_ownerElement() const;
      void set_ownerElement(Elementp elem);

      static Attrp create(ustring name);

    protected:
      virtual void copyTo(dom::Nodep n, bool deep) const;

    private:
      Attr(ustring name);
      
      Elementwp elem;
      ustring name;
    };
  }
}

#endif
