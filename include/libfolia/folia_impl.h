/*
  Copyright (c) 2006 - 2016
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

#ifndef FOLIA_IMPL_H
#define FOLIA_IMPL_H

#include <set>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include "unicode/unistr.h"
#include "libxml/tree.h"

namespace folia {
  class Document;
  class AbstractSpanAnnotation;
  class Alternative;
  class PosAnnotation;
  class LemmaAnnotation;
  class MorphologyLayer;
  class Sentence;
  class Word;
  class TextContent;
  class PhonContent;
  class Correction;
  class New;
  class Original;
  class Current;
  class Suggestion;
  class Division;
  class DependencyDependent;
  class Paragraph;
  class Morpheme;

  class properties;
  extern const std::set<ElementType> default_ignore_annotations;

#define NOT_IMPLEMENTED {						\
    throw NotImplementedError( xmltag() + "::" + std::string(__func__) ); \
  }

  class FoliaElement {
    friend std::ostream& operator<<( std::ostream&, const FoliaElement& );
    friend std::ostream& operator<<( std::ostream&, const FoliaElement* );
    friend bool operator==( const FoliaElement&, const FoliaElement& );
  public:
    virtual ~FoliaElement(){};
    virtual void init() {};
    virtual bool has_base( ElementType ) const = 0;
    virtual size_t size() const = 0;
    virtual FoliaElement* index( size_t ) const = 0;
    virtual FoliaElement* rindex( size_t ) const = 0;
    FoliaElement* operator[]( size_t i ) const {
      return index(i);
    }

    virtual bool isinstance( ElementType et ) const = 0;
    template <typename F>
      bool isinstance() const {
      F obj((Document*)0);
      return element_id() == obj.element_id();
    }
    bool isSubClass( ElementType ) const;
    bool isSubClass( const FoliaElement *c ) const {
      return dynamic_cast<decltype(c)>(this) != 0;
    };

    virtual void fixupDoc( Document* ) = 0;
    virtual FoliaElement *parent() const = 0;
    virtual void setParent( FoliaElement *p ) = 0;
    virtual bool acceptable( ElementType ) const = 0;
    virtual bool addable( const FoliaElement * ) const = 0;
    virtual FoliaElement *append( FoliaElement* ) = 0;
    virtual FoliaElement *postappend( ) = 0;
    virtual void remove( size_t, bool = true ) = 0;
    virtual void remove( FoliaElement *, bool = true ) = 0;
    virtual std::vector<FoliaElement*> findreplacables( FoliaElement * ) const = 0;
    virtual void replace( FoliaElement * ) = 0;
    virtual FoliaElement* replace( FoliaElement *, FoliaElement* ) = 0;


    virtual FoliaElement *head() const NOT_IMPLEMENTED;

    // Sentences
    virtual Sentence *addSentence( const KWargs& ) = 0;
    Sentence *addSentence( const std::string& s ="" ){
      return addSentence( getArgs(s) );
    };

    // Selections
    template <typename F>
      std::vector<F*> select( const std::string& st,
			      const std::set<ElementType>& exclude,
			      bool recurse = true ) const {
      F obj((Document*)0);
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj.element_id(),
					       st,
					       exclude,
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( const std::string& st,
			      bool recurse = true ) const {
      F obj((Document*)0);
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj.element_id(),
					       st,
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( const char* st,
			      bool recurse = true ) const {
      F obj((Document*)0);
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj.element_id(),
					       std::string(st),
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( const std::set<ElementType>& exclude,
			      bool recurse = true ) const {
      F obj((Document*)0);
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj.element_id(),
					       exclude,
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    template <typename F>
      std::vector<F*> select( bool recurse = true ) const {
      F obj((Document*)0);
      std::vector<F*> res;
      std::vector<FoliaElement*> tmp = select( obj.element_id(),
					       recurse );
      for ( size_t i = 0; i < tmp.size(); ++i ){
	res.push_back( dynamic_cast<F*>( tmp[i]) );
      }
      return res;
    }

    // annotations

    virtual bool allowannotations() const { return false; };
    virtual const std::string annotator( ) const = 0;
    virtual void annotator( const std::string& ) = 0;
    virtual AnnotatorType annotatortype() const = 0;
    virtual void annotatortype( AnnotatorType t ) =  0;
    virtual AnnotationType::AnnotationType annotation_type() const = 0;
    virtual PosAnnotation *addPosAnnotation( const KWargs& ) NOT_IMPLEMENTED;
    virtual LemmaAnnotation *addLemmaAnnotation( const KWargs& ) NOT_IMPLEMENTED;
    virtual MorphologyLayer *addMorphologyLayer( const KWargs& ) NOT_IMPLEMENTED;

    virtual PosAnnotation *getPosAnnotations( const std::string&,
					      std::vector<PosAnnotation*>& ) const NOT_IMPLEMENTED;
    virtual LemmaAnnotation *getLemmaAnnotations( const std::string&,
						  std::vector<LemmaAnnotation*>& ) const NOT_IMPLEMENTED;
    virtual MorphologyLayer *getMorphologyLayers( const std::string&,
						  std::vector<MorphologyLayer*>& ) const NOT_IMPLEMENTED;

    template <typename F>
      std::vector<F*> annotations( const std::string& s = "" ) const {
      if ( allowannotations() ){
	std::vector<F*> v = select<F>( s, default_ignore_annotations );
	if ( v.size() >= 1 )
	  return v;
	else {
	  F obj((Document*)0);
	  if ( s.empty() )
	    throw NoSuchAnnotation( obj.classname() );
	  else
	    throw NoSuchAnnotation( obj.classname() + " for set='" + s + "'" );
	}
      }
      else NOT_IMPLEMENTED;
    }

    template <typename F>
      bool hasannotation( const std::string& st = "" ) const {
      try {
	std::vector<F*> v = annotations<F>( st );
	return true;
      }
      catch ( NoSuchAnnotation& e ){
	return false;
      }
    }

    template <typename F>
      F *annotation( const std::string& st = "" ) const {
      std::vector<F*>v = annotations<F>( st );
      return v[0]; // always exist, otherwise annotations would throw()
    }

    template <typename F>
      F *addAnnotation( const KWargs& args ) {
      F *res = 0;
      try {
	res = new F( doc(), args);
      }
      catch( std::exception& ){
	delete res;
	throw;
      }
      append( res );
      return res;
    }
    // span annotation
    virtual std::vector<AbstractSpanAnnotation*> selectSpan() const = 0;
    virtual std::vector<AbstractSpanAnnotation*> findspans( ElementType,
							    const std::string& = "" ) const NOT_IMPLEMENTED;
    template <typename F>
      std::vector<AbstractSpanAnnotation*> findspans( const std::string& st = "" ) const {
      F obj((Document*)0);
      return findspans( obj.element_id(), st );
    }
    virtual AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const NOT_IMPLEMENTED;

    // features
    virtual std::vector<std::string> feats( const std::string& ) const = 0;
    virtual const std::string feat( const std::string& ) const = 0;

    //XML (de)serialisation
    virtual FoliaElement* parseXml( const xmlNode * ) = 0;
    const std::string xmlstring() const; // serialize to a string (XML fragment)
    virtual xmlNode *xml( bool, bool = false ) const = 0; //serialize to XML

    // text/string content
    bool hastext( const std::string& = "current" ) const;

    virtual const std::string str( const std::string& = "current" ) const = 0;
    const UnicodeString unicode( const std::string& cls = "current" ) const { return text( cls ); };
    virtual const UnicodeString text( const std::string& = "current",
				bool = false, bool = false ) const = 0;
    const UnicodeString stricttext( const std::string& = "current" ) const;
    const UnicodeString toktext( const std::string& = "current" ) const;
    virtual const UnicodeString phon( const std::string& = "current",
				bool = false ) const = 0;
    virtual bool printable() const = 0;
    virtual bool speakable() const = 0;

    // Word
    virtual Word *previous() const NOT_IMPLEMENTED;
    virtual Word *next() const NOT_IMPLEMENTED;
    virtual const Word* resolveword( const std::string& ) const = 0;
    virtual std::vector<Word*> context( size_t,
					const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual std::vector<Word*> leftcontext( size_t,
					    const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual std::vector<Word*> rightcontext( size_t,
					     const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual Word *addWord( const KWargs& ) = 0;
    Word *addWord( const std::string& s ){
      return addWord( getArgs(s) );
    }

    // corrections
    virtual New *getNew() const NOT_IMPLEMENTED;
    virtual FoliaElement *getNew( size_t ) const NOT_IMPLEMENTED;
    virtual Original *getOriginal() const NOT_IMPLEMENTED;
    virtual FoliaElement *getOriginal( size_t ) const NOT_IMPLEMENTED;
    virtual Current *getCurrent() const NOT_IMPLEMENTED;
    virtual FoliaElement *getCurrent( size_t ) const NOT_IMPLEMENTED;
    virtual Correction *incorrection() const NOT_IMPLEMENTED;
    virtual Correction *split( FoliaElement *, FoliaElement *,
			       const std::string& = "" ) NOT_IMPLEMENTED;

    virtual Correction *mergewords( FoliaElement *,
				    const std::vector<FoliaElement *>&,
				    const std::string& = "" ) NOT_IMPLEMENTED;

    virtual Correction *deleteword( FoliaElement *,
				    const std::string& = "" ) NOT_IMPLEMENTED;
    virtual Correction *insertword( FoliaElement *, FoliaElement *,
				    const std::string& = "" ) NOT_IMPLEMENTED;
    virtual std::vector<Suggestion*> suggestions() const NOT_IMPLEMENTED;
    virtual Suggestion *suggestions( size_t ) const NOT_IMPLEMENTED;

    virtual Correction *correct( const std::vector<FoliaElement*>&,
				 const std::vector<FoliaElement*>&,
				 const std::vector<FoliaElement*>&,
				 const std::vector<FoliaElement*>&,
				 const KWargs& ) NOT_IMPLEMENTED;
    virtual Correction* correct( FoliaElement*,
				 FoliaElement*,
				 const KWargs& ) NOT_IMPLEMENTED;
    virtual Correction* correct( FoliaElement*,
				 FoliaElement*,
				 const std::vector<FoliaElement*>&,
				 const KWargs& ) NOT_IMPLEMENTED;
    virtual Correction *correct( const std::string& = "" ) NOT_IMPLEMENTED;

    // TextContent
    virtual TextContent *textcontent( const std::string& = "current" ) const = 0;
    TextContent *settext( const std::string&, const std::string& = "current" );
    TextContent *settext( const std::string&, int , const std::string& = "current" );
    TextContent *setutext( const UnicodeString&, const std::string& = "current" );
    TextContent *setutext( const UnicodeString&, int , const std::string& = "current" );
    virtual int offset() const NOT_IMPLEMENTED;

    // PhonContent
    virtual PhonContent *phoncontent( const std::string& = "current" ) const = 0;

    // properties
    virtual const std::string& getTextDelimiter( bool retaintok=false ) const = 0;
    virtual void setDateTime( const std::string& ) = 0;
    virtual const std::string getDateTime() const = 0;
    virtual const std::string pos( const std::string& = "" ) const = 0;
    virtual const std::string lemma( const std::string& = "" ) const = 0;
    virtual const std::string cls() const = 0;
    virtual const std::string sett() const = 0;
    virtual const std::string classname() const = 0;
    virtual const std::string n() const = 0;
    virtual const std::string id() const = 0;
    virtual const std::string begintime() const = 0;
    virtual const std::string endtime() const = 0;
    virtual const std::string speech_src() const = 0;
    virtual const std::string speech_speaker() const = 0;
    virtual double confidence() const = 0;
    virtual void confidence( double ) = 0;
    virtual ElementType element_id() const = 0;
    virtual size_t occurrences() const = 0;
    virtual size_t occurrences_per_set() const = 0;
    virtual Attrib required_attributes() const = 0;
    virtual Attrib optional_attributes() const = 0;
    virtual const std::string& xmltag() const = 0;
    virtual const std::string subset() const NOT_IMPLEMENTED;

    virtual Document *doc() const = 0;
    virtual Sentence *sentence() const NOT_IMPLEMENTED;
    virtual Paragraph *paragraph() const NOT_IMPLEMENTED;
    virtual Division *division() const NOT_IMPLEMENTED;
    virtual std::vector<Paragraph*> paragraphs() const NOT_IMPLEMENTED;
    virtual std::vector<Sentence*> sentences() const NOT_IMPLEMENTED;
    virtual std::vector<Word*> words() const NOT_IMPLEMENTED;
    virtual std::vector<FoliaElement*> wrefs() const NOT_IMPLEMENTED;
    virtual FoliaElement* wrefs( size_t ) const NOT_IMPLEMENTED;

    virtual std::vector<Morpheme*> morphemes( const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual Morpheme* morpheme( size_t, const std::string& ="" ) const NOT_IMPLEMENTED;
    virtual Sentence *sentences( size_t ) const NOT_IMPLEMENTED;
    virtual Sentence *rsentences( size_t ) const NOT_IMPLEMENTED;
    virtual Paragraph *paragraphs( size_t ) const NOT_IMPLEMENTED;
    virtual Paragraph *rparagraphs( size_t ) const NOT_IMPLEMENTED;
    virtual Word *words( size_t ) const NOT_IMPLEMENTED;
    virtual std::vector<Word *> wordParts() const NOT_IMPLEMENTED;
    virtual Word *rwords( size_t ) const NOT_IMPLEMENTED;

    virtual DependencyDependent *dependent() const NOT_IMPLEMENTED;

    virtual const std::string description() const;

    // alternatives
    template <typename F>
      F *addAlternative();
    template <typename F>
      F *addAlternative( const KWargs& );
    virtual std::vector<Alternative *> alternatives( ElementType,
						     const std::string& = ""
						     ) const NOT_IMPLEMENTED;
    std::vector<Alternative*> alternatives( const std::string& s = "" ) const {
      return alternatives( BASE, s );
    }

    virtual const std::string content() const NOT_IMPLEMENTED;
    virtual const std::string src() const NOT_IMPLEMENTED;
    virtual const UnicodeString caption() const NOT_IMPLEMENTED;
    virtual std::vector<FoliaElement *> resolve() const NOT_IMPLEMENTED;
    virtual const FoliaElement* resolveid() const NOT_IMPLEMENTED;
    virtual bool checkAtts() = 0;


    virtual std::vector<FoliaElement*> select( ElementType elementtype,
					       bool = true ) const = 0;
    virtual std::vector<FoliaElement*> select( ElementType elementtype,
					       const std::set<ElementType>& ,
					       bool = true ) const = 0;
    virtual std::vector<FoliaElement*> select( ElementType elementtype,
					       const std::string&,
					       bool = true ) const = 0;
    virtual std::vector<FoliaElement*> select( ElementType elementtype,
					       const std::string&,
					       const std::set<ElementType>& ,
					       bool = true ) const = 0;
    // some 'internal stuff
    virtual int refcount() const = 0;
    virtual void increfcount() = 0;
    virtual void setAttributes( const KWargs& ) = 0;
    virtual KWargs collectAttributes() const = 0;
    virtual void setAuth( bool b ) = 0;
    virtual const std::string generateId( const std::string& ) NOT_IMPLEMENTED;
  };

  class FoliaImpl: public virtual FoliaElement {
  public:
    //Constructor
    FoliaImpl( const properties& p, Document* = 0 );
    // static element Constructor
    static FoliaElement *createElement( Document *, ElementType );
    static FoliaElement *createElement( Document *, const std::string&  );
    virtual ~FoliaImpl();

    void classInit( const std::string& s="" ){
      init(); // virtual init
      if ( !s.empty() ){
	// this enables the init of empty classes, which hopefully get their
	// attributes in a later state
	setAttributes(  getArgs( s ) );
      }
    }

    void classInit( const KWargs& a ){
      init(); // virtual init
      setAttributes( a ); // also virtual!
    }

    bool has_base( ElementType ) const { return false; };
    //functions regarding contained data
    size_t size() const { return data.size(); };
    FoliaElement* index( size_t ) const;
    FoliaElement* rindex( size_t ) const;

    bool isinstance( ElementType et ) const {
      return et == element_id();
    }
    template <typename F>
      bool isinstance() const {
      F obj((Document*)0);
      return element_id() == obj.element_id();
    }

    void fixupDoc( Document* );
    FoliaElement *parent() const { return _parent; };
    void setParent( FoliaElement *p ) { _parent = p ; };
    bool acceptable( ElementType ) const;
    bool addable( const FoliaElement * ) const;
    FoliaElement *append( FoliaElement* );
    FoliaElement *postappend( ) { return this; };
    void remove( size_t, bool = true );
    void remove( FoliaElement *, bool = true );
    std::vector<FoliaElement*> findreplacables( FoliaElement * ) const;
    void replace( FoliaElement * );
    FoliaElement* replace( FoliaElement *, FoliaElement* );

    // Sentences
    Sentence *addSentence( const KWargs& );

    // Selections
    template <typename F>
      std::vector<F*> select( bool recurse = true ) const {
      return FoliaElement::select<F>(recurse);
    }

    template <typename F>
      std::vector<F*> select( const std::string& st,
			      const std::set<ElementType>& exclude,
			      bool recurse = true ) const {
      return FoliaElement::select<F>( st, exclude, recurse );
    }

    template <typename F>
      std::vector<F*> select( const std::string& st,
			      bool recurse = true ) const {
      return FoliaElement::select<F>( st, recurse );
    }

    template <typename F>
      std::vector<F*> select( const char* st,
			      bool recurse = true ) const {
      return FoliaElement::select<F>( st, recurse );
    }

    template <typename F>
      std::vector<F*> select( const std::set<ElementType>& exclude,
			      bool recurse = true ) const {
      return FoliaElement::select<F>( exclude, recurse );
    }

    const std::string annotator( ) const { return _annotator; };
    void annotator( const std::string& a ) { _annotator = a; };
    AnnotatorType annotatortype() const { return _annotator_type; };
    void annotatortype( AnnotatorType t ) { _annotator_type =  t; };

    template <typename F>
      F *addAnnotation( const KWargs& args ) {
      return FoliaElement::addAnnotation<F>( args );
    }

    template <typename F>
      std::vector<F*> annotations( const std::string& s = "" ) const {
      return FoliaElement::annotations<F>( s );
    }

    template <typename F>
      bool hasannotation( const std::string& st = "" ) const {
      return FoliaElement::hasannotation<F>(st);
    }

    template <typename F>
      F *annotation( const std::string& st = "" ) const {
      return FoliaElement::annotation<F>(st);
    }

    // Span annotations
    std::vector<AbstractSpanAnnotation*> selectSpan() const;

    // features
    std::vector<std::string> feats( const std::string& ) const;
    const std::string feat( const std::string& ) const;

    //XML (de)serialisation
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool = false ) const;

    // text/string content

    const std::string str( const std::string& = "current" ) const;
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;

    const UnicodeString phon( const std::string& = "current",
			      bool = false ) const;
    const UnicodeString deeptext( const std::string& = "current",
				  bool = false ) const;
    const UnicodeString deepphon( const std::string& = "current" ) const;

    // Word
    const Word* resolveword( const std::string& ) const { return 0; };
    Word *addWord( const KWargs& );

    // TextContent
    TextContent *textcontent( const std::string& = "current" ) const;
    // PhonContent
    PhonContent *phoncontent( const std::string& = "current" ) const;

    // properties
    const std::string& getTextDelimiter( bool retaintok=false ) const;
    void setDateTime( const std::string& );
    const std::string getDateTime() const;
    const std::string pos( const std::string& = "" ) const ;
    const std::string lemma( const std::string& = "" ) const;
    const std::string cls() const { return _class; };
    const std::string sett() const { return _set; };
    const std::string n() const { return _n; };
    const std::string id() const { return _id; };
    const std::string href() const { return _href; };
    const std::string begintime() const { return _begintime; };
    const std::string endtime() const { return _endtime; };
    const std::string speech_src() const;
    const std::string speech_speaker() const;

    double confidence() const { return _confidence; };
    void confidence( double d ) { _confidence = d; };

    // generic properties
    ElementType element_id() const;
    const std::string classname() const;
    size_t occurrences() const;
    size_t occurrences_per_set() const;
    Attrib required_attributes() const;
    Attrib optional_attributes() const;
    const std::string& xmltag() const;
    AnnotationType::AnnotationType annotation_type() const;
    const std::set<ElementType>& accepted_data() const;
    bool printable() const;
    bool speakable() const;
    bool xlink() const;


    Document *doc() const { return mydoc; };

    bool checkAtts();

    std::vector<FoliaElement*> select( ElementType elementtype,
				       bool = true ) const;
    std::vector<FoliaElement*> select( ElementType elementtype,
				       const std::set<ElementType>& ,
				       bool = true ) const;
    std::vector<FoliaElement*> select( ElementType elementtype,
				       const std::string&,
				       bool = true ) const;
    std::vector<FoliaElement*> select( ElementType elementtype,
				       const std::string&,
				       const std::set<ElementType>& ,
				       bool = true ) const;
    int refcount() const { return _refcount; };
    void increfcount() { ++_refcount; };
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    void setAuth( bool b ){ _auth = b; };

  protected:
    xmlNs *foliaNs() const;
    std::vector<FoliaElement*> data;
    FoliaElement *_parent;
    bool _auth;
    Document *mydoc;
    std::string _id;
    std::string _class;
    std::string _set;
    std::string _href;
    std::string _src;
  private:
    void addFeatureNodes( const KWargs& args );
    std::string _annotator;
    std::string _n;
    std::string _datetime;
    std::string _begintime;
    std::string _endtime;
    std::string _speaker;
    AnnotatorType _annotator_type;
    double _confidence;
    int _refcount;
    const properties& _props;
  };

  bool isSubClass( const ElementType e1, const ElementType e2 );

  bool isSubClass( const FoliaElement *e1, const FoliaElement *e2 );

  template <typename T1, typename T2>
    bool isSubClass(){
    T1 t1((Document*)0);
    return dynamic_cast<T2*>(&t1) != 0;
  }

  bool operator==( const FoliaElement&, const FoliaElement& );
  inline bool operator!=( const FoliaElement& e1, const FoliaElement& e2 ){
    return !( e1 == e2 );
  }

  inline size_t len( const FoliaElement *e ) {
    return e->size(); }

  template <typename T>
    inline size_t len( const std::vector<T>& v ) {
    return v.size(); }

  inline const std::string str( const FoliaElement *e,
				const std::string& cls = "current" ) {
    return e->str( cls ); }

  inline const UnicodeString text( const FoliaElement *e,
				   const std::string& cls = "current" ) {
    if ( e )
      return e->text( cls );
    else
      throw ValueError( "text() for empty element" );
  }

  inline const UnicodeString unicode( const FoliaElement *e ) {
    return e->unicode(); }

  inline bool isinstance( const FoliaElement *e, ElementType t ) {
    return e->isinstance( t ); }

  class AllowGenerateID: public virtual FoliaElement {
  public:
    void setMaxId( FoliaElement * );
    int getMaxId( const std::string& );
    const std::string generateId( const std::string& tag ){
      return IDgen( tag, this );
    }
  private:
    std::string IDgen( const std::string&, const FoliaElement* );
    std::map<std::string, int> maxid;
  };

  class AllowCorrection: public virtual FoliaElement {
  public:
    Correction *correct( const std::vector<FoliaElement*>& v1,
			 const std::vector<FoliaElement*>& v2,
 			 const std::vector<FoliaElement*>& v3,
			 const std::vector<FoliaElement*>& v4,
			 const KWargs& args );
    Correction *correct( FoliaElement* o,
			 FoliaElement* n,
			 const KWargs& args ) {
      const std::vector<FoliaElement*> v;
      return correct( o, n, v, args );
    }
    Correction* correct( FoliaElement*,
			 FoliaElement*,
			 const std::vector<FoliaElement*>&,
			 const KWargs& );
    Correction *correct( const std::string& = "" );
  };

  class AllowAnnotation: public virtual FoliaElement {
  public:
    bool allowannotations() const { return true; };
    PosAnnotation *addPosAnnotation( const KWargs& );
    PosAnnotation *getPosAnnotations( const std::string&,
				      std::vector<PosAnnotation*>& ) const;
    LemmaAnnotation *addLemmaAnnotation( const KWargs& );
    LemmaAnnotation *getLemmaAnnotations( const std::string&,
					  std::vector<LemmaAnnotation*>& ) const;
  };

  class AbstractStructureElement: public FoliaImpl,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractStructureElement as a real node!!
    AbstractStructureElement( const properties& props, Document *d=0 ):
      FoliaImpl( props, d ){ classInit(); };
    AbstractStructureElement( Document *d=0 ):
      AbstractStructureElement( PROPS, d ){};
    public:
      const std::string str( const std::string& = "current" ) const;
      std::vector<Alternative *> alternatives( ElementType = BASE,
					       const std::string& = "" ) const;

      FoliaElement *append( FoliaElement* );
      std::vector<Paragraph*> paragraphs() const;
      std::vector<Sentence*> sentences() const;
      std::vector<Word*> words() const;
      Sentence *sentences( size_t ) const;
      Sentence *rsentences( size_t ) const;
      Paragraph *paragraphs( size_t ) const;
      Paragraph *rparagraphs( size_t ) const;
      Word *words( size_t ) const;
      Word *rwords( size_t ) const;
      const Word* resolveword( const std::string& ) const;
  private:
      static properties PROPS;
    };

  class AbstractAnnotation: public FoliaImpl {
    friend void static_init();
  protected:
    // DO NOT USE AbstractAnnotation as a real node!!
  AbstractAnnotation( const properties& props, Document *d=0 ):
    FoliaImpl( props, d ){};
  AbstractAnnotation( Document *d=0 ):
    AbstractAnnotation( PROPS, d ){};
  private:
    static properties PROPS;
  };

  class AbstractTokenAnnotation:
    public AbstractAnnotation,
    public AllowGenerateID
    {
      friend void static_init();
    protected:
    // DO NOT USE AbstractTokenAnnotation as a real node!!
    AbstractTokenAnnotation( const properties& props, Document *d=0 ):
      AbstractAnnotation( props, d ){ classInit(); };
    AbstractTokenAnnotation( Document *d=0 ):
      AbstractTokenAnnotation( PROPS, d ){};
    private:
      static properties PROPS;
    };

  class AbstractSpanAnnotation: public AbstractAnnotation,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractSpanAnnotation as a real node!!
    AbstractSpanAnnotation( const properties& props, Document *d=0 ):
      AbstractAnnotation( props, d ){ classInit(); };
    AbstractSpanAnnotation( Document *d=0 ):
      AbstractSpanAnnotation( PROPS, d ){};
    public:
      xmlNode *xml( bool, bool=false ) const;
      FoliaElement *append( FoliaElement* );

      std::vector<FoliaElement*> wrefs() const;
      FoliaElement *wrefs( size_t ) const;

    private:
      static properties PROPS;
    };

  const std::string EMPTY_STRING = "";

  class AbstractTextMarkup: public AbstractAnnotation {
    friend void static_init();
  protected:
    // DO NOT USE AbstractTextMarkup as a real node!!
  AbstractTextMarkup( const properties& props, Document *d=0 ):
    AbstractAnnotation( props, d ){ classInit(); };
  AbstractTextMarkup( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){};
  public:
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const FoliaElement* resolveid() const;
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;
  protected:
    const std::string& getTextDelimiter( bool ) const { return EMPTY_STRING; };
    std::string idref;
  private:
    static properties PROPS;
  };

  class TextMarkupGap: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupGap( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTextMarkup_t;
    }
    static properties PROPS;
  };

  class TextMarkupString: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupString( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ) { classInit(); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTextMarkup_t;
    }
    static properties PROPS;
  };

  class TextMarkupCorrection: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupCorrection( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTextMarkup_t;
    }
    static properties PROPS;
    std::string _original;
  };

  class TextMarkupError: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupError( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTextMarkup_t;
    }
    static properties PROPS;
  };

  class TextMarkupStyle: public AbstractTextMarkup {
    friend void static_init();
  public:
  TextMarkupStyle( Document *d=0 ):
    AbstractTextMarkup( PROPS, d ){ classInit(); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTextMarkup_t;
    }
    static properties PROPS;
  };

  class TextContent: public FoliaImpl {
    friend void static_init();
  public:
  TextContent( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d){ classInit( s ); }
  TextContent( const std::string& s="" ):
    TextContent( 0, s ){}
  TextContent( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); }
  TextContent( const KWargs& a ):
    TextContent( 0, a ){}
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const std::string str( const std::string& = "current" ) const;
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;
    int offset() const { return _offset; };
    TextContent *postappend();
    std::vector<FoliaElement*> findreplacables( FoliaElement * ) const;
  private:
    static properties PROPS;
    int _offset = -1;
  };

  class PhonContent: public FoliaImpl {
    friend void static_init();
  public:
  PhonContent( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d){ classInit( s ); }
  PhonContent( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); }
  PhonContent( const std::string& s="" ):
    PhonContent(0,s){}
  PhonContent( const KWargs& a ):
    PhonContent(0,a){}
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const UnicodeString phon( const std::string& = "current",
			      bool = false ) const;
    int offset() const { return _offset; };
  private:
    static properties PROPS;
    int _offset;
  };

  class FoLiA: public FoliaImpl {
    friend void static_init();
  public:
  FoLiA( Document *d, const std::string& s=""):
    FoliaImpl(PROPS,d) { classInit( s ); }
  FoLiA( Document *d, const KWargs& a ):
    FoliaImpl(PROPS, d) { classInit( a ); }
  FoLiA( const std::string& s="" ):
    FoLiA(0,s) {}
  FoLiA( const KWargs& a ):
    FoLiA(0,a) {}

  private:
    static properties PROPS;
  };

  class DCOI: public FoliaImpl {
    friend void static_init();
  public:
  DCOI( Document *d, const std::string& s=""):
    FoliaImpl(PROPS, d) { classInit( s ); }
  DCOI( Document *d, const KWargs& a ):
    FoliaImpl(PROPS, d) { classInit( a ); }
  DCOI( const std::string& s=""):
    DCOI(0,s) {};
  DCOI( const KWargs& a ):
    DCOI(0,a) {};

  private:
    static properties PROPS;
  };

  class Head: public AbstractStructureElement {
    friend void static_init();
  public:
  Head( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) {  classInit( s ); }
  Head( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); }
  Head( const std::string& s=""):
    Head( 0, s ) {}
  Head( const KWargs& a ):
    Head( 0, a ) {}

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class TableHead: public AbstractStructureElement {
    friend void static_init();
  public:
  TableHead( const std::string& s=""):
    TableHead( 0, s ) {};
  TableHead( const KWargs& a ):
    TableHead( 0, a ) {};
  TableHead( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) {  classInit( s ); }
  TableHead( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Table: public AbstractStructureElement {
    friend void static_init();
  public:
  Table( const std::string& s=""):
    Table( 0, s ) {};
  Table( const KWargs& a ):
    Table( 0, a ) {};
  Table( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) {  classInit( s ); };
  Table( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Row: public AbstractStructureElement {
    friend void static_init();
  public:
  Row( const std::string& s=""):
    Row( 0, s ) {};
  Row( const KWargs& a ):
    Row( 0, a ) {};
  Row( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) {  classInit( s ); };
  Row( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Cell: public AbstractStructureElement {
    friend void static_init();
  public:
  Cell( const std::string& s=""):
    Cell( 0, s ) {}
  Cell( const KWargs& a ):
    Cell( 0, a ) {}
  Cell( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) {  classInit( s ); };
  Cell( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) {  classInit( a ); };
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Gap: public FoliaImpl {
    friend void static_init();
  public:
  Gap( const std::string& s=""):
    Gap( 0, s ) {}
  Gap( const KWargs& a ):
    Gap( 0, a ) {}
  Gap( Document *d, const std::string& s=""):
    FoliaImpl(PROPS,d) { classInit( s ); }
  Gap( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d) { classInit( a ); }

    const std::string content() const;
  private:
    static properties PROPS;
  };

  class Content: public FoliaImpl {
    friend void static_init();
  public:
  Content( const std::string& s=""):
    Content( 0, s ) {}
  Content( const KWargs& a ):
    Content( 0, a ) {}
  Content( Document *d, const std::string& s=""):
    FoliaImpl(PROPS,d) { classInit( s ); }
  Content( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool = false ) const;
    const std::string content() const { return value; };

  private:
    static properties PROPS;
    std::string value;
  };

  class Metric: public FoliaImpl {
    friend void static_init();
  public:
  Metric( const std::string& s=""):
    Metric( 0, s ) {}
  Metric( const KWargs& a ):
    Metric( 0, a ) {}
  Metric( Document *d, const std::string& s=""):
    FoliaImpl(PROPS,d) { classInit( s ); }
  Metric( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d) { classInit( a ); }

  private:
    static properties PROPS;
  };

  class Division: public AbstractStructureElement {
    friend void static_init();
  public:
  Division( const std::string& s=""):
    Division( 0, s ) {};
  Division( const KWargs& a ):
    Division( 0, a ) {};
  Division( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ) { classInit( s ); };
  Division( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) { classInit( a ); };
    Head *head() const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class LineBreak: public AbstractStructureElement {
    friend void static_init();
  public:
  LineBreak( const std::string& s=""):
    LineBreak( 0, s ) {}
  LineBreak( const KWargs& a ):
    LineBreak( 0, a ) {}
  LineBreak( Document *d,  const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  LineBreak( Document *d,  const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const {
      return "";
    }
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class WhiteSpace: public AbstractStructureElement {
    friend void static_init();
  public:
  WhiteSpace( const std::string& s=""):
    WhiteSpace( 0, s ) {}
  WhiteSpace( const KWargs& a ):
    WhiteSpace( 0, a ) {}
  WhiteSpace( Document *d,  const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  WhiteSpace( Document *d,  const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Word: public AbstractStructureElement {
    friend void static_init();
  public:
  Word( Document *d, const std::string& s=""):
    Word( PROPS, d, s ){}
  Word( const std::string& s="" ):
    Word( PROPS, 0, s ){}
  Word( const properties& p, const std::string& s="" ):
    Word( p, 0, s ){}
  Word( const KWargs& a ):
    Word( 0, a ) {}
  Word( const properties& p, Document *d, const std::string& s=""):
    AbstractStructureElement( p, d ) { classInit( s ); }
  Word( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ) { classInit( a ); };

    Correction *split( FoliaElement *, FoliaElement *,
		       const std::string& = "" );
    Sentence *sentence() const;
    Paragraph *paragraph() const;
    Division *division() const;
    std::vector<Morpheme *> morphemes( const std::string& ="" ) const;
    Morpheme* morpheme( size_t, const std::string& ="" ) const;
    Correction *incorrection() const;
    Word *previous() const;
    Word *next() const;
    std::vector<Word*> context( size_t,
				const std::string& ="" ) const;
    std::vector<Word*> leftcontext( size_t,
				    const std::string& = "" ) const;
    std::vector<Word*> rightcontext( size_t,
				     const std::string& ="" ) const;
    std::vector<AbstractSpanAnnotation*> findspans( ElementType,
					  const std::string& = "" ) const;
    FoliaElement *append( FoliaElement *);
    const Word* resolveword( const std::string& ) const;
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const std::string& getTextDelimiter( bool=false) const;
    MorphologyLayer *addMorphologyLayer( const KWargs& );
    MorphologyLayer *getMorphologyLayers( const std::string&,
					  std::vector<MorphologyLayer*>& ) const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
    bool _space = true;
  };

  class Part: public AbstractStructureElement {
    friend void static_init();
  public:
  Part( const std::string& s="" ):
    Part( 0, s ){}
  Part( const KWargs& a ):
    Part( 0, a ){}
  Part( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Part( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class String:
    public AbstractTokenAnnotation,
    public AllowAnnotation,
    public AllowCorrection {
    friend void static_init();
  public:
  String( const std::string& s="" ):
    String( 0, s ) {}
  String( const KWargs& a ):
    String( 0, a ) {}
  String( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  String( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t;
    }
    static properties PROPS;
    };

  class PlaceHolder: public Word {
    friend void static_init();
  public:
  PlaceHolder( const std::string& s=""):
    PlaceHolder( 0, s ) {}
  PlaceHolder( const KWargs& a ):
    PlaceHolder( 0, a ) {}
  PlaceHolder( Document *d, const std::string& s=""):
    Word( PROPS, d ){ classInit( s ); }
  PlaceHolder( Document *d, const KWargs& a ):
    Word( PROPS, d ){ classInit( a ); }

    void setAttributes( const KWargs& );
  private:
    bool has_base( ElementType e ) const {
      return e == Word_t || e == AbstractStructureElement_t;
      }
    static properties PROPS;
  };

  class Sentence: public AbstractStructureElement {
    friend void static_init();
  public:
  Sentence( const std::string& s=""):
    Sentence( 0, s ) {}
  Sentence( const KWargs& a ):
    Sentence( 0, a ) {}
  Sentence( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Sentence( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    Correction *splitWord( FoliaElement *, FoliaElement *,
			   FoliaElement *, const KWargs& );
    Correction *mergewords( FoliaElement *,
			    const std::vector<FoliaElement *>&,
			    const std::string& = "" );
    Correction *deleteword( FoliaElement *, const std::string& args );
    Correction *insertword( FoliaElement *, FoliaElement *,
			    const std::string& args );
    std::vector<Word*> wordParts() const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
    Correction *correctWords( const std::vector<FoliaElement *>&,
			      const std::vector<FoliaElement *>&,
			      const KWargs& );
  };

  class Speech: public AbstractStructureElement {
    friend void static_init();
  public:
  Speech( const std::string& s=""):
    Speech( 0, s ) {}
  Speech( const KWargs& a ):
    Speech( 0, a ) {}
  Speech( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Speech( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Text: public AbstractStructureElement {
    friend void static_init();
  public:
  Text( const std::string& s=""):
    Text( 0, s ) {}
  Text( const KWargs& a ):
    Text( 0, a ) {}
  Text( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Text( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Utterance: public AbstractStructureElement {
    friend void static_init();
  public:
  Utterance( const std::string& s=""):
    Utterance( 0, s ) {}
  Utterance( const KWargs& a ):
    Utterance( 0, a ) {}
  Utterance( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Utterance( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Event: public AbstractStructureElement {
    friend void static_init();
  public:
  Event( const std::string& s=""):
    Event( 0, s ) {}
  Event( const KWargs& a ):
    Event( 0, a ) {}
  Event( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Event( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Caption: public AbstractStructureElement {
    friend void static_init();
  public:
  Caption( const std::string& s="" ):
    Caption( 0, s ) {}
  Caption( const KWargs& a ):
    Caption( 0, a ) {}
  Caption( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Caption( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Label: public AbstractStructureElement {
    friend void static_init();
  public:
  Label( const std::string& s=""):
    Label( 0, s ) {}
  Label( const KWargs& a ):
    Label( 0, a ) {}
  Label( Document *d, const std::string& s="" ):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Label( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Item: public AbstractStructureElement {
    friend void static_init();
  public:
  Item( const std::string& s=""):
    Item( 0, s ) {}
  Item( const KWargs& a ):
    Item( 0, a ) {}
  Item( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Item( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class List: public AbstractStructureElement {
    friend void static_init();
  public:
  List( const std::string& s=""):
    List( 0, s ) {}
  List( const KWargs& a ):
    List( 0, a ) {}
  List( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  List( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Figure: public AbstractStructureElement {
    friend void static_init();
  public:
  Figure( const std::string& s=""):
    Figure( 0, s ) {}
  Figure( const KWargs& a ):
    Figure( 0, a ) {}
  Figure( Document *d, const std::string& s="" ):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Figure( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    const std::string src() const { return _src; };
    const UnicodeString caption() const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Paragraph: public AbstractStructureElement {
    friend void static_init();
  public:
  Paragraph( const std::string& s=""):
    Paragraph( 0, s ) {}
  Paragraph( const KWargs& a ):
    Paragraph( 0, a ) {}
  Paragraph( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Paragraph( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Alternative: public AbstractStructureElement {
    friend void static_init();
  public:
  Alternative( const std::string& s=""):
    Alternative( 0, s ) {}
  Alternative( const KWargs& a ):
    Alternative( 0, a ) {}
  Alternative( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Alternative( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    void init();
    static properties PROPS;
  };


  class AlternativeLayers: public FoliaImpl {
    friend void static_init();
  public:
  AlternativeLayers( const std::string& s=""):
    AlternativeLayers( 0, s ) {}
  AlternativeLayers( const KWargs& a ):
    AlternativeLayers( 0, a ) {}
  AlternativeLayers( Document *d, const std::string& s=""):
    FoliaImpl(PROPS,d){ classInit( s ); }
  AlternativeLayers( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); }

  private:
    void init();
    static properties PROPS;
  };

  template <typename F>
    F *FoliaElement::addAlternative( const KWargs& args ){
    KWargs kw;
    std::string id = generateId( "alt" );
    kw["id"] = id;
    F *res = 0;
    Alternative *alt = 0;
    try {
      alt = new Alternative( doc(), kw );
      res = alt->addAnnotation<F>( args );
    }
    catch( std::exception& ){
      delete alt;
      throw;
    }
    append( alt );
    return res;
  }

  template <typename F>
    F *FoliaElement::addAlternative(){
    KWargs numb;
    return addAlternative<F>( numb );
  }

  class PosAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  PosAnnotation( const std::string& s=""):
    PosAnnotation( 0, s ) {}
  PosAnnotation( const KWargs& a ):
    PosAnnotation( 0, a ) {}
  PosAnnotation( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  PosAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t;
    }
    static properties PROPS;
  };

  class LemmaAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  LemmaAnnotation( const std::string& s=""):
    LemmaAnnotation( 0, s ) {}
  LemmaAnnotation( const KWargs& a ):
    LemmaAnnotation( 0, a ) {}
  LemmaAnnotation( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  LemmaAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t;
    }
    static properties PROPS;
  };

  class LangAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  LangAnnotation( const std::string& s=""):
    LangAnnotation( 0, s ) {}
  LangAnnotation( const KWargs& a ):
    LangAnnotation( 0, a ) {}
  LangAnnotation( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  LangAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t;
    }
    static properties PROPS;
  };

  class Phoneme: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  Phoneme( const std::string& s=""):
    Phoneme( 0, s ) {}
  Phoneme( const KWargs& a ):
    Phoneme( 0, a ) {}
  Phoneme( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  Phoneme( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t;
    }
    static properties PROPS;
  };

  class DomainAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  DomainAnnotation(  const std::string& s=""):
    DomainAnnotation( 0, s ) {}
  DomainAnnotation(  const KWargs& a ):
    DomainAnnotation( 0, a ) {}
  DomainAnnotation( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  DomainAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t;
    }
    static properties PROPS;
  };

  class SenseAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  SenseAnnotation( const std::string& s="" ):
    SenseAnnotation( 0, s ) {}
  SenseAnnotation( const KWargs& a ):
    SenseAnnotation( 0, a ) {}
  SenseAnnotation( Document *d, const std::string& s="" ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  SenseAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t;
    }
    static properties PROPS;
  };

  class SubjectivityAnnotation: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  SubjectivityAnnotation( const std::string& s="" ):
    SubjectivityAnnotation( 0, s ) {}
  SubjectivityAnnotation( const KWargs& a ):
    SubjectivityAnnotation( 0, a ) {}
  SubjectivityAnnotation( Document *d, const std::string& s="" ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  SubjectivityAnnotation( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t;
    }
    static properties PROPS;
  };

  class Quote: public AbstractStructureElement {
    friend void static_init();
  public:
  Quote( const std::string& s="" ):
    Quote( 0, s ) {}
  Quote( const KWargs& a ):
    Quote( 0, a ) {}
  Quote( Document *d, const std::string& s="" ):
    AbstractStructureElement( PROPS, d ){ classInit( s ); };
  Quote( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); };
    FoliaElement *append( FoliaElement *);
    std::vector<Word*> wordParts() const;
    const std::string& getTextDelimiter( bool=false) const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Feature: public FoliaImpl {
    friend void static_init();
  public:
  Feature( const properties&p, Document *d, const std::string& s=""):
    FoliaImpl(p,d){ classInit( s ); }
  Feature( const std::string& s=""):
    Feature(PROPS, 0, s ){}
  Feature( const properties& p, const std::string& s=""):
    Feature(p,0,s){}
  Feature( Document *d, const std::string& s=""):
    Feature(PROPS,d,s){}

  Feature( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); }
  Feature( const KWargs& a ):
    Feature(0, a){}

    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
    const std::string subset() const { return _subset; };
  protected:
    std::string _subset;

  private:
    static properties PROPS;
  };

  class BeginDateTimeFeature: public Feature {
    friend void static_init();
  public:
  BeginDateTimeFeature( const std::string& s="" ):
    BeginDateTimeFeature( 0, s ) {}
  BeginDateTimeFeature( const KWargs& a ):
    BeginDateTimeFeature( 0, a ) {}
  BeginDateTimeFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  BeginDateTimeFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class EndDateTimeFeature: public Feature {
    friend void static_init();
  public:
  EndDateTimeFeature( const std::string& s="" ):
    EndDateTimeFeature( 0, s ) {}
  EndDateTimeFeature( const KWargs& a ):
    EndDateTimeFeature( 0, a ) {}
  EndDateTimeFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  EndDateTimeFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class SynsetFeature: public Feature {
    friend void static_init();
  public:
  SynsetFeature( const std::string& s="" ):
    SynsetFeature( 0, s ) {}
  SynsetFeature( const KWargs& a ):
    SynsetFeature( 0, a ) {}
  SynsetFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  SynsetFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class ActorFeature: public Feature {
    friend void static_init();
  public:
  ActorFeature( const std::string& s="" ):
    ActorFeature( 0, s ) {}
  ActorFeature( const KWargs& a ):
    ActorFeature( 0, a ) {}
  ActorFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  ActorFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class HeadFeature: public Feature {
    friend void static_init();
  public:
  HeadFeature( const std::string& s="" ):
    HeadFeature(0, s ) {}
  HeadFeature( const KWargs& a ):
    HeadFeature(0, a ) {}
  HeadFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  HeadFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class ValueFeature: public Feature {
    friend void static_init();
  public:
  ValueFeature( const std::string& s="" ):
    ValueFeature( 0, s ) {}
  ValueFeature( const KWargs& a ):
    ValueFeature( 0, a ) {}
  ValueFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  ValueFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class FunctionFeature: public Feature {
    friend void static_init();
  public:
  FunctionFeature( const std::string& s="" ):
    FunctionFeature( 0, s  ) {}
  FunctionFeature( const KWargs& a ):
    FunctionFeature( 0, a  ) {}
  FunctionFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  FunctionFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class TimeFeature: public Feature {
    friend void static_init();
  public:
  TimeFeature( const std::string& s="" ):
    TimeFeature( 0, s ) {}
  TimeFeature( const KWargs& a ):
    TimeFeature( 0, a ) {}
  TimeFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  TimeFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class LevelFeature: public Feature {
    friend void static_init();
  public:
  LevelFeature( const std::string& s="" ):
    LevelFeature( 0, s ) {}
  LevelFeature( const KWargs& a ):
    LevelFeature( 0, a ) {}
  LevelFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  LevelFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class ModalityFeature: public Feature {
    friend void static_init();
  public:
  ModalityFeature( const std::string& s="" ):
    ModalityFeature( 0, s ) {}
  ModalityFeature( const KWargs& a ):
    ModalityFeature( 0, a ) {}
  ModalityFeature( Document *d, const std::string& s="" ):
    Feature( PROPS, d ){ classInit( s ); }
  ModalityFeature( Document *d, const KWargs& a ):
    Feature( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == Feature_t;
    }
    static properties PROPS;
    void init();
  };

  class WordReference: public FoliaImpl {
    friend void static_init();
  public:
  WordReference( const std::string& s="" ):
    WordReference( 0, s ) {}
  WordReference( const KWargs& a ):
    WordReference( 0, a ) {}
  WordReference( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d){ classInit( s ); }
  WordReference( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); }

  private:
    static properties PROPS;
    void init();
    FoliaElement* parseXml( const xmlNode *node );
  };

  class Alignment: public FoliaImpl {
    friend void static_init();
  public:
  Alignment( const std::string& s="" ):
    Alignment( 0, s ) {}
  Alignment( const KWargs& a ):
    Alignment( 0, a ) {}
  Alignment( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d){ classInit( s ); }
  Alignment( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); }

    std::vector<FoliaElement *>resolve() const;

  private:
    static properties PROPS;
  };

  class AlignReference: public FoliaImpl {
    friend void static_init();
    friend class Alignment;
  public:
  AlignReference( const std::string& s="" ):
    AlignReference( 0, s ) {}
  AlignReference( const KWargs& a ):
    AlignReference( 0, a ) {}
  AlignReference( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d){ classInit( s ); }
  AlignReference( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d){ classInit( a ); }

    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );

  private:
    static properties PROPS;
    FoliaElement* parseXml( const xmlNode *node );
    FoliaElement *resolve( const Alignment *ref ) const;
    std::string refId;
    std::string ref_type;
    std::string _t;
  };

  class SyntacticUnit: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  SyntacticUnit( const std::string& s="" ):
    SyntacticUnit( 0, s ) {}
  SyntacticUnit( const KWargs& a ):
    SyntacticUnit( 0, a ) {}
  SyntacticUnit( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  SyntacticUnit( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class Chunk: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Chunk( const std::string& s="" ):
    Chunk( 0, s ) {}
  Chunk( const KWargs& a ):
    Chunk( 0, a ) {}
  Chunk( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  Chunk( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class Entity: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Entity( const std::string& s="" ):
    Entity( 0, s ) {}
  Entity( const KWargs& a ):
    Entity( 0, a ) {}
  Entity( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  Entity( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class Headwords: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Headwords( const std::string& s="" ):
    Headwords( 0, s ) {}
  Headwords( const KWargs& a ):
    Headwords( 0, a ) {}
  Headwords( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  Headwords( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class DependencyDependent: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  DependencyDependent( const std::string& s="" ):
    DependencyDependent( 0, s ) {}
  DependencyDependent( const KWargs& a ):
    DependencyDependent( 0, a ) {}
  DependencyDependent( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  DependencyDependent( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class Dependency: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  Dependency( const std::string& s="" ):
    Dependency( 0, s ) {}
  Dependency( const KWargs& a ):
    Dependency( 0, a ) {}
  Dependency( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  Dependency( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

    Headwords *head() const;
    DependencyDependent *dependent() const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class CoreferenceLink: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  CoreferenceLink( const std::string& s="" ):
    CoreferenceLink( 0, s ) {}
  CoreferenceLink( const KWargs& a ):
    CoreferenceLink( 0, a ) {}
  CoreferenceLink( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  CoreferenceLink( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class CoreferenceChain: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  CoreferenceChain( const std::string& s="" ):
    CoreferenceChain( 0, s ) {}
  CoreferenceChain( const KWargs& a ):
    CoreferenceChain( 0, a ) {}
  CoreferenceChain( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  CoreferenceChain( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class SemanticRole: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  SemanticRole( const std::string& s="" ):
    SemanticRole( 0, s ) {}
  SemanticRole( const KWargs& a ):
    SemanticRole( 0, a ) {}
  SemanticRole( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  SemanticRole( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class AbstractAnnotationLayer: public FoliaImpl,
    public AllowGenerateID,
    public AllowAnnotation,
    public AllowCorrection
    {
      friend void static_init();
    protected:
      // DO NOT USE AbstractAnnotationLayer as a real node!!
    AbstractAnnotationLayer( Document *d = 0 ):
      AbstractAnnotationLayer( PROPS, d ) {}
    AbstractAnnotationLayer( const properties& props, Document *d = 0 ):
      AbstractAnnotationLayer( props, d, "" ) {}
    AbstractAnnotationLayer( const properties& props, const KWargs& a ):
      AbstractAnnotationLayer( props, 0, a ) {}
    AbstractAnnotationLayer( const properties& props, Document *d, const std::string& s ):
      FoliaImpl( props, d ) { classInit( s ); };
    AbstractAnnotationLayer( const properties& props, Document *d, const KWargs& a ):
      FoliaImpl( props, d ) { classInit( a ); };
  public:
    AbstractSpanAnnotation *findspan( const std::vector<FoliaElement*>& ) const;
    FoliaElement *append( FoliaElement * );
  private:
    static properties PROPS;
    void assignset( FoliaElement * );
  };

  class AbstractCorrectionChild: public FoliaImpl {
    friend void static_init();
  protected:
    // DO NOT USE AbstractCorrectionChild as a real node!!
  AbstractCorrectionChild( const properties& props, Document *d=0 ):
    FoliaImpl( props, d ){ classInit(); };
  AbstractCorrectionChild( Document *d=0 ):
    AbstractCorrectionChild( PROPS, d ){};
  private:
    static properties PROPS;
  };

  class New: public AbstractCorrectionChild {
    friend void static_init();
  public:
  New( const std::string& s=""):
    New( 0, s ) {}
  New( const KWargs& a ):
    New( 0, a ) {}
  New( Document *d, const std::string& s=""):
    AbstractCorrectionChild( PROPS, d ) { classInit( s ); }
  New( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractCorrectionChild_t;
    }
    static properties PROPS;
  };

  class Current: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Current( const std::string& s=""):
    Current( 0, s ) {}
  Current( const KWargs& a ):
    Current( 0, a ) {}
  Current( Document *d, const std::string& s=""):
    AbstractCorrectionChild( PROPS, d ) { classInit( s ); }
  Current( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractCorrectionChild_t;
    }
    static properties PROPS;
  };

  class Original: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Original( const std::string& s=""):
    Original( 0, s ) {}
  Original( const KWargs& a ):
    Original( 0, a ) {}
  Original( Document *d, const std::string& s=""):
    AbstractCorrectionChild( PROPS, d ) { classInit( s ); }
  Original( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractCorrectionChild_t;
    }
    static properties PROPS;
    void init();
  };

  class Suggestion: public AbstractCorrectionChild {
    friend void static_init();
  public:
  Suggestion( const std::string& s=""):
    Suggestion( 0, s ) {}
  Suggestion( const KWargs& a ):
    Suggestion( 0, a ) {}
  Suggestion( Document *d, const std::string& s=""):
    AbstractCorrectionChild( PROPS, d ) { classInit( s ); }
  Suggestion( Document *d, const KWargs& a ):
    AbstractCorrectionChild( PROPS, d ) { classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractCorrectionChild_t;
    }
    static properties PROPS;
    void init();
  };

  class Description: public FoliaImpl {
    friend void static_init();
  public:
  Description( const std::string& s=""):
    Description( 0, s ) {}
  Description( const KWargs& a ):
    Description( 0, a ) {}
  Description( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d) { classInit( s ); }
  Description( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d) { classInit( a ); }

    const std::string description() const { return _value; };
    void setAttributes( const KWargs& kwargs );
    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;

  private:
    static properties PROPS;
    std::string _value;
  };

  class XmlComment: public FoliaImpl {
    friend void static_init();
  public:
  XmlComment( const std::string& s=""):
    XmlComment( 0, s ) {}
  XmlComment( const KWargs& a ):
    XmlComment( 0, a ) {}
  XmlComment( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d) { classInit( s ); }
  XmlComment( Document *d, const KWargs& a ):
    FoliaImpl(PROPS,d) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
  private:
    static properties PROPS;
    std::string _value;
  };

  class XmlText: public FoliaImpl {
    friend void static_init();
  public:
  XmlText( const std::string& s=""):
    XmlText( 0, s ) {}
  XmlText( const KWargs& a ):
    XmlText( 0, a ) {}
  XmlText( Document *d, const std::string& s="" ):
    FoliaImpl(PROPS,d) { classInit( s ); }
  XmlText( Document *d, const KWargs& a ):
    FoliaImpl(PROPS, d ) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    xmlNode *xml( bool, bool=false ) const;
    bool setvalue( const std::string& s ) { _value = s; return true; };
    const std::string& getTextDelimiter( bool ) const { return EMPTY_STRING; };
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;
  private:
    static properties PROPS;
    std::string _value; //UTF8 value
  };

  class External: public FoliaImpl {
    friend void static_init();
  public:
  External( const std::string& s=""):
    External( 0, s ) {}
  External( const KWargs& a ):
    External( 0, a ) {}
  External( Document *d, const std::string& s="" ):
    FoliaImpl( PROPS, d ), _include(false) { classInit( s ); }
  External( Document *d, const KWargs& a ):
    FoliaImpl( PROPS, d ), _include(false) { classInit( a ); }

    FoliaElement* parseXml( const xmlNode * );
    void resolve();
    void setAttributes( const KWargs& );
    KWargs collectAttributes() const;
  private:
    static properties PROPS;
    bool _include;
  };

  class Note: public AbstractStructureElement {
    friend void static_init();
  public:
  Note( const std::string& s=""):
    Note( 0, s ) {}
  Note( const KWargs& a ):
    Note( 0, a ) {}
  Note( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Note( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
    std::string refId;
  };

  class Definition: public AbstractStructureElement {
    friend void static_init();
  public:
  Definition( const std::string& s=""):
    Definition( 0, s ) {}
  Definition( const KWargs& a ):
    Definition( 0, a ) {}
  Definition( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Definition( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Term: public AbstractStructureElement {
    friend void static_init();
  public:
  Term( const std::string& s=""):
    Term( 0, s ) {}
  Term( const KWargs& a ):
    Term( 0, a ) {}
  Term( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Term( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Example: public AbstractStructureElement {
    friend void static_init();
  public:
  Example( const std::string& s=""):
    Example( 0, s ) {}
  Example( const KWargs& a ):
    Example( 0, a ) {}
  Example( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Example( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Entry: public AbstractStructureElement {
    friend void static_init();
  public:
  Entry( const std::string& s=""):
    Entry( 0, s ) {}
  Entry( const KWargs& a ):
    Entry( 0, a ) {}
  Entry( Document *d, const std::string& s=""):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Entry( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class Reference: public AbstractStructureElement {
    friend void static_init();
    friend class Note;
  public:
  Reference( const std::string& s="" ):
    Reference( 0, s ) {}
  Reference( const KWargs& a ):
    Reference( 0, a ) {}
  Reference( Document *d, const std::string& s="" ):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Reference( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

    KWargs collectAttributes() const;
    void setAttributes( const KWargs& );
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
    std::string refId;
    std::string ref_type;
  };


  class Correction: public AbstractTokenAnnotation {
    friend void static_init();
  public:
  Correction( const std::string& s=""):
    Correction( 0, s ) {}
  Correction( const KWargs& a ):
    Correction( 0, a ) {}
  Correction( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  Correction( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

    bool hasNew() const;
    bool hasOriginal() const;
    bool hasCurrent() const;
    bool hasSuggestions() const;
    New *getNew() const;
    FoliaElement *getNew( size_t ) const;
    Original *getOriginal() const;
    FoliaElement *getOriginal( size_t ) const;
    Current *getCurrent() const;
    FoliaElement *getCurrent( size_t ) const;
    std::vector<Suggestion*> suggestions() const;
    Suggestion *suggestions( size_t ) const;
    const UnicodeString text( const std::string& = "current",
			      bool = false, bool = false ) const;
    TextContent *textcontent( const std::string& = "current" ) const;
    PhonContent *phoncontent( const std::string& = "current" ) const;
    const std::string& getTextDelimiter( bool=false) const;
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t;
    }
    static properties PROPS;
  };

  class ErrorDetection: public AbstractTokenAnnotation  {
    friend void static_init();
  public:
  ErrorDetection( const std::string& s=""):
    ErrorDetection( 0, s ) {}
  ErrorDetection( const KWargs& a ):
    ErrorDetection( 0, a ) {}
  ErrorDetection( Document *d, const std::string& s=""):
    AbstractTokenAnnotation( PROPS, d ){ classInit( s ); }
  ErrorDetection( Document *d, const KWargs& a ):
    AbstractTokenAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractTokenAnnotation_t;
    }
    static properties PROPS;
  };

  class TimeSegment: public AbstractSpanAnnotation {
    friend void static_init();
  public:
  TimeSegment( const std::string& s="" ):
    TimeSegment( 0, s ) {}
  TimeSegment( const KWargs& a ):
    TimeSegment( 0, a ) {}
  TimeSegment( Document *d, const std::string& s="" ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( s ); }
  TimeSegment( Document *d, const KWargs& a ):
    AbstractSpanAnnotation( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractSpanAnnotation_t;
    }
    static properties PROPS;
  };

  class Morpheme: public AbstractStructureElement {
    friend void static_init();
  public:
  Morpheme( const std::string& s="" ):
    Morpheme( 0, s ) {}
  Morpheme( const KWargs& a ):
    Morpheme( 0, a ) {}
  Morpheme( Document *d, const std::string& s="" ):
    AbstractStructureElement( PROPS, d ){ classInit( s ); }
  Morpheme( Document *d, const KWargs& a ):
    AbstractStructureElement( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractStructureElement_t;
    }
    static properties PROPS;
  };

  class SyntaxLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  SyntaxLayer( const std::string& s=""):
    SyntaxLayer( 0, s ) {}
  SyntaxLayer( const KWargs& a ):
    SyntaxLayer( 0, a ) {}
  SyntaxLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  SyntaxLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class ChunkingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  ChunkingLayer( const std::string& s=""):
    ChunkingLayer( 0, s ) {}
  ChunkingLayer( const KWargs& a ):
    ChunkingLayer( 0, a ) {}
  ChunkingLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  ChunkingLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class EntitiesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  EntitiesLayer( const std::string& s=""):
    EntitiesLayer( 0, s ) {}
  EntitiesLayer( const KWargs& a ):
    EntitiesLayer( 0, a ) {}
  EntitiesLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  EntitiesLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class TimingLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  TimingLayer( const std::string& s=""):
    TimingLayer( 0, s ) {}
  TimingLayer( const KWargs& a ):
    TimingLayer( 0, a ) {}
  TimingLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  TimingLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class MorphologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  MorphologyLayer( const std::string& s="" ):
    MorphologyLayer( 0, s ) {}
  MorphologyLayer( const KWargs& a ):
    MorphologyLayer( 0, a ) {}
  MorphologyLayer( Document *d, const std::string& s="" ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  MorphologyLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class PhonologyLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  PhonologyLayer( const std::string& s="" ):
    PhonologyLayer( 0, s ) {}
  PhonologyLayer( const KWargs& a ):
    PhonologyLayer( 0, a ) {}
  PhonologyLayer( Document *d, const std::string& s="" ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  PhonologyLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class DependenciesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  DependenciesLayer( const std::string& s=""):
    DependenciesLayer( 0, s ) {}
  DependenciesLayer( const KWargs& a ):
    DependenciesLayer( 0, a ) {}
  DependenciesLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  DependenciesLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class CoreferenceLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  CoreferenceLayer( const std::string& s=""):
    CoreferenceLayer( 0, s )  {}
  CoreferenceLayer( const KWargs& a ):
    CoreferenceLayer( 0, a)  {}
  CoreferenceLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  CoreferenceLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }

  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  class SemanticRolesLayer: public AbstractAnnotationLayer {
    friend void static_init();
  public:
  SemanticRolesLayer( const std::string& s=""):
    SemanticRolesLayer( 0, s ) {}
  SemanticRolesLayer( const KWargs& a ):
    SemanticRolesLayer( 0, a ) {}
  SemanticRolesLayer( Document *d, const std::string& s=""):
    AbstractAnnotationLayer( PROPS, d ){ classInit( s ); }
  SemanticRolesLayer( Document *d, const KWargs& a ):
    AbstractAnnotationLayer( PROPS, d ){ classInit( a ); }
  private:
    bool has_base( ElementType e ) const {
      return e == AbstractAnnotationLayer_t;
    }
    static properties PROPS;
  };

  std::string VersionName();
  std::string Version();

} // namespace folia

#endif