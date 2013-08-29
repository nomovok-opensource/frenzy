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

#include "dom/node.hpp"
#include "dom/document.hpp"
#include "dom/element.hpp"
#include "dom/htmlelement.hpp"
#include "dom/pointers.hpp"
#include "test_helpers.hpp"

using namespace frenzy;
using namespace frenzy::dom;
using namespace frenzy::test_helpers;

namespace
{
}

BOOST_AUTO_TEST_SUITE(dom_tests)

BOOST_AUTO_TEST_CASE(document_owner)
{
  Documentp doc = Document::create();

  Elementp elem = doc->createElement("div");

  BOOST_CHECK_EQUAL(elem->get_ownerDocument(), doc);
  BOOST_CHECK(!doc->get_documentElement());

  doc->appendChild(elem);

  BOOST_CHECK_EQUAL(elem->get_parentNode(), doc);
  BOOST_CHECK_EQUAL(doc->get_documentElement(), elem);
}

BOOST_AUTO_TEST_CASE(dangling_references)
{
  Documentp doc = Document::create();

  Elementp elem1 = doc->createElement("div");
  Elementp elem2 = doc->createElement("div");
  Attrp attr = doc->createAttribute("class");

  attr->set_value("foo");

  doc->appendChild(elem1);
  elem1->appendChild(elem2);
  elem2->setAttributeNode(attr);

  BOOST_CHECK_EQUAL(elem1->get_ownerDocument(), doc);
  BOOST_CHECK_EQUAL(elem2->get_ownerDocument(), doc);
  BOOST_CHECK_EQUAL(doc->get_documentElement(), elem1);

  BOOST_CHECK_EQUAL(elem1->get_parentNode(), doc);
  BOOST_CHECK_EQUAL(elem2->get_parentNode(), elem1);

  // Grab weak references for checking aliveness
  Documentwp wdoc = doc;
  Elementwp welem1 = elem1;
  Elementwp welem2 = elem2;
  Attrwp wattr = attr;

  attr.reset();

  BOOST_CHECK(!wdoc.expired());
  BOOST_CHECK(!welem1.expired());
  BOOST_CHECK(!welem2.expired());
  BOOST_CHECK(!wattr.expired());

  elem2.reset();
  
  BOOST_CHECK(!wdoc.expired());
  BOOST_CHECK(!welem1.expired());
  BOOST_CHECK(!welem2.expired());
  BOOST_CHECK(!wattr.expired());
  
  elem1.reset();

  BOOST_CHECK(!wdoc.expired());
  BOOST_CHECK(!welem1.expired());
  BOOST_CHECK(!welem2.expired());
  BOOST_CHECK(!wattr.expired());

  doc.reset();

  BOOST_CHECK(wdoc.expired());
  BOOST_CHECK(welem1.expired());
  BOOST_CHECK(welem2.expired());
  BOOST_CHECK(wattr.expired());
}

BOOST_AUTO_TEST_CASE(attribute_ownerelement_died)
{
  Documentp doc = Document::create();

  Elementp elem = doc->createElement("div");
  Attrp attr = doc->createAttribute("class");

  attr->set_value("foo");
  elem->setAttributeNode(attr);

  Elementwp welem = elem;
  elem.reset();

  BOOST_REQUIRE(welem.expired());
  BOOST_CHECK(!attr->get_ownerElement());
}

BOOST_AUTO_TEST_CASE(moving_between_documents)
{
  Documentp doc1 = Document::create();
  Documentp doc2 = Document::create();

  Elementp elem = doc1->createElement("div");

  doc1->appendChild(elem);

  BOOST_CHECK_EQUAL(elem->get_ownerDocument(), doc1);
  BOOST_CHECK_EQUAL(elem->get_parentNode(), doc1);
  BOOST_CHECK_EQUAL(doc1->get_documentElement(), elem);
  BOOST_CHECK(!doc2->get_documentElement());

  // Note: This only works if the two documents are of the same
  // type. When different document types are implemented, this test
  // needs to make sure the documents really are of the same type.
  doc2->appendChild(elem);

  BOOST_CHECK_EQUAL(elem->get_ownerDocument(), doc2);
  BOOST_CHECK_EQUAL(elem->get_parentNode(), doc2);
  BOOST_CHECK(!doc1->get_documentElement());
  BOOST_CHECK_EQUAL(doc2->get_documentElement(), elem);
}

