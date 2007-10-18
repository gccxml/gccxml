#ifndef __VALARRAY_H
#define __VALARRAY_H
#pragma option push -b -a8 -pc -Vx- -Ve- -w-inl -w-aus -w-sig
// -*- C++ -*-
#ifndef __RW_VALARRAY__
#define __RW_VALARRAY__

/***************************************************************************
 *
 * valaray - Declarations for the Standard Library valarray
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
#include <rw/valimp>

#ifndef _RWSTD_NO_NAMESPACE
namespace std {
#endif

// forward declarations
  class _RWSTDExport slice;
  template <class T> class slice_array;
  class _RWSTDExport gslice;
  template <class T> class gslice_array;
  template <class T> class mask_array;
  template <class T> class indirect_array;
/*************************************************************
 *                     CLASS VALARRAY                        *
 *************************************************************/

  template <class T>  class valarray {

  public:

    typedef T value_type;

// constructors

    valarray( ) {;}

    _EXPLICIT valarray(size_t size)
    { memory_array._initial_size(size); }

    valarray(const T& value, size_t size)
    { memory_array._initialize_with_value(value,size); }

    valarray(const T* pointer, size_t size)
    { memory_array._initialize_with_array(pointer,size); }

    valarray(const valarray<T>& array)
    { memory_array._copy_memory_array(array._RW_get_memory_array()); }

    valarray(const slice_array<T>&);
    valarray(const gslice_array<T>&);
    valarray(const mask_array<T>&);
    valarray(const indirect_array<T>&);

    // operator =

    valarray<T>& operator= (const valarray<T>& array)
    {
      if ( this != &array )
        memory_array._copy_memory_array(array._RW_get_memory_array());
      return *this;
    }

    valarray<T>& operator= (const T&);
    valarray<T>& operator= (const slice_array<T>&);
    valarray<T>& operator= (const gslice_array<T>&);
    valarray<T>& operator= (const mask_array<T>&);
    valarray<T>& operator= (const indirect_array<T>&);

    // operator[]

    T operator[] (size_t ind) const
    { return memory_array[ind];  }

    T& operator[] (size_t ind)
    { return memory_array[ind]; }

    valarray<T> operator[](slice) const;
    inline slice_array<T> operator[](slice);
    valarray<T> operator[](const gslice&) const;
    inline gslice_array<T> operator[](const gslice&);
    valarray<T> operator[](const valarray<bool>&) const;
    inline mask_array<T> operator[](const valarray<bool>&);
    valarray<T> operator[](const valarray<size_t>&) const;
    inline indirect_array<T> operator[](const valarray<size_t>&);

    // unary operators

    valarray<T> operator+() const;
    valarray<T> operator-() const;
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
    valarray<T> operator~() const;
    valarray<bool> operator!() const;
#endif

    // computed assignment

    valarray<T>& operator*= (const valarray<T>& array);
    valarray<T>& operator/= (const valarray<T>& array);
    valarray<T>& operator+= (const valarray<T>& array);
    valarray<T>& operator-= (const valarray<T>& array);
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
    valarray<T>& operator%= (const valarray<T>& array);
    valarray<T>& operator^= (const valarray<T>& array);
    valarray<T>& operator&= (const valarray<T>& array);
    valarray<T>& operator|= (const valarray<T>& array);
    valarray<T>& operator<<= (const valarray<T>& array);
    valarray<T>& operator>>= (const valarray<T>& array);
#endif
    valarray<T>& operator*= (const T& val);
    valarray<T>& operator/= (const T& val);
    valarray<T>& operator%= (const T& val);
    valarray<T>& operator+= (const T& val);
    valarray<T>& operator-= (const T& val);
    valarray<T>& operator^= (const T& val);
    valarray<T>& operator&= (const T& val);
    valarray<T>& operator|= (const T& val);
    valarray<T>& operator<<= (const T& val);
    valarray<T>& operator>>= (const T& val);
    // others

    size_t size() const { return memory_array._get_length(); }

    T sum() const;
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
    T min() const;
    T max() const;
#endif

    valarray<T> shift(int sh) const;
    valarray<T> cshift(int sh) const;

    valarray<T> apply(T func(T)) const;
    valarray<T> apply(T func(const T&)) const;

    void free()
    { memory_array._RW_resize_without_copy(0); }

    void resize(size_t sz, const T c= T() )
    {
      memory_array._RW_resize_without_copy(sz);
      *this = c;
    }

    // implementation specific

    const _RW_IMP_SPACE(_RW_array<T>)& _RW_get_memory_array( ) const
    { return memory_array; }

    _RW_IMP_SPACE(_RW_array<T>)* _RW_get_memory_array_adr( )
    { return &memory_array; }

    valarray(_RW_IMP_SPACE(_RW_temporary<T>)* tmp)
    {
      memory_array._replace(tmp->store_adr,tmp->length);
      delete tmp;
    }

  private:

    _RW_IMP_SPACE(_RW_array<T>)  memory_array;

  };

