#ifndef __LOCIMPL_H
#define __LOCIMPL_H
#pragma option push -b -a8 -pc -Vx- -Ve- -w-inl -w-aus -w-sig
// -*- C++ -*-
/***************************************************************************
 *
 * rw/locimpl - Declarations for the Standard Library locale private
 *              implementation classes.
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
 **************************************************************************/

#ifndef __STD_RW_LOCIMPL__
#define __STD_RW_LOCIMPL__

#include <stdcomp.h>

// Macro for declaring all the has_facet and use_facet functions to be friends.

#ifndef _RWSTD_NO_FRIEND_TEMPLATES
#ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE
#define _RWSTD_FRIEND_USE_HAS_FACET                                     \
  template <class Facet> friend                                         \
      inline const Facet& std::use_facet (const locale&);               \
  template <class Facet> friend                                         \
      inline bool std::has_facet (const locale&) _RWSTD_THROW_SPEC_NULL;
#else
#define _RWSTD_FRIEND_USE_HAS_FACET                                     \
  template <class Facet> friend                                         \
    inline const Facet& use_facet (const locale&,Facet*);               \
  template <class Facet> friend                                         \
    inline bool has_facet (const locale&,Facet*) _RWSTD_THROW_SPEC_NULL;
#endif // _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE
#else
#define _RWSTD_FRIEND_USE_HAS_FACET public:
#endif

#ifndef _RWSTD_NO_NAMESPACE
namespace __rwstd {
  using std::use_facet;
  using std::locale;
#endif

// Implementation class template -- timepunct<charT>
//
// A facet such as this should have been included in the standard.  We just
// declare it here; the definition occurs below, after locale::facet has been
// defined.

template <class charT> class timepunct;

#if defined(__TURBOC__) && 0

// Instantiations that go in the library:

template class _RWSTDExport timepunct<char>;
#ifndef _RWSTD_NO_WIDE_CHAR
template class _RWSTDExport timepunct<wchar_t>;
#endif

#endif // __TURBOC__

// Implementation forward declarations:

class _RWSTDExport locale_imp;
class _RWSTDExport facet_imp;

class _RWSTDExport digit_map_base;
template <class charT> class _RWSTDExportTemplate digit_map;
template <class charT> class _RWSTDExportTemplate keyword_def;
template <class charT> class _RWSTDExportTemplate keyword_map;
template <class charT> class _RWSTDExportTemplate keyword_cracker;

class _RWSTDExport digit_reader_base;
template <class charT> class digit_reader_base_1;
template <class charT,class InputIterator>
  class _RWSTDExportTemplate digit_reader;

class _RWSTDExport digit_writer_base;
template <class charT>
  class _RWSTDExportTemplate digit_writer_base_1;
template <class charT,class OutputIterator>
  class _RWSTDExportTemplate digit_writer;

// ------------------------------------
// Implementation class -- ref_counted.
// ------------------------------------

// Common base class for reference-counted classes.  Currently used only by
// locale_imp and facet_imp, but could be used more generally.

class _RWSTDExport ref_counted {
  friend class _RW_STD::locale;

  size_t __ref_count;
  #ifdef _RWSTD_MULTI_THREAD
  _RWSTDMutex __mutex;
  #endif

 protected:
  ref_counted (size_t initial_refs):
      __ref_count(initial_refs) { }

  // Ensure derived class destructors are always virtual.
  virtual ~ref_counted (void);

  // Does protected access make these accessible to friends of derived classes?
  // We're about to find out ...

  static void add_reference (ref_counted *ob) {
    STDGUARD(ob->__mutex);
    ++ob->__ref_count;
  }

  static void remove_reference (ref_counted *ob) {
    size_t refs;
    {
      STDGUARD(ob->__mutex);
      refs=--ob->__ref_count;
    }
    if (!refs)
      delete ob;
  }

  static void add_possible_reference (ref_counted *ob) {
    if (ob)
      add_reference(ob);
  }

  static void remove_possible_reference (ref_counted *ob) {
    if (ob)
      remove_reference(ob);
  }
};

// We would prefer to define locale::id and locale::facet entirely as nested
// classes of locale, but current compilers have problems with out-of-line
// definition of members of such classes, so we have to derive most of their
// behavior from unnested implementation classes:

// --------------------------------------
// Implementation class -- locale_id_imp.
// --------------------------------------

class _RWSTDExport locale_id_imp {
 protected:
  _MUTABLE size_t __id_value;
  static size_t __last_used_id;
  
  locale_id_imp() :  __id_value(0) {;}

  #ifdef _RWSTD_MULTI_THREAD
  static _RWSTDMutex __mutex;
  #endif

