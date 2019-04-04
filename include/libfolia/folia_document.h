/*
  Copyright (c) 2006 - 2019
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

#ifndef FOLIA_DOCUMENT_H
#define FOLIA_DOCUMENT_H

#include <list>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "unicode/unistr.h"
#include "unicode/regex.h"
#include "libxml/tree.h"
#include "libxml/xpath.h"
#include "libfolia/folia.h"

using namespace icu;

namespace folia {
  void initMT();

  extern const std::string NSFOLIA;

  enum ElementType : unsigned int;

  class Pattern {
    friend std::ostream& operator<<( std::ostream&, const Pattern& );
  public:
    Pattern( const std::vector<std::string>&,
	     const ElementType = BASE,
	     const std::string& = "" );
    Pattern( const std::vector<std::string>&,  const std::string& );

    ~Pattern();
    bool match( const UnicodeString& , size_t&, int&, bool&, bool& ) const;
    size_t size() const { return sequence.size(); };
    void unsetwild();
    bool variablesize() const;
    std::set<int> variablewildcards() const;
    ElementType matchannotation;
    bool regexp;
  private:
    bool case_sensitive;
    int maxgapsize;
    std::vector<UnicodeString> sequence;
    std::vector<RegexMatcher*> matchers;
    std::string matchannotationset;
  };

  class FoliaElement;
  class Word;
  class Sentence;
  class Paragraph;
  class Document;

  class processor {
    friend class Document;
  public:
    processor( const xmlNode * );
    processor();
    ~processor();
    void init( const KWargs& );
    void set_system_defaults();
    std::string name() const { return _name; };
    std::string annotator() const { return _name; };
    std::string id() const { return _id; };
    std::string type() const { return _type; };
    std::string annotatortype() const { return _type; };
    std::string version() const { return _version; } ;
    std::string document_version() const { return _document_version; };
    std::string folia_version() const { return _folia_version; };
    std::string command() const { return _command; };
    std::string host() const { return _host; };
    std::string user() const { return _user; };
    std::string begindatetime() const { return _begindatetime; };
    std::string enddatetime() const { return _enddatetime; };
    std::string resourcelink() const { return _resourcelink; };
    std::vector<processor*> processors() const { return _processors; };
    void print( std::ostream&, const int ) const;
  private:
    std::string _name;
    std::string _id;
    std::string _type;
    std::string _version;
    std::string _document_version;
    std::string _folia_version;
    std::string _command;
    std::string _host;
    std::string _user;
    std::string _begindatetime;
    std::string _enddatetime;
    std::string _resourcelink;
    std::map<std::string,std::string> _metadata;
    std::vector<processor*> _processors;
  };

  std::ostream& operator<<( std::ostream&, const processor& );
  std::ostream& operator<<( std::ostream&, const processor * );

  class Provenance {
  public:
    Provenance(){};
    ~Provenance();
    const processor *get_processor( const std::string& ) const;
    xmlNode *xml();
    Provenance *parseXml( const xmlNode * );
    std::vector<processor*> processors;
    const processor* operator []( const std::string& s ) const {
      //select processor with index s
      return get_processor( s );
    };
    const processor* index( const std::string& s ) const {
      //select processor with index s
      return get_processor( s );
    };
  private:
    Provenance( const Provenance& ); // inhibit copy
    Provenance& operator=( const Provenance& ); // inhibit copies
  };

  std::ostream& operator<<( std::ostream&, const Provenance& );
  std::ostream& operator<<( std::ostream&, const Provenance * );


  class Document {
    friend bool operator==( const Document&, const Document& );
    friend std::ostream& operator<<( std::ostream&, const Document * );
    enum Mode { NOMODE=0, PERMISSIVE=1, CHECKTEXT=2, FIXTEXT=4, STRIP=8 };
    friend class Processor;
  public:
    Document();
    explicit Document( const KWargs& );
    explicit Document( const std::string& );
    ~Document();
    void init();
    void init_args( const KWargs& );
    bool readFromFile( const std::string& );
    bool readFromString( const std::string& );
    bool save( std::ostream&, const std::string&, bool = false ) const;
    bool save( std::ostream& os, bool kanon = false ) const {
      return save( os, "", kanon );
    }
    bool save( const std::string&, const std::string&, bool = false ) const ;
    bool save( const std::string& s, bool kanon = false ) const {
      return save( s, "", kanon );
    }
    std::string xmlstring( bool k = false ) const;
    int size() const;
    FoliaElement* doc() const { return foliadoc; }
    FoliaElement* append( FoliaElement *t );
    Text* setTextRoot();
    Text* setTextRoot( KWargs& );
    Speech* setSpeechRoot();
    Speech* setSpeechRoot( KWargs& );
    // backward compatible:
    Text* addText( KWargs& a ){ return setTextRoot( a ); };
    Text* addText( Text *t ){ return dynamic_cast<Text*>( append(t) ); };
    Speech* addSpeech( KWargs& a ){ return setSpeechRoot( a ); };
    Speech* addSpeech( Speech *s ){ return dynamic_cast<Speech*>( append(s) ); };

    void set_foreign_metadata( xmlNode * );
    void addStyle( const std::string&, const std::string& );
    void replaceStyle( const std::string&, const std::string& );
    UnicodeString text( const std::string& = "current",
			bool = false,
			bool = false ) const;
    std::vector<Paragraph*> paragraphs() const;
    std::vector<Sentence*> sentences() const;
    std::vector<Sentence*> sentenceParts() const;
    std::vector<Word*> words() const;
    std::vector<std::vector<Word*> > findwords( const Pattern&,
						const std::string& ="" ) const;
    std::vector<std::vector<Word*> > findwords( std::list<Pattern>&,
						const std::string& = "" ) const;
    Word *words( size_t ) const;
    Word *rwords( size_t ) const;
    Paragraph *paragraphs( size_t ) const;
    Paragraph *rparagraphs( size_t ) const;
    Sentence *sentences( size_t ) const;
    Sentence *rsentences( size_t ) const;
    std::string toXml( const std::string& ="",
		       bool = false ) const;
    bool toXml( const std::string&,
		const std::string& ="",
		bool = false ) const;
    std::string metadatatype() const;
    std::string metadatafile() const;
    void set_metadata( const std::string&, const std::string& );
    const std::string get_metadata( const std::string&) const;
    const processor *get_processor( const std::string& ) const;
    void addDocIndex( FoliaElement*, const std::string& );
    void delDocIndex( const FoliaElement*, const std::string& );

    FoliaElement* operator []( size_t ) const; //select i'th element from data

    FoliaElement *index( const std::string& ) const; //retrieve element with specified ID
    FoliaElement* operator []( const std::string& ) const ; //index as operator
    bool isDeclared( AnnotationType::AnnotationType,
		     const std::string&,
		     const std::string&,
		     const std::string&,
		     const std::string& ) const;
    bool isDeclared( AnnotationType::AnnotationType,
		     const std::string&,
		     const std::string&,
		     const std::string&,
		     const std::set<std::string>& ) const;
    bool isDeclared( AnnotationType::AnnotationType,
		     const std::string& = "" ) const;
    bool declared( AnnotationType::AnnotationType at,
		   const std::string& s="" ) const {
      return isDeclared( at, s );
    }
    bool declared( ElementType, const std::string& = "" ) const;
    std::string unalias( AnnotationType::AnnotationType,
			 const std::string& ) const;
    std::string alias( AnnotationType::AnnotationType,
		       const std::string& ) const;

    processor *add_processor( const KWargs&, processor * =0 );
    std::vector<std::string> getannotators( AnnotationType::AnnotationType,
					    const std::string& ="" ) const;
    std::vector<const processor *> getprocessors( AnnotationType::AnnotationType,
						  const std::string& ="" ) const;

    std::string defaultset( AnnotationType::AnnotationType ) const;

    std::string defaultannotator( AnnotationType::AnnotationType,
				  const std::string& ="" ) const;
    std::string defaultannotatortype( AnnotationType::AnnotationType,
				      const std::string& ="" ) const;

    std::string defaultdatetime( AnnotationType::AnnotationType,
				 const std::string& ="" ) const;
    std::string defaultprocessor( AnnotationType::AnnotationType,
				  const std::string& ="" ) const;

    FoliaElement* parseXml( );

    std::string id() const { return _id; };
    std::string language() const;
    void declare( AnnotationType::AnnotationType,
		  const std::string&,
		  const std::string& = "" );
    void declare( AnnotationType::AnnotationType,
		  const std::string&, const std::string&,
		  const std::string&, const std::string&,
		  const std::set<std::string>&,
		  const std::string& = "" );
    void un_declare( AnnotationType::AnnotationType,
		     const std::string& );
    xmlDoc *XmlDoc() const { return _xmldoc; };
    xmlNs *foliaNs() const { return _foliaNsOut; };
    void keepForDeletion( FoliaElement *p ) { delSet.insert( p ); };
    void addExternal( External *p ) { externals.push_back( p ); };
    void resolveExternals();
    int debug;
    bool permissive() const { return mode & PERMISSIVE; };
    bool checktext() const {
      return mode & CHECKTEXT;
    };
    bool fixtext() const {
      return mode & FIXTEXT;
    };
    bool strip() const { return mode & STRIP; };
    bool set_permissive( bool ) const; // defines const, but the mode is mutable!
    bool set_checktext( bool ) const; // defines const, but the mode is mutable!
    bool set_fixtext( bool ) const; // defines const, but the mode is mutable!
    bool set_strip( bool ) const; // defines const, but the mode is mutable!
    class at_t {
      friend std::ostream& operator<<( std::ostream&, const at_t& );
    public:
    at_t( const std::string& _a,
	  const std::string& _t,
	  const std::string& _d,
	  const std::set<std::string>& _p ): a(_a),t(_t),d(_d),p(_p){};
      std::string a;
      std::string t;
      std::string d;
      std::set<std::string> p;
    };
    void incrRef( AnnotationType::AnnotationType, const std::string& );
    void decrRef( AnnotationType::AnnotationType, const std::string& );
    void setmode( const std::string& ) const;
    std::string getmode() const;
    int setdebug( int val ){ int ret=debug; debug=val; return ret;};
    std::multimap<AnnotationType::AnnotationType,std::string> unused_declarations( ) const;
    const MetaData *get_submetadata( const std::string& m ){
      const auto& it = submetadata.find( m );
      if ( it == submetadata.end() ){
	return 0;
      } else {
	return it->second;
      }
    }
    void cache_textcontent( TextContent *tc ){
      t_offset_validation_buffer.push_back( tc );
    }
    void cache_phoncontent( PhonContent *tc ){
      p_offset_validation_buffer.push_back( tc );
    }
    bool validate_offsets() const;
    int compare_to_lib_version() const;
    std::string version() const { return _version_string; };
    std::string doc_version() const;
    std::string update_version();
    bool version_below( int, int );
    std::map<AnnotationType::AnnotationType,std::multimap<std::string,at_t> > annotationdefaults() const { return _annotationdefaults; };
    void parse_metadata( const xmlNode * );
    void setDocumentProps( KWargs& );
    Provenance *provenance() const { return _provenance;};
  private:
    void adjustTextMode();
    std::map<AnnotationType::AnnotationType,std::multimap<std::string,at_t> > _annotationdefaults;
    std::map<AnnotationType::AnnotationType,std::map<std::string,bool> > _groupannotations;
    std::vector<std::pair<AnnotationType::AnnotationType,std::string>> _anno_sort;
    std::map<AnnotationType::AnnotationType,std::map<std::string,int> > _annotationrefs;
    std::map<AnnotationType::AnnotationType,std::map<std::string,std::string>> _alias_set;
    std::map<AnnotationType::AnnotationType,std::map<std::string,std::string>> _set_alias;
    std::vector<TextContent*> t_offset_validation_buffer;
    std::vector<PhonContent*> p_offset_validation_buffer;

    void setimdi( xmlNode * );
    void parseannotations( const xmlNode * );
    void parseprovenance( const xmlNode * );
    void parsesubmeta( const xmlNode * );
    void getstyles();
    void setannotations( xmlNode *) const;
    void setprovenance( xmlNode * ) const;
    void setmetadata( xmlNode * ) const;
    void addsubmetadata( xmlNode *) const;
    void setstyles( xmlDoc* ) const;
    void append_processor( xmlNode *, const processor * ) const;
    xmlDoc *to_xmlDoc( const std::string& ="", bool=false ) const;
    std::map<std::string, FoliaElement* > sindex;
    std::vector<FoliaElement* > iindex;
    std::vector<FoliaElement*> data;
    std::vector<External*> externals;
    std::string _id;
    std::set<FoliaElement *> delSet;
    FoliaElement *foliadoc;
    xmlDoc *_xmldoc;
    const xmlChar* _foliaNsIn_href;
    const xmlChar* _foliaNsIn_prefix;
    mutable xmlNs *_foliaNsOut;
    MetaData *_metadata;
    Provenance *_provenance;
    std::map<std::string,MetaData *> submetadata;
    std::multimap<std::string,std::string> styles;
    mutable Mode mode;
    std::string _source_filename;
    std::string _version_string;
    int major_version;
    int minor_version;
    int sub_version;
    std::string patch_version;
    bool external;
    Document( const Document& ); // inhibit copies
    Document& operator=( const Document& ); // inhibit copies
  };

  bool operator==( const Document&, const Document& );
  inline bool operator!=( const Document& d1, const Document& d2 ){
    return !( d1==d2 );
  }

  std::ostream& operator<<( std::ostream&, const Document * );
  inline std::ostream& operator<<( std::ostream& os, const Document& d ){
    os << &d;
    return os;
  }

  std::ostream& operator<<( std::ostream& os, const Document::at_t& at );

  void expand_version_string( const std::string&,
			      int&, int&, int&, std::string& );

  std::string library_version();
  std::string folia_version();

} // namespace folia

#endif // FOLIA_DOCUMENT_H
