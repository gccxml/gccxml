#ifndef __FSTREAM_H
#define __FSTREAM_H
#pragma option push -b -a8 -pc -Vx- -Ve- -w-inl -w-aus -w-sig
// -*- C++ -*-
#ifndef __STD_FSTREAM__
#define __STD_FSTREAM__

/***************************************************************************
 *
 * fstream -- Declarations for the Standard Library file bufs and streams
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

#include <stdcomp.h>

#include <istream>
#include <ostream>
#include <streambuf>

#ifndef _RWSTD_NO_NEW_HEADER
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#ifndef _RWSTD_STRICT_ANSI
# if defined (_MSC_VER) || defined (__TURBOC__)
#  include <io.h>
# else
#ifdef _HPACC_
using std::off_t;
using std::ssize_t;
using std::pid_t;
using std::useconds_t;
using std::mode_t;
#endif
#  include <unistd.h>
# endif // defined (_MSC_VER) || defined (__TURBOC__)
# include <fcntl.h>
#else
# ifndef _RWSTD_NO_EXTENSION
#  define _RWSTD_NO_EXTENSION 1
# endif
#endif // _RWSTD_STRICT_ANSI

#ifndef _RWSTD_NO_NAMESPACE
namespace __rwstd {
#endif  

#ifndef _RWSTD_STRICT_ANSI
  extern const int _RWSTDExport __rwOpenTable[32];
#else
  extern const char _RWSTDExportFunc(*) __rwOpenTable[32];
#endif

#ifndef _RWSTD_NO_NAMESPACE
} namespace std {
#endif  

  /*
   *  Class basic_filebuf
   */
  
  template<class charT, class traits>
  class _RWSTDExportTemplate basic_filebuf : public basic_streambuf<charT, traits> {

#ifndef _RWSTD_STRICT_ANSI
    typedef int __file_type;
#else
    typedef FILE* __file_type;
#endif

    typedef basic_ios<charT, traits>      ios_type;
    typedef _TYPENAME traits::state_type  state_t; 
    
  public:
    //
    // Types:
    //
    typedef charT                         char_type;
    typedef _TYPENAME traits::int_type    int_type;
    typedef _TYPENAME traits::pos_type    pos_type;
    typedef _TYPENAME traits::off_type    off_type;
    typedef traits                        traits_type;
      
    basic_filebuf();
    virtual ~basic_filebuf();

    bool is_open() const;
    basic_filebuf<charT, traits> *
    open(const char *s, ios_base::openmode, long protection = 0666 );
    basic_filebuf<charT, traits> *close();
    basic_filebuf(__file_type fd);

#ifndef _RWSTD_NO_EXTENSION
    basic_filebuf<charT, traits> *open(int fd);
    int fd() const { return __file;}
#endif //_RWSTD_NO_EXTENSION

  protected:

    virtual streamsize      showmanyc();
    virtual int_type underflow();
    virtual int_type overflow(int_type c = traits::eof());
    virtual int_type pbackfail(int_type c = traits::eof());
      
    virtual basic_streambuf<charT,traits>* 
    setbuf(char_type *s,streamsize n);
 
    virtual pos_type seekoff(off_type off,
                             ios_base::seekdir way,
                             ios_base::openmode which =
                             ios_base::in | ios_base::out);
    virtual pos_type seekpos(pos_type sp,
                             ios_base::openmode which =
                             ios_base::in | ios_base::out);

    virtual int sync( );
    virtual streamsize xsputn(const char_type *s, streamsize n);

  private:

    // Deal with CR/LF translation on win32
    off_type __get_chars(charT* start, charT* finish)
    {
#ifndef __WIN32__
      return finish-start;
#else
      if (this->mode_ & ios_base::binary)
        return finish-start;

#ifndef _RWSTD_NO_TEMPLATE_ON_RETURN_TYPE
      charT eol = use_facet< ctype<charT> >(this->getloc()).widen('\n'); 
#else
      charT eol = use_facet( this->getloc(), (ctype<charT>* )0).widen('\n'); 
#endif

      off_type n=0;
      while (start != finish)
      {
        if (*start++ == eol)
          n+=2;
        else
          ++n;
      }
      return n;
#endif // __WIN32__
    }
       

    off_type __file_seek(__file_type f, long offset, int origin)
    {
#ifndef _RWSTD_STRICT_ANSI
      return lseek(f, offset, origin);
#else
      fseek(f, offset, origin);
      return ftell(f);
#endif // _RWSTD_STRICT_ANSI
    }

    int __file_close(__file_type f)
    {
#ifndef _RWSTD_STRICT_ANSI
      return ::close(f);
#else
      return fclose(f);
#endif // _RWSTD_STRICT_ANSI
    }

    bool __file_write(__file_type f, const char* buffer, size_t size, size_t count)
    {
#ifndef _RWSTD_STRICT_ANSI
      return (size_t) ::write(f,buffer,size*count) == size*count ? true : false;
#else
      return fwrite(buffer,size,count,f) == count ? true : false;
#endif // _RWSTD_STRICT_ANSI
    }

    size_t __file_read(__file_type f, char* buffer, size_t size, size_t count)
    {
#ifndef _RWSTD_STRICT_ANSI
      int n = ::read(f,buffer,size*count);
      return n < 0 ? 0 : (size_t)n;
#else
      return fread(buffer,size,count,f)*size;
#endif
    }

    __file_type __closed()
    {
#ifndef _RWSTD_STRICT_ANSI
       return  -1;    
#else
       return 0;
#endif
    }

    bool __file_is_std(__file_type f)
    {
#ifndef _RWSTD_STRICT_ANSI
       return  f < 3;    
#else
       return (f == stdin || f == stdout || f == stderr);
#endif
    }

    __file_type                 __file;
    bool                        __read_buff;
    bool                        __write_buff;
    streamsize                  __last_numb_read;
    streamsize                  __rwBufferSize;
    streamsize                  __absolute_pos;
    state_t                    *__state_beg;
    state_t                    *__state_end;
    char_type                  *__data;
  };
  template<class charT, class traits>
  class _RWSTDExportTemplate basic_ifstream : public basic_istream<charT, traits> {

  public:
    //
    // Types:
    //
    typedef charT                              char_type;
    typedef _TYPENAME traits::int_type         int_type;
    typedef _TYPENAME traits::pos_type         pos_type;
    typedef _TYPENAME traits::off_type         off_type;
    typedef traits                             traits_type;
    typedef basic_ios<charT, traits>           ios_type;
      
  public:

    basic_ifstream();
    _EXPLICIT basic_ifstream(const char *s, 
                             ios_base::openmode mode = ios_base::in,
                             long protection = 0666 );

#ifndef _RWSTD_NO_EXTENSION
    _EXPLICIT basic_ifstream(int fd);
    basic_ifstream(int fd, char_type *buf, int len);
#endif //_RWSTD_NO_EXTENSION

    virtual ~basic_ifstream();
    basic_filebuf<charT, traits> *rdbuf() const;
    bool is_open();
    void open(const char *s, ios_base::openmode mode = ios_base::in,
              long protection = 0666 );

    void close();
  protected:

  private:
    basic_filebuf<charT, traits>              __fb;
  };
  template<class charT, class traits>
  class _RWSTDExportTemplate basic_ofstream : public basic_ostream<charT, traits> {

  public:
    //
    //Types:
    //
    typedef charT                                       char_type;
    typedef _TYPENAME traits::int_type                  int_type;
    typedef _TYPENAME traits::pos_type                  pos_type;
    typedef _TYPENAME traits::off_type                  off_type;
    typedef traits                                      traits_type;

    typedef basic_ios<charT, traits>                   ios_type;
      
  public:

    basic_ofstream();
    _EXPLICIT basic_ofstream(const char *s, ios_base::openmode mode =
                             ios_base::out,
                             long protection = 0666 );

#ifndef _RWSTD_NO_EXTENSION
    _EXPLICIT basic_ofstream(int fd);
    basic_ofstream(int fd, char_type *buf, int len);
#endif //_RWSTD_NO_EXTENSION

    virtual ~basic_ofstream();

    basic_filebuf<charT, traits> *rdbuf() const;
    bool is_open();
     
    void open(const char *s, ios_base::openmode mode = 
              ios_base::out,
              long protection = 0666 );
     
    void close();
  protected:

  private:
    basic_filebuf<charT, traits>              __fb;
  };

  /*
   *  Class basic_fstream
   */

  template<class charT, class traits>
  class _RWSTDExportTemplate basic_fstream : public basic_iostream<charT, traits> {

  public:
    //
    // Types:
    //
    typedef charT                             char_type;
    typedef _TYPENAME traits::int_type        int_type;
    typedef _TYPENAME traits::pos_type        pos_type;
    typedef _TYPENAME traits::off_type        off_type;
    typedef traits                            traits_type;
    typedef basic_ios<charT, traits>          ios_type;

    basic_fstream();
    _EXPLICIT basic_fstream(const char *s, ios_base::openmode mode = 
                            ios_base::in | ios_base::out, 
                            long protection= 0666 );

#ifndef _RWSTD_NO_EXTENSION
    _EXPLICIT basic_fstream(int fd);
    basic_fstream(int fd, char_type *buf, int len);
#endif //_RWSTD_NO_EXTENSION

    virtual ~basic_fstream();

    basic_filebuf<charT, traits> *rdbuf() const;
    bool is_open();
    void open(const char *s, ios_base::openmode mode = 
              ios_base::in | ios_base::out, 
              long protection = 0666 );

    void close();
  protected:

  private:
    basic_filebuf<charT, traits>              __fb;
  };
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  typedef basic_filebuf<char>                             filebuf;
#else
  typedef basic_filebuf<char, char_traits<char> >         filebuf;
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES

