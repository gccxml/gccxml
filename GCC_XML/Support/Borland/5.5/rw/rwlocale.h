#ifndef __RWLOCALE_H
#define __RWLOCALE_H
#pragma option push -b -a8 -pc -Vx- -Ve- -w-inl -w-aus -w-sig
// -*- C++ -*-
/***************************************************************************
 *
 * locale - Declarations for the Standard Library locale classes
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

#ifndef __STD_RWLOCALE__
#define __STD_RWLOCALE__ 1

#include <rw/rwstderr.h>

#ifndef __STD_RWCOMPILER_H__
#include <stdcomp.h>
#endif

#ifndef __STD_STRING__
#include <string>
#endif

#ifndef __STD_IOSFWD__
#include <iosfwd>
#endif

#ifndef _RWSTD_NO_NEW_HEADER
#include <ctime>
#else
#include <time.h>
#endif

#ifndef _RWSTD_NO_BAD_CAST
#include <typeinfo>
#endif

// In case these are (wrongly!) defined as macros in <cctype>.

#undef isspace
#undef isprint
#undef iscntrl
#undef isupper
#undef islower
#undef isalpha
#undef isdigit
#undef ispunct
#undef isxdigit
#undef isalnum
#undef isgraph
#undef toupper
#undef tolower

#ifndef _RWSTD_NO_NAMESPACE
namespace __rwstd {
#endif

#ifdef _RWSTD_LOCALIZED_ERRORS
  extern const unsigned int _RWSTDExport __rw_LocaleNotPresent;
#else
  extern const char _RWSTDExportFunc(*) __rw_LocaleNotPresent;
#endif

#ifndef _RWSTD_NO_NAMESPACE
} namespace std {
#endif

// Forward declarations of functions and classes specified by the locale clause
// of the C++ Standard Library working paper.

class _RWSTDExport locale;

#ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE
  template <class Facet> inline const Facet& use_facet (const locale&);
  template <class Facet>
    inline bool has_facet (const locale&) _RWSTD_THROW_SPEC_NULL;
#else
  template <class Facet>
    _RWSTD_TRICKY_INLINE const Facet& use_facet (const locale&, Facet*);
  template <class Facet>
    inline bool has_facet (const locale&, Facet*) _RWSTD_THROW_SPEC_NULL;
#endif // _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE

template <class charT> inline bool isspace  (charT c, const locale&);
template <class charT> inline bool isprint  (charT c, const locale&);
template <class charT> inline bool iscntrl  (charT c, const locale&);
template <class charT> inline bool isupper  (charT c, const locale&);
template <class charT> inline bool islower  (charT c, const locale&);
template <class charT> inline bool isalpha  (charT c, const locale&);
template <class charT> inline bool isdigit  (charT c, const locale&);
template <class charT> inline bool ispunct  (charT c, const locale&);
template <class charT> inline bool isxdigit (charT c, const locale&);
template <class charT> inline bool isalnum  (charT c, const locale&);
template <class charT> inline bool isgraph  (charT c, const locale&);

template <class charT> inline charT toupper (charT c, const locale&);
template <class charT> inline charT tolower (charT c, const locale&);

class ctype_base;
template <class charT> class ctype;
_RWSTD_TEMPLATE class _RWSTDExport ctype<char>;
template <class charT> class ctype_byname;
_RWSTD_TEMPLATE class _RWSTDExport ctype_byname<char>;
class codecvt_base;
template  <class internT, class externT, class stateT> class codecvt;
_RWSTD_TEMPLATE class _RWSTDExport codecvt<char,char,mbstate_t>;     // (imp)

#ifndef _RWSTD_NO_WIDE_CHAR
_RWSTD_TEMPLATE class _RWSTDExport ctype<wchar_t>;               // (imp)
_RWSTD_TEMPLATE class _RWSTDExport ctype_byname<wchar_t>;
_RWSTD_TEMPLATE class _RWSTDExport codecvt<wchar_t,char,mbstate_t>;  // (imp)
#endif // _RWSTD_NO_WIDE_CHAR

template <class internT, class externT, class stateT> class codecvt_byname;

template <class charT,
    class InputIterator _RWSTD_COMPLEX_DEFAULT(istreambuf_iterator<charT>)>
    class num_get;
template <class charT,
    class OutputIterator _RWSTD_COMPLEX_DEFAULT(ostreambuf_iterator<charT>)>
    class num_put;
template <class charT> class numpunct;
template <class charT> class numpunct_byname;

template <class charT> class collate;
template <class charT> class collate_byname;

class time_base;
template <class charT,
    class InputIterator _RWSTD_COMPLEX_DEFAULT(istreambuf_iterator<charT>)>
    class time_get;
template <class charT,
    class InputIterator _RWSTD_COMPLEX_DEFAULT(istreambuf_iterator<charT>)>
    class time_get_byname;
template <class charT,
    class OutputIterator _RWSTD_COMPLEX_DEFAULT(ostreambuf_iterator<charT>)>
    class time_put;
template <class charT,
    class OutputIterator _RWSTD_COMPLEX_DEFAULT(ostreambuf_iterator<charT>)>
    class time_put_byname;

class _RWSTDExport money_base;
template <class charT,
    class InputIterator _RWSTD_COMPLEX_DEFAULT(istreambuf_iterator<charT>)>
    class money_get;
template <class charT,
    class OutputIterator _RWSTD_COMPLEX_DEFAULT(ostreambuf_iterator<charT>)>
    class money_put;
template <class charT, bool Intl _RWSTD_SIMPLE_DEFAULT(false)>
    class moneypunct;
template <class charT, bool Intl _RWSTD_SIMPLE_DEFAULT(false)>
    class moneypunct_byname;

class messages_base;
template <class charT> class messages;
template <class charT> class messages_byname;

// Instantiations that go in the library:

#if defined(__TURBOC__) && 0

typedef istreambuf_iterator<char,char_traits<char> > __isit;
typedef ostreambuf_iterator<char,char_traits<char> > __osit;

template class _RWSTDExport codecvt_byname<char,char,mbstate_t>;
template class _RWSTDExport num_get<char,__isit>;
template class _RWSTDExport num_put<char,__osit>;
template class _RWSTDExport numpunct<char>;
template class _RWSTDExport numpunct_byname<char>;
template class _RWSTDExport collate<char>;
template class _RWSTDExport collate_byname<char>;
template class _RWSTDExport time_get<char,__isit>;
template class _RWSTDExport time_put<char,__osit>;
template class _RWSTDExport money_get<char,__isit>;
template class _RWSTDExport money_put<char,__osit>;
template class _RWSTDExport moneypunct<char,false>;
template class _RWSTDExport moneypunct<char,true>;
template class _RWSTDExport moneypunct_byname<char,false>;
template class _RWSTDExport moneypunct_byname<char,true>;
template class _RWSTDExport messages<char>;
template class _RWSTDExport messages_byname<char>;

#ifndef _RWSTD_NO_WIDE_CHAR

typedef istreambuf_iterator<wchar_t,char_traits<wchar_t> > __wisit;
typedef ostreambuf_iterator<wchar_t,char_traits<wchar_t> > __wosit;

template class _RWSTDExport codecvt_byname<wchar_t,wchar_t,mbstate_t>;
template class _RWSTDExport num_get<wchar_t,__wisit>;
template class _RWSTDExport num_put<wchar_t,__wosit>;
template class _RWSTDExport numpunct<wchar_t>;
template class _RWSTDExport numpunct_byname<wchar_t>;
template class _RWSTDExport collate<wchar_t>;
template class _RWSTDExport collate_byname<wchar_t>;
template class _RWSTDExport time_get<wchar_t,__wisit>;
template class _RWSTDExport time_put<wchar_t,__wosit>;
template class _RWSTDExport money_get<wchar_t,__wisit>;
template class _RWSTDExport money_put<wchar_t,__wosit>;
template class _RWSTDExport moneypunct<wchar_t,false>;
template class _RWSTDExport moneypunct<wchar_t,true>;
template class _RWSTDExport moneypunct_byname<wchar_t,false>;
template class _RWSTDExport moneypunct_byname<wchar_t,true>;
template class _RWSTDExport messages<wchar_t>;
template class _RWSTDExport messages_byname<wchar_t>;

#endif // _RWSTD_NO_WIDE_CHAR

#endif // __TURBOC__

#ifndef _RWSTD_NO_NAMESPACE
} // namespace std
#endif

#ifndef __STD_RW_LOCIMPL__
#include <rw/locimpl>
#endif

#ifndef _RWSTD_NO_NAMESPACE
namespace std {
#endif

// -------------
// Class locale.
// -------------

class _RWSTDExport locale {
 public:
  class facet;
  class id;

  friend class facet;
  friend class id;

  typedef int category;

  // The following constants identify standard categories of facets.  The
  // standard specifies them as const members of type category (i.e. as const
  // ints).  For compilers that can't initialize const members here, we define
  // them as members of an anonymous enum instead.

  // Some compilers get confused by the name collision of these constants with
  // the facet templates collate, ctype and messages.  As a work-around, we
  // also define non-standard names of the form __rw_xxx_category for these
  // three categories.  If you have one of the confused compilers (i.e. if
  // _RWSTD_CONFUSED_ENUM_SCOPE is #defined for you), you must use these non-
  // standard category names.

  #ifndef _RWSTD_CONFUSED_ENUM_SCOPE
  #define _RWSTD_CATEGORY_NAMES                             \
    collate = 0x0010, ctype = 0x0020, monetary = 0x0040,    \
    numeric = 0x0080, time  = 0x0100, messages = 0x0200,    \
    all     = 0x03f0, none  = 0x0000
  #else
  #define _RWSTD_CATEGORY_NAMES                             \
    monetary = 0x0040, numeric = 0x0080, time  = 0x0100,    \
    all      = 0x03f0, none  = 0x0000
  #endif

  #define _RWSTD_IMPLEMENTATION_VALS                                  \
    __rw_collate_category  = 0x0010, __rw_ctype_category    = 0x0020, \
    __rw_messages_category = 0x0200, __rw_Clib_LC_constants = 0x000F, \
    __rw_num_categories    = 6,      __rw_first_category    = 0x0010

  #ifndef _RWSTD_NO_STI_SIMPLE
  static const category _RWSTD_CATEGORY_NAMES, _RWSTD_IMPLEMENTATION_VALS;
  #else
  enum { _RWSTD_CATEGORY_NAMES, _RWSTD_IMPLEMENTATION_VALS };
  #endif

  #undef _RWSTD_CATEGORY_NAMES
  #undef _RWSTD_IMPLEMENTATION_VALS

  // The default constructor creates a copy of the current global locale.
  // This is the locale specified in the most recent call to locale::global().
  // If locale::global() has not been called, it is the classic "C" locale.

  inline locale () _RWSTD_THROW_SPEC_NULL;

  // The copy constructor (and the assignment operator, below) can be used
  // freely.  Like a string, most of a locale's contents are in a separate,
  // reference-counted implementation object, so copying and assigning locales
  // has little overhead.

  inline locale (const locale& other) _RWSTD_THROW_SPEC_NULL;

  // The following constructor creates a locale composed of by-name facets and
  // assigns it a name.  The valid arguments are "", "C", and a set of strings
  // defined by the compiler vendor.  These cause the facets of the locale to
  // be obtained, respectively, from the user's preferred locale, from the
  // classic locale, or from the compiler's locale database.  (In many cases,
  // the preferred locale is specified by environment variables such as LANG
  // or LC_ALL.)  If the argument is not recognized, the constructor throws
  // runtime_error.

  _EXPLICIT locale (const char* name);

  // The following constructor copies its first argument except for the facets
  // in the categories identified by the third argument, which are obtained by
  // name using the second argument.  Can throw runtime_error.

  locale (const locale& other, const char* name, category);

  // The following templatized constructor is only available if your compiler
  // supports member function templates.  It copies its first argument except
  // the single facet of type Facet, which it gets from the second argument.

  #ifndef _RWSTD_NO_MEMBER_TEMPLATES

  template <class Facet>
  inline locale (const locale& other,Facet* f);

  #else

  // If your compiler does not support member function templates, we provide
  // the following work-around to let you accrete facets onto a locale.  This
  // constructor copies its first argument except for the single facet of the
  // type of the second argument, for which it uses the second argument.
  //
  // To determine the type of the second argument, it calls the non-standard
  // virtual method __get_id in the second argument.  If you are creating your
  // own facet types on a compiler that does not support member templates, you
  // must code a __get_id member as follows in each new base class facet (i.e.
  // in each facet class that has its own static member id of type locale::id):
  //
  //    virtual locale::id &__get_id (void) const { return id; }
  //
  // See the __get_id members in the standard facets below for examples.

  inline locale (const locale& other, facet* f);

  #endif // _RWSTD_NO_MEMBER_TEMPLATES

  // The following constructor copies its first argument except for the facets
  // in the categories identified by the third argument, which are obtained
  // from the second argument.

  locale (const locale &other, const locale &second, category);

  // The destructor is non-virtual.  Other classes are not supposed to be
  // derived from locale.

  ~locale ()
  {
    __RWSTD::ref_counted::remove_reference(__imp);
  }

  const locale& operator = (const locale& other) _RWSTD_THROW_SPEC_NULL;

  // The following member function template is available only if your compiler
  // supports member templates that are templatized on types that don't appear
  // in the argument list.  It returns a copy of this locale (*this) except
  // the single facet of type Facet, which it gets from the other locale.

  #ifndef _RWSTD_NO_MEMBER_TEMPLATES
  #ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE
  template <class Facet>
  inline locale combine (const locale& other) const;
  #endif // _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE
  #endif // _RWSTD_NO_MEMBER_TEMPLATES

  // The following returns the locale name, or "*" if the locale is unnamed.

  string name () const;

  // Two locales are equal if they are the same object, or one is a copy of the
  // other (i.e. they have the same implementation object), or both are named
  // and their names are the same.

  bool operator == (const locale& other) const;
  bool operator != (const locale& other) const { return !(*this==other); }

  // The following templatized operator () satisfies STL requirements for a
  // comparator predicate template argument for comparing strings according to
  // the collating sequence of the locale.  It lets you use a locale directly
  // as a comparator using syntax like sort(v.begin(),v.end(),loc), where v is
  // a vector of some string type and loc is a locale.  If your compiler does
  // not support member function templates, we provide explicit support for
  // string and (if applicable) wstring.

  #ifndef _RWSTD_NO_MEMBER_TEMPLATES
  template <class charT, class Traits, class Allocator>
  bool operator() (const basic_string<charT,Traits,Allocator>& s1,
                   const basic_string<charT,Traits,Allocator>& s2) const;
  #else
  bool operator() (const string &s1,const string &s2) const;
  #ifndef _RWSTD_NO_WIDE_CHAR
  bool operator() (const wstring &s1,const wstring &s2) const;
  #endif
  #endif

  // Static members.

  static locale global (const locale&);  // Replaces the current global locale
  static const locale &classic ();       // Returns the classic "C" locale

  // class facet -- base class for locale feature sets.

  // Any class deriving from facet that wants to be perceived as a distinct
  // facet, as opposed to a re-implementation of an existing facet, must
  // declare a static member: static std::locale::id id;

  class facet: public __RWSTD::facet_imp {
    friend class __RWSTD::locale_imp;
    friend class locale;
   protected:
    _EXPLICIT facet (size_t refs=0,int cat=0):
        __RWSTD::facet_imp(refs,cat) { }

    virtual ~facet() { }

    #ifdef _RWSTD_NO_MEMBER_TEMPLATES
    virtual id &__get_id (void) const=0;
    #endif

   private:
    facet (const facet&);           // not defined
    void operator = (const facet&); // not defined
  };

  // class id -- facet type identifier.

  // This is mostly an implementation class.  It is used internally as an index
  // to find facets within a locale.  Each distinct facet (i.e. each T that can
  // be the parameter of a use_facet<T> call) has a unique static member of type
  // locale::id named id.  The class is made public to enable extension of the
  // set of standard facets.  Objects of this type don't need to be constructed
  // or referenced in any other circumstances.

  class id: private __RWSTD::locale_id_imp {
    _RWSTD_FRIEND_USE_HAS_FACET
    friend class locale;
#if defined ( _MSC_VER ) || defined (__TURBOC__)
   public:
#endif
#ifdef _HPACC_
    operator size_t () const { return __RWSTD::locale_id_imp::__id_value; }
#else
    operator size_t () const { return __id_value; }
#endif

    // The private copy constructor and assignment operator help restrict the
    // class to its intended use.  The copy constructor prevents the compiler
    // from generating a do-nothing default constructor, so we provide one.
    // We omit the constructors on compilers that don't support static template
    // members with constructors.

#ifndef _RWSTD_NO_STATIC_DEF3
   public:
    id () { }
   private:
    id (const id&);
#endif
   private:
    void operator = (const id&);
  };

 private:

  // Implementation.
  friend class __RWSTD::locale_imp;
  _RWSTD_FRIEND_USE_HAS_FACET

#if defined ( _MSC_VER ) || defined (__TURBOC__)
 public:
#endif

  // The only data member is a pointer to a ref-counted implementation object:
  __RWSTD::locale_imp *__imp;

  // Typedef for the implementation-defined call-back functions that must be
  // passed to __make_explicit (below).
  typedef __RWSTD::facet_maker_func __facet_maker_func;

  // The following function retrieves an implicit facet from a cache,
  // or creates one if needed (via call to the passed call-back
  // function), and makes it an explicit facet of the locale.

  __RWSTD::facet_imp * __make_explicit (const id &facet_id, bool ok_implicit,
       category facet_cat, __facet_maker_func maker) const;

  inline __RWSTD::facet_imp *get_facet (size_t) const;

#if defined(_MSC_VER) && !defined(__BORLANDC__)
 private:
#endif

  void __install (__RWSTD::facet_imp *f, const id& i) const;

  static __RWSTD::locale_imp *__the_classic_locale;
  static __RWSTD::locale_imp *__the_native_locale;
  static __RWSTD::locale_imp *__global;

  // Construct a locale from an implementation object.
  inline _EXPLICIT locale (__RWSTD::locale_imp *m);

 private:
  // We have to say private again in case _RWSTD_FRIEND_USE_HAS_FACET evaluated
  // to public:

  // Create initial implementation objects.
  static void init ();
};

#undef _RWSTD_FRIEND_USE_HAS_FACET

// ----------------------------
// Class locale inline members.
// ----------------------------

// Private constructor for use by implementation, constructs a locale from
// a locale_imp implementation object.
inline locale::locale (__RWSTD::locale_imp *m): __imp(m) {
  if (!__global) init();
  __RWSTD::ref_counted::add_reference(__imp);
}

// Default constructor, returns a copy of the current global locale.
inline locale::locale () _RWSTD_THROW_SPEC_NULL {
  if (!__global) init(); // This is not throw() ... something has to give.
  __RWSTD::ref_counted::add_reference(__imp=__global);
}

// Copy constructor.
inline locale::locale (const locale& other) _RWSTD_THROW_SPEC_NULL {
  __RWSTD::ref_counted::add_reference(__imp=other.__imp);
}

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

// Constructor to accrete or replace a single facet.
template <class Facet>
inline locale::locale (const locale& other, Facet* f)
{
  if (f) {
    __imp=new __RWSTD::locale_imp (*other.__imp,1);
    __install(f,Facet::id);
    __imp->named_=false;
  } else
    __RWSTD::ref_counted::add_reference(__imp=other.__imp);
}

#ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE

template <class Facet>
inline locale locale::combine (const locale& other) const
{
  return locale (*this,&use_facet<Facet>(other));
}

#endif /* _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE */

