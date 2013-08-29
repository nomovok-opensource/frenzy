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

#ifndef FRENZY_HTMLELEMENT_HPP
#define FRENZY_HTMLELEMENT_HPP

#include <boost/optional.hpp>

#include "util/unicode.hpp"
#include "util/vector.hpp"
#include "element.hpp"
#include "pointers.hpp"

// TODO: Should these be split to separate files somehow?

namespace frenzy
{
  namespace dom
  {
    // HTMLElement is a base type for all HTML element interfaces, and
    // is also used directly as a concrete type for some elements.
    struct HTMLElement : Element
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLElementp create(ustring localname);

      //
      // Graphics
      //

      //      virtual void drop_graphics();

    protected:
      HTMLElement(ustring localname);
    };

    // Used when no matching type exists
    struct HTMLUnknownElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLUnknownElementp create(ustring localname);

    private:
      HTMLUnknownElement(ustring localname);
    };

    //
    // Normal cases
    //

    struct HTMLAnchorElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLAnchorElementp create(ustring localname);

    protected:
      HTMLAnchorElement(ustring localname);
    };

    struct HTMLAreaElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLAreaElementp create(ustring localname);

    protected:
      HTMLAreaElement(ustring localname);
    };

    struct HTMLBaseElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLBaseElementp create(ustring localname);

    protected:
      HTMLBaseElement(ustring localname);
    };

    struct HTMLBodyElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLBodyElementp create(ustring localname);

    protected:
      HTMLBodyElement(ustring localname);
    };

    struct HTMLBRElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLBRElementp create(ustring localname);

    protected:
      HTMLBRElement(ustring localname);
    };

    struct HTMLButtonElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLButtonElementp create(ustring localname);

    protected:
      HTMLButtonElement(ustring localname);
    };

    struct HTMLCanvasElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLCanvasElementp create(ustring localname);

    protected:
      HTMLCanvasElement(ustring localname);
    };

    struct HTMLCommandElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLCommandElementp create(ustring localname);

    protected:
      HTMLCommandElement(ustring localname);
    };

    struct HTMLDataElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLDataElementp create(ustring localname);

    protected:
      HTMLDataElement(ustring localname);
    };

    struct HTMLDataListElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLDataListElementp create(ustring localname);

    protected:
      HTMLDataListElement(ustring localname);
    };

    struct HTMLDetailsElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLDetailsElementp create(ustring localname);

    protected:
      HTMLDetailsElement(ustring localname);
    };

    struct HTMLDialogElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLDialogElementp create(ustring localname);

    protected:
      HTMLDialogElement(ustring localname);
    };

    struct HTMLDivElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLDivElementp create(ustring localname);

    protected:
      HTMLDivElement(ustring localname);
    };

    struct HTMLDListElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLDListElementp create(ustring localname);

    protected:
      HTMLDListElement(ustring localname);
    };

    struct HTMLEmbedElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLEmbedElementp create(ustring localname);

    protected:
      HTMLEmbedElement(ustring localname);
    };

    struct HTMLFieldSetElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLFieldSetElementp create(ustring localname);

    protected:
      HTMLFieldSetElement(ustring localname);
    };

    struct HTMLFormElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLFormElementp create(ustring localname);

    protected:
      HTMLFormElement(ustring localname);
    };

    struct HTMLHeadElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLHeadElementp create(ustring localname);

    protected:
      HTMLHeadElement(ustring localname);
    };

    struct HTMLHeadingElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLHeadingElementp create(ustring localname);

    protected:
      HTMLHeadingElement(ustring localname);
    };

    struct HTMLHRElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLHRElementp create(ustring localname);

    protected:
      HTMLHRElement(ustring localname);
    };

    struct HTMLHtmlElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLHtmlElementp create(ustring localname);

    protected:
      HTMLHtmlElement(ustring localname);
    };

    struct HTMLIFrameElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLIFrameElementp create(ustring localname);

    protected:
      HTMLIFrameElement(ustring localname);
    };

    struct HTMLImageElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLImageElementp create(ustring localname);

    protected:
      HTMLImageElement(ustring localname);
    };

    struct HTMLInputElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLInputElementp create(ustring localname);

    protected:
      HTMLInputElement(ustring localname);
    };

    struct HTMLKeygenElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLKeygenElementp create(ustring localname);

    protected:
      HTMLKeygenElement(ustring localname);
    };

    struct HTMLLabelElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLLabelElementp create(ustring localname);

    protected:
      HTMLLabelElement(ustring localname);
    };

    struct HTMLLegendElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLLegendElementp create(ustring localname);

    protected:
      HTMLLegendElement(ustring localname);
    };

    struct HTMLLIElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLLIElementp create(ustring localname);

    protected:
      HTMLLIElement(ustring localname);
    };

    struct HTMLLinkElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLLinkElementp create(ustring localname);

    protected:
      HTMLLinkElement(ustring localname);
    };

    struct HTMLMapElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLMapElementp create(ustring localname);

    protected:
      HTMLMapElement(ustring localname);
    };

    struct HTMLMenuElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLMenuElementp create(ustring localname);

    protected:
      HTMLMenuElement(ustring localname);
    };

    struct HTMLMetaElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLMetaElementp create(ustring localname);

    protected:
      HTMLMetaElement(ustring localname);
    };

    struct HTMLMeterElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLMeterElementp create(ustring localname);

    protected:
      HTMLMeterElement(ustring localname);
    };

    struct HTMLModElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLModElementp create(ustring localname);

    protected:
      HTMLModElement(ustring localname);
    };

    struct HTMLObjectElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLObjectElementp create(ustring localname);

    protected:
      HTMLObjectElement(ustring localname);
    };

    struct HTMLOListElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLOListElementp create(ustring localname);

    protected:
      HTMLOListElement(ustring localname);
    };

    struct HTMLOptGroupElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLOptGroupElementp create(ustring localname);

    protected:
      HTMLOptGroupElement(ustring localname);
    };

    struct HTMLOptionElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLOptionElementp create(ustring localname);

    protected:
      HTMLOptionElement(ustring localname);
    };

    struct HTMLOutputElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLOutputElementp create(ustring localname);

    protected:
      HTMLOutputElement(ustring localname);
    };

    struct HTMLParagraphElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLParagraphElementp create(ustring localname);

    protected:
      HTMLParagraphElement(ustring localname);
    };

    struct HTMLParamElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLParamElementp create(ustring localname);

    protected:
      HTMLParamElement(ustring localname);
    };

    struct HTMLPreElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLPreElementp create(ustring localname);

    protected:
      HTMLPreElement(ustring localname);
    };

    struct HTMLProgressElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLProgressElementp create(ustring localname);

    protected:
      HTMLProgressElement(ustring localname);
    };

    struct HTMLQuoteElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLQuoteElementp create(ustring localname);

    protected:
      HTMLQuoteElement(ustring localname);
    };

    struct HTMLScriptElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLScriptElementp create(ustring localname);

      //
      // Graphics
      //

      // This element is not supposed to draw anything.
      virtual vec2 layout(vec)
      {
	return vec2(0, 0);
      }

    protected:
      HTMLScriptElement(ustring localname);
    };

    struct HTMLSelectElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLSelectElementp create(ustring localname);

    protected:
      HTMLSelectElement(ustring localname);
    };

    struct HTMLSourceElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLSourceElementp create(ustring localname);

    protected:
      HTMLSourceElement(ustring localname);
    };

    struct HTMLSpanElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLSpanElementp create(ustring localname);

    protected:
      HTMLSpanElement(ustring localname);
    };

    struct HTMLStyleElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLStyleElementp create(ustring localname);

      //
      // Graphics
      //

      // This element is not supposed to draw anything.
      virtual vec2 layout(vec)
      {
	return vec2(0, 0);
      }

    protected:
      HTMLStyleElement(ustring localname);
    };

    struct HTMLTableCaptionElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTableCaptionElementp create(ustring localname);

    protected:
      HTMLTableCaptionElement(ustring localname);
    };

    struct HTMLTableColElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTableColElementp create(ustring localname);

    protected:
      HTMLTableColElement(ustring localname);
    };

    struct HTMLTableElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTableElementp create(ustring localname);

    protected:
      HTMLTableElement(ustring localname);
    };

    struct HTMLTableRowElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTableRowElementp create(ustring localname);

    protected:
      HTMLTableRowElement(ustring localname);
    };

    struct HTMLTableSectionElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTableSectionElementp create(ustring localname);

    protected:
      HTMLTableSectionElement(ustring localname);
    };

    struct HTMLTextAreaElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTextAreaElementp create(ustring localname);

    protected:
      HTMLTextAreaElement(ustring localname);
    };

    struct HTMLTimeElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTimeElementp create(ustring localname);

    protected:
      HTMLTimeElement(ustring localname);
    };

    struct HTMLTitleElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTitleElementp create(ustring localname);

    protected:
      HTMLTitleElement(ustring localname);
    };

    struct HTMLTrackElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTrackElementp create(ustring localname);

    protected:
      HTMLTrackElement(ustring localname);
    };

    struct HTMLUListElement : HTMLElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLUListElementp create(ustring localname);

    protected:
      HTMLUListElement(ustring localname);
    };

    //
    // Special cases
    //

    struct HTMLTableCellElement : HTMLElement
    {
      virtual ~HTMLTableCellElement() = 0;

    protected:
      HTMLTableCellElement(ustring localname);
    };

    struct HTMLMediaElement : HTMLElement
    {
      virtual ~HTMLMediaElement() = 0;

    protected:
      HTMLMediaElement(ustring localname);
    };

    struct HTMLTableDataCellElement : HTMLTableCellElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTableDataCellElementp create(ustring localname);

    private:
      HTMLTableDataCellElement(ustring localname);
    };

    struct HTMLTableHeaderCellElement : HTMLTableCellElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLTableHeaderCellElementp create(ustring localname);

    private:
      HTMLTableHeaderCellElement(ustring localname);
    };

    struct HTMLAudioElement : HTMLMediaElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLAudioElementp create(ustring localname);

    private:
      HTMLAudioElement(ustring localname);
    };

    struct HTMLVideoElement : HTMLMediaElement
    {
      virtual Nodep cloneNode(bool deep = true) const;

      static HTMLVideoElementp create(ustring localname);

    private:
      HTMLVideoElement(ustring localname);
    };
  }
}

#endif
