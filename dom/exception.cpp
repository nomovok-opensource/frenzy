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

#include "exception.hpp"

namespace
{
  const char* code_to_str(frenzy::dom::DOMException::exceptionCode c)
  {
    using namespace frenzy::dom;

    switch (c)
    {
    case DOMException::INDEX_SIZE_ERR:
      return "INDEX_SIZE_ERR";
    case DOMException::DOMSTRING_SIZE_ERR:
      return "DOMSTRING_SIZE_ERR";
    case DOMException::HIERARCHY_REQUEST_ERR:
      return "HIERARCHY_REQUEST_ERR";
    case DOMException::WRONG_DOCUMENT_ERR:
      return "WRONG_DOCUMENT_ERR";
    case DOMException::INVALID_CHARACTER_ERR:
      return "INVALID_CHARACTER_ERR";
    case DOMException::NO_DATA_ALLOWED_ERR:
      return "NO_DATA_ALLOWED_ERR";
    case DOMException::NO_MODIFICATION_ALLOWED_ERR:
      return "NO_MODIFICATION_ALLOWED_ERR";
    case DOMException::NOT_FOUND_ERR:
      return "NOT_FOUND_ERR";
    case DOMException::NOT_SUPPORTED_ERR:
      return "NOT_SUPPORTED_ERR";
    case DOMException::INUSE_ATTRIBUTE_ERR:
      return "INUSE_ATTRIBUTE_ERR";
    default:
      return "Unknown DOMException";
    }
  }
}

frenzy::dom::DOMException::DOMException(frenzy::dom::DOMException::exceptionCode c)
  : code(c)
{
}

const char*
frenzy::dom::DOMException::what() const throw()
{
  return code_to_str(code);
}