/*
 *
 *   VALARRAY NON MEMBER FUNCTIONS
 *
 */

  // binary operators

  template<class T>
  valarray<T> operator* (const valarray<T>& , const valarray<T>&  );
 
  template<class T>
  valarray<T> operator/ (const valarray<T>& , const valarray<T>& );
 
  template<class T>
  valarray<T> operator% (const valarray<T>&, const valarray<T>&);

  template<class T>
  valarray<T> operator+ (const valarray<T>& , const valarray<T>& );
 
  template<class T>
  valarray<T> operator- (const valarray<T>& , const valarray<T>& );
  
  template<class T>
  valarray<T> operator^ (const valarray<T>&, const valarray<T>&);

  template<class T>
  valarray<T> operator& (const valarray<T>&, const valarray<T>&);

  template<class T>
  valarray<T> operator| (const valarray<T>&, const valarray<T>&);

  template<class T>
  valarray<T> operator<< (const valarray<T>&, const valarray<T>&);

  template<class T>
  valarray<T> operator>> (const valarray<T>&, const valarray<T>&);

  template<class T>
  valarray<bool> operator&& (const valarray<T>&, const valarray<T>&);

  template<class T>
  valarray<bool> operator|| (const valarray<T>&, const valarray<T>&);

// with non array second param

  template<class T>
  valarray<T> operator* (const valarray<T>& , const T& );
 
  template<class T>
  valarray<T> operator/ (const valarray<T>& , const T& );
 
  template<class T>
  valarray<T> operator% (const valarray<T>&, const T&);

  template<class T>
  valarray<T> operator+ (const valarray<T>& , const T& );
 
  template<class T>
  valarray<T> operator- (const valarray<T>& , const T& );
  
  template<class T>
  valarray<T> operator^ (const valarray<T>&, const T&);

  template<class T>
  valarray<T> operator& (const valarray<T>&, const T&);

  template<class T>
  valarray<T> operator| (const valarray<T>&, const T&);

  template<class T>
  valarray<T> operator<< (const valarray<T>&, const T&);

  template<class T>
  valarray<T> operator>> (const valarray<T>&, const T&);

  template<class T>
  valarray<bool> operator&& (const valarray<T>&, const T&);

  template<class T>
  valarray<bool> operator|| (const valarray<T>&, const T&);
// with non array first param

  template<class T>
  valarray<T> operator* (const T& , const valarray<T>& ); 
 
  template<class T>
  valarray<T> operator/ (const T& , const valarray<T>& );
 
  template<class T>
  valarray<T> operator% (const T&, const valarray<T>&);

  template<class T>
  valarray<T> operator+ (const T& , const valarray<T>& );
 
  template<class T>
  valarray<T> operator- (const T& , const valarray<T>& );
 
  template<class T>
  valarray<T> operator^ (const T&, const valarray<T>&);

  template<class T>
  valarray<T> operator& (const T&, const valarray<T>&);

  template<class T>
  valarray<T> operator| (const T&, const valarray<T>&);

  template<class T>
  valarray<T> operator<< (const T&, const valarray<T>&);

  template<class T>
  valarray<T> operator>> (const T&, const valarray<T>&);

  template<class T>
  valarray<bool> operator&& (const T&, const valarray<T>&);

  template<class T>
  valarray<bool> operator|| (const T&, const valarray<T>&);

