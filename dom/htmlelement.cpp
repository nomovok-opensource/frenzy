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

#include "htmlelement.hpp"
#include "text.hpp"

frenzy::dom::Nodep
frenzy::dom::HTMLElement::cloneNode(bool deep) const
{
  HTMLElementp ret = HTMLElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLElementp
frenzy::dom::HTMLElement::create(frenzy::ustring localname)
{
  return HTMLElementp(new HTMLElement(localname));
}

frenzy::dom::HTMLElement::HTMLElement(frenzy::ustring localname)
  : Element(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLUnknownElement::cloneNode(bool deep) const
{
  HTMLUnknownElementp ret = HTMLUnknownElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLUnknownElementp
frenzy::dom::HTMLUnknownElement::create(frenzy::ustring localname)
{
  return HTMLUnknownElementp(new HTMLUnknownElement(localname));
}

frenzy::dom::HTMLUnknownElement::HTMLUnknownElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLAnchorElement::cloneNode(bool deep) const
{
  HTMLAnchorElementp ret = HTMLAnchorElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLAnchorElementp
frenzy::dom::HTMLAnchorElement::create(frenzy::ustring localname)
{
  return HTMLAnchorElementp(new HTMLAnchorElement(localname));
}

frenzy::dom::HTMLAnchorElement::HTMLAnchorElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLAreaElement::cloneNode(bool deep) const
{
  HTMLAreaElementp ret = HTMLAreaElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLAreaElementp
frenzy::dom::HTMLAreaElement::create(frenzy::ustring localname)
{
  return HTMLAreaElementp(new HTMLAreaElement(localname));
}

frenzy::dom::HTMLAreaElement::HTMLAreaElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLBaseElement::cloneNode(bool deep) const
{
  HTMLBaseElementp ret = HTMLBaseElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLBaseElementp
frenzy::dom::HTMLBaseElement::create(frenzy::ustring localname)
{
  return HTMLBaseElementp(new HTMLBaseElement(localname));
}

frenzy::dom::HTMLBaseElement::HTMLBaseElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLBodyElement::cloneNode(bool deep) const
{
  HTMLBodyElementp ret = HTMLBodyElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLBodyElementp
frenzy::dom::HTMLBodyElement::create(frenzy::ustring localname)
{
  return HTMLBodyElementp(new HTMLBodyElement(localname));
}

frenzy::dom::HTMLBodyElement::HTMLBodyElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLBRElement::cloneNode(bool deep) const
{
  HTMLBRElementp ret = HTMLBRElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLBRElementp
frenzy::dom::HTMLBRElement::create(frenzy::ustring localname)
{
  return HTMLBRElementp(new HTMLBRElement(localname));
}

frenzy::dom::HTMLBRElement::HTMLBRElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLButtonElement::cloneNode(bool deep) const
{
  HTMLButtonElementp ret = HTMLButtonElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLButtonElementp
frenzy::dom::HTMLButtonElement::create(frenzy::ustring localname)
{
  return HTMLButtonElementp(new HTMLButtonElement(localname));
}

frenzy::dom::HTMLButtonElement::HTMLButtonElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLCanvasElement::cloneNode(bool deep) const
{
  HTMLCanvasElementp ret = HTMLCanvasElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLCanvasElementp
frenzy::dom::HTMLCanvasElement::create(frenzy::ustring localname)
{
  return HTMLCanvasElementp(new HTMLCanvasElement(localname));
}

frenzy::dom::HTMLCanvasElement::HTMLCanvasElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLCommandElement::cloneNode(bool deep) const
{
  HTMLCommandElementp ret = HTMLCommandElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLCommandElementp
frenzy::dom::HTMLCommandElement::create(frenzy::ustring localname)
{
  return HTMLCommandElementp(new HTMLCommandElement(localname));
}

frenzy::dom::HTMLCommandElement::HTMLCommandElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLDataElement::cloneNode(bool deep) const
{
  HTMLDataElementp ret = HTMLDataElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLDataElementp
frenzy::dom::HTMLDataElement::create(frenzy::ustring localname)
{
  return HTMLDataElementp(new HTMLDataElement(localname));
}

frenzy::dom::HTMLDataElement::HTMLDataElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLDataListElement::cloneNode(bool deep) const
{
  HTMLDataListElementp ret = HTMLDataListElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLDataListElementp
frenzy::dom::HTMLDataListElement::create(frenzy::ustring localname)
{
  return HTMLDataListElementp(new HTMLDataListElement(localname));
}

frenzy::dom::HTMLDataListElement::HTMLDataListElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLDetailsElement::cloneNode(bool deep) const
{
  HTMLDetailsElementp ret = HTMLDetailsElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLDetailsElementp
frenzy::dom::HTMLDetailsElement::create(frenzy::ustring localname)
{
  return HTMLDetailsElementp(new HTMLDetailsElement(localname));
}

frenzy::dom::HTMLDetailsElement::HTMLDetailsElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLDialogElement::cloneNode(bool deep) const
{
  HTMLDialogElementp ret = HTMLDialogElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLDialogElementp
frenzy::dom::HTMLDialogElement::create(frenzy::ustring localname)
{
  return HTMLDialogElementp(new HTMLDialogElement(localname));
}

frenzy::dom::HTMLDialogElement::HTMLDialogElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLDivElement::cloneNode(bool deep) const
{
  HTMLDivElementp ret = HTMLDivElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLDivElementp
frenzy::dom::HTMLDivElement::create(frenzy::ustring localname)
{
  return HTMLDivElementp(new HTMLDivElement(localname));
}

frenzy::dom::HTMLDivElement::HTMLDivElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLDListElement::cloneNode(bool deep) const
{
  HTMLDListElementp ret = HTMLDListElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLDListElementp
frenzy::dom::HTMLDListElement::create(frenzy::ustring localname)
{
  return HTMLDListElementp(new HTMLDListElement(localname));
}

frenzy::dom::HTMLDListElement::HTMLDListElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLEmbedElement::cloneNode(bool deep) const
{
  HTMLEmbedElementp ret = HTMLEmbedElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLEmbedElementp
frenzy::dom::HTMLEmbedElement::create(frenzy::ustring localname)
{
  return HTMLEmbedElementp(new HTMLEmbedElement(localname));
}

frenzy::dom::HTMLEmbedElement::HTMLEmbedElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLFieldSetElement::cloneNode(bool deep) const
{
  HTMLFieldSetElementp ret = HTMLFieldSetElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLFieldSetElementp
frenzy::dom::HTMLFieldSetElement::create(frenzy::ustring localname)
{
  return HTMLFieldSetElementp(new HTMLFieldSetElement(localname));
}

frenzy::dom::HTMLFieldSetElement::HTMLFieldSetElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLFormElement::cloneNode(bool deep) const
{
  HTMLFormElementp ret = HTMLFormElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLFormElementp
frenzy::dom::HTMLFormElement::create(frenzy::ustring localname)
{
  return HTMLFormElementp(new HTMLFormElement(localname));
}

frenzy::dom::HTMLFormElement::HTMLFormElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLHeadElement::cloneNode(bool deep) const
{
  HTMLHeadElementp ret = HTMLHeadElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLHeadElementp
frenzy::dom::HTMLHeadElement::create(frenzy::ustring localname)
{
  return HTMLHeadElementp(new HTMLHeadElement(localname));
}

frenzy::dom::HTMLHeadElement::HTMLHeadElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLHeadingElement::cloneNode(bool deep) const
{
  HTMLHeadingElementp ret = HTMLHeadingElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLHeadingElementp
frenzy::dom::HTMLHeadingElement::create(frenzy::ustring localname)
{
  return HTMLHeadingElementp(new HTMLHeadingElement(localname));
}

frenzy::dom::HTMLHeadingElement::HTMLHeadingElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLHRElement::cloneNode(bool deep) const
{
  HTMLHRElementp ret = HTMLHRElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLHRElementp
frenzy::dom::HTMLHRElement::create(frenzy::ustring localname)
{
  return HTMLHRElementp(new HTMLHRElement(localname));
}

frenzy::dom::HTMLHRElement::HTMLHRElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLHtmlElement::cloneNode(bool deep) const
{
  HTMLHtmlElementp ret = HTMLHtmlElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLHtmlElementp
frenzy::dom::HTMLHtmlElement::create(frenzy::ustring localname)
{
  return HTMLHtmlElementp(new HTMLHtmlElement(localname));
}

frenzy::dom::HTMLHtmlElement::HTMLHtmlElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLIFrameElement::cloneNode(bool deep) const
{
  HTMLIFrameElementp ret = HTMLIFrameElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLIFrameElementp
frenzy::dom::HTMLIFrameElement::create(frenzy::ustring localname)
{
  return HTMLIFrameElementp(new HTMLIFrameElement(localname));
}

frenzy::dom::HTMLIFrameElement::HTMLIFrameElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLImageElement::cloneNode(bool deep) const
{
  HTMLImageElementp ret = HTMLImageElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLImageElementp
frenzy::dom::HTMLImageElement::create(frenzy::ustring localname)
{
  return HTMLImageElementp(new HTMLImageElement(localname));
}

frenzy::dom::HTMLImageElement::HTMLImageElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLInputElement::cloneNode(bool deep) const
{
  HTMLInputElementp ret = HTMLInputElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLInputElementp
frenzy::dom::HTMLInputElement::create(frenzy::ustring localname)
{
  return HTMLInputElementp(new HTMLInputElement(localname));
}

frenzy::dom::HTMLInputElement::HTMLInputElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLKeygenElement::cloneNode(bool deep) const
{
  HTMLKeygenElementp ret = HTMLKeygenElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLKeygenElementp
frenzy::dom::HTMLKeygenElement::create(frenzy::ustring localname)
{
  return HTMLKeygenElementp(new HTMLKeygenElement(localname));
}

frenzy::dom::HTMLKeygenElement::HTMLKeygenElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLLabelElement::cloneNode(bool deep) const
{
  HTMLLabelElementp ret = HTMLLabelElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLLabelElementp
frenzy::dom::HTMLLabelElement::create(frenzy::ustring localname)
{
  return HTMLLabelElementp(new HTMLLabelElement(localname));
}

frenzy::dom::HTMLLabelElement::HTMLLabelElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLLegendElement::cloneNode(bool deep) const
{
  HTMLLegendElementp ret = HTMLLegendElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLLegendElementp
frenzy::dom::HTMLLegendElement::create(frenzy::ustring localname)
{
  return HTMLLegendElementp(new HTMLLegendElement(localname));
}

frenzy::dom::HTMLLegendElement::HTMLLegendElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLLIElement::cloneNode(bool deep) const
{
  HTMLLIElementp ret = HTMLLIElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLLIElementp
frenzy::dom::HTMLLIElement::create(frenzy::ustring localname)
{
  return HTMLLIElementp(new HTMLLIElement(localname));
}

frenzy::dom::HTMLLIElement::HTMLLIElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLLinkElement::cloneNode(bool deep) const
{
  HTMLLinkElementp ret = HTMLLinkElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLLinkElementp
frenzy::dom::HTMLLinkElement::create(frenzy::ustring localname)
{
  return HTMLLinkElementp(new HTMLLinkElement(localname));
}

frenzy::dom::HTMLLinkElement::HTMLLinkElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLMapElement::cloneNode(bool deep) const
{
  HTMLMapElementp ret = HTMLMapElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLMapElementp
frenzy::dom::HTMLMapElement::create(frenzy::ustring localname)
{
  return HTMLMapElementp(new HTMLMapElement(localname));
}

frenzy::dom::HTMLMapElement::HTMLMapElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLMenuElement::cloneNode(bool deep) const
{
  HTMLMenuElementp ret = HTMLMenuElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLMenuElementp
frenzy::dom::HTMLMenuElement::create(frenzy::ustring localname)
{
  return HTMLMenuElementp(new HTMLMenuElement(localname));
}

frenzy::dom::HTMLMenuElement::HTMLMenuElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLMetaElement::cloneNode(bool deep) const
{
  HTMLMetaElementp ret = HTMLMetaElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLMetaElementp
frenzy::dom::HTMLMetaElement::create(frenzy::ustring localname)
{
  return HTMLMetaElementp(new HTMLMetaElement(localname));
}

frenzy::dom::HTMLMetaElement::HTMLMetaElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLMeterElement::cloneNode(bool deep) const
{
  HTMLMeterElementp ret = HTMLMeterElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLMeterElementp
frenzy::dom::HTMLMeterElement::create(frenzy::ustring localname)
{
  return HTMLMeterElementp(new HTMLMeterElement(localname));
}

frenzy::dom::HTMLMeterElement::HTMLMeterElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLModElement::cloneNode(bool deep) const
{
  HTMLModElementp ret = HTMLModElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLModElementp
frenzy::dom::HTMLModElement::create(frenzy::ustring localname)
{
  return HTMLModElementp(new HTMLModElement(localname));
}

frenzy::dom::HTMLModElement::HTMLModElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLObjectElement::cloneNode(bool deep) const
{
  HTMLObjectElementp ret = HTMLObjectElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLObjectElementp
frenzy::dom::HTMLObjectElement::create(frenzy::ustring localname)
{
  return HTMLObjectElementp(new HTMLObjectElement(localname));
}

frenzy::dom::HTMLObjectElement::HTMLObjectElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLOListElement::cloneNode(bool deep) const
{
  HTMLOListElementp ret = HTMLOListElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLOListElementp
frenzy::dom::HTMLOListElement::create(frenzy::ustring localname)
{
  return HTMLOListElementp(new HTMLOListElement(localname));
}

frenzy::dom::HTMLOListElement::HTMLOListElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLOptGroupElement::cloneNode(bool deep) const
{
  HTMLOptGroupElementp ret = HTMLOptGroupElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLOptGroupElementp
frenzy::dom::HTMLOptGroupElement::create(frenzy::ustring localname)
{
  return HTMLOptGroupElementp(new HTMLOptGroupElement(localname));
}

frenzy::dom::HTMLOptGroupElement::HTMLOptGroupElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLOptionElement::cloneNode(bool deep) const
{
  HTMLOptionElementp ret = HTMLOptionElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLOptionElementp
frenzy::dom::HTMLOptionElement::create(frenzy::ustring localname)
{
  return HTMLOptionElementp(new HTMLOptionElement(localname));
}

frenzy::dom::HTMLOptionElement::HTMLOptionElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLOutputElement::cloneNode(bool deep) const
{
  HTMLOutputElementp ret = HTMLOutputElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLOutputElementp
frenzy::dom::HTMLOutputElement::create(frenzy::ustring localname)
{
  return HTMLOutputElementp(new HTMLOutputElement(localname));
}

frenzy::dom::HTMLOutputElement::HTMLOutputElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLParagraphElement::cloneNode(bool deep) const
{
  HTMLParagraphElementp ret = HTMLParagraphElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLParagraphElementp
frenzy::dom::HTMLParagraphElement::create(frenzy::ustring localname)
{
  return HTMLParagraphElementp(new HTMLParagraphElement(localname));
}

frenzy::dom::HTMLParagraphElement::HTMLParagraphElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLParamElement::cloneNode(bool deep) const
{
  HTMLParamElementp ret = HTMLParamElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLParamElementp
frenzy::dom::HTMLParamElement::create(frenzy::ustring localname)
{
  return HTMLParamElementp(new HTMLParamElement(localname));
}

frenzy::dom::HTMLParamElement::HTMLParamElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLPreElement::cloneNode(bool deep) const
{
  HTMLPreElementp ret = HTMLPreElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLPreElementp
frenzy::dom::HTMLPreElement::create(frenzy::ustring localname)
{
  return HTMLPreElementp(new HTMLPreElement(localname));
}

frenzy::dom::HTMLPreElement::HTMLPreElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLProgressElement::cloneNode(bool deep) const
{
  HTMLProgressElementp ret = HTMLProgressElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLProgressElementp
frenzy::dom::HTMLProgressElement::create(frenzy::ustring localname)
{
  return HTMLProgressElementp(new HTMLProgressElement(localname));
}

frenzy::dom::HTMLProgressElement::HTMLProgressElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLQuoteElement::cloneNode(bool deep) const
{
  HTMLQuoteElementp ret = HTMLQuoteElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLQuoteElementp
frenzy::dom::HTMLQuoteElement::create(frenzy::ustring localname)
{
  return HTMLQuoteElementp(new HTMLQuoteElement(localname));
}

frenzy::dom::HTMLQuoteElement::HTMLQuoteElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLScriptElement::cloneNode(bool deep) const
{
  HTMLScriptElementp ret = HTMLScriptElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLScriptElementp
frenzy::dom::HTMLScriptElement::create(frenzy::ustring localname)
{
  return HTMLScriptElementp(new HTMLScriptElement(localname));
}

frenzy::dom::HTMLScriptElement::HTMLScriptElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLSelectElement::cloneNode(bool deep) const
{
  HTMLSelectElementp ret = HTMLSelectElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLSelectElementp
frenzy::dom::HTMLSelectElement::create(frenzy::ustring localname)
{
  return HTMLSelectElementp(new HTMLSelectElement(localname));
}

frenzy::dom::HTMLSelectElement::HTMLSelectElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLSourceElement::cloneNode(bool deep) const
{
  HTMLSourceElementp ret = HTMLSourceElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLSourceElementp
frenzy::dom::HTMLSourceElement::create(frenzy::ustring localname)
{
  return HTMLSourceElementp(new HTMLSourceElement(localname));
}

frenzy::dom::HTMLSourceElement::HTMLSourceElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLSpanElement::cloneNode(bool deep) const
{
  HTMLSpanElementp ret = HTMLSpanElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLSpanElementp
frenzy::dom::HTMLSpanElement::create(frenzy::ustring localname)
{
  return HTMLSpanElementp(new HTMLSpanElement(localname));
}

frenzy::dom::HTMLSpanElement::HTMLSpanElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLStyleElement::cloneNode(bool deep) const
{
  HTMLStyleElementp ret = HTMLStyleElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLStyleElementp
frenzy::dom::HTMLStyleElement::create(frenzy::ustring localname)
{
  return HTMLStyleElementp(new HTMLStyleElement(localname));
}

frenzy::dom::HTMLStyleElement::HTMLStyleElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTableCaptionElement::cloneNode(bool deep) const
{
  HTMLTableCaptionElementp ret = HTMLTableCaptionElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTableCaptionElementp
frenzy::dom::HTMLTableCaptionElement::create(frenzy::ustring localname)
{
  return HTMLTableCaptionElementp(new HTMLTableCaptionElement(localname));
}

frenzy::dom::HTMLTableCaptionElement::HTMLTableCaptionElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTableColElement::cloneNode(bool deep) const
{
  HTMLTableColElementp ret = HTMLTableColElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTableColElementp
frenzy::dom::HTMLTableColElement::create(frenzy::ustring localname)
{
  return HTMLTableColElementp(new HTMLTableColElement(localname));
}

frenzy::dom::HTMLTableColElement::HTMLTableColElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTableElement::cloneNode(bool deep) const
{
  HTMLTableElementp ret = HTMLTableElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTableElementp
frenzy::dom::HTMLTableElement::create(frenzy::ustring localname)
{
  return HTMLTableElementp(new HTMLTableElement(localname));
}

frenzy::dom::HTMLTableElement::HTMLTableElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTableRowElement::cloneNode(bool deep) const
{
  HTMLTableRowElementp ret = HTMLTableRowElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTableRowElementp
frenzy::dom::HTMLTableRowElement::create(frenzy::ustring localname)
{
  return HTMLTableRowElementp(new HTMLTableRowElement(localname));
}

frenzy::dom::HTMLTableRowElement::HTMLTableRowElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTableSectionElement::cloneNode(bool deep) const
{
  HTMLTableSectionElementp ret = HTMLTableSectionElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTableSectionElementp
frenzy::dom::HTMLTableSectionElement::create(frenzy::ustring localname)
{
  return HTMLTableSectionElementp(new HTMLTableSectionElement(localname));
}

frenzy::dom::HTMLTableSectionElement::HTMLTableSectionElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTextAreaElement::cloneNode(bool deep) const
{
  HTMLTextAreaElementp ret = HTMLTextAreaElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTextAreaElementp
frenzy::dom::HTMLTextAreaElement::create(frenzy::ustring localname)
{
  return HTMLTextAreaElementp(new HTMLTextAreaElement(localname));
}

frenzy::dom::HTMLTextAreaElement::HTMLTextAreaElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTimeElement::cloneNode(bool deep) const
{
  HTMLTimeElementp ret = HTMLTimeElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTimeElementp
frenzy::dom::HTMLTimeElement::create(frenzy::ustring localname)
{
  return HTMLTimeElementp(new HTMLTimeElement(localname));
}

frenzy::dom::HTMLTimeElement::HTMLTimeElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTitleElement::cloneNode(bool deep) const
{
  HTMLTitleElementp ret = HTMLTitleElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTitleElementp
frenzy::dom::HTMLTitleElement::create(frenzy::ustring localname)
{
  return HTMLTitleElementp(new HTMLTitleElement(localname));
}

frenzy::dom::HTMLTitleElement::HTMLTitleElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTrackElement::cloneNode(bool deep) const
{
  HTMLTrackElementp ret = HTMLTrackElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTrackElementp
frenzy::dom::HTMLTrackElement::create(frenzy::ustring localname)
{
  return HTMLTrackElementp(new HTMLTrackElement(localname));
}

frenzy::dom::HTMLTrackElement::HTMLTrackElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLUListElement::cloneNode(bool deep) const
{
  HTMLUListElementp ret = HTMLUListElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLUListElementp
frenzy::dom::HTMLUListElement::create(frenzy::ustring localname)
{
  return HTMLUListElementp(new HTMLUListElement(localname));
}

frenzy::dom::HTMLUListElement::HTMLUListElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::HTMLTableCellElement::~HTMLTableCellElement()
{
}

frenzy::dom::HTMLTableCellElement::HTMLTableCellElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::HTMLMediaElement::~HTMLMediaElement()
{
}

frenzy::dom::HTMLMediaElement::HTMLMediaElement(frenzy::ustring localname)
  : HTMLElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTableDataCellElement::cloneNode(bool deep) const
{
  HTMLTableDataCellElementp ret = HTMLTableDataCellElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTableDataCellElementp
frenzy::dom::HTMLTableDataCellElement::create(frenzy::ustring localname)
{
  return HTMLTableDataCellElementp(new HTMLTableDataCellElement(localname));
}

frenzy::dom::HTMLTableDataCellElement::HTMLTableDataCellElement(frenzy::ustring localname)
  : HTMLTableCellElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLTableHeaderCellElement::cloneNode(bool deep) const
{
  HTMLTableHeaderCellElementp ret = HTMLTableHeaderCellElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLTableHeaderCellElementp
frenzy::dom::HTMLTableHeaderCellElement::create(frenzy::ustring localname)
{
  return HTMLTableHeaderCellElementp(new HTMLTableHeaderCellElement(localname));
}

frenzy::dom::HTMLTableHeaderCellElement::HTMLTableHeaderCellElement(frenzy::ustring localname)
  : HTMLTableCellElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLAudioElement::cloneNode(bool deep) const
{
  HTMLAudioElementp ret = HTMLAudioElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLAudioElementp
frenzy::dom::HTMLAudioElement::create(frenzy::ustring localname)
{
  return HTMLAudioElementp(new HTMLAudioElement(localname));
}

frenzy::dom::HTMLAudioElement::HTMLAudioElement(frenzy::ustring localname)
  : HTMLMediaElement(localname)
{
}

frenzy::dom::Nodep
frenzy::dom::HTMLVideoElement::cloneNode(bool deep) const
{
  HTMLVideoElementp ret = HTMLVideoElement::create(get_localName());
  copyTo(ret, deep);
  return ret;
}

frenzy::dom::HTMLVideoElementp
frenzy::dom::HTMLVideoElement::create(frenzy::ustring localname)
{
  return HTMLVideoElementp(new HTMLVideoElement(localname));
}

frenzy::dom::HTMLVideoElement::HTMLVideoElement(frenzy::ustring localname)
  : HTMLMediaElement(localname)
{
}
