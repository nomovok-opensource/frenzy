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

#include <map>
#include <boost/shared_ptr.hpp>

#include "document.hpp"
#include "element.hpp"
#include "htmlelement.hpp"
#include "text.hpp"
#include "exception.hpp"
#include "graphics.hpp"

frenzy::dom::Documentp
frenzy::dom::Document::create()
{
  return Documentp(new Document());
}

frenzy::dom::DOMImplementationp
frenzy::dom::Document::get_implementation() const
{
  static DOMImplementationp impl = DOMImplementation::create();
  return impl;
}

frenzy::dom::Node::nodeType
frenzy::dom::Document::get_nodeType() const
{
  return Node::DOCUMENT_NODE;
}

frenzy::ustring
frenzy::dom::Document::get_nodeName() const
{
  return "#document";
}

frenzy::dom::Nodep
frenzy::dom::Document::cloneNode(bool deep) const
{
  Documentp ret = Document::create();
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::DocumentTypep
frenzy::dom::Document::get_doctype() const
{
  // The child-handling functions in Node already handle the predicate
  // that there's only one DocumentType child. Therefore, the first
  // DOCUMENT_TYPE_NODE in the list of children is our document type.

  for (children_t::const_iterator it = children.begin();
       it != children.end();
       ++it)
  {
    if ((*it)->get_nodeType() == Node::DOCUMENT_TYPE_NODE)
      return dom_cast<DocumentType>(*it);
  }

  return DocumentTypep();
}

frenzy::dom::Elementp
frenzy::dom::Document::get_documentElement() const
{
  // The child-handling functions in Node already handle the predicate
  // that there's only one element child. Therefore, the first
  // ELEMENT_NODE in the list of children is our document element.

  for (children_t::const_iterator it = children.begin();
       it != children.end();
       ++it)
  {
    if ((*it)->get_nodeType() == Node::ELEMENT_NODE)
      return dom_cast<Element>(*it);
  }

  return Elementp();
}

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
frenzy::dom::Document::getElementsByTagName(frenzy::ustring localName)
{
  // Special case
  if (localName == "*")
    return NodeList::create(shared_from_this(), TRAVERSE_TREE, type_filter(Node::ELEMENT_NODE));

  return NodeList::create(shared_from_this(), TRAVERSE_TREE, localnamematch(localName));
}

namespace
{
  struct elementfactory_base
  {
    virtual frenzy::dom::Elementp create(frenzy::ustring localname) = 0;
  };

  template <typename T>
  struct elementfactory : elementfactory_base
  {
    virtual frenzy::dom::Elementp create(frenzy::ustring localname)
    {
      return T::create(localname);
    }
  };

  template <typename T>
  boost::shared_ptr<elementfactory_base> factory()
  {
    return boost::shared_ptr<elementfactory<T> >(new elementfactory<T>());
  }

  
  typedef std::map<frenzy::ustring, boost::shared_ptr<elementfactory_base> > factorymap_t;

  factorymap_t get_factorymap()
  {
    factorymap_t ret;

    using namespace frenzy::dom;

    ret.insert(std::make_pair("a", factory<HTMLAnchorElement>()));
    ret.insert(std::make_pair("abbr", factory<HTMLElement>()));
    ret.insert(std::make_pair("address", factory<HTMLElement>()));
    ret.insert(std::make_pair("area", factory<HTMLAreaElement>()));
    ret.insert(std::make_pair("article", factory<HTMLElement>()));
    ret.insert(std::make_pair("aside", factory<HTMLElement>()));
    ret.insert(std::make_pair("audio", factory<HTMLAudioElement>()));
    ret.insert(std::make_pair("b", factory<HTMLElement>()));
    ret.insert(std::make_pair("base", factory<HTMLBaseElement>()));
    ret.insert(std::make_pair("bdi", factory<HTMLElement>()));
    ret.insert(std::make_pair("bdo", factory<HTMLElement>()));
    ret.insert(std::make_pair("blockquote", factory<HTMLQuoteElement>()));
    ret.insert(std::make_pair("body", factory<HTMLBodyElement>()));
    ret.insert(std::make_pair("br", factory<HTMLBRElement>()));
    ret.insert(std::make_pair("button", factory<HTMLButtonElement>()));
    ret.insert(std::make_pair("canvas", factory<HTMLCanvasElement>()));
    ret.insert(std::make_pair("caption", factory<HTMLTableCaptionElement>()));
    ret.insert(std::make_pair("cite", factory<HTMLElement>()));
    ret.insert(std::make_pair("code", factory<HTMLElement>()));
    ret.insert(std::make_pair("col", factory<HTMLTableColElement>()));
    ret.insert(std::make_pair("colgroup", factory<HTMLTableColElement>()));
    ret.insert(std::make_pair("command", factory<HTMLCommandElement>()));
    ret.insert(std::make_pair("data", factory<HTMLDataElement>()));
    ret.insert(std::make_pair("datalist", factory<HTMLDataListElement>()));
    ret.insert(std::make_pair("dd", factory<HTMLElement>()));
    ret.insert(std::make_pair("del", factory<HTMLModElement>()));
    ret.insert(std::make_pair("details", factory<HTMLDetailsElement>()));
    ret.insert(std::make_pair("dfn", factory<HTMLElement>()));
    ret.insert(std::make_pair("dialog", factory<HTMLDialogElement>()));
    ret.insert(std::make_pair("div", factory<HTMLDivElement>()));
    ret.insert(std::make_pair("dl", factory<HTMLDListElement>()));
    ret.insert(std::make_pair("dt", factory<HTMLElement>()));
    ret.insert(std::make_pair("em", factory<HTMLElement>()));
    ret.insert(std::make_pair("embed", factory<HTMLEmbedElement>()));
    ret.insert(std::make_pair("fieldset", factory<HTMLFieldSetElement>()));
    ret.insert(std::make_pair("figcaption", factory<HTMLElement>()));
    ret.insert(std::make_pair("figure", factory<HTMLElement>()));
    ret.insert(std::make_pair("footer", factory<HTMLElement>()));
    ret.insert(std::make_pair("form", factory<HTMLFormElement>()));
    ret.insert(std::make_pair("h1", factory<HTMLHeadingElement>()));
    ret.insert(std::make_pair("h2", factory<HTMLHeadingElement>()));
    ret.insert(std::make_pair("h3", factory<HTMLHeadingElement>()));
    ret.insert(std::make_pair("h4", factory<HTMLHeadingElement>()));
    ret.insert(std::make_pair("h5", factory<HTMLHeadingElement>()));
    ret.insert(std::make_pair("h6", factory<HTMLHeadingElement>()));
    ret.insert(std::make_pair("head", factory<HTMLHeadElement>()));
    ret.insert(std::make_pair("header", factory<HTMLElement>()));
    ret.insert(std::make_pair("hgroup", factory<HTMLElement>()));
    ret.insert(std::make_pair("hr", factory<HTMLHRElement>()));
    ret.insert(std::make_pair("html", factory<HTMLHtmlElement>()));
    ret.insert(std::make_pair("i", factory<HTMLElement>()));
    ret.insert(std::make_pair("iframe", factory<HTMLIFrameElement>()));
    ret.insert(std::make_pair("img", factory<HTMLImageElement>()));
    ret.insert(std::make_pair("input", factory<HTMLInputElement>()));
    ret.insert(std::make_pair("ins", factory<HTMLModElement>()));
    ret.insert(std::make_pair("kbd", factory<HTMLElement>()));
    ret.insert(std::make_pair("keygen", factory<HTMLKeygenElement>()));
    ret.insert(std::make_pair("label", factory<HTMLLabelElement>()));
    ret.insert(std::make_pair("legend", factory<HTMLLegendElement>()));
    ret.insert(std::make_pair("li", factory<HTMLLIElement>()));
    ret.insert(std::make_pair("link", factory<HTMLLinkElement>()));
    ret.insert(std::make_pair("map", factory<HTMLMapElement>()));
    ret.insert(std::make_pair("mark", factory<HTMLElement>()));
    ret.insert(std::make_pair("menu", factory<HTMLMenuElement>()));
    ret.insert(std::make_pair("meta", factory<HTMLMetaElement>()));
    ret.insert(std::make_pair("meter", factory<HTMLMeterElement>()));
    ret.insert(std::make_pair("nav", factory<HTMLElement>()));
    ret.insert(std::make_pair("noscript", factory<HTMLElement>()));
    ret.insert(std::make_pair("object", factory<HTMLObjectElement>()));
    ret.insert(std::make_pair("ol", factory<HTMLOListElement>()));
    ret.insert(std::make_pair("optgroup", factory<HTMLOptGroupElement>()));
    ret.insert(std::make_pair("option", factory<HTMLOptionElement>()));
    ret.insert(std::make_pair("output", factory<HTMLOutputElement>()));
    ret.insert(std::make_pair("p", factory<HTMLParagraphElement>()));
    ret.insert(std::make_pair("param", factory<HTMLParamElement>()));
    ret.insert(std::make_pair("pre", factory<HTMLPreElement>()));
    ret.insert(std::make_pair("progress", factory<HTMLProgressElement>()));
    ret.insert(std::make_pair("q", factory<HTMLQuoteElement>()));
    ret.insert(std::make_pair("rp", factory<HTMLElement>()));
    ret.insert(std::make_pair("rt", factory<HTMLElement>()));
    ret.insert(std::make_pair("ruby", factory<HTMLElement>()));
    ret.insert(std::make_pair("s", factory<HTMLElement>()));
    ret.insert(std::make_pair("samp", factory<HTMLElement>()));
    ret.insert(std::make_pair("script", factory<HTMLScriptElement>()));
    ret.insert(std::make_pair("section", factory<HTMLElement>()));
    ret.insert(std::make_pair("select", factory<HTMLSelectElement>()));
    ret.insert(std::make_pair("small", factory<HTMLElement>()));
    ret.insert(std::make_pair("source", factory<HTMLSourceElement>()));
    ret.insert(std::make_pair("span", factory<HTMLSpanElement>()));
    ret.insert(std::make_pair("strong", factory<HTMLElement>()));
    ret.insert(std::make_pair("style", factory<HTMLStyleElement>()));
    ret.insert(std::make_pair("sub", factory<HTMLElement>()));
    ret.insert(std::make_pair("summary", factory<HTMLElement>()));
    ret.insert(std::make_pair("sup", factory<HTMLElement>()));
    ret.insert(std::make_pair("table", factory<HTMLTableElement>()));
    ret.insert(std::make_pair("tbody", factory<HTMLTableSectionElement>()));
    ret.insert(std::make_pair("td", factory<HTMLTableDataCellElement>()));
    ret.insert(std::make_pair("textarea", factory<HTMLTextAreaElement>()));
    ret.insert(std::make_pair("tfoot", factory<HTMLTableSectionElement>()));
    ret.insert(std::make_pair("th", factory<HTMLTableHeaderCellElement>()));
    ret.insert(std::make_pair("thead", factory<HTMLTableSectionElement>()));
    ret.insert(std::make_pair("time", factory<HTMLTimeElement>()));
    ret.insert(std::make_pair("title", factory<HTMLTitleElement>()));
    ret.insert(std::make_pair("tr", factory<HTMLTableRowElement>()));
    ret.insert(std::make_pair("track", factory<HTMLTrackElement>()));
    ret.insert(std::make_pair("u", factory<HTMLElement>()));
    ret.insert(std::make_pair("ul", factory<HTMLUListElement>()));
    ret.insert(std::make_pair("var", factory<HTMLElement>()));
    ret.insert(std::make_pair("video", factory<HTMLVideoElement>()));
    ret.insert(std::make_pair("wbr", factory<HTMLElement>()));

    return ret;
  }
}

frenzy::dom::Elementp
frenzy::dom::Document::createElement(frenzy::ustring localName)
{
  verify_valid_name(localName);

  static const factorymap_t factories = get_factorymap();

  Elementp ret;

  factorymap_t::const_iterator it = factories.find(localName);
  if (it != factories.end())
  {
    ret = it->second->create(localName);
  }
  else
  {
    ret = HTMLUnknownElement::create(localName);
  }

  ret->recursive_set_ownerdocument(shared_from_this());
  return ret;
}

frenzy::dom::DocumentFragmentp
frenzy::dom::Document::createDocumentFragment()
{
  DocumentFragmentp ret = DocumentFragment::create();
  ret->recursive_set_ownerdocument(shared_from_this());
  return ret;
}

frenzy::dom::Textp
frenzy::dom::Document::createTextNode(frenzy::ustring data)
{
  Textp ret = Text::create(data);
  ret->recursive_set_ownerdocument(shared_from_this());
  return ret;
}

frenzy::dom::Commentp
frenzy::dom::Document::createComment(frenzy::ustring data)
{
  Commentp ret = Comment::create(data);
  ret->recursive_set_ownerdocument(shared_from_this());
  return ret;
}

frenzy::dom::Nodep
frenzy::dom::Document::adoptNode(frenzy::dom::Nodep node)
{
  if (node->get_nodeType() == Node::DOCUMENT_NODE)
  {
    // TODO: Throw a NotSupported exception
    return node;
  }

  if (node->get_nodeType() == Node::ELEMENT_NODE)
  {
    // TODO: base URL change
  }

  if (Nodep p = node->get_parentNode())
  {
    p->removeChild(node);
  }

  node->recursive_set_ownerdocument(shared_from_this());

  return node;
}

frenzy::dom::CDATASectionp
frenzy::dom::Document::createCDATASection(frenzy::ustring)
{
  throw DOMException(DOMException::NOT_SUPPORTED_ERR);
}

frenzy::dom::Attrp
frenzy::dom::Document::createAttribute(frenzy::ustring name)
{
  verify_valid_name(name);

  Attrp ret = Attr::create(name);
  ret->recursive_set_ownerdocument(shared_from_this());
  return ret;
}

frenzy::dom::EntityReferencep
frenzy::dom::Document::createEntityReference(ustring)
{
  throw DOMException(DOMException::NOT_SUPPORTED_ERR);
}

void
frenzy::dom::Document::graphicsfactory(boost::shared_ptr<frenzy::graphics::factory> f)
{
  recursive_drop_graphics();

  fact = f;

  layout_document();
}

boost::shared_ptr<frenzy::graphics::factory>
frenzy::dom::Document::graphicsfactory() const
{
  return fact;
}

void
frenzy::dom::Document::layout_document()
{
  if (!fact)
    return;

  get_documentElement()->layout(fact->page_width());
}

frenzy::dom::Document::Document()
{
}

frenzy::dom::Documentp
frenzy::dom::Document::shared_from_this()
{
  return boost::static_pointer_cast<Document>(Node::shared_from_this());
}

void
frenzy::dom::Document::verify_valid_name(frenzy::ustring name)
{
  if (name.empty())
    throw DOMException(DOMException::INVALID_CHARACTER_ERR);

  // DOM3 specifies valid names in terms of supported XML version,
  // which isn't really helpful here. HTML4 specifies NAME tokens to
  // consist of "a letter, followed by any number of letters, digits,
  // hyphens, underscores, colons and periods." This sounds like it
  // means the _value_ of a `name' attribute, but we'll just use that
  // here.
  ustring::const_iterator it = name.begin();
  if ((*it < 0x41 || *it > 0x5A) && // A-Z
      (*it < 0x61 || *it > 0x7A))   // a-z
  {
    throw DOMException(DOMException::INVALID_CHARACTER_ERR);
  }

  for (++it;
       it != name.end();
       ++it)
  {
    uchar u = *it;
    if ((u < 0x41 || u > 0x5A) && // A-Z
	(u < 0x61 || u > 0x7A) && // a-z
	(u < 0x30 || u > 0x39) && // 0-9
	u != '-' && u != '_' &&
	u != ':' && u != '.')
    {
      throw DOMException(DOMException::INVALID_CHARACTER_ERR);
    }    
  }
}

frenzy::dom::Node::nodeType
frenzy::dom::DocumentFragment::get_nodeType() const
{
  return Node::DOCUMENT_FRAGMENT_NODE;
}

frenzy::ustring
frenzy::dom::DocumentFragment::get_nodeName() const
{
  return "#document-fragment";
}

frenzy::dom::Nodep
frenzy::dom::DocumentFragment::cloneNode(bool deep) const
{
  DocumentFragmentp ret = DocumentFragment::create();
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::DocumentFragmentp
frenzy::dom::DocumentFragment::create()
{
  return DocumentFragmentp(new DocumentFragment);
}

frenzy::dom::DocumentFragment::DocumentFragment()
{
}

frenzy::dom::Node::nodeType
frenzy::dom::DocumentType::get_nodeType() const
{
  return Node::DOCUMENT_TYPE_NODE;
}

frenzy::ustring
frenzy::dom::DocumentType::get_nodeName() const
{
  return get_name();
}

frenzy::dom::Nodep
frenzy::dom::DocumentType::cloneNode(bool deep) const
{
  DocumentTypep ret = DocumentType::create(name, publicid, systemid);
  copyTo(ret, deep);
  return ret;
}

frenzy::ustring
frenzy::dom::DocumentType::get_name() const
{
  return name;
}

frenzy::ustring
frenzy::dom::DocumentType::get_publicId() const
{
  return publicid;
}

frenzy::ustring
frenzy::dom::DocumentType::get_systemId() const
{
  return systemid;
}

frenzy::dom::NamedNodeMapp
frenzy::dom::DocumentType::get_entities() const
{
  return NamedNodeMapp();
}

frenzy::dom::NamedNodeMapp
frenzy::dom::DocumentType::get_notations() const
{
  return NamedNodeMapp();
}

frenzy::dom::DocumentTypep
frenzy::dom::DocumentType::create(frenzy::ustring name,
				  frenzy::ustring publicid,
				  frenzy::ustring systemid)
{
  return DocumentTypep(new DocumentType(name, publicid, systemid));
}

frenzy::dom::DocumentType::DocumentType(frenzy::ustring name,
					frenzy::ustring publicid,
					frenzy::ustring systemid)
  : name(name)
  , publicid(publicid)
  , systemid(systemid)
{
}

bool
frenzy::dom::DOMImplementation::hasFeature(frenzy::ustring feature,
					   frenzy::ustring version) const
{
  // TODO: Proper checks required for SVG features. For other
  // features, this function is specified to always return true (not
  // kidding).
  (void)feature;
  (void)version;
  return true;
}

frenzy::dom::DOMImplementationp
frenzy::dom::DOMImplementation::create()
{
  return DOMImplementationp(new DOMImplementation);
}

frenzy::dom::DOMImplementation::DOMImplementation()
{
}