// comparison operators

  template<class T>
  valarray<bool> operator== (const valarray<T>& , const valarray<T>& );
  
  template<class T>
  valarray<bool> operator!= (const valarray<T>& , const valarray<T>& );
   
  template<class T>
  valarray<bool> operator< (const valarray<T>& , const valarray<T>& );
 
  template<class T>
  valarray<bool> operator> (const valarray<T>& , const valarray<T>& );
   
  template<class T>
  valarray<bool> operator<= (const valarray<T>& , const valarray<T>& ); 

  template<class T>
  valarray<bool> operator>= (const valarray<T>& , const valarray<T>& );
 
// comparison operators, non valarray second param

  template<class T>
  valarray<bool> operator== (const valarray<T>& , const T& );
 
  template<class T>
  valarray<bool> operator!= (const valarray<T>& , const T& );
 
  template<class T>
  valarray<bool> operator< (const valarray<T>& , const T& );

  template<class T>
  valarray<bool> operator> (const valarray<T>& , const T& );
 
  template<class T>
  valarray<bool> operator<= (const valarray<T>& , const T& );
  
  template<class T>
  valarray<bool> operator>= (const valarray<T>& , const T& );
 
// comparison operators, non valarray first param

  template<class T>
  valarray<bool> operator== (const T& , const valarray<T>& );
 
  template<class T>
  valarray<bool> operator!= (const T& , const valarray<T>& );
 
  template<class T>
  valarray<bool> operator< (const T& , const valarray<T>& );
 
  template<class T>
  valarray<bool> operator> (const T& , const valarray<T>& );
 
  template<class T>
  valarray<bool> operator<= (const T& , const valarray<T>& ); 
  
  template<class T>
  valarray<bool> operator>= (const T& , const valarray<T>& );
 
   
// transcendentals

  template<class T>
  valarray<T> abs(const valarray<T>& );

  template<class T>
  valarray<T> acos(const valarray<T>& );

  template<class T>
  valarray<T> asin(const valarray<T>& );

  template<class T>
  valarray<T> atan(const valarray<T>& );

  template<class T>
  valarray<T> cos(const valarray<T>& );

  template<class T>
  valarray<T> cosh(const valarray<T>& );

  template<class T>
  valarray<T> exp(const valarray<T>& );

  template<class T>
  valarray<T> log(const valarray<T>& );

  template<class T>
  valarray<T> log10(const valarray<T>& );

  template<class T>
  valarray<T> sinh(const valarray<T>& );

  template<class T>
  valarray<T> sin(const valarray<T>& );

  template<class T>
  valarray<T> sqrt(const valarray<T>& );

  template<class T>
  valarray<T> tan(const valarray<T>& );

  template<class T>
  valarray<T> tanh(const valarray<T>& );

  template<class T>
  valarray<T> atan2(const valarray<T>& , const valarray<T>& );

  template<class T>
  valarray<T> atan2(const valarray<T>& , const T& );

  template<class T>
  valarray<T> atan2(const T& , const valarray<T>& );

  template<class T>
  valarray<T> pow(const valarray<T>& , const valarray<T>& );

  template<class T>
  valarray<T> pow(const valarray<T>& , const T& );

  template<class T>
  valarray<T> pow(const T& , const valarray<T>& );

// Global min and max template fonction
// for compiler that try to instantiate all the member function

#ifdef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
  template <class T> T max(const valarray<T>&);
  template <class T> T min(const valarray<T>&);
#endif