  void __initid () const;
};

// ----------------------------------
// Implementation class -- facet_imp.
// ----------------------------------

class _RWSTDExport facet_imp: public ref_counted
{
  _RWSTD_FRIEND_USE_HAS_FACET
  friend class locale_imp;
  friend class _RW_STD::locale;
public:
  enum {
    __facet_cat = 0,
    __ok_implicit = 0,
    __initdone = 1
  };

 private:
  int __flags;
  int __category;     // Should be same type as locale::category

 protected:
  facet_imp (size_t refs, int cat=0):
      ref_counted(refs), __flags(0), __category(cat) { }

  // __initfacet() is called by locale::__install the first time a facet is 
  // installed in its first locale.  Some facets override it to set up private 
  // data that depends on return values of virtual do_xxx functions that can't
  // be called yet in a constructor.
  virtual void __initfacet (const locale*) { }
};

#ifndef _RWSTD_NO_NAMESPACE
}
#endif

#include <rw/locvector> 

#ifndef _RWSTD_NO_NAMESPACE
namespace __rwstd {
#endif

// -----------------------------------
// Implementation class -- locale_imp.
// -----------------------------------

class _RWSTDExport locale_imp: public ref_counted
{
  _RWSTD_FRIEND_USE_HAS_FACET
  friend class _RW_STD::locale;

  // Same type as locale::category ...
  typedef int locale_category;
  locale_vector<_RW_STD::string> names_;
  locale_vector<facet_imp*> vec_;

  locale_category native_cats_;
  locale_category named_cats_;
  _RW_STD::string big_name_;
  bool named_;

  locale_imp (size_t sz=36, size_t refs=0);
  locale_imp (const locale_imp&,size_t refs);
  ~locale_imp ();

 public:
  inline facet_imp *get_facet (size_t i) const
    { return i<vec_.size()? vec_[i] : NULL; }

 private:
  const char *category_name (locale_category) const;

  // Map C library LC_xxx constants into facet categories.
  static locale_category map_category (locale_category);

  // Parse a locale name into category names.
  static bool parse_name (locale_vector<_RW_STD::string>&,const char*);

  // Clean up name if necessary
  static _RW_STD::string clean_name(_RW_STD::string s)
  {
#ifdef __BORLANDC__
    return _RW_STD::string(s,s.find('=')+1);
#else
    return s;
#endif // __BORLANDC__
  }

