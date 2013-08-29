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

#ifndef FRENZY_TEXT_HPP
#define FRENZY_TEXT_HPP

#include "util/unicode.hpp"
#include "util/vector.hpp"
#include "node.hpp"
#include "pointers.hpp"

namespace frenzy
{
  namespace dom
  {
    struct CharacterData : Node
    {
      virtual ~CharacterData() = 0;

      virtual boost::optional<ustring> get_nodeValue() const;
      virtual void set_nodeValue(ustring str);

      ustring get_data() const;
      void set_data(ustring data);
      size_t get_length() const;
      ustring substringData(int offset, int count);
      void appendData(ustring data);
      void insertData(int offset, ustring data);
      void deleteData(int offset, int count);
      void replaceData(int offset, int count, ustring data);

    protected:
      CharacterData(ustring data);

      // No copyTo() implementation needed. Derived classes already
      // construct the data when created.

    private:
      ustring datastr;

      // Verifies offset and count values, throws INDEX_SIZE_ERR when
      // offset or count is negative, or if offset is larger than data
      // size. Note! offset + count is allowed to be larger than data
      // size.
      void verify_values(int offset, int count);
    };

    struct Text : CharacterData
    {
      virtual Node::nodeType get_nodeType() const;
      virtual ustring get_nodeName() const;
      virtual Nodep cloneNode(bool deep = true) const;

      Textp splitText(int offset);

      static Textp create(ustring data);

      //
      // Graphics
      //

      virtual void drop_graphics();
      virtual vec2 layout(vec maxwidth);

    private:
      Text(ustring data);

      void ensure_graphics_object();

      boost::shared_ptr<graphics::text> drawntext;
    };

    struct Comment : CharacterData
    {
      virtual Node::nodeType get_nodeType() const;
      virtual ustring get_nodeName() const;
      virtual Nodep cloneNode(bool deep = true) const;

      static Commentp create(ustring data);
    private:

      Comment(ustring data);
    };

    struct CDATASection : Text
    {
      virtual Node::nodeType get_nodeType() const;
      virtual ustring get_nodeName() const;
      virtual Nodep cloneNode(bool deep = true) const;

      static CDATASectionp create(ustring data);
    private:
      CDATASection(ustring data);
    };
  }
}

#endif