/****************************************************************
 *                SLICE AND SLICE_ARRAY                         *
 ****************************************************************/

  class _RWSTDExport slice {

  public:

    slice()
      : start_(0), length_(0), stride_(0)
    {;}
    slice(size_t start, size_t length, size_t stride)
      : start_(start)
      , length_(length)
      , stride_(stride)
    {;}

    slice(const slice& sl)
      :start_(sl.start())
      ,length_(sl.size())
      ,stride_(sl.stride())
    {;}

    size_t start() const { return start_; }
    size_t size() const { return length_; }
    size_t stride() const { return stride_; }

  private:

    size_t start_;
    size_t length_;
    size_t stride_;

  };

  template <class T> class  slice_array {

  public:

    typedef T value_type;

    slice_array( _RW_IMP_SPACE(_RW_array<T>)* pt,const slice& a ) 
      :ref_mem_array(pt)
      ,slice_(a)
    {;}

    slice_array(const slice_array<T>& sl)
      :ref_mem_array(sl.get_ref_mem_array())
      ,slice_(sl.get_slice())
    {;}

    _RW_IMP_SPACE(_RW_array<T>)* get_ref_mem_array() const { return ref_mem_array; }
    slice get_slice() const { return slice_; }

    // assignment

    void operator= (const valarray<T>& ) const;
    void operator= (const T&) const;

    // computed assignment

    void operator*= (const valarray<T>& ) const;
    void operator/= (const valarray<T>& ) const;
    void operator+= (const valarray<T>& ) const;
    void operator-= (const valarray<T>& ) const;
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
    void operator%= (const valarray<T>& ) const;
    void operator^= (const valarray<T>& ) const;
    void operator&= (const valarray<T>& ) const;
    void operator|= (const valarray<T>& ) const;
    void operator<<= (const valarray<T>& ) const;
    void operator>>= (const valarray<T>& ) const;
#endif

  private:

    slice_array();
    slice_array<T>& operator= (const slice_array<T>&);

    _RW_IMP_SPACE(_RW_array<T>)*  ref_mem_array;
    slice slice_;
    
  };
/****************************************************************
 *                GSLICE AND GSLICE_ARRAY                       *
 ****************************************************************/

  class _RWSTDExport gslice {

  public:

    gslice()
      : start_(0)
      , reset_(true)
    {;}
         

    gslice(size_t s, const valarray<size_t>& l, const valarray<size_t>& d)
      : start_(s)
      , length_(l)
      , stride_(d)
      , reset_(true)
      , r_length_((size_t)0,l.size())
    {;}

    gslice(const gslice& sl) 
      : start_(sl.start())
      , length_(sl.size())
      , stride_(sl.stride())
      , reset_(true)
      , r_length_((size_t)0,sl.size().size())
    {;}

    size_t start() const { return start_; }
    valarray<size_t> size() const { return length_; }
    valarray<size_t> stride() const { return stride_; }

    size_t next_ind();
    inline size_t is_reseted() const { return reset_; }
    size_t ind_max() const;
    size_t ind_numb() const;

  private:

    size_t start_;
    valarray<size_t> length_;
    valarray<size_t> stride_;

    bool reset_;
    valarray<size_t>  r_length_;

  };
// class gslice_array

  template <class T> class gslice_array {
  public:

    typedef T value_type;

    gslice_array( _RW_IMP_SPACE(_RW_array<T>)* pt,const gslice& a ) 
      :ref_mem_array(pt)
      ,slice_(a)
    {;}

    gslice_array(const gslice_array<T>& sl)
      :ref_mem_array(sl.get_ref_mem_array())
      ,slice_(sl.get_slice())
    {;}

    _RW_IMP_SPACE(_RW_array<T>)* get_ref_mem_array() const { return ref_mem_array; }
    gslice get_slice() const { return slice_; }

    // assignment

    void operator= (const valarray<T>& ) const;
    void operator= (const T&) const;

    // computed assignment

    void operator*= (const valarray<T>& ) const;
    void operator/= (const valarray<T>& ) const;
    void operator+= (const valarray<T>& ) const;
    void operator-= (const valarray<T>& ) const;
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
    void operator%= (const valarray<T>& ) const;
    void operator^= (const valarray<T>& ) const;
    void operator&= (const valarray<T>& ) const;         
    void operator|= (const valarray<T>& ) const;         
    void operator<<= (const valarray<T>& ) const;        
    void operator>>= (const valarray<T>& ) const;
#endif

  private:

    gslice_array();
    gslice_array<T>& operator= (const gslice_array<T>&);

    _RW_IMP_SPACE(_RW_array<T>)*  ref_mem_array;
    gslice slice_;
    
  };
 

/****************************************************************
 *                         MASK_ARRAY                           *
 ****************************************************************/
