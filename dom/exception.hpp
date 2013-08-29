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

#ifndef FRENZY_EXCEPTION_HPP
#define FRENZY_EXCEPTION_HPP

#include <exception>

namespace frenzy
{
  namespace dom
  {
    struct DOMException : std::exception
    {
      enum exceptionCode
      {
	INDEX_SIZE_ERR     = 1,
	DOMSTRING_SIZE_ERR = 2,
	HIERARCHY_REQUEST_ERR = 3,
	WRONG_DOCUMENT_ERR = 4,
	INVALID_CHARACTER_ERR = 5,
	NO_DATA_ALLOWED_ERR = 6,
	NO_MODIFICATION_ALLOWED_ERR = 7,
	NOT_FOUND_ERR      = 8,
	NOT_SUPPORTED_ERR  = 9,
	INUSE_ATTRIBUTE_ERR = 10
      };

      DOMException(exceptionCode c);

      const exceptionCode code;
      
      const char* what() const throw();
    };
  }
}

#endif
