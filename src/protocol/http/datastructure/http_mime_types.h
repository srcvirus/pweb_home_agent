/*
 * http_mime_types.h
 *
 *  Created on: 2014-02-10
 *      Author: sr2chowd
 */

#ifndef HTTP_MIME_TYPES_H_
#define HTTP_MIME_TYPES_H_

// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <string>

using namespace std;

namespace mime_types {
/// Convert a file extension into a MIME type.
std::string extension_to_type(const std::string &extension);

} // namespace mime_types

#endif /* HTTP_MIME_TYPES_H_ */
