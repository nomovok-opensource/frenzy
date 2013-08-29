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

#ifndef FRENZY_DOCUMENT_HPP
#define FRENZY_DOCUMENT_HPP

#include <boost/optional.hpp>

#include "util/unicode.hpp"
#include "node.hpp"
#include "pointers.hpp"
#include "graphics.hpp"

namespace frenzy
{
  namespace dom
  {
    struct Document : Node
    {
      static Documentp create();

      DOMImplementationp get_implementation() const;

      virtual Node::nodeType get_nodeType() const;
      virtual ustring get_nodeName() const;
      virtual Nodep cloneNode(bool deep = true) const;

      DocumentTypep get_doctype() const;
      Elementp get_documentElement() const;
      NodeListp getElementsByTagName(ustring localName);

      Elementp createElement(ustring localName);
      Elementp createElementNS(boost::optional<ustring> nspace, ustring qualifiedName);
      DocumentFragmentp createDocumentFragment();
      Textp createTextNode(ustring data);
      Commentp createComment(ustring data);

      Nodep adoptNode(Nodep node);

      // DOM1

      CDATASectionp createCDATASection(ustring data);
      Attrp createAttribute(ustring name);
      EntityReferencep createEntityReference(ustring name);

      //
      // Graphics
      //
      
      // Set the graphics provider.
      void graphicsfactory(boost::shared_ptr<graphics::factory> fact);
      // Retrieve it.
      boost::shared_ptr<graphics::factory> graphicsfactory() const;

      // Layout the document. Does nothing if there is no graphics
      // factory set.
      void layout_document();

    private:
      Document();

      Documentp shared_from_this();

      // Checks that the passed string is a valid name. Throws
      // DOMException appropriately if necessary.
      void verify_valid_name(ustring name);

      boost::shared_ptr<graphics::factory> fact;
    };

    struct XMLDocument : Document
    {
    };

    struct DocumentFragment : Node
    {
      virtual Node::nodeType get_nodeType() const;
      virtual ustring get_nodeName() const;
      virtual Nodep cloneNode(bool deep = true) const;

      static DocumentFragmentp create();

    private:
      DocumentFragment();
    };

    struct DocumentType : Node
    {
      virtual Node::nodeType get_nodeType() const;
      virtual ustring get_nodeName() const;
      virtual Nodep cloneNode(bool deep = true) const;

      //
      // DOM4
      //

      ustring get_name() const;
      ustring get_publicId() const;
      ustring get_systemId() const;

      //
      // DOM1
      //

      NamedNodeMapp get_entities() const;
      NamedNodeMapp get_notations() const;

      static DocumentTypep create(ustring name, ustring publicid, ustring systemid);

    private:
      DocumentType(ustring name, ustring publicid, ustring systemid);

      ustring name;
      ustring publicid;
      ustring systemid;
    };

    struct DOMImplementation
    {
      bool hasFeature(ustring feature, ustring version) const;

      static DOMImplementationp create();
    private:
      DOMImplementation();
    };

    struct EntityReference : Node
    {
    };
  }
}

#endif
