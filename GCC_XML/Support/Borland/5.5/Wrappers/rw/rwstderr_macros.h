#ifndef __STD_RWSTDERROR_H
#define __STD_RWSTDERROR_H
/***************************************************************************
 *
 * Macro definitions for error message indicators.
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-1999 Rogue Wave Software, Inc.  All Rights Reserved.
 *
 * This computer software is owned by Rogue Wave Software, Inc. and is
 * protected by U.S. copyright laws and other laws and by international
 * treaties.  This computer software is furnished by Rogue Wave Software,
 * Inc. pursuant to a written license agreement and may be used, copied,
 * transmitted, and stored only in accordance with the terms of such
 * license and with the inclusion of the above copyright notice.  This
 * computer software or any other copies thereof may not be provided or
 * otherwise made available to any other person.
 *
 * U.S. Government Restricted Rights.  This computer software is provided
 * with Restricted Rights.  Use, duplication, or disclosure by the
 * Government is subject to restrictions as set forth in subparagraph (c)
 * (1) (ii) of The Rights in Technical Data and Computer Software clause
 * at DFARS 252.227-7013 or subparagraphs (c) (1) and (2) of the
 * Commercial Computer Software – Restricted Rights at 48 CFR 52.227-19,
 * as applicable.  Manufacturer is Rogue Wave Software, Inc., 5500
 * Flatiron Parkway, Boulder, Colorado 80301 USA.
 *
 ***************************************************************************/
#ifndef  _RWSTD_ERROR_MSG_FILE
 #define _RWSTD_ERROR_MSG_FILE "rwstderr"
#endif

#ifndef  _RWSTD_ERROR_SET
 #define _RWSTD_ERROR_SET 1
#endif

#ifndef  _RWSTD_ERROR_FIRST
 #define _RWSTD_ERROR_FIRST 1
#endif

#define _RWSTD_ERROR_FAILBIT_SET                  _RWSTD_ERROR_FIRST + 1
#define _RWSTD_ERROR_BADBIT_SET                   _RWSTD_ERROR_FIRST + 2
#define _RWSTD_ERROR_EOFBIT_SET                   _RWSTD_ERROR_FIRST + 3
#define _RWSTD_ERROR_LOCALE_BAD_NUMERIC_NAME      _RWSTD_ERROR_FIRST + 4
#define _RWSTD_ERROR_LOCALE_NOT_PRESENT           _RWSTD_ERROR_FIRST + 5
#define _RWSTD_ERROR_LOCALE_INIT_CALLED_TWICE     _RWSTD_ERROR_FIRST + 6
#define _RWSTD_ERROR_LOCALE_NULL_POINTER          _RWSTD_ERROR_FIRST + 7
#define _RWSTD_ERROR_LOCALE_BAD_NAME              _RWSTD_ERROR_FIRST + 8
#define _RWSTD_ERROR_LOCALE_ERROR_NAME            _RWSTD_ERROR_FIRST + 9
#define _RWSTD_ERROR_LOCALE_CODE_CONV_FAILED      _RWSTD_ERROR_FIRST + 10
#define _RWSTD_ERROR_LOCALE_BAD_MONEY_NAME        _RWSTD_ERROR_FIRST + 11
#define _RWSTD_ERROR_BITSET_INVALID_POSITION      _RWSTD_ERROR_FIRST + 12
#define _RWSTD_ERROR_BITSET_INVALID_CTOR_ARGUMENT _RWSTD_ERROR_FIRST + 13
#define _RWSTD_ERROR_BITSET_CONVERSION_OVERFLOW   _RWSTD_ERROR_FIRST + 14
#define _RWSTD_ERROR_OUT_OF_RANGE                 _RWSTD_ERROR_FIRST + 15
#define _RWSTD_ERROR_INVALID_SIZE_PARAM           _RWSTD_ERROR_FIRST + 16
#define _RWSTD_ERROR_POS_BEYOND_END_OF_STRING     _RWSTD_ERROR_FIRST + 17
#define _RWSTD_ERROR_RESULT_LEN_INVALID           _RWSTD_ERROR_FIRST + 18
#define _RWSTD_ERROR_STRING_INDEX_OUT_OF_RANGE    _RWSTD_ERROR_FIRST + 19
#define _RWSTD_ERROR_UNEXPECTED_NULL_PTR          _RWSTD_ERROR_FIRST + 20

         
#endif// __STD_RWSTDERR_H
