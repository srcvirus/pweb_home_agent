/*
 * typedefs.h
 *
 *  Created on: 2014-01-22
 *      Author: sr2chowd
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

enum OpCode {
  O_QUERY,
  O_IQUERY,
  O_STATUS,
  O_RESERVED
};

enum ReturnCode {
  R_SUCCESS,
  R_FORMAT_ERROR,
  R_SERVER_FAILURE,
  R_NAME_ERROR,
  R_NOT_IMPLEMENTED,
  R_REFUSED,
  R_RESERVED
};

enum QueryType {
  QTYPE_MIN = 0,
  T_MIN = 1,
  T_A = 1,
  T_NS = 2,
  T_MD = 3,
  T_MF = 4,
  T_CNAME = 5,
  T_SOA = 6,
  T_MB = 7,
  T_MG = 8,
  T_MR = 9,
  T_NULL = 10,
  T_WKS = 11,
  T_PTR = 12,
  T_HINFO = 13,
  T_MINFO = 14,
  T_MX = 15,
  T_TXT = 16,

  T_AAAA = 28,

  T_MAX = 28,
  QT_MIN = 252,

  QT_AXFR = 252,
  QT_MAILB = 253,
  QT_MAILA = 254,
  QT_ALL = 255,

  QT_MAX = 255,
  QTYPE_MAX = 65535
};

enum QueryClass {
  QCLASS_MIN = 0,
  C_MIN = 1,

  C_IN = 1,
  C_CS = 2,
  C_CH = 3,
  C_HS = 4,

  C_MAX = 4,
  QC_MIN = 255,

  QC_ANY = 255,

  QC_MAX = 255,
  QCLASS_MAX = 65535
};

#endif /* TYPEDEFS_H_ */
