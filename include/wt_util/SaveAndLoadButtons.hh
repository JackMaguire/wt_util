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

//Wt
#include <Wt/WPushButton.h>
#include <Wt/WLink.h>
#include <Wt/WFileResource.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WMessageBox.h>

//CEREAL
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>

namespace wt_util {

namespace impl {
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
std::map< std::string, std::string >
deserialize( T const & t, std::string const & filename, bool const run_asserts ){
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

  return all_data;
}

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
  SaveButton( Wt::WString const & text, T const * object, Wt::WContainerWidget * const root ) :
    Wt::WPushButton( text )
  {
    setLink( Wt::WLink( std::make_shared< SaveButtonResouce< T > >( object, root ) ) );
  }

};


template< typename T >
class LoadButton : public Wt::WPushButton {
public:
  //no ownership in this implementation!
  LoadButton(
    Wt::WString const & text,
    T const * object, //too deserialize
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
	std::string const filename = fu->spoolFileName();
	if( filename.size() > 1 ){	  
	  impl::deserialize( *object, filename, true );
	}
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
	    root_->removeChild( messageBox );
	    enable();
	  }
	);
	messageBox->show();	
      }
    );
  }

};

}