BOOST_AUTO_TEST_CASE(child_relations)
{
  Documentp doc = Document::create();

  Elementp root = doc->createElement("div");
  Elementp one = doc->createElement("div");
  Elementp two = doc->createElement("div");
  Elementp three = doc->createElement("div");

  root->appendChild(one);
  root->appendChild(two);
  root->appendChild(three);

  BOOST_CHECK_EQUAL(root->get_firstChild(), one);
  BOOST_CHECK_EQUAL(root->get_lastChild(), three);
  
  BOOST_CHECK_EQUAL(one->get_nextSibling(), two);
  BOOST_CHECK_EQUAL(two->get_nextSibling(), three);
  BOOST_CHECK(!three->get_nextSibling());

  BOOST_CHECK_EQUAL(three->get_previousSibling(), two);
  BOOST_CHECK_EQUAL(two->get_previousSibling(), one);
  BOOST_CHECK(!one->get_previousSibling());

  one->appendChild(three);

  BOOST_CHECK_EQUAL(three->get_parentNode(), one);
  BOOST_CHECK(!two->get_nextSibling());
  BOOST_CHECK(!three->get_nextSibling());
  BOOST_CHECK(!three->get_previousSibling());
  BOOST_CHECK_EQUAL(one->get_firstChild(), three);
  BOOST_CHECK_EQUAL(one->get_lastChild(), three);
}

