/*
 * http_mime_types.cpp
 *
 *  Created on: 2014-02-10
 *      Author: sr2chowd
 */

// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "http_mime_types.h"

namespace mime_types {

struct mapping {
  const char *extension;
  const char *mime_type;
} mappings[] = { { "gif", "image/gif" }, { "htm", "text/html" },
                 { "html", "text/html" }, { "jpg", "image/jpeg" },
                 { "png", "image/png" }, { "json", "application/json" },
                 { 0, 0 } // Marks end of list.
};

std::string extension_to_type(const std::string &extension) {
  for (mapping *m = mappings; m->extension; ++m) {
    if (m->extension == extension) {
      return m->mime_type;
    }
  }

  return "text/plain";
}

} // namespace mime_types