// class mask_array

  template <class T> class  mask_array {
  public:

    typedef T value_type;

    mask_array( _RW_IMP_SPACE(_RW_array<T>)* pt,const valarray<bool>& a )
      :ref_mem_array(pt)
      ,array(a)
    {;}

    mask_array(const mask_array<T>& sl)
      :ref_mem_array(sl.get_ref_mem_array())
      ,array(sl.get_array())
    {;}

    _RW_IMP_SPACE(_RW_array<T>)* get_ref_mem_array() const { return ref_mem_array; }
    valarray<bool> get_array() const { return array; }
    valarray<bool>* get_array_pt() { return &array; }

    // assignment

    void operator= (const valarray<T>& ) const;
    void operator= (const T&) const;

    // computed assignment

    void operator*= (const valarray<T>& ) const;
    void operator/= (const valarray<T>& ) const;
    void operator+= (const valarray<T>& ) const;
    void operator-= (const valarray<T>& ) const;
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
    void operator%= (const valarray<T>& ) const;
    void operator^= (const valarray<T>& ) const;
    void operator&= (const valarray<T>& ) const;
    void operator|= (const valarray<T>& ) const;
    void operator<<= (const valarray<T>& ) const;
    void operator>>= (const valarray<T>& ) const;
#endif

  private:

    mask_array();
    mask_array<T>& operator= (const mask_array<T>&); 

    _RW_IMP_SPACE(_RW_array<T>)*  ref_mem_array;
    valarray<bool> array;
    
  };
 
  
/****************************************************************
 *                       INDIRECT_ARRAY                         *
 ****************************************************************/
// class indirect_array

  template <class T> class  indirect_array {
  public:

    typedef T value_type;

    indirect_array( _RW_IMP_SPACE(_RW_array<T>)* pt,const valarray<size_t>& a ) 
      :ref_mem_array(pt)
      ,array(a)
    {;}

    indirect_array(const indirect_array<T>& sl)
      :ref_mem_array(sl.get_ref_mem_array())
      ,array(sl.get_array())
    {;}

    _RW_IMP_SPACE(_RW_array<T>)* get_ref_mem_array() const { return ref_mem_array; }
    valarray<size_t> get_array() const { return array; }
    valarray<size_t>* get_array_pt() { return &array; }

    // assignment

    void operator= (const valarray<T>& ) const;
    void operator= (const T& ) const;

    // computed assignment

    void operator*= (const valarray<T>& ) const;
    void operator/= (const valarray<T>& ) const;
    void operator+= (const valarray<T>& ) const;
    void operator-= (const valarray<T>& ) const;
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
    void operator%= (const valarray<T>& ) const;
    void operator^= (const valarray<T>& ) const;
    void operator&= (const valarray<T>& ) const;
    void operator|= (const valarray<T>& ) const;
    void operator<<= (const valarray<T>& ) const;
    void operator>>= (const valarray<T>& ) const;
#endif

  private:

    indirect_array();
    indirect_array<T>& operator= (const indirect_array<T>&); 

    _RW_IMP_SPACE(_RW_array<T>)*  ref_mem_array;
    valarray<size_t> array;

  };

/*
 *   VALARRAY INLINE MEMBER FUNCTIONS
 */
  template <class T>
  inline valarray<T>& valarray<T>::operator=(const T& value)
  {
    memory_array._initialize_with_value(value, size());
    return *this;
  }

// operator[] for slice
  
  template <class T>
  inline slice_array<T> valarray<T>::operator[](slice sl)
  {
    return slice_array<T>(&memory_array, sl);
  }

  // operator[] for gslice

     
  template <class T>
  inline gslice_array<T> valarray<T>::operator[](const gslice& sl)
  {
    return gslice_array<T>(&memory_array, sl);
  }
// operator[] for valarray[valarray<bool>] used with mask_array

  
  template <class T>
  inline mask_array<T> valarray<T>::operator[](const valarray<bool>& array)
  {
    return mask_array<T>(&memory_array, array);
  }
// operator[] for valarray[valarray<size_t>] used with indirect_array
  template <class T>
  inline indirect_array<T> valarray<T>::operator[](const valarray<size_t>& array)
  {
    return indirect_array<T>(&memory_array, array);
  }

#ifndef _RWSTD_NO_NAMESPACE
}
#endif

#ifdef _RWSTD_NO_TEMPLATE_REPOSITORY
#include <valarray.cc>
#endif
#endif /* __VALARRAY__ */

#ifndef __USING_STD_NAMES__
  using namespace std;
#endif

#pragma option pop
#endif /* __VALARRAY_H */
