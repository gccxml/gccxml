#ifndef __VECTOR_H
#define __VECTOR_H
#pragma option push -b -a8 -pc -Vx- -Ve- -A- -w-inl -w-aus -w-sig
// -*- C++ -*-
/***************************************************************************
 *
 * vector - declarations for the Standard Library vector class
 *
 ***************************************************************************
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
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

#ifndef __STD_VECTOR__
#define __STD_VECTOR__

#include <stdcomp.h>
#include <rw/stddefs.h> 
#include <rw/rwdispatch.h> 

#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>

#ifndef vector
#define vector vector
#endif

#ifndef _RWSTD_NO_NAMESPACE
namespace std {
#endif

//
// Note that _RWSTD_COMPLEX_DEFAULT(x)
// will expand to: ' = x', or nothing,
// depending on your compiler's capabilities and/or
// flag settings (see stdcomp.h).
//
  template <class T, class Allocator _RWSTD_COMPLEX_DEFAULT(allocator<T>) >
  class vector
  {

  private:
#if defined( _RWSTD_ALLOCATOR) && !defined(_HPACC_)
    typedef _TYPENAME Allocator::template rebind<T>::other __value_alloc_type;
#else
    typedef allocator_interface<Allocator,T> __value_alloc_type;
#endif

  public:
    //
    // Types.
    //
    typedef T                                          value_type;
    typedef Allocator                                  allocator_type;

#ifndef _RWSTD_NO_COMPLICATED_TYPEDEF
    typedef _TYPENAME _RWSTD_ALLOC_SIZE_TYPE               size_type;
    typedef _TYPENAME _RWSTD_ALLOC_DIFF_TYPE               difference_type;
    typedef _TYPENAME __value_alloc_type::pointer          iterator;
    typedef _TYPENAME __value_alloc_type::const_pointer    const_iterator;
    typedef _TYPENAME __value_alloc_type::reference        reference;
    typedef _TYPENAME __value_alloc_type::const_reference  const_reference;
    typedef _TYPENAME __value_alloc_type::pointer          pointer;
    typedef _TYPENAME __value_alloc_type::const_pointer    const_pointer;
#else
    typedef size_t          size_type;
    typedef ptrdiff_t       difference_type;
    typedef T*              iterator;
    typedef const T*        const_iterator;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T*              pointer;
    typedef const T*        const_pointer;
#endif  //_RWSTD_NO_COMPLICATED_TYPEDEF

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC 
    typedef _RW_STD::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef _RW_STD::reverse_iterator<iterator>  reverse_iterator;
#else
    typedef _RW_STD::reverse_iterator<const_iterator, 
      random_access_iterator_tag, value_type, 
      const_reference, const_pointer, difference_type>
      const_reverse_iterator;
    typedef _RW_STD::reverse_iterator<iterator, 
      random_access_iterator_tag, value_type,
      reference, pointer, difference_type>
      reverse_iterator;
#endif

protected:

    size_type          __buffer_size;
    iterator           __start;
    iterator           __finish;
    __RWSTD::__rw_basis<iterator,allocator_type>  __end_of_storage;

    void __insert_aux (iterator position, const T& x);
    void __insert_aux (iterator position, size_type n, const T& x);
#ifndef _RWSTD_NO_MEMBER_TEMPLATES
    template<class InputIterator>
    void __insert_aux (iterator position, InputIterator first, InputIterator last, _RW_is_not_integer)
    { __insert_aux2 (position, first, last); }
    template<class InputIterator>
    void __insert_aux (iterator position, InputIterator first, InputIterator last, _RW_is_integer)
    { __insert_aux (position, (size_type)first, last); }
    template<class InputIterator>
    void __insert_aux2 (iterator position, InputIterator first, InputIterator last);
#else
    void __insert_aux2 (iterator position, const_iterator first, const_iterator last);
#endif

    void __destroy(iterator start, iterator finish)
    {
      while ( start != finish)
        __value_alloc_type(__end_of_storage).destroy(start++);
    }

    // 
    //  Allocate buffers and fill with n values
    //
    void __initn(size_type n, const T& value)
    {
      __init();
      __start = __value_alloc_type(__end_of_storage).allocate(n,0);
#ifndef _RWSTD_NO_EXCEPTIONS
      try {
        uninitialized_fill_n(__start, n, value);
      } catch(...) {
        __value_alloc_type(__end_of_storage).deallocate(__start,n);
        throw;
      }      
#else
      uninitialized_fill_n(__start, n, value);
#endif // _RWSTD_NO_EXCEPTIONS
      __finish = __start + n;
      __end_of_storage = __finish;
    } 

    void __init() 
    {
      __buffer_size = 
      max((size_type)1,__RWSTD::__rw_allocation_size((value_type*)0,(size_type)0,(size_type)0));
    }
  public:
    //
    // construct/copy/destroy
    //
    _EXPLICIT vector (const Allocator& alloc _RWSTD_DEFAULT_ARG(Allocator())) 
      : __start(0), __finish(0), __end_of_storage(0,alloc)
    { 
      __init(); 
    }

#ifdef _RWSTD_NO_DEFAULT_TEMPLATE_ARGS
    vector (void) 
      : __start(0), __finish(0), __end_of_storage(0,Allocator())
    { ; }

    vector (size_type n, const T& value) 
      : __start(0), __finish(0), __end_of_storage(0,Allocator())
    {
      __initn(n,value);
    }
#endif

    _EXPLICIT vector (size_type n)
      : __start(0), __finish(0), __end_of_storage(0,Allocator())
    {
      T value = T();
      __initn(n,value);
    }

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    template<class InputIterator>
    void __init_aux (InputIterator first, InputIterator last, _RW_is_not_integer)
    {
      if (__is_input_iterator(__iterator_category(first))) {
        copy(first, last, back_inserter(*this));
      } else {
        size_type n;
        __init(); 
        __initialize(n, size_type(0));
        distance(first, last, n);
        __start = __value_alloc_type(__end_of_storage).allocate(n,0);
#ifndef _RWSTD_NO_EXCEPTIONS
        try {
          __finish = uninitialized_copy(first, last, __start);
        } catch(...) {
          __value_alloc_type(__end_of_storage).deallocate(__start,n);
          throw;
        }      
#else
        __finish = uninitialized_copy(first, last, __start);
#endif // _RWSTD_NO_EXCEPTIONS
        __end_of_storage = __finish;
      }
    }
    template<class InputIterator>
    void __init_aux (InputIterator first, InputIterator last, _RW_is_integer)
    { __initn((size_type)first,last); }

    template<class InputIterator>
    vector (InputIterator first, InputIterator last,
            const Allocator& alloc _RWSTD_DEFAULT_ARG(Allocator()))
      : __start(0), __finish(0), __end_of_storage(0,alloc)
    {
        typedef _TYPENAME _RWdispatch<InputIterator>::_RWtype _RWtype;
        __init_aux(first, last, _RWtype());
    }
    vector (size_type n, const T& value,
            const Allocator& alloc _RWSTD_DEFAULT_ARG(Allocator()))
      : __start(0), __finish(0), __end_of_storage(0,alloc)
    { __initn((size_type)n,value); }
#else
    //
    // Build a vector of size n with each element set to copy of value.
    //
    vector (size_type n, const T& value,
            const Allocator& alloc _RWSTD_DEFAULT_ARG(Allocator()))
      : __start(0), __finish(0), __end_of_storage(0,alloc)
    {
      __initn(n,value);
    }

    vector (const_iterator first, const_iterator last,
            const Allocator& alloc _RWSTD_DEFAULT_ARG(Allocator()))
      : __start(0), __finish(0), __end_of_storage(0,alloc)
    {
      size_type n;
      __init(); 
      __initialize(n, size_type(0));
      distance(first, last, n);
      __start = __value_alloc_type(__end_of_storage).allocate(n,0);
#ifndef _RWSTD_NO_EXCEPTIONS
      try {
        __finish = uninitialized_copy(first, last, __start);
      } catch(...) {
        __value_alloc_type(__end_of_storage).deallocate(__start,n);
        throw;
      }      
#else
      __finish = uninitialized_copy(first, last, __start);
#endif // _RWSTD_NO_EXCEPTIONS
      __end_of_storage = __finish;
    }

#ifdef _RWSTD_NO_DEFAULT_TEMPLATE_ARGS
    vector (const_iterator first, const_iterator last)
      : __start(0), __finish(0), __end_of_storage(0,Allocator())
    {
      size_type n;
      __init(); 
      __initialize(n, size_type(0));
      distance(first, last, n);
      __start = __value_alloc_type(__end_of_storage).allocate(n,0);
#ifndef _RWSTD_NO_EXCEPTIONS
      try {
        __finish = uninitialized_copy(first, last, __start);
      } catch(...) {
        __value_alloc_type(__end_of_storage).deallocate(__start,n);
        throw;
      }      
#else
      __finish = uninitialized_copy(first, last, __start);
#endif //  _RWSTD_NO_EXCEPTIONS
      __end_of_storage = __finish;
    }
#endif // _RWSTD_NO_DEFAULT_TEMPLATE_ARGS
#endif // _RWSTD_NO_MEMBER_TEMPLATES

    vector (const vector<T,Allocator>& x)
      : __start(0), __finish(0), __end_of_storage(0,x.get_allocator())
    {
      __init(); 
      __start = __value_alloc_type(__end_of_storage).allocate(x.end() - x.begin(),0);
      __finish = uninitialized_copy(x.begin(), x.end(), __start);
      __end_of_storage = __finish;
    }

    ~vector ()
    { 
      __destroy(__start, __finish); 
      __value_alloc_type(__end_of_storage).deallocate(__start,__end_of_storage.data()-__start);
    }

    vector<T,Allocator>& operator= (const vector<T,Allocator>& x);

#ifndef _RWSTD_NO_MEMBER_TEMPLATES
        template<class InputIterator>
        void assign (InputIterator first, InputIterator last)
        {
                erase(begin(), end());
        typedef _TYPENAME _RWdispatch<InputIterator>::_RWtype _RWtype;
            __insert_aux(begin(), first, last, _RWtype());
        }
        void assign (size_type n, const T& t)
        {
                erase(begin(), end()); insert(begin(), n, t);
        }
#else
    void assign (const_iterator first, const_iterator last)
    { erase(begin(), end()); insert(begin(), first, last); }
    //
    // Assign n copies of t to this vector.
    //
    void assign (size_type n, const T& t)
    { erase(begin(), end()); insert(begin(), n, t); }
#endif // _RWSTD_NO_MEMBER_TEMPLATES

    allocator_type get_allocator() const
    {
      return (allocator_type)__end_of_storage;
    }

    //
    // Iterators.
    //
    iterator       begin ()       { return __start;  }
    const_iterator begin () const { return __start;  }
    iterator       end ()         { return __finish; }
    const_iterator end ()   const { return __finish; }

    reverse_iterator rbegin ()
    { 
      reverse_iterator tmp(end()); return tmp;
    }
    const_reverse_iterator rbegin () const
    { 
      const_reverse_iterator tmp(end()); return tmp;
    }
    reverse_iterator rend ()
    { 
      reverse_iterator tmp(begin()); return tmp;
    }
    const_reverse_iterator rend () const
    { 
      const_reverse_iterator tmp(begin()); return tmp;
    }

    //
    // Capacity.
    //
    size_type size ()     const { return size_type(end() - begin()); }
    size_type max_size () const { return __value_alloc_type(__end_of_storage).max_size();   }
    void resize (size_type new_size);
    void resize (size_type new_size, T value);

    size_type capacity () const { return size_type(__end_of_storage.data() - begin()); }
    bool      empty ()    const { return begin() == end();                    }
    void reserve (size_type n)
    {
      _RWSTD_THROW(n > max_size(), length_error,
        __RWSTD::except_msg_string(__RWSTD::__rwse_InvalidSizeParam,
          "vector::reserve(size_t)",n,max_size()).msgstr());

      if (capacity() < n)
      {
        __value_alloc_type va(__end_of_storage);
        iterator tmp = va.allocate(n,__start);
#ifndef _RWSTD_NO_EXCEPTIONS
        try {
          uninitialized_copy(begin(), end(), tmp);
        } catch(...) {
          __value_alloc_type(__end_of_storage).deallocate(tmp,n);
          throw;
        }
#else
        uninitialized_copy(begin(), end(), tmp);
#endif // _RWSTD_NO_EXCEPTIONS
        int tmp_size = size(); // RW_BUG: Fix for bts-78394
        __destroy(__start, __finish);
        va.deallocate(__start,__end_of_storage.data()-__start);
        __finish = tmp + tmp_size; // RW_BUG: Fix for bts-78394
        __start = tmp;
        __end_of_storage = begin() + n;
      }
    }

    //
    // Element access.
    //
    reference       operator[] (size_type n)       
    {
#ifdef _RWSTD_BOUNDS_CHECKING
      _RWSTD_THROW(n >= size(), out_of_range,
        __RWSTD::except_msg_string(__RWSTD::rwse_OutOfRange,
          "vector::operator[](size_t)",n,size()).msgstr());
#endif
      return *(begin() + n);

    }
  
    const_reference operator[] (size_type n) const 
    {
#ifdef _RWSTD_BOUNDS_CHECKING
      _RWSTD_THROW(n >= size(), out_of_range,
        __RWSTD::except_msg_string(__RWSTD::rwse_OutOfRange,
          "vector::operator[](size_t) const",n,size()).msgstr());
#endif
      return *(begin() + n);
    }
  
    reference       at (size_type n)               
    { 
      _RWSTD_THROW(n >= size(), out_of_range,
        __RWSTD::except_msg_string(__RWSTD::rwse_OutOfRange,
          "vector:: at(size_t)",n,size()).msgstr());
      return *(begin() + n); 
    }
    const_reference at (size_type n)  const 
    { 
      _RWSTD_THROW(n >= size(), out_of_range,
        __RWSTD::except_msg_string(__RWSTD::rwse_OutOfRange,
          "vector:: at(size_t) const",n,size()).msgstr());
      return *(begin() + n); 
    }
    reference       front ()                       { return *begin();       }
    const_reference front ()                 const { return *begin();       }
    reference       back ()                        { return *(end() - 1);   }
    const_reference back ()                  const { return *(end() - 1);   }

    //
    // Modifiers.
    //
    void push_back (const T& x)
    {
      if (__finish != __end_of_storage.data())
      {
        __value_alloc_type(__end_of_storage).construct(__finish, x); 
        __finish++;
      }
      else
        __insert_aux(end(), x);
    }
    void pop_back()
    {
      --__finish; 
      __value_alloc_type(__end_of_storage).destroy(__finish);
    }

    //
    // Insert x at position.
    //
    iterator insert (iterator position, const T& x)
    {
      size_type n = position - begin();
      if (__finish != __end_of_storage.data() && position == end())
      {
        __value_alloc_type(__end_of_storage).construct(__finish, x); __finish++;
      }
      else
        __insert_aux(position, x);
      return begin() + n;
    }

#ifndef _RWSTD_NO_MEMBER_TEMPLATES
    template<class InputIterator>
    void insert (iterator position, InputIterator first, 
                 InputIterator last)
    {
        typedef _TYPENAME _RWdispatch<InputIterator>::_RWtype _RWtype;
        __insert_aux(position, first, last, _RWtype());
    }
    void insert (iterator position, size_type n, const T& value)
    { __insert_aux(position,n,value); }
#else
    void insert (iterator position, size_type n, const T& x)
    { __insert_aux(position,n,x); }
    void insert (iterator position, const_iterator first, const_iterator last)
    { __insert_aux2(position, first, last); }
#endif // _RWSTD_NO_MEMBER_TEMPLATES

    iterator erase (iterator position)
    {
      if (position + 1 != end()) 
        copy(position + 1, end(), position);
      --__finish;
      __value_alloc_type(__end_of_storage).destroy(__finish);
      return position;
    }
    iterator erase (iterator first, iterator last)
    {
      iterator i = copy(last, end(), first);
      iterator tmp = __finish;
      __finish = __finish - (last - first); 
      __destroy(i, tmp);
      return first;
    }

    void swap (vector<T,Allocator>& x)
    {
      if((allocator_type)__end_of_storage==(allocator_type)x.__end_of_storage)
      {
#ifndef _RWSTD_NO_NAMESPACE
        std::swap(__start, x.__start);
        std::swap(__finish, x.__finish);
        std::swap(__end_of_storage, x.__end_of_storage);
#else
        ::swap(__start, x.__start);
        ::swap(__finish, x.__finish);
        ::swap(__end_of_storage, x.__end_of_storage);
#endif // _RWSTD_NO_NAMESPACE
      }
      else
      {
        vector<T,Allocator> _x = *this;
        *this = x;
        x=_x;
      } 
    }

    void clear()
    {
      erase(begin(),end());
    }

#ifndef _RWSTD_STRICT_ANSI
    // Non-standard function for setting buffer allocation size
    size_type allocation_size() { return __buffer_size; }
    size_type allocation_size(size_type new_size) 
    { 
      size_type tmp = __buffer_size; 
      __buffer_size = max((size_type)1,new_size);
      return tmp;
    }
#endif  // _RWSTD_STRICT_ANSI
  };

  template <class T, class Allocator>
  inline bool operator== (const vector<T,Allocator>& x, const vector<T,Allocator>& y)
  {
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
  }

  template <class T, class Allocator>
  inline bool operator< (const vector<T,Allocator>& x, const vector<T,Allocator>& y)
  {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
  }

#if !defined(_RWSTD_NO_NAMESPACE) || !defined(_RWSTD_NO_PART_SPEC_OVERLOAD)
  template <class T, class Allocator>
  inline bool operator!= (const vector<T,Allocator>& x, const vector<T,Allocator>& y)
  {
    return !(x == y);
  }

  template <class T, class Allocator>
  inline bool operator> (const vector<T,Allocator>& x, const vector<T,Allocator>& y)
  {
    return y < x;
  }

  template <class T, class Allocator>
  inline bool operator>= (const vector<T,Allocator>& x, const vector<T,Allocator>& y)
  {
    return !(x < y);
  }

  template <class T, class Allocator>
  inline bool operator<= (const vector<T,Allocator>& x, const vector<T,Allocator>& y)
  {
    return !(y <  x);
  }

  template <class T, class Allocator>
  inline void swap(vector<T,Allocator>& a, vector<T,Allocator>& b)
  {
    a.swap(b);
  }
#endif // !defined(_RWSTD_NO_NAMESPACE) || !defined(_RWSTD_NO_PART_SPEC_OVERLOAD)
#if defined (_RWSTD_NO_TEMPLATE_REPOSITORY) && defined (__BORLANDC__) && (__TURBOC__ < 0x540)
#ifndef _RWSTD_NO_NAMESPACE
}
#endif

#include <vector.cc>

#ifndef _RWSTD_NO_NAMESPACE
namespace std {
#endif

#endif // (_RWSTD_NO_TEMPLATE_REPOSITORY) && defined (__BORLANDC__)

//
// If bool is a builtin type, we provide a vector<bool,allocator> specialization.
// We do not provide the allocator interface for this specialization.
//
#ifndef _RWSTD_NO_BOOL

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC

  template <class Allocator>
  class _RWSTDExport vector<bool, Allocator >
  {

#else

// Use a macro to mutate Allocator into allocator<bool>
#define Allocator allocator<bool>

  _RWSTD_TEMPLATE
  class _RWSTDExport vector<bool, allocator<bool> >
  {

#endif // _RWSTD_NO_CLASS_PARTIAL_SPEC

  public:  
    //
    // types
    //
    typedef Allocator                                 allocator_type;
    typedef bool                                      value_type;

  private:
#if defined ( _RWSTD_ALLOCATOR) && !defined(_HPACC_)
#ifdef _RWSTD_NO_CLASS_PARTIAL_SPEC
    typedef _TYPENAME allocator_type::template rebind<unsigned int>::other __value_alloc_type;
#else
    typedef typename allocator_type::template rebind<unsigned int>::other __value_alloc_type;
#endif
#else
    typedef allocator_interface<allocator_type,unsigned int> __value_alloc_type;
#endif

  public:
#ifdef _RWSTD_NO_EMBEDDED_TYPEDEF
    typedef allocator<bool>::size_type               size_type;
    typedef allocator<bool>::difference_type         difference_type;
#else
#ifdef _RWSTD_NO_CLASS_PARTIAL_SPEC
    typedef allocator_type::size_type          size_type;
    typedef allocator_type::difference_type    difference_type;
#else
    typedef _TYPENAME allocator_type::size_type          size_type;
    typedef _TYPENAME allocator_type::difference_type    difference_type;
#endif
#endif

  protected:
#ifdef _RWSTD_NO_CLASS_PARTIAL_SPEC
    typedef __value_alloc_type::pointer       pointer;
    typedef __value_alloc_type::const_pointer const_pointer;
#else
    typedef _TYPENAME __value_alloc_type::pointer       pointer;
    typedef _TYPENAME __value_alloc_type::const_pointer const_pointer;
#endif

  public:

    //
    // forward declarations
    //
    class iterator;
    class const_iterator;

    //
    // bit reference
    //
    class reference
    {
      friend class iterator;
      friend class const_iterator;
    protected:
      unsigned int* p;
      unsigned int mask;
      reference (unsigned int* x, unsigned int y) : p(x), mask(y) {}
    public:
      reference () : p(0), mask(0) {}
      operator bool () const { return !(!(*p & mask)); }
      reference& operator= (bool x)
      {
        if (x)      
          *p |= mask;
        else
          *p &= ~mask;
        return *this;
      }
      reference& operator= (const reference& x) { return *this = bool(x); }

#ifndef _RWSTD_STRICT_ANSI
      bool operator== (const reference& x) const
      {
        return bool(*this) == bool(x);
      }
      bool operator< (const reference& x) const
      {
#if !defined(_MSC_VER) || defined(__BORLANDC__)
        return bool(*this) < bool(x);
#else
        return int(*this) < int(x);
#endif
      }
      bool operator!= (const reference& x) const
      {
        return !(*this == x);
      }
      bool operator> (const reference& x) const
      {
        return  x < *this;
      }
      bool operator>= (const reference& x) const
      {
        return !(*this < x);
      }
      bool operator<= (const reference& x) const
      {
        return !(*this > x);
      }
#endif // _RWSTD_STRICT_ANSI

      void flip () { *p ^= mask; }
    };
    
    typedef bool const_reference;
    //
    // Definition of our iterator.
    //
    class iterator : public _RW_STD::iterator<random_access_iterator_tag,
                                     value_type, difference_type,
                                     pointer,reference>
    {
#if !defined(_RWSTD_NO_CLASS_PARTIAL_SPEC)
      //friend class vector<bool,Allocator>;
#else
      friend class vector<bool,allocator<bool> >;
#endif
      friend class const_iterator;

      //protected:

      unsigned int* p;
      unsigned int  offset;

      void __bump_up ()
      {
        if (offset++ == _RWSTD_WORD_BIT - 1)
        {
          offset = 0; ++p;
        }
      }
      void __bump_down ()
      {
        if (offset-- == 0)
        {
          offset = _RWSTD_WORD_BIT - 1; --p;
        }
      }

    public:
      iterator () : p(0), offset(0) {}
      iterator (unsigned int* x, unsigned int y) : p(x), offset(y) {}

      reference operator* () const { return reference(p, 1U << offset); }
      iterator& operator++ ()
      {
        __bump_up(); return *this;
      }
      iterator operator++ (int)
      {
        iterator tmp = *this; __bump_up(); return tmp;
      }
      iterator& operator-- ()
      {
        __bump_down(); return *this;
      }
      iterator operator-- (int)
      {
        iterator tmp = *this; __bump_down(); return tmp;
      }
      iterator& operator+= (difference_type i)
      {
        difference_type n = i + offset;
        p += n / _RWSTD_WORD_BIT;
        n = n % _RWSTD_WORD_BIT;
        if (n < 0)
        {
          offset = n + _RWSTD_WORD_BIT; --p;
        }
        else
          offset = n;
        return *this;
      }
      iterator& operator-= (difference_type i)
      {
        *this += -i; return *this;
      }
      iterator operator+ (difference_type i) const
      {
        iterator tmp = *this; return tmp += i;
      }
      iterator operator- (difference_type i) const
      {
        iterator tmp = *this; return tmp -= i;
      }
      difference_type operator- (iterator x) const
      {
        return _RWSTD_WORD_BIT * (p - x.p) + offset - x.offset;
      }
      reference operator[] (difference_type i)
      {
        return *(*this + i);
      }
      bool operator== (const iterator& x) const
      {
        return p == x.p && offset == x.offset;
      }
      bool operator< (const iterator& x) const
      {
        return p < x.p || (p == x.p && offset < x.offset);
      }
      bool operator!= (const iterator& x) const
      {
        return !(*this == x);
      }
      bool operator> (const iterator& x) const
      {
        return x < *this;
      }
      bool operator>= (const iterator& x) const
      {
        return !(*this < x);
      }
      bool operator<= (const iterator& x) const
      {
        return !(*this > x);
      }
    };
    //
    // Definition of our const_iterator.
    //
    class const_iterator
      : public _RW_STD::iterator<random_access_iterator_tag,
                        value_type, difference_type, 
                        const_pointer, const_reference>
    {
#if !defined(_RWSTD_NO_CLASS_PARTIAL_SPEC)
      //friend class vector<bool,Allocator>;
#else
      friend class vector<bool,allocator<bool> >;
#endif

      //protected:

      unsigned int* p;
      unsigned int offset;
      void  __bump_up ()
      {
        if (offset++ == _RWSTD_WORD_BIT - 1)
        {
          offset = 0; ++p;
        }
      }
      void __bump_down()
      {
        if (offset-- == 0)
        {
          offset = _RWSTD_WORD_BIT - 1; --p;
        }
      }

    public:
      const_iterator () : p(0), offset(0) {}
      const_iterator (unsigned int* x, unsigned int y) : p(x), offset(y) {}
#if !defined(_MSC_VER) || defined(__BORLANDC__)
#ifdef _RWSTD_NO_CLASS_PARTIAL_SPEC
      const_iterator (const vector<bool,Allocator>::iterator& x) : p(x.p), offset(x.offset) {}
#else
      const_iterator (const _TYPENAME vector<bool,Allocator>::iterator& x) : p(x.p), offset(x.offset) {}
#endif
#else
      const_iterator (const iterator& x) : p(x.p), offset(x.offset) {}
#endif
      const_reference operator* () const
      {
        return vector<bool,Allocator>::reference(p, 1U << offset);
      }
      const_iterator& operator++ ()
      {
        __bump_up(); return *this;
      }
      const_iterator operator++ (int)
      {
        const_iterator tmp = *this; __bump_up(); return tmp;
      }
      const_iterator& operator-- ()
      {
        __bump_down(); return *this;
      }
      const_iterator operator-- (int)
      {
        const_iterator tmp = *this; __bump_down(); return tmp;
      }
      const_iterator& operator+= (difference_type i)
      {
        difference_type n = i + offset;
        p += n / _RWSTD_WORD_BIT;
        n = n % _RWSTD_WORD_BIT;
        if (n < 0)
        {
          offset = n + _RWSTD_WORD_BIT; --p;
        }
        else
          offset = n;
        return *this;
      }
      const_iterator& operator-= (difference_type i)
      {
        *this += -i; return *this;
      }
      const_iterator operator+ (difference_type i) const
      {
        const_iterator tmp = *this; return tmp += i;
      }
      const_iterator operator- (difference_type i) const
      {
        const_iterator tmp = *this; return tmp -= i;
      }
      difference_type operator- (const_iterator x) const
      {
        return _RWSTD_WORD_BIT * (p - x.p) + offset - x.offset;
      }
      const_reference operator[] (difference_type i)
      { 
        return *(*this + i); 
      }
      bool operator== (const const_iterator& x) const
      {
        return p == x.p && offset == x.offset;
      }
      bool operator< (const const_iterator& x) const
      {
        return p < x.p || (p == x.p && offset < x.offset);
      }
      bool operator!= (const const_iterator& x) const
      {
        return !(*this == x);
      }
      bool operator> (const const_iterator& x) const
      {
        return x < *this;
      }
      bool operator>= (const const_iterator& x) const
      {
        return !(*this < x);
      }
      bool operator<= (const const_iterator& x) const
      {
        return !(*this > x);
      }
    };
    //
    // types
    //
#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC 
    typedef _RW_STD::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef _RW_STD::reverse_iterator<iterator>  reverse_iterator;
#else
    typedef _RW_STD::reverse_iterator<const_iterator, 
      random_access_iterator_tag, value_type, 
      const_reference, const_pointer, difference_type>
      const_reverse_iterator;
    typedef _RW_STD::reverse_iterator<iterator, 
      random_access_iterator_tag, value_type,
      reference, pointer, difference_type>
      reverse_iterator;
#endif

  private:
    //
    // These private functions are replicas of generic algorithms.
    //  We provide them here to avoid putting instantiations of 
    //  the generic algorithms into an archive or shared library.
    //  This gives you full flexibilty in deciding where you want
    //  to put particular instantiations of the generic 
    //  algorithms.
    //
  
    void __fill (iterator first, iterator last, 
               const bool& value)
    {
      while (first != last) *first++ = value;
    }
    void __fill_n (iterator first, size_type n,
                 const bool& value)
    {
      while (n-- > 0) *first++ = value;
    }
#ifndef _RWSTD_NO_MEMBER_TEMPLATES
    template <class Iterator>
    iterator __copy (Iterator first, Iterator last,
                   iterator result)
    {
      while (first != last) *result++ = *first++;
      return result;
    }
    template <class Iterator>
    iterator __copy_backward (Iterator first, Iterator last,
                            iterator result)
    {
      while (first != last) *--result = *--last;
      return result;
    }
#else
    iterator __copy (const_iterator first, const_iterator last,
                   iterator result)
    {
      while (first != last) *result++ = *first++;
      return result;
    }
    iterator __copy (const bool* first, const bool* last,
                   iterator result)
    {
      while (first != last) *result++ = *first++;
      return result;
    }
    iterator __copy_backward (const_iterator first, const_iterator last,
                            iterator result)
    {
      while (first != last) *--result = *--last;
      return result;
    }
    iterator __copy_backward (const bool* first, const bool* last,
                            iterator result)
    {
      while (first != last) *--result = *--last;
      return result;
    }
#endif

  protected:

    iterator                __start;
    iterator                __finish;
    __RWSTD::__rw_basis<unsigned int*,allocator_type>   __end_of_storage;

    unsigned int* __bit_alloc (size_type n)
    {
      return __value_alloc_type(__end_of_storage).allocate((n + _RWSTD_WORD_BIT - 1)/_RWSTD_WORD_BIT,__start.p);
    }
    void __init (size_type n)
    {
      unsigned int* q = __bit_alloc(n);
      __end_of_storage = q + (n + _RWSTD_WORD_BIT - 1)/_RWSTD_WORD_BIT;
      __start = iterator(q, 0);
      __finish = __start + n;
    }
    void __insert_aux (iterator position, bool x);

  public:

    //
    // construct/copy/destroy
    //
    vector<bool,Allocator> (const Allocator&  alloc _RWSTD_DEFAULT_ARG(Allocator()))
      : __start(iterator()), __finish(iterator()), 
        __end_of_storage(0,alloc)
    { ; }
    _EXPLICIT vector<bool,Allocator> (size_type n, bool value = bool(), 
       const Allocator&  alloc _RWSTD_DEFAULT_ARG(Allocator()))
      : __end_of_storage(0,alloc)
    {
      __init(n); 
      unsigned int * first = __start.p;
      size_type m = (n + _RWSTD_WORD_BIT - 1)/_RWSTD_WORD_BIT;
      while (m-- > 0) *first++ = value ? ~0 : 0;
    }

    vector<bool,Allocator> (const vector<bool,Allocator >& x)
      : __end_of_storage(0,x.get_allocator())
    {
      __init(x.size()); 
      __copy(x.begin(), x.end(), __start);
    }

#ifndef _RWSTD_NO_MEMBER_TEMPLATES
    template<class InputIterator>
    vector<bool,Allocator>  (InputIterator first, InputIterator last)
      : __end_of_storage(0,Allocator())
    {
      size_type n;
      __initialize(n, size_type(0));
      distance(first, last, n);
      __init(n);
      __copy(first, last, __start);
    }
#else
    vector<bool,Allocator> (const_iterator first, const_iterator last)
      : __end_of_storage(0,Allocator())
    {
      size_type n;
      __initialize(n, size_type(0));
      distance(first, last, n);
      __init(n);
      __copy(first, last, __start);
    }
    vector<bool,Allocator> (const bool* first, const bool* last)
      : __end_of_storage(0,Allocator())
    {
      size_type n;
      __initialize(n, size_type(0));
      distance(first, last, n);
      __init(n);
      __copy(first, last, __start);
    }
#endif
    ~vector<bool,Allocator> () {
      __value_alloc_type(__end_of_storage).deallocate(__start.p,  
        __end_of_storage.data()-__start.p); 
    }
    vector<bool,Allocator>& operator= (const vector<bool, Allocator>& x)
    {
      if (&x == this) return *this;
      if (x.size() > capacity())
      {
        __value_alloc_type(__end_of_storage).deallocate(__start.p,
          __end_of_storage.data()-__start.p); 
        __init(x.size());
      }
      __copy(x.begin(), x.end(), begin());
      __finish = begin() + x.size();
      return *this;
    }
#ifndef _RWSTD_NO_MEMBER_TEMPLATES
    template<class InputIterator>
    void assign (InputIterator first, InputIterator last)
    { erase(begin(), end()); insert(begin(), first, last); }
#else
    void assign (const_iterator first, const_iterator last)
    { erase(begin(), end()); insert(begin(), first, last); }
#endif

    void assign (size_type n, const bool& t = bool())
    { erase(begin(), end()); insert(begin(), n, t);  }

    allocator_type get_allocator() const
    {
      return (allocator_type)__end_of_storage;
    }

    //
    // iterators
    //
    iterator       begin ()       { return __start; }
    const_iterator begin () const 
    { return const_iterator(__start.p,__start.offset); }
    iterator       end   ()       { return __finish; }
    const_iterator end   () const 
    { return const_iterator(__finish.p,__finish.offset); }

    reverse_iterator       rbegin () { return reverse_iterator(end()); }
    const_reverse_iterator rbegin () const
    { 
      return const_reverse_iterator(end()); 
    }
    reverse_iterator       rend () { return reverse_iterator(begin()); }
    const_reverse_iterator rend () const
    { 
      return const_reverse_iterator(begin()); 
    }

    //
    // capacity
    //
    size_type size     () const { return size_type(end() - begin());  }
    size_type max_size () const { return __value_alloc_type(__end_of_storage).max_size(); }
    void resize (size_type new_size, bool c = false);
    size_type capacity () const
    {
      return size_type(const_iterator(__end_of_storage.data(), 0) - begin());
    }
    bool empty () const { return begin() == end(); }
    void reserve (size_type n)
    {
      _RWSTD_THROW(n > max_size(), length_error,
        __RWSTD::except_msg_string(__RWSTD::__rwse_InvalidSizeParam,
          "vector<bool>::reserve(size_t)",n,max_size()).msgstr());
      if (capacity() < n)
      {
        unsigned int* q = __bit_alloc(n);
        __finish = __copy(begin(), end(), iterator(q, 0));
        __value_alloc_type(__end_of_storage).deallocate(__start.p,
                                                   __end_of_storage.data()-__start.p);
        __start = iterator(q, 0);
        __end_of_storage = q + (n + _RWSTD_WORD_BIT - 1)/_RWSTD_WORD_BIT;
      }
    }

    //
    // element access
    //
    reference       operator[] (size_type n)       
    { 
#ifdef _RWSTD_BOUNDS_CHECKING
      _RWSTD_THROW(n >= size(), out_of_range,
        __RWSTD::except_msg_string(__RWSTD::rwse_OutOfRange,
          "vector<bool>::[](size_t)",n,size()).msgstr());
#endif
      return *(begin() + n); 
    }
    const_reference operator[] (size_type n) const 
    { 
#ifdef _RWSTD_BOUNDS_CHECKING
      _RWSTD_THROW(n >= size(), out_of_range,
        __RWSTD::except_msg_string(__RWSTD::rwse_OutOfRange,
          "vector<bool>::[](size_t)",n,size()).msgstr());
#endif
      return *(begin() + n); 
    }
    reference       at (size_type n)               
    { 
      _RWSTD_THROW(n >= size(), out_of_range,
        __RWSTD::except_msg_string(__RWSTD::rwse_OutOfRange,
          "vector<bool>:: at(size_t)",n,size()).msgstr());
      return *(begin() + n); 
    }
    const_reference at (size_type n)   const 
    {
      _RWSTD_THROW(n >= size(), out_of_range,
        __RWSTD::except_msg_string( __RWSTD::rwse_OutOfRange,
          "vector<bool>:: at(size_t) const",n,size()).msgstr());

      return *(begin() + n); 
    }
    reference       front ()       { return *begin();     }
    const_reference front () const { return *begin();     }
    reference       back  ()       { return *(end() - 1); }
    const_reference back  () const { return *(end() - 1); }
    
    //
    // modifiers
    //
    void push_back (const bool& x)
    {
      if (__finish.p != __end_of_storage.data())
        *__finish++ = x;
      else
        __insert_aux(end(), x);
    }
    void pop_back () { --__finish; }

    iterator insert (iterator position, const bool& x = bool())
    {
      size_type n = position - begin();
      if (__finish.p != __end_of_storage.data() && position == end())
        *__finish++ = x;
      else
        __insert_aux(position, x);
      return begin() + n;
    }
    void insert (iterator position, size_type n, const bool& x);

#ifndef _RWSTD_NO_MEMBER_TEMPLATES
    template<class InputIterator>
    void insert (iterator position, InputIterator first, InputIterator last);
#else
    void insert (iterator position, const_iterator first, 
                 const_iterator last);
#endif

    iterator erase (iterator position)
    {
      if (!(position + 1 == end()))
        __copy(position + 1, end(), position);
      --__finish;
      return position;
    }
    iterator erase(iterator first, iterator last)
    {
      __finish = __copy(last, end(), first);
      return first;
    }
    void swap (vector<bool,Allocator >& x)
    {
      if((allocator_type)__end_of_storage==(allocator_type)x.__end_of_storage)
      {
#ifndef _RWSTD_NO_NAMESPACE
        std::swap(__start,          x.__start);
        std::swap(__finish,         x.__finish);
        std::swap(__end_of_storage, x.__end_of_storage);
#else
        ::swap(__start,          x.__start); 
        ::swap(__finish,         x.__finish);
        ::swap(__end_of_storage, x.__end_of_storage);
#endif // _RWSTD_NO_NAMESPACE
      }
      else
      {
        vector<bool,Allocator> _x = *this;
        *this = x;
        x=_x;
      } 
    }
    static void swap(reference x, reference y);
    void flip ();
    void clear()
    {
      erase(begin(),end());
    }
  };

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
  template <class Allocator>
#endif
  inline bool operator== (const vector<bool,Allocator >& x, 
                          const vector<bool,Allocator >& y)
  {
    if (x.size() == y.size())
    {
#ifdef _RWSTD_NO_CLASS_PARTIAL_SPEC
      vector<bool,Allocator >::const_iterator first1 = x.begin(), 
#else
      _TYPENAME vector<bool,Allocator >::const_iterator first1 = x.begin(), 
#endif
      last1 =x.end(),
      first2 = y.begin();
            
      while (first1 != last1 && *first1 == *first2)
      {
        ++first1;
        ++first2;
      }
      return first1 == last1;
    }
    return false;
  }

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
  template <class Allocator>
#endif
  inline bool operator< (const vector<bool,Allocator >& x, 
                         const vector<bool,Allocator >& y)
  {
#ifdef _RWSTD_NO_CLASS_PARTIAL_SPEC
    vector<bool,Allocator >::const_iterator first1 = x.begin(), 
#else
    _TYPENAME vector<bool,Allocator >::const_iterator first1 = x.begin(), 
#endif
    last1 =x.end(),
    first2 = y.begin(),
    last2 = y.end();

    while (first1 != last1 && first2 != last2)
    {
      if ((int)*first1 < (int)*first2)     return true;
      if ((int)*first2++ < (int)*first1++) return false;
    }
    return first1 == last1 && first2 != last2;
  }

#if !defined(_RWSTD_NO_NAMESPACE) || !defined(_RWSTD_NO_PART_SPEC_OVERLOAD)
#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
  template <class Allocator>
#endif
  inline bool operator!= (const vector<bool,Allocator >& x, 
                          const vector<bool,Allocator >& y)
  {
    return !(x == y);
  }

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
  template <class Allocator>
#endif
  inline bool operator> (const vector<bool,Allocator >& x, 
                         const vector<bool,Allocator >& y)
  {
    return y < x;
  }

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
  template <class Allocator>
#endif
  inline bool operator>= (const vector<bool,Allocator >& x, 
                          const vector<bool,Allocator >& y)
  {
    return !(x < y);
  }

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
  template <class Allocator>
#endif
  inline bool operator<= (const vector<bool,Allocator >& x, 
                          const vector<bool,Allocator >& y)
  {
    return !(y <  x);
  }

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
  template <class Allocator>
#endif
  inline void swap(vector<bool,Allocator >& a, vector<bool,Allocator >& b)
  {
    a.swap(b);
  }

#endif

#ifdef _RWSTD_NO_CLASS_PARTIAL_SPEC
#undef Allocator 
#endif

#endif /*_RWSTD_NO_BOOL*/

#ifndef _RWSTD_NO_NAMESPACE
}
#endif

#if defined (_RWSTD_NO_TEMPLATE_REPOSITORY) && (!defined (__TURBOC__) || (__TURBOC__ > 0x530))
#include <vector.cc>
#endif

#undef vector

#endif /*__STD_VECTOR__*/

#ifndef __USING_STD_NAMES__
  using namespace std;
#endif

#pragma option pop
#endif /* __VECTOR_H */