#ifndef _RWSTD_NO_WIDE_CHAR
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  typedef basic_filebuf<wchar_t>                          wfilebuf;
#else
  typedef basic_filebuf<wchar_t, char_traits<wchar_t> >   wfilebuf;
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
#endif // _RWSTD_NO_WIDE_CHAR
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  typedef basic_ifstream<char>                            ifstream;
#else
  typedef basic_ifstream<char, char_traits<char> >        ifstream;
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
 
#ifndef _RWSTD_NO_WIDE_CHAR
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  typedef basic_ifstream<wchar_t>                         wifstream;
#else
  typedef basic_ifstream<wchar_t, char_traits<wchar_t> >  wifstream;
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
#endif // _RWSTD_NO_WIDE_CHAR 
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  typedef basic_ofstream<char>                            ofstream;
#else
  typedef basic_ofstream<char, char_traits<char> >        ofstream;
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES 

#ifndef _RWSTD_NO_WIDE_CHAR
#ifndef _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
  typedef basic_ofstream<wchar_t>                         wofstream;
#else
  typedef basic_ofstream<wchar_t, char_traits<wchar_t> >  wofstream;
#endif // _RWSTD_NO_COMPLEX_DEFAULT_TEMPLATES
#endif // _RWSTD_NO_WIDE_CHAR

#ifndef _RWSTD_NO_DEFAULT_TEMPLATES
  typedef basic_fstream<char>                             fstream;
#else
  typedef basic_fstream<char, char_traits<char> >         fstream;
#endif // _RWSTD_NO_DEFAULT_TEMPLATES 

#ifndef _RWSTD_NO_WIDE_CHAR
#ifndef _RWSTD_NO_DEFAULT_TEMPLATES  
  typedef basic_fstream<wchar_t>                          wfstream;
#else
  typedef basic_fstream<wchar_t, char_traits<wchar_t> >   wfstream;
#endif // _RWSTD_NO_DEFAULT_TEMPLATES  
#endif // _RWSTD_NO_WIDE_CHAR 

#ifndef _RWSTD_NO_NAMESPACE
}
#endif

#ifdef _RWSTD_COMPILE_INSTANTIATE
#include <fstream.cc>
#endif

#endif //__STD_FSTREAM__ 

#ifndef __USING_STD_NAMES__
  using namespace std;
#endif

#pragma option pop
#endif /* __FSTREAM_H */