#else

inline locale::locale (const locale& other, facet* f):
    __imp(new __RWSTD::locale_imp (*other.__imp,1))
{
  __install(f,f->__get_id());
  __imp->named_=false;
}

#endif // _RWSTD_NO_MEMBER_TEMPLATES
// Private implementation helper function.
inline __RWSTD::facet_imp* locale::get_facet (size_t i) const {
  return __imp->get_facet(i);
}

#ifndef _RWSTD_NO_NAMESPACE
} namespace __rwstd {
#endif
// -------------------------------------------------------
// Implementation function template -- create_named_facet.
// -------------------------------------------------------
// 
// The default for facets with no derived byname version is to create a facet
// with classic ("C") behavior, ignoring the passed name.

template <class Facet>
inline Facet* create_named_facet (Facet*,const char* /* name */ ,size_t refs)
{
  return new Facet(refs);
}

#ifndef _RWSTD_NO_NAMESPACE
} // namespace __rwstd
#endif

// Get declarations for vendor-defined extensions, such as 
// declarations of the facets of the native ("") locale.
#include <rw/vendor>

#ifdef _RWSTD_NO_TEMPLATE_REPOSITORY
#include <rw/rwlocale.cc>
#endif

#endif // __STD_RWLOCALE__
#pragma option pop
#endif /* __RWLOCALE_H */
