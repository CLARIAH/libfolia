/*
  Copyright (c) 2006 - 2017
  CLST  - Radboud University
  ILK   - Tilburg University

  This file is part of libfolia

  libfolia is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  libfolia is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.

  For questions and suggestions, see:
      https://github.com/LanguageMachines/ticcutils/issues
  or send mail to:
      lamasoftware (at ) science.ru.nl
*/

#ifndef FOLIA_UTILS_H
#define FOLIA_UTILS_H

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <exception>
#include <ctime>
#include "unicode/unistr.h"
#include "unicode/unistr.h"
#include <unicode/ustream.h>
#include "libxml/tree.h"

#include "ticcutils/StringOps.h"

namespace folia {

  namespace AnnotationType {
    enum AnnotationType : int;
  }
  enum AnnotatorType : int;
  enum ElementType : unsigned int;

  class ArgsError: public std::runtime_error {
  public:
  ArgsError( const std::string& s ): std::runtime_error( "error in argument list: " + s  ){};
  };

  class KeyError: public std::out_of_range {
  public:
  KeyError(): std::out_of_range( "key out of range" ){};
  };

  class NotImplementedError: public std::runtime_error {
  public:
  NotImplementedError( const std::string& s ):
    std::runtime_error( "NOT IMPLEMENTED: " + s ){};
  };

  class ValueError: public std::runtime_error {
  public:
  ValueError( const std::string& s ): std::runtime_error( s ){};
  };

  class XmlError: public std::runtime_error {
  public:
  XmlError( const std::string& s ): std::runtime_error( "XML error: " + s ){};
  };

  class NoSuchAnnotation: public std::runtime_error {
  public:
  NoSuchAnnotation( const std::string& s ): std::runtime_error( "no such annotation: " + s ){};
  };

  class NoSuchText: public std::runtime_error {
  public:
  NoSuchText( const std::string& s ): std::runtime_error( "no such text: " + s ){};
  };

  class NoSuchPhon: public std::runtime_error {
  public:
  NoSuchPhon( const std::string& s ): std::runtime_error( "no such phoneme: " + s ){};
  };

  class DuplicateAnnotationError: public std::runtime_error {
  public:
  DuplicateAnnotationError( const std::string& s ): std::runtime_error( s ){};
  };

  class DuplicateIDError: public std::runtime_error {
  public:
  DuplicateIDError( const std::string& s ): std::runtime_error( "duplicate ID : " + s ){};
  };

  class NoDefaultError: public std::runtime_error {
  public:
  NoDefaultError( const std::string& s ): std::runtime_error( "No Default found: " + s ){};
  };

  UnicodeString UTF8ToUnicode( const std::string& );
  std::string UnicodeToUTF8( const UnicodeString& );

  std::string toString( const AnnotationType::AnnotationType& );
  AnnotationType::AnnotationType stringToAT( const std::string& );

  AnnotatorType stringToANT( const std::string& );

  std::string toString( const ElementType& );
  ElementType stringToET( const std::string& );

  typedef std::map<std::string, std::string> KWargs;

  KWargs getArgs( const std::string& );
  std::string toString( const KWargs& );

  inline std::ostream& operator<<( std::ostream& os,
				   const AnnotationType::AnnotationType& at ){
    os << toString( at );
    return os;
  }

  void addAttributes( xmlNode *, const KWargs& );
  KWargs getAttributes( const xmlNode * );

  std::string parseDate( const std::string& );
  std::string parseTime( const std::string& );

  bool AT_sanity_check();
  bool Attrib_sanity_check();
  bool ET_sanity_check();

  bool isNCName( const std::string& );

} // namespace folia

namespace TiCC {
  // add some specializations to the TiCC stringTo() family

  template<>
    inline folia::AnnotationType::AnnotationType stringTo( const std::string& str ) {
    return folia::stringToAT( str );
  }

  template<>
    inline folia::ElementType stringTo( const std::string& str ) {
    return folia::stringToET( str );
  }

  template<>
    inline folia::AnnotatorType stringTo( const std::string& str ) {
    return folia::stringToANT( str );
  }

} // namespace TiCC

#endif // FOLIA_UTILS
