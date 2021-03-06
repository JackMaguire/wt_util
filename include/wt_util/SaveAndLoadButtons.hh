#pragma once

#include <wt_util/OnTheFlyFileResource.hh>

//C++
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
#include <memory>
#include <filesystem> //remove
//#include <iostream> //debug

//Wt
#include <Wt/WPushButton.h>
#include <Wt/WLink.h>
#include <Wt/WFileResource.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WMessageBox.h>

//CEREAL
#ifdef WT_UTIL_USE_CEREAL
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#endif

namespace wt_util {

namespace impl {



#ifdef WT_UTIL_USE_CEREAL
/*
  t must have these signatures:

  std::map< std::string, std::string >
  serialize() const {}

  void
  deserialize( std::map< std::string, std::string > const & ) {}
*/
template< typename T >
std::string
serialize( T const & t ){
  std::stringstream ss;
  cereal::BinaryOutputArchive archive( ss );

  archive( std::string("BEGIN") );

  std::map< std::string, std::string > const all_data = t.serialize();
  for( auto const pair : all_data ){
    std::string const & key = pair.first;
    std::string const & value = pair.second;
    archive( key );
    archive( value );
  }

  archive( std::string("END") );
  return ss.str();
}

template< typename T >
void
deserialize( T & t, std::string const & filename, bool const run_asserts ){
  //std::stringstream ss( data );
  std::ifstream ss( filename );
  cereal::BinaryInputArchive unarchive( ss );

  std::string first_token;
  unarchive( first_token );
  if( run_asserts )
    assert( first_token == "BEGIN" ); //Check for corruption

  std::map< std::string, std::string > all_data;

  std::string key;
  std::string value;
  while( true ){
    unarchive( key );
    if( key == "END" ) break;

    if( run_asserts )
      assert( all_data.find( key ) == all_data.end() );

    unarchive( value );
    all_data[ key ] = value;
  }

  t.deserialize( all_data );
}
#else

/////////////
//Raw Strings

/*
  t must have these signatures:

  std::map< std::string, std::string >
  serialize() const {}

  void
  deserialize( std::map< std::string, std::string > const & ) {}
*/
template< typename T >
std::string
serialize( T const & t ){
  std::stringstream ss;

  std::string const delimiter = "@&$#!";
  assert( delimiter.size() < 10 );

  ss << delimiter.size();

  auto && archive =
    [&]( std::string const & s ){
      assert( s.find(delimiter) == std::string::npos );
      ss << delimiter << s;
    };

  archive( std::string("BEGIN") );

  std::map< std::string, std::string > const all_data = t.serialize();
  for( auto const pair : all_data ){
    std::string const & key = pair.first;
    std::string const & value = pair.second;
    archive( key );
    archive( value );
  }

  archive( std::string("END") );
  return ss.str();
}

template< typename T >
void
deserialize( T & t, std::string const & filename, bool const run_asserts ){
  //https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
  std::ifstream ss( filename );
  std::stringstream strStream;
  strStream << ss.rdbuf(); //read the file

  std::string const entire_file_contents = strStream.str();
  std::string const delimiter = "@&$#!";

  //std::cout << delimiter << std::endl;
  //std::cout << entire_file_contents.substr( 1, 5 ) << std::endl;

  assert( entire_file_contents.substr( 1, 5 ) == delimiter ); //TODO this should define delimiter
  std::string::size_type curr_pos = 6;

  auto && unarchive =
    [&]( std::string & destination ){
      std::string::size_type const find_result =
	entire_file_contents.find( delimiter, curr_pos );
      destination = entire_file_contents.substr( curr_pos, find_result-curr_pos );
      curr_pos = find_result + delimiter.size();
    };

  std::string first_token;
  unarchive( first_token );
  if( run_asserts )
    assert( first_token == "BEGIN" ); //Check for corruption

  std::map< std::string, std::string > all_data;

  std::string key;
  std::string value;
  while( true ){
    unarchive( key );
    if( key == "END" ) break;

    if( run_asserts )
      assert( all_data.find( key ) == all_data.end() );

    unarchive( value );
    all_data[ key ] = value;
  }

  t.deserialize( all_data );
}



#endif
} //namespace impl

template< typename T >
class SaveButtonResouce : public OnTheFlyFileResource {
public:
  SaveButtonResouce( T const * object, Wt::WContainerWidget * const root ) :
    OnTheFlyFileResource( root ),
    t_( object )
  {}

  OnTheFlyFileResource::Result
  run() override {
    OnTheFlyFileResource::Result result;

    try {
      result.message = impl::serialize( *t_ );
      result.success = true;
    } catch( ... ){
      result.message = "Serialization Failed!";
      result.success = false;
    }

    return result;
  }


private:
  T const * t_;
};

template< typename T >
class SaveButton : public Wt::WPushButton {
public:
  //no ownership in this implementation!
  SaveButton(
    Wt::WString const & text,
    T const * object,
    Wt::WContainerWidget * const root,
    Wt::WString const & suggested_file_name
  ) :
    Wt::WPushButton( text ),
    suggested_file_name_( suggested_file_name )
  {
    resource_ = std::make_shared< SaveButtonResouce< T > >( object, root );
    resource_->setDispositionType( Wt::ContentDisposition::Attachment );
    resource_->suggestFileName( get_suggested_file_name() );
    setLink( Wt::WLink( resource_ ) );
  }

  Wt::WString
  get_suggested_file_name() const {
    return suggested_file_name_;
  }

  void
  set_file_name( Wt::WString const & name ) {
    suggested_file_name_ = name;
    resource_->suggestFileName( suggested_file_name_ );
  }

private:
  Wt::WString suggested_file_name_;
  std::shared_ptr< SaveButtonResouce< T > > resource_;
  
};


template< typename T >
class LoadButton : public Wt::WPushButton {
public:
  //no ownership in this implementation!
  LoadButton(
    Wt::WString const & text,
    T * object, //too deserialize
    Wt::WContainerWidget * const root,
    Wt::WFileUpload * const fileupload
  ) :
    Wt::WPushButton( text )
  {
    clicked().connect(
      [=] {
	fileupload->upload();
	disable();
      }
    );

    fileupload->uploaded().connect(
      [=] {
	std::string const filename = fileupload->spoolFileName();
	impl::deserialize( *object, filename, true );
	std::filesystem::remove( filename );
	enable();
      }
    );

    fileupload->fileTooLarge().connect(
      [=] {
	Wt::WMessageBox * const messageBox = root->addChild(
	  std::make_unique< Wt::WMessageBox >(
	    "Error",
	    "File is too large!",
	    Wt::Icon::Critical,
	    Wt::StandardButton::Ok
	  )
	);
	messageBox->setModal( false );
	messageBox->buttonClicked().connect(
	  [=] {
	    root->removeChild( messageBox );
	    enable();
	  }
	);
	messageBox->show();
      }
    );
  }

};

}