BOOST_AUTO_TEST_CASE(create_element_proper_types)
{
  Documentp doc = Document::create();

  // HTML5 section 4: The elements of HTML

  // TODO: A proper test would also check that the concrete type is
  // _not_ anything derived from HTMLElement, in the cases where the
  // proper type is HTMLElement directly.
  BOOST_CHECK(dom_cast<HTMLAnchorElement>(doc->createElement("a")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("abbr")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("address")));
  BOOST_CHECK(dom_cast<HTMLAreaElement>(doc->createElement("area")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("article")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("aside")));
  BOOST_CHECK(dom_cast<HTMLAudioElement>(doc->createElement("audio")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("b")));
  BOOST_CHECK(dom_cast<HTMLBaseElement>(doc->createElement("base")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("bdi")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("bdo")));
  BOOST_CHECK(dom_cast<HTMLQuoteElement>(doc->createElement("blockquote")));
  BOOST_CHECK(dom_cast<HTMLBodyElement>(doc->createElement("body")));
  BOOST_CHECK(dom_cast<HTMLBRElement>(doc->createElement("br")));
  BOOST_CHECK(dom_cast<HTMLButtonElement>(doc->createElement("button")));
  BOOST_CHECK(dom_cast<HTMLCanvasElement>(doc->createElement("canvas")));
  BOOST_CHECK(dom_cast<HTMLTableCaptionElement>(doc->createElement("caption")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("cite")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("code")));
  BOOST_CHECK(dom_cast<HTMLTableColElement>(doc->createElement("col")));
  BOOST_CHECK(dom_cast<HTMLTableColElement>(doc->createElement("colgroup")));
  BOOST_CHECK(dom_cast<HTMLCommandElement>(doc->createElement("command")));
  BOOST_CHECK(dom_cast<HTMLDataElement>(doc->createElement("data")));
  BOOST_CHECK(dom_cast<HTMLDataListElement>(doc->createElement("datalist")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("dd")));
  BOOST_CHECK(dom_cast<HTMLModElement>(doc->createElement("del")));
  BOOST_CHECK(dom_cast<HTMLDetailsElement>(doc->createElement("details")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("dfn")));
  BOOST_CHECK(dom_cast<HTMLDialogElement>(doc->createElement("dialog")));
  BOOST_CHECK(dom_cast<HTMLDivElement>(doc->createElement("div")));
  BOOST_CHECK(dom_cast<HTMLDListElement>(doc->createElement("dl")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("dt")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("em")));
  BOOST_CHECK(dom_cast<HTMLEmbedElement>(doc->createElement("embed")));
  BOOST_CHECK(dom_cast<HTMLFieldSetElement>(doc->createElement("fieldset")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("figcaption")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("figure")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("footer")));
  BOOST_CHECK(dom_cast<HTMLFormElement>(doc->createElement("form")));
  BOOST_CHECK(dom_cast<HTMLHeadingElement>(doc->createElement("h1")));
  BOOST_CHECK(dom_cast<HTMLHeadingElement>(doc->createElement("h2")));
  BOOST_CHECK(dom_cast<HTMLHeadingElement>(doc->createElement("h3")));
  BOOST_CHECK(dom_cast<HTMLHeadingElement>(doc->createElement("h4")));
  BOOST_CHECK(dom_cast<HTMLHeadingElement>(doc->createElement("h5")));
  BOOST_CHECK(dom_cast<HTMLHeadingElement>(doc->createElement("h6")));
  BOOST_CHECK(dom_cast<HTMLHeadElement>(doc->createElement("head")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("header")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("hgroup")));
  BOOST_CHECK(dom_cast<HTMLHRElement>(doc->createElement("hr")));
  BOOST_CHECK(dom_cast<HTMLHtmlElement>(doc->createElement("html")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("i")));
  BOOST_CHECK(dom_cast<HTMLIFrameElement>(doc->createElement("iframe")));
  BOOST_CHECK(dom_cast<HTMLImageElement>(doc->createElement("img")));
  BOOST_CHECK(dom_cast<HTMLInputElement>(doc->createElement("input")));
  BOOST_CHECK(dom_cast<HTMLModElement>(doc->createElement("ins")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("kbd")));
  BOOST_CHECK(dom_cast<HTMLKeygenElement>(doc->createElement("keygen")));
  BOOST_CHECK(dom_cast<HTMLLabelElement>(doc->createElement("label")));
  BOOST_CHECK(dom_cast<HTMLLegendElement>(doc->createElement("legend")));
  BOOST_CHECK(dom_cast<HTMLLIElement>(doc->createElement("li")));
  BOOST_CHECK(dom_cast<HTMLLinkElement>(doc->createElement("link")));
  BOOST_CHECK(dom_cast<HTMLMapElement>(doc->createElement("map")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("mark")));
  BOOST_CHECK(dom_cast<HTMLMenuElement>(doc->createElement("menu")));
  BOOST_CHECK(dom_cast<HTMLMetaElement>(doc->createElement("meta")));
  BOOST_CHECK(dom_cast<HTMLMeterElement>(doc->createElement("meter")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("nav")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("noscript")));
  BOOST_CHECK(dom_cast<HTMLObjectElement>(doc->createElement("object")));
  BOOST_CHECK(dom_cast<HTMLOListElement>(doc->createElement("ol")));
  BOOST_CHECK(dom_cast<HTMLOptGroupElement>(doc->createElement("optgroup")));
  BOOST_CHECK(dom_cast<HTMLOptionElement>(doc->createElement("option")));
  BOOST_CHECK(dom_cast<HTMLOutputElement>(doc->createElement("output")));
  BOOST_CHECK(dom_cast<HTMLParagraphElement>(doc->createElement("p")));
  BOOST_CHECK(dom_cast<HTMLParamElement>(doc->createElement("param")));
  BOOST_CHECK(dom_cast<HTMLPreElement>(doc->createElement("pre")));
  BOOST_CHECK(dom_cast<HTMLProgressElement>(doc->createElement("progress")));
  BOOST_CHECK(dom_cast<HTMLQuoteElement>(doc->createElement("q")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("rp")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("rt")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("ruby")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("s")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("samp")));
  BOOST_CHECK(dom_cast<HTMLScriptElement>(doc->createElement("script")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("section")));
  BOOST_CHECK(dom_cast<HTMLSelectElement>(doc->createElement("select")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("small")));
  BOOST_CHECK(dom_cast<HTMLSourceElement>(doc->createElement("source")));
  BOOST_CHECK(dom_cast<HTMLSpanElement>(doc->createElement("span")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("strong")));
  BOOST_CHECK(dom_cast<HTMLStyleElement>(doc->createElement("style")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("sub")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("summary")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("sup")));
  BOOST_CHECK(dom_cast<HTMLTableElement>(doc->createElement("table")));
  BOOST_CHECK(dom_cast<HTMLTableSectionElement>(doc->createElement("tbody")));
  BOOST_CHECK(dom_cast<HTMLTableDataCellElement>(doc->createElement("td")));
  BOOST_CHECK(dom_cast<HTMLTextAreaElement>(doc->createElement("textarea")));
  BOOST_CHECK(dom_cast<HTMLTableSectionElement>(doc->createElement("tfoot")));
  BOOST_CHECK(dom_cast<HTMLTableHeaderCellElement>(doc->createElement("th")));
  BOOST_CHECK(dom_cast<HTMLTableSectionElement>(doc->createElement("thead")));
  BOOST_CHECK(dom_cast<HTMLTimeElement>(doc->createElement("time")));
  BOOST_CHECK(dom_cast<HTMLTitleElement>(doc->createElement("title")));
  BOOST_CHECK(dom_cast<HTMLTableRowElement>(doc->createElement("tr")));
  BOOST_CHECK(dom_cast<HTMLTrackElement>(doc->createElement("track")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("u")));
  BOOST_CHECK(dom_cast<HTMLUListElement>(doc->createElement("ul")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("var")));
  BOOST_CHECK(dom_cast<HTMLVideoElement>(doc->createElement("video")));
  BOOST_CHECK(dom_cast<HTMLElement>(doc->createElement("wbr")));

  BOOST_CHECK(dom_cast<HTMLUnknownElement>(doc->createElement("foobarquz")));
}

BOOST_AUTO_TEST_SUITE_END()