  // Combine category names to create a locale name.
  static bool build_name (_RW_STD::string&,const locale_vector<_RW_STD::string>&);
};

// ---------------------------------------
// Implementation class -- digit_map_base.
// ---------------------------------------

// A place to stash some static constants, so that each instantiation of the
// derived class rwstd::digit_map does not have to have a separate copy.

struct _RWSTDExport digit_map_base {
  enum { zero, minus, plus, X, x, E, e };
  static const char punct_chars[7];   // "0-+XxEe"
  static const char digit_chars[22];  // "0123456789ABCDEFabcdef"
  static const char char_values[22];  // Corresponding values in range 0-15
};

// --------------------------------------------------
// Implementation class template -- digit_map<charT>.
// --------------------------------------------------

// Maps digits into their corresponding numeric values, and caches widened
// equivalents of some number-related punctuation characters that don't depend
// on the numpunct facet.  A private instance of this class is hidden in
// ctype<charT> for use by numeric conversion facets.  A call to init must
// precede the first call to eval if is_inited() is false.  Eval returns 0-15
// if argument is a valid digit, a negative value otherwise.
//
// Specialized for char for performance.  The specialization assumes digits
// fit into the char code-set in an ASCII-like manner ('0'..'9' contiguous,
// 'A'..'F' contiguous, 'a'..'f' contiguous, '0' < 'A' < 'a').

_RWSTD_TEMPLATE
class _RWSTDExportTemplate digit_map<char>:
    public digit_map_base
{
 public:
  typedef char char_type;

  bool is_inited (void) const { return true; }
  const char *get_punct (void) const { return punct_chars; }
  inline int eval (char c) const;
  static inline const digit_map<char>&
      get_digit_map (const _RW_STD::ctype<char>&);
};

// Inline members of digit_map<char>:
//
// (Note that the definition of get_digit_map is in <rw/ctype> because it
//  has to come after the definition of ctype<char>.)

inline int digit_map<char>::eval (char c) const {
  int num=c;
  if ((num-='0')>9) {
    if (((num-=('A'-'0'))>5 && (num-=('a'-'A'))>5) || (num+=10)<10)
      num=-1;
  }
  return num;
}

template <class charT>
class _RWSTDExportTemplate digit_map:
    public digit_map_base
{
  bool inited;
  charT punct_array[7];
  charT digit_array[22];
  char value_array[22];
 public:
  typedef charT char_type;

  digit_map (void): inited(false) { }
  bool is_inited (void) const { return inited; }
  void init (const _RW_STD::ctype<charT>& ct);
  const charT *get_punct (void) const { return punct_array; }
  int eval (charT) const;

  // Can-opener for getting the digit_map out of a ctype.  (Works because of
  // the friend declaration in ctype_helper<charT> below.)
  static inline const digit_map<charT>&
      get_digit_map (const _RW_STD::ctype<charT>& ct)
  {
    if (!ct.__digit_map.inited)
      (_RWSTD_CONST_CAST(digit_map<char_type>&,ct.__digit_map))
          .init(ct);
    return ct.__digit_map;
  }
};

// ----------------------------------------------------
// Implementation class template -- keyword_def<charT>.
// ----------------------------------------------------

// Helper class used in parsing keywords from input (such as true/false in
// num_get, month and day names in time_get, etc).

template <class charT>
class _RWSTDExportTemplate keyword_def {
 public:
  const charT *s;
  int v;
};

// ----------------------------------------------------
// Implementation class template -- keyword_map<charT>.
// ----------------------------------------------------

// Defines a set of keywords to be recognized on input and to be written to
// output.  Private instances are hidden in numpunct (for true/false) and
// rwstd::timepunct (for month and weekday names).

template <class charT>
class _RWSTDExportTemplate keyword_map {
 public:
  int num_defs_;
  const keyword_def<charT> *defs_;
};

// ---------------------------------------------------
// Implementation class template -- punct_data<charT>.
// ---------------------------------------------------

// Common base class for rwstd::numpunct_data and rwstd::moneypunct_data.

template <class charT>
class _RWSTDExportTemplate punct_data {
  friend class digit_reader_base_1<charT>;
  friend class digit_writer_base_1<charT>;
 public:
  typedef _RW_STD::basic_string<charT,_RW_STD::char_traits<charT>,_RW_STD::allocator<charT> > string_type;
 protected:
  charT dp_, ts_;
  _RW_STD::string gr_;
};

// ------------------------------------------------------
// Implementation function templates -- create_xxx_facet.
// ------------------------------------------------------

// The facet_maker<Facet>::maker_func functions described above delegate the
// actual construction of facets to three inline function templates named
// create_xxx_facet, where xxx is 'classic' or 'native' or 'named'.  The
// default (template) versions of these functions construct facets as follows:
//
//   classic -- default constructor for the facet with only the refs argument.
//   native -- calls create_named_facet with a name of "".
//   named -- calls create_classic_facet, ignoring the passed name.
//
// This default behavior is overridden (specialized) for certain facet types.
// In particular, create_named_facet is specialized for all facet types that
// have a derived _byname version, to construct that version with the passed
// name (see <rw/rwlocale>) and create_native_facet is specialized for all
// facet types whose "native" behavior (as determined by the vendor) differs
// from the byname facet with a name of "" (see <rw/vendor>).

template <class Facet>
inline Facet*  _RWSTDExportTemplate create_named_facet
    (Facet*,const char*,size_t refs);

template <class Facet>
inline Facet*  _RWSTDExportTemplate create_native_facet (Facet*);

template <class Facet>
inline Facet* _RWSTDExportTemplate create_classic_facet (Facet*)
{
 return new Facet(1);
}

// ----------------------------------------------------
// Implementation class template -- facet_maker<Facet>.
// ----------------------------------------------------

// When use_facet (inline) finds that a locale does not contain an explicit
// facet of the requested type, it calls locale::__make_explicit (non-template)
// to create or find the facet in a cache, and install it in the locale.  As a
// parameter to __make_explicit, use_facet passes a call-back function which
// __make_explicit can call to construct a facet of the requested type if
// needed.  The call-back functions are obtained by instantiating the following
// helper class template:

template <class Facet>
class _RWSTDExportTemplate facet_maker {
 public:
  static facet_imp *maker_func (int t, const char* name, size_t refs)
  {
    if (t==0)
      return create_classic_facet ((Facet*)0);
    else if (t==1)
      return create_native_facet ((Facet*)0);
    else
      return create_named_facet ((Facet*)0,name,refs);
  }
};

// Typedef for the above facet_maker functions, for use in the declaration
// of locale::__make_explicit.

typedef facet_imp *facet_maker_func (int,const char*,size_t);

#ifndef _RWSTD_NO_NAMESPACE
} // namespace __rwstd
#endif

#ifdef _RWSTD_COMPILE_INSTANTIATE
#include <rw/locimpl.cc>
#endif

#endif // __STD_RW_LOCIMPL__
#pragma option pop
#endif /* __LOCIMPL_H */
