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

#ifndef FRENZY_POINTERS_HPP
#define FRENZY_POINTERS_HPP

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#define FRENZY_DECLARE_TYPE(x) \
  typedef boost::shared_ptr<struct x> x ## p; \
  typedef boost::weak_ptr<struct x> x ## wp; \
  typedef boost::shared_ptr<const struct x> C ## x ## p; \
  typedef boost::weak_ptr<const struct x> C ## x ## wp

namespace frenzy
{
  namespace dom
  {
    // event.hpp
    FRENZY_DECLARE_TYPE(Event);
    FRENZY_DECLARE_TYPE(CustomEvent);
    FRENZY_DECLARE_TYPE(EventTarget);
    FRENZY_DECLARE_TYPE(EventListener);

    // node.hpp
    FRENZY_DECLARE_TYPE(Node);
    FRENZY_DECLARE_TYPE(NodeList);

    // element.hpp
    FRENZY_DECLARE_TYPE(Element);
    FRENZY_DECLARE_TYPE(HTMLCollection);
    FRENZY_DECLARE_TYPE(NamedNodeMap);
    FRENZY_DECLARE_TYPE(Attr);

    // document.hpp
    FRENZY_DECLARE_TYPE(Document);
    FRENZY_DECLARE_TYPE(XMLDocument);
    FRENZY_DECLARE_TYPE(DocumentFragment);
    FRENZY_DECLARE_TYPE(DocumentType);
    FRENZY_DECLARE_TYPE(DOMImplementation);
    FRENZY_DECLARE_TYPE(EntityReference);

    // text.hpp
    FRENZY_DECLARE_TYPE(CharacterData);
    FRENZY_DECLARE_TYPE(Text);
    FRENZY_DECLARE_TYPE(Comment);
    FRENZY_DECLARE_TYPE(CDATASection);

    // htmlelement.hpp
    FRENZY_DECLARE_TYPE(HTMLElement);
    FRENZY_DECLARE_TYPE(HTMLUnknownElement);
    FRENZY_DECLARE_TYPE(HTMLHtmlElement);
    FRENZY_DECLARE_TYPE(HTMLHeadElement);
    FRENZY_DECLARE_TYPE(HTMLTitleElement);
    FRENZY_DECLARE_TYPE(HTMLBaseElement);
    FRENZY_DECLARE_TYPE(HTMLLinkElement);
    FRENZY_DECLARE_TYPE(HTMLMetaElement);
    FRENZY_DECLARE_TYPE(HTMLStyleElement);
    FRENZY_DECLARE_TYPE(HTMLScriptElement);
    FRENZY_DECLARE_TYPE(HTMLBodyElement);
    FRENZY_DECLARE_TYPE(HTMLAnchorElement);
    FRENZY_DECLARE_TYPE(HTMLAreaElement);
    FRENZY_DECLARE_TYPE(HTMLAudioElement);
    FRENZY_DECLARE_TYPE(HTMLBRElement);
    FRENZY_DECLARE_TYPE(HTMLButtonElement);
    FRENZY_DECLARE_TYPE(HTMLCanvasElement);
    FRENZY_DECLARE_TYPE(HTMLCommandElement);
    FRENZY_DECLARE_TYPE(HTMLDListElement);
    FRENZY_DECLARE_TYPE(HTMLDataElement);
    FRENZY_DECLARE_TYPE(HTMLDataListElement);
    FRENZY_DECLARE_TYPE(HTMLDetailsElement);
    FRENZY_DECLARE_TYPE(HTMLDialogElement);
    FRENZY_DECLARE_TYPE(HTMLDivElement);
    FRENZY_DECLARE_TYPE(HTMLEmbedElement);
    FRENZY_DECLARE_TYPE(HTMLFieldSetElement);
    FRENZY_DECLARE_TYPE(HTMLFormElement);
    FRENZY_DECLARE_TYPE(HTMLHRElement);
    FRENZY_DECLARE_TYPE(HTMLHeadElement);
    FRENZY_DECLARE_TYPE(HTMLHeadingElement);
    FRENZY_DECLARE_TYPE(HTMLHtmlElement);
    FRENZY_DECLARE_TYPE(HTMLIFrameElement);
    FRENZY_DECLARE_TYPE(HTMLImageElement);
    FRENZY_DECLARE_TYPE(HTMLInputElement);
    FRENZY_DECLARE_TYPE(HTMLKeygenElement);
    FRENZY_DECLARE_TYPE(HTMLLIElement);
    FRENZY_DECLARE_TYPE(HTMLLabelElement);
    FRENZY_DECLARE_TYPE(HTMLLegendElement);
    FRENZY_DECLARE_TYPE(HTMLLinkElement);
    FRENZY_DECLARE_TYPE(HTMLMapElement);
    FRENZY_DECLARE_TYPE(HTMLMenuElement);
    FRENZY_DECLARE_TYPE(HTMLMetaElement);
    FRENZY_DECLARE_TYPE(HTMLMeterElement);
    FRENZY_DECLARE_TYPE(HTMLModElement);
    FRENZY_DECLARE_TYPE(HTMLOListElement);
    FRENZY_DECLARE_TYPE(HTMLObjectElement);
    FRENZY_DECLARE_TYPE(HTMLOptGroupElement);
    FRENZY_DECLARE_TYPE(HTMLOptionElement);
    FRENZY_DECLARE_TYPE(HTMLOutputElement);
    FRENZY_DECLARE_TYPE(HTMLParagraphElement);
    FRENZY_DECLARE_TYPE(HTMLParamElement);
    FRENZY_DECLARE_TYPE(HTMLPreElement);
    FRENZY_DECLARE_TYPE(HTMLProgressElement);
    FRENZY_DECLARE_TYPE(HTMLQuoteElement);
    FRENZY_DECLARE_TYPE(HTMLScriptElement);
    FRENZY_DECLARE_TYPE(HTMLSelectElement);
    FRENZY_DECLARE_TYPE(HTMLSourceElement);
    FRENZY_DECLARE_TYPE(HTMLSpanElement);
    FRENZY_DECLARE_TYPE(HTMLStyleElement);
    FRENZY_DECLARE_TYPE(HTMLTableCaptionElement);
    FRENZY_DECLARE_TYPE(HTMLTableColElement);
    FRENZY_DECLARE_TYPE(HTMLTableDataCellElement);
    FRENZY_DECLARE_TYPE(HTMLTableElement);
    FRENZY_DECLARE_TYPE(HTMLTableHeaderCellElement);
    FRENZY_DECLARE_TYPE(HTMLTableRowElement);
    FRENZY_DECLARE_TYPE(HTMLTableSectionElement);
    FRENZY_DECLARE_TYPE(HTMLTextAreaElement);
    FRENZY_DECLARE_TYPE(HTMLTimeElement);
    FRENZY_DECLARE_TYPE(HTMLTitleElement);
    FRENZY_DECLARE_TYPE(HTMLTrackElement);
    FRENZY_DECLARE_TYPE(HTMLUListElement);
    FRENZY_DECLARE_TYPE(HTMLVideoElement);
  }

  template <typename T, typename U>
  boost::shared_ptr<T> dom_cast(boost::shared_ptr<U> p)
  {
    return boost::dynamic_pointer_cast<T>(p);
  }
}

#undef FRENZY_DECLARE_TYPE

#endif
