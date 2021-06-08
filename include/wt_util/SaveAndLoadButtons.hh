#pragma once

#include <wt_util/OnTheFlyFileResource.hh>

//#include <Wt/WStreamResource.h>
//#include <Wt/WMessageBox.h>

//#include <sstream>
//#include <fstream>
//#include <memory>
//#include <filesystem> //remove
#include <cassert>

//C++
#include <map>
#include <string>
#include <sstream>

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
deserialize( T const & t, std::string const & data, bool const run_asserts ){
  std::stringstream ss( data );
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

}
