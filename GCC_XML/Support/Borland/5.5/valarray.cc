#ifndef __VALARRAY_CC
#define __VALARRAY_CC
#pragma option push -b -a8 -pc -Vx- -Ve- -w-inl -w-aus -w-sig

/***************************************************************************
 *
 * valaray.cc - Declarations for the Standard Library valarray
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

#if defined ( __SUNPRO_CC ) || defined (__EDG__)
#include<stdlib.h>
#endif

#ifndef _RWSTD_NO_NAMESPACE
namespace std {
#endif

#if !defined (_RWSTD_NO_NAMESPACE) && defined (_RWSTD_NO_NEW_HEADER)
using ::abs;
using ::cos;
using ::sin;
using ::tan;
using ::asin;
using ::acos;
using ::atan;
using ::atan2;
using ::sinh;
using ::cosh;
using ::tanh;
using ::exp;
using ::log;
using ::log10;
using ::sqrt;
using ::pow;
#endif

/*****************************************************************
 *                                                                *
 *                 VALARRAY MEMBER FUNCTIONS                      *
 *                                                                *
 ******************************************************************/
// unary operators

  template<class T>
  valarray<T> valarray<T>::operator+() const
  {
    valarray<T> tmp_array(size());
      
    for(size_t ind=0; ind< size(); ind++ )
      tmp_array[ind] = +memory_array[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template <class T>
  valarray<T> valarray<T>::operator-() const
  {
    valarray<T> tmp_array(size());
      
    for(size_t ind=0; ind< size(); ind++ )
      tmp_array[ind] = -memory_array[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
  template <class T>
  valarray<T> valarray<T>::operator~() const
  {
    valarray<T> tmp_array(size());

    for(size_t ind=0; ind< size(); ind++ )
      tmp_array[ind] = ~memory_array[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template <class T>
  valarray<bool> valarray<T>::operator!() const
  {
    valarray<bool> tmp_array(size());

    for(size_t ind=0; ind< size(); ind++ )
      tmp_array[ind] = !memory_array[ind];

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }
#endif

// computed assignment

  template <class T>
  valarray<T>& valarray<T>::operator*= (const valarray<T>& array)
  {
    size_t upper_l = ( size() < array.size() ) ? size() : array.size();

    for(size_t ind=0; ind < upper_l; ind++)
      memory_array[ind]*= array[ind];

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator/= (const valarray<T>& array)
  {
    size_t upper_l = ( size() < array.size() ) ? size() : array.size();

    for(size_t ind=0; ind < upper_l; ind++)
      memory_array[ind]/= array[ind];

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator+= (const valarray<T>& array)
  {
    size_t upper_l = ( size() < array.size() ) ? size() : array.size();

    for(size_t ind=0; ind < upper_l; ind++)
      memory_array[ind]+= array[ind];

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator-= (const valarray<T>& array)
  {
    size_t upper_l = ( size() < array.size() ) ? size() : array.size();

    for(size_t ind=0; ind < upper_l; ind++)
      memory_array[ind]-= array[ind];

    return *this;
  }

#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
  template <class T>
  valarray<T>& valarray<T>::operator%= (const valarray<T>& array)
  {
    size_t upper_l = ( size() < array.size() ) ? size() : array.size();

    for(size_t ind=0; ind < upper_l; ind++)
      memory_array[ind]%= array[ind];

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator^= (const valarray<T>& array)
  {
    size_t upper_l = ( size() < array.size() ) ? size() : array.size();

    for(size_t ind=0; ind < upper_l; ind++)
      memory_array[ind]^= array[ind];

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator&= (const valarray<T>& array)
  {
    size_t upper_l = ( size() < array.size() ) ? size() : array.size();

    for(size_t ind=0; ind < upper_l; ind++)
      memory_array[ind]&= array[ind];

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator|= (const valarray<T>& array)
  {
    size_t upper_l = ( size() < array.size() ) ? size() : array.size();

    for(size_t ind=0; ind < upper_l; ind++)
      memory_array[ind]|= array[ind];

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator<<= (const valarray<T>& array)
  {
    size_t upper_l = ( size() < array.size() ) ? size() : array.size();

    for(size_t ind=0; ind < upper_l; ind++)
      memory_array[ind]<<= array[ind];

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator>>= (const valarray<T>& array)
  {
    size_t upper_l = ( size() < array.size() ) ? size() : array.size();

    for(size_t ind=0; ind < upper_l; ind++)
      memory_array[ind]>>= array[ind];

    return *this;
  }
#endif

  template <class T>
  valarray<T>& valarray<T>::operator*= (const T& val)
  {
    for(size_t ind=0; ind < size(); ind++)
      memory_array[ind]*= val;

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator/= (const T& val)
  {
    for(size_t ind=0; ind < size(); ind++)
      memory_array[ind]/= val;

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator+= (const T& val)
  {
    for(size_t ind=0; ind < size(); ind++)
      memory_array[ind]+= val;

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator-= (const T& val)
  {
    for(size_t ind=0; ind < size(); ind++)
      memory_array[ind]-= val;

    return *this;
  }

#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
  template <class T>
  valarray<T>& valarray<T>::operator%= (const T& val)
  {
    for(size_t ind=0; ind < size(); ind++)
      memory_array[ind]%= val;

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator^= (const T& val)
  {
    for(size_t ind=0; ind < size(); ind++)
      memory_array[ind]^= val;

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator&= (const T& val)
  {
    for(size_t ind=0; ind < size(); ind++)
      memory_array[ind]&= val;

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator|= (const T& val)
  {
    for(size_t ind=0; ind < size(); ind++)
      memory_array[ind]|= val;

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator<<= (const T& val)
  {
    for(size_t ind=0; ind < size(); ind++)
      memory_array[ind]<<= val;

    return *this;
  }

  template <class T>
  valarray<T>& valarray<T>::operator>>= (const T& val)
  {
    for(size_t ind=0; ind < size(); ind++)
      memory_array[ind]>>= val;

    return *this;
  }
#endif

// other valarray member functions

  template <class T>
  T valarray<T>::sum() const
  {
    T tmp;
    if ( size() > 0 )
    {
      tmp = memory_array[0];
      for(size_t ind=1; ind<size(); ind++)
        tmp+= memory_array[ind];
    }

    return tmp;
  }

  template <class T>
  valarray<T> valarray<T>::shift(int sh) const
  {
    valarray<T> tmp_array(static_cast<T>(T()),size());
      
    int right=0;
    int left=0;

    if ( sh < 0 ) right = -sh;
    else left = sh;

    for(size_t ind=left; ind< (size()-right); ind++ )
      tmp_array[ind+right-left] = memory_array[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  } 

  template <class T>
  valarray<T> valarray<T>::cshift(int sh) const
  {
    valarray<T> tmp_array(static_cast<T>(T()),size());
      
    if ( sh >= 0 )
    {
      for(size_t ind=0; ind< size(); ind++ )
        tmp_array[ind] = memory_array[(ind+sh)%size()];
    }
    else
    {
      for(size_t ind=size()+sh; ind< (2*size()+sh); ind++ )
        tmp_array[ind-size()-sh] = memory_array[ind%size()];
    }

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  } 

  template <class T>
  valarray<T> valarray<T>::apply(T func(T)) const
  {
    valarray<T> tmp_array(size());
      
    for(size_t ind=0; ind< size(); ind++ )
      tmp_array[ind] = func(memory_array[ind]);

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  } 

  template <class T>
  valarray<T> valarray<T>::apply(T func(const T&)) const
  {
    valarray<T> tmp_array(size());
      
    for(size_t ind=0; ind< size(); ind++ )
      tmp_array[ind] = func(memory_array[ind]);

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  } 

// operator[] for slice

  template <class T> 
  valarray<T> valarray<T>::operator[](slice sl) const
  {
    valarray<T> tmp_array(sl.size());
      
    size_t ind = sl.start();
    size_t cpt = 0; 

    while( cpt < sl.size() )
    {
      tmp_array[cpt] = memory_array[ind];
      ind+= sl.stride();
      cpt++;
    }

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = sl.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

// copy ctor and assignment for slice

  template <class T>
  valarray<T>::valarray(const slice_array<T>& sl_ar)
  {
    valarray<T> tmp_array(sl_ar.get_slice().size());
      
    size_t ind = sl_ar.get_slice().start();
    size_t cpt = 0; 

    while( cpt < sl_ar.get_slice().size() )
    {
      tmp_array[cpt] = (*(sl_ar.get_ref_mem_array()))[ind];
      ind+= sl_ar.get_slice().stride();
      cpt++;
    }
    memory_array._replace(tmp_array._RW_get_memory_array()._RW_get_storage(),sl_ar.get_slice().size());

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();   
  }
   
  template <class T>
  valarray<T>& valarray<T>::operator= (const slice_array<T>& sl_ar)
  { 
    valarray<T> tmp_array(sl_ar.get_slice().size());
      
    size_t ind = sl_ar.get_slice().start();
    size_t cpt = 0; 

    while( cpt < sl_ar.get_slice().size() )
    {
      tmp_array[cpt] = (*(sl_ar.get_ref_mem_array()))[ind];
      ind+= sl_ar.get_slice().stride();
      cpt++;
    }

    if ( &memory_array == sl_ar.get_ref_mem_array() )
      memory_array._RW_resize_without_copy(0); 

    memory_array._replace(tmp_array._RW_get_memory_array()._RW_get_storage(),sl_ar.get_slice().size());

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return *this;
  }

  // operator[] for gslice

  template <class T> 
  valarray<T> valarray<T>::operator[](const gslice& sl) const
  {
    valarray<T> tmp_array(sl.ind_numb());
      
    gslice *gsl = (gslice *)&sl;

    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( !sl.is_reseted() )
    {
      tmp_array[cpt] = memory_array[ind];
      ind= gsl->next_ind();
      cpt++;
    }

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = tmp_array.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

// copy ctor and assignment for gslice

  template <class T>
  valarray<T>::valarray(const gslice_array<T>& sl_ar)
  {
    gslice sl(sl_ar.get_slice());
    valarray<T> tmp_array(sl.ind_numb());

    size_t ind = sl.next_ind();
    size_t cpt = 0;

    while( !sl.is_reseted() )
    {
      tmp_array[cpt] = (*(sl_ar.get_ref_mem_array()))[ind];
      ind= sl.next_ind();
      cpt++;
    }

    memory_array._replace(tmp_array._RW_get_memory_array()._RW_get_storage(),tmp_array.size());

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();   
  }

  template <class T>
  valarray<T>& valarray<T>::operator= (const gslice_array<T>& sl_ar)
  { 
    gslice sl(sl_ar.get_slice());
    valarray<T> tmp_array(sl.ind_numb());

    size_t ind = sl.next_ind();
    size_t cpt = 0;

    while( !sl.is_reseted() )
    {
      tmp_array[cpt] = (*(sl_ar.get_ref_mem_array()))[ind];
      ind= sl.next_ind();
      cpt++;
    }

    if ( &memory_array == sl_ar.get_ref_mem_array() )
      memory_array._RW_resize_without_copy(0); 

    memory_array._replace(tmp_array._RW_get_memory_array()._RW_get_storage(),tmp_array.size());

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return *this;
  }
// operator[] for valarray[valarray<bool>] used with mask_array
  template <class T> 
  valarray<T> valarray<T>::operator[](const valarray<bool>& array) const
  {
    size_t iter,size =0;

    for(iter=0; iter < array.size(); iter++ )
      if ( array[iter] ) size++;

    valarray<T> tmp_array(size);
      
    size_t cpt = 0; 

    for( iter=0; iter < array.size(); iter++ )
      if ( array[iter] ) tmp_array[cpt++] = memory_array[iter];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = size;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

// copy ctor and assignment for mask_array

  template <class T>
  valarray<T>::valarray(const mask_array<T>& mask)
  {

    mask_array<T> *msk = (mask_array<T> *)&mask;
    valarray<bool>* sec = msk->get_array_pt();

    size_t iter,size =0;

    for(iter=0; iter < sec->size(); iter++ )
      if ( (*sec)[iter] ) size++;

    valarray<T> tmp_array(size);
      
    size_t cpt = 0; 

    for( iter=0; iter < sec->size(); iter++ )
      if ( (*sec)[iter] ) tmp_array[cpt++] = (*(mask.get_ref_mem_array()))[iter];

    memory_array._replace(tmp_array._RW_get_memory_array()._RW_get_storage(),size);

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();   
  }

  template <class T>
  valarray<T>& valarray<T>::operator= (const mask_array<T>& mask)
  { 
    mask_array<T> *msk = (mask_array<T> *)&mask;
    valarray<bool>* sec = msk->get_array_pt();

    size_t iter,size =0;

    for(iter=0; iter < sec->size(); iter++ )
      if ( (*sec)[iter] ) size++;

    valarray<T> tmp_array(size);
      
    size_t cpt = 0; 

    for( iter=0; iter < sec->size(); iter++ )
      if ( (*sec)[iter] ) tmp_array[cpt++] = (*(mask.get_ref_mem_array()))[iter];

    if ( &memory_array == mask.get_ref_mem_array() )
      memory_array._RW_resize_without_copy(0); 

    memory_array._replace(tmp_array._RW_get_memory_array()._RW_get_storage(),size);

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return *this;
  }

// operator[] for valarray[valarray<size_t>] used with indirect_array

  template <class T> 
  valarray<T> valarray<T>::operator[](const valarray<size_t>& array) const
  {
    valarray<T> tmp_array(array.size());

    for( size_t iter=0; iter < array.size(); iter++ )
      tmp_array[iter] = memory_array[array[iter]];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = array.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

// copy ctor and assignment for indirect_array

  template <class T>
  valarray<T>::valarray(const indirect_array<T>& indir)
  {

    indirect_array<T> *indr= (indirect_array<T> *)&indir;
    valarray<size_t>* sec = indr->get_array_pt();

    valarray<T> tmp_array(sec->size());
      
    size_t cpt = 0; 

    for(size_t iter=0; iter < sec->size(); iter++ )
      tmp_array[cpt++] = (*(indir.get_ref_mem_array()))[(*sec)[iter]];

    memory_array._replace(tmp_array._RW_get_memory_array()._RW_get_storage(),sec->size());

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();   
  }
  template <class T>
  valarray<T>& valarray<T>::operator= (const indirect_array<T>& indir)
  { 
    indirect_array<T> *indr= (indirect_array<T> *)&indir;
    valarray<size_t>* sec = indr->get_array_pt();

    valarray<T> tmp_array(sec->size());
      
    size_t cpt = 0; 

    for(size_t iter=0; iter < sec->size(); iter++ )
      tmp_array[cpt++] = (*(indir.get_ref_mem_array()))[(*sec)[iter]];

    if ( &memory_array == indir.get_ref_mem_array() )
      memory_array._RW_resize_without_copy(0); 

    memory_array._replace(tmp_array._RW_get_memory_array()._RW_get_storage(),sec->size());

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return *this;
  }
/*
 *
 *   VALARRAY NON MEMBER FUNCTIONS
 *
 */
  template<class T>
  valarray<T> operator* (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]*b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator/ (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]/b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator% (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]%b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator+ (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]+b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator- (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]-b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }
  template<class T>
  valarray<T> operator^ (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]^b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator& (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]&b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator| (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]|b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator<< (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]<<b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator>> (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]>>b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator&& (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind] && b[ind];

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator|| (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind] || b[ind];

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

// with non array second parameter

  template<class T>
  valarray<T> operator* (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]*b;

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator/ (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]/b;

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator% (const valarray<T>& a,const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]%b;

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator+ (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]+b;

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator- (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]-b;

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }
  template<class T>
  valarray<T> operator^ (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]^b;

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator& (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]&b;

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator| (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]|b;

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator<< (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]<<b;

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator>> (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind]>>b;

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator&& (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind] && b;

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator|| (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a[ind] || b;

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

// with non array first parameter

  template<class T>
  valarray<T> operator* (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a*b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator/ (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a/b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator% (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a%b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator+ (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a+b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator- (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a-b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }
  template<class T>
  valarray<T> operator^ (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a^b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator& (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a&b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator| (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a|b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator<< (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a<<b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> operator>> (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a>>b[ind];

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator&& (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a && b[ind];

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator|| (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = a || b[ind];

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

// comparison operators

  template<class T>
  valarray<bool> operator== (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]==b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator!= (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]!=b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator< (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]<b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator> (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]>b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator<= (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]<=b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator>= (const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]>=b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

// comparison operators, non valarray second param

  template<class T>
  valarray<bool> operator== (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]==b);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator!= (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]!=b);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator< (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]<b);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator> (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]>b);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator<= (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]<=b);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator>= (const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a[ind]>=b);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

// comparison operators, non valarray first param

  template<class T>
  valarray<bool> operator== (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a==b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator!= (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a!=b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator< (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a<b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator> (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a>b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator<= (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a<=b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<bool> operator>= (const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<bool> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = (a>=b[ind]);

    _RW_IMP_SPACE(_RW_temporary<bool>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<bool>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
  // min and max functions

  template <class T>
  T valarray<T>::min()const
  {
    T tmp;
    if ( size() > 0 )
    {
      tmp = memory_array[0];
      for(size_t ind=1; ind< size(); ind++)
        if ( memory_array[ind] < tmp )
          tmp= memory_array[ind];
    }

    return tmp;
  }

  template <class T>
  T valarray<T>::max()const
  {
    T tmp;
    if ( size() > 0 )
    {
      tmp = memory_array[0];
      for(size_t ind=1; ind< size(); ind++)
        if ( memory_array[ind] > tmp )
          tmp= memory_array[ind];
    }

    return tmp;
  }
#else
  template <class T>
  T min(const valarray<T>& ar)
  {
    T tmp;
    if ( ar.size() > 0 )
    {
      tmp = ar[0];
      for(size_t ind=1; ind< ar.size(); ind++)
        if ( ar[ind] < tmp )
          tmp= ar[ind];
    }

    return tmp;
  }

  template <class T>
  T max(const valarray<T>& ar)
  {
    T tmp;
    if ( ar.size() > 0 )
    {
      tmp = ar[0];
      for(size_t ind=1; ind< ar.size(); ind++)
        if ( ar[ind] > tmp )
          tmp= ar[ind];
    }

    return tmp;
  }
#endif   
// transcendentals
  template<class T>
  valarray<T> abs(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = abs(a[ind]);             

    // note: abs need to be overloaded for 
    // float, long double and long see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> acos(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = acos(a[ind]);             

    // note: acos need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> asin(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = asin(a[ind]);             

    // note: asin need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> atan(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = atan(a[ind]);             

    // note: atan need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> cos(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = cos(a[ind]);

    // note: cos need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> cosh(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = cosh(a[ind]);             

    // note: cosh need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> exp(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = exp(a[ind]);             

    // note: exp need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> log(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = log(a[ind]);             

    // note: log need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> log10(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = log10(a[ind]);             

    // note: log10 need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> sinh(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = sinh(a[ind]);             

    // note: sinh need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> sin(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = sin(a[ind]);             

    // note: sin need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> sqrt(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = sqrt(a[ind]);             

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> tan(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = tan(a[ind]);             

    // note: tan need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }
  template<class T>
  valarray<T> tanh(const valarray<T>& a)
  {
    valarray<T> tmp_array(a.size());

    for(size_t ind=0; ind< a.size(); ind++ )
      tmp_array[ind] = tanh(a[ind]);             

    // note: tanh need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = a.size();

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> atan2(const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = atan2(a[ind],b[ind]);             

    // note: atan2 need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> atan2(const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = atan2(a[ind],b);             

    // note: atan2 need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> atan2(const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = atan2(a,b[ind]);             

    // note: atan2 need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> pow(const valarray<T>& a, const valarray<T>& b)
  {
    size_t length= ( a.size() > b.size() ) ? b.size() : a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = pow(a[ind],b[ind]);             

    // note: pow need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> pow(const valarray<T>& a, const T& b)
  {
    size_t length= a.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = pow(a[ind],b);             

    // note: pow need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }

  template<class T>
  valarray<T> pow(const T& a, const valarray<T>& b)
  {
    size_t length= b.size();
    valarray<T> tmp_array(length);

    for(size_t ind=0; ind< length; ind++ )
      tmp_array[ind] = pow(a,b[ind]);             

    // note: pow need to be overloaded for 
    // float, long double see (17.3.1.1)

    _RW_IMP_SPACE(_RW_temporary<T>)* _tmp_ret = new _RW_IMP_SPACE(_RW_temporary<T>);

    _tmp_ret->store_adr = tmp_array._RW_get_memory_array()._RW_get_storage();
    _tmp_ret->length = length;

    tmp_array._RW_get_memory_array_adr()->_RW_invalidate();

    return _tmp_ret;
  }
/*****************************************************************
 *                                                                *
 *                 SLICE_ARRAY MEMBER FUNCTIONS                   *
 *                                                                *
 ******************************************************************/

// slice_array inline member functions

  template <class T>
  void slice_array<T>::operator= (const valarray<T>& array) const
  { 
    size_t ind = slice_.start();
    size_t cpt = 0; 

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] = array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }

  template <class T>
  void slice_array<T>::operator= (const T& value) const
  { 
    size_t ind = slice_.start();
    size_t cpt = 0; 

    while( cpt < slice_.size() )
    {
      if ( ind<ref_mem_array->_get_length() )
        (*ref_mem_array)[ind] = value;
      ind+= slice_.stride();
      cpt++;
    }
  }
// computed assignment

  template <class T>
  void slice_array<T>::operator*= (const valarray<T>& array) const
  { 
    size_t ind = slice_.start();
    size_t cpt = 0; 

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] *= array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }

  template <class T>
  void slice_array<T>::operator/= (const valarray<T>& array) const
  { 
    size_t ind = slice_.start();
    size_t cpt = 0; 

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] /= array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }

  template <class T>
  void slice_array<T>::operator+= (const valarray<T>& array) const
  {
    size_t ind = slice_.start();
    size_t cpt = 0;

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] += array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }

  template <class T>
  void slice_array<T>::operator-= (const valarray<T>& array) const
  {
    size_t ind = slice_.start();
    size_t cpt = 0;

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] -= array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }

#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
  template <class T>
  void slice_array<T>::operator%= (const valarray<T>& array) const
  {
    size_t ind = slice_.start();
    size_t cpt = 0;

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] %= array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }

  template <class T>
  void slice_array<T>::operator^= (const valarray<T>& array) const
  {
    size_t ind = slice_.start();
    size_t cpt = 0; 

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] ^= array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }

  template <class T>
  void slice_array<T>::operator&= (const valarray<T>& array) const
  { 
    size_t ind = slice_.start();
    size_t cpt = 0; 

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] &= array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }

  template <class T>
  void slice_array<T>::operator|= (const valarray<T>& array) const
  { 
    size_t ind = slice_.start();
    size_t cpt = 0; 

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] |= array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }

  template <class T>
  void slice_array<T>::operator<<= (const valarray<T>& array) const
  { 
    size_t ind = slice_.start();
    size_t cpt = 0; 

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] <<= array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }

  template <class T>
  void slice_array<T>::operator>>= (const valarray<T>& array) const
  { 
    size_t ind = slice_.start();
    size_t cpt = 0; 

    while( cpt < slice_.size() )
    {
      if ( (cpt<array.size()) && (ind<ref_mem_array->_get_length()) )
        (*ref_mem_array)[ind] >>= array[cpt];
      ind+= slice_.stride();
      cpt++;
    }
  }
#endif
/*****************************************************************
 *                                                                *
 *                 GSLICE_ARRAY MEMBER FUNCTIONS                  *
 *                                                                *
 ******************************************************************/
// gslice_array inline member functions

  template <class T>
  void gslice_array<T>::operator= (const valarray<T>& array) const
  {
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] = array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }

  template <class T>
  void gslice_array<T>::operator= (const T& value) const
  { 
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();

    while( !gsl->is_reseted() )
    {
      (*ref_mem_array)[ind] = value;
      ind= gsl->next_ind();
    }
  }
// computed assignment

  template <class T>
  void gslice_array<T>::operator*= (const valarray<T>& array) const
  {
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] *= array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }

  template <class T>
  void gslice_array<T>::operator/= (const valarray<T>& array) const
  { 
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] /= array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }

  template <class T>
  void gslice_array<T>::operator+= (const valarray<T>& array) const
  { 
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] += array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }

  template <class T>
  void gslice_array<T>::operator-= (const valarray<T>& array) const
  { 
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] -= array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }

#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION
  template <class T>
  void gslice_array<T>::operator%= (const valarray<T>& array) const
  { 
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] %= array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }

  template <class T>
  void gslice_array<T>::operator^= (const valarray<T>& array) const
  { 
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] ^= array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }

  template <class T>
  void gslice_array<T>::operator&= (const valarray<T>& array) const
  { 
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] &= array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }

  template <class T>
  void gslice_array<T>::operator|= (const valarray<T>& array) const
  { 
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] |= array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }

  template <class T>
  void gslice_array<T>::operator<<= (const valarray<T>& array) const
  { 
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] <<= array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }

  template <class T>
  void gslice_array<T>::operator>>= (const valarray<T>& array) const
  { 
    gslice *gsl = (gslice *)&slice_;
    size_t ind = gsl->next_ind();
    size_t cpt = 0;

    while( (!gsl->is_reseted()) && (cpt < array.size()) )
    {
      (*ref_mem_array)[ind] >>= array[cpt];
      ind= gsl->next_ind();
      cpt++;
    }
  }
#endif
/*****************************************************************
 *                                                                *
 *                 MASK_ARRAY MEMBER FUNCTIONS                    *
 *                                                                *
 ******************************************************************/

// mask_array inline member functions

  template <class T>
  void mask_array<T>::operator= (const valarray<T>& ar) const
  {  
    size_t cpt = 0; 

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] ) 
        (*ref_mem_array)[iter]= ar[cpt++];
  }

  template <class T>
  void mask_array<T>::operator= (const T& value) const
  {
    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] )
        (*ref_mem_array)[iter]= value;
  }
  template <class T>
  void mask_array<T>::operator*= (const valarray<T>& ar) const
  {  
    size_t cpt = 0; 

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] ) 
        (*ref_mem_array)[iter] *= ar[cpt++];
  }

  template <class T>
  void mask_array<T>::operator/= (const valarray<T>& ar) const
  {  
    size_t cpt = 0; 

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] ) 
        (*ref_mem_array)[iter]/= ar[cpt++];
  }

  template <class T>
  void mask_array<T>::operator+= (const valarray<T>& ar) const
  {  
    size_t cpt = 0; 

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] )
        (*ref_mem_array)[iter]+= ar[cpt++];
  }

  template <class T>
  void mask_array<T>::operator-= (const valarray<T>& ar) const
  {  
    size_t cpt = 0;

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] )
        (*ref_mem_array)[iter]-= ar[cpt++];
  }
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION

  template <class T>
  void mask_array<T>::operator%= (const valarray<T>& ar) const
  {
    size_t cpt = 0;

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] )
        (*ref_mem_array)[iter]%= ar[cpt++];
  }

  template <class T>
  void mask_array<T>::operator^= (const valarray<T>& ar) const
  {
    size_t cpt = 0;

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] )
        (*ref_mem_array)[iter]^= ar[cpt++];
  }

  template <class T>
  void mask_array<T>::operator&= (const valarray<T>& ar) const
  {
    size_t cpt = 0;

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] )
        (*ref_mem_array)[iter]&= ar[cpt++];
  }

  template <class T>
  void mask_array<T>::operator|= (const valarray<T>& ar) const
  {
    size_t cpt = 0;

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] )
        (*ref_mem_array)[iter]|= ar[cpt++];
  }

  template <class T>
  void mask_array<T>::operator<<= (const valarray<T>& ar) const
  {
    size_t cpt = 0;

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] )
        (*ref_mem_array)[iter]<<= ar[cpt++];
  }

  template <class T>
  void mask_array<T>::operator>>= (const valarray<T>& ar) const
  {
    size_t cpt = 0;

    for(size_t iter=0; iter < array.size(); iter++ )
      if ( array[iter] )
        (*ref_mem_array)[iter]>>= ar[cpt++];
  }
#endif

/*****************************************************************
 *                                                                *
 *                 INDIRECT_ARRAY MEMBER FUNCTIONS                *
 *                                                                *
 ******************************************************************/

// indirect_array inline member functions

  template <class T>
  void indirect_array<T>::operator= (const valarray<T>& ar) const
  {
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] = ar[cpt++];
  }

  template <class T>
  void indirect_array<T>::operator= (const T& value) const
  {
    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] = value;
  }
  template <class T>
  void indirect_array<T>::operator*= (const valarray<T>& ar) const
  {
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] *= ar[cpt++];
  }

  template <class T>
  void indirect_array<T>::operator/= (const valarray<T>& ar) const
  {
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] /= ar[cpt++];
  }

  template <class T>
  void indirect_array<T>::operator+= (const valarray<T>& ar) const
  {
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] += ar[cpt++];
  }

  template <class T>
  void indirect_array<T>::operator-= (const valarray<T>& ar) const
  {
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] -= ar[cpt++];
  }
#ifndef _RWSTD_NO_ONLY_NEEDED_INSTANTIATION

  template <class T>
  void indirect_array<T>::operator%= (const valarray<T>& ar) const
  {
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] %= ar[cpt++];
  }

  template <class T>
  void indirect_array<T>::operator^= (const valarray<T>& ar) const
  {
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] ^= ar[cpt++];
  }

  template <class T>
  void indirect_array<T>::operator&= (const valarray<T>& ar) const
  { 
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] &= ar[cpt++];
  }

  template <class T>
  void indirect_array<T>::operator|= (const valarray<T>& ar) const
  { 
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] |= ar[cpt++];
  }
  template <class T>
  void indirect_array<T>::operator<<= (const valarray<T>& ar) const
  { 
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] <<= ar[cpt++];
  }
  template <class T>
  void indirect_array<T>::operator>>= (const valarray<T>& ar) const
  { 
    size_t cpt=0;

    for(size_t iter=0; iter < array.size(); iter++ )
      (*ref_mem_array)[array[iter]] >>= ar[cpt++];
  }
#endif
#ifndef _RWSTD_NO_NAMESPACE
}
#endif

#pragma option pop
#endif /* __VALARRAY_CC */
