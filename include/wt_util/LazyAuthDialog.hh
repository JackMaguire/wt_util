#pragma once

#include <string>
#include <vector>

#include <Wt/WDialog.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WGridLayout.h>

#include <cassert>

namespace wt_util {

class LazyAuthDialog : public Wt::WDialog {
public:
  LazyAuthDialog( std::vector< std::string > const & passwords, Wt::WContainerWidget * parent ){
    setMinimumSize( 500, 500 );

    auto layout = contents()->setLayout( std::make_unique< Wt::WGridLayout >() );
 
    uint const npasswords = passwords.size();
    std::vector< Wt::WTextArea * > guesses( npasswords );

    for( uint i = 0; i < npasswords; ++i ){
      layout->addWidget< Wt::WText >( std::make_unique< Wt::WText >( "Password #" + std::to_string(i+1) ), i, 0 );
      guesses[ i ] = layout->addWidget< Wt::WTextArea >( std::make_unique< Wt::WTextArea >( "???" ), i, 1 );
    }


    Wt::WPushButton * const button = layout->addWidget< Wt::WPushButton >( std::make_unique< Wt::WPushButton >( "Log In" ), npasswords, 1 );
    button->setMinimumSize( 10, 40 );
    button->clicked().connect(
      [=]{
	for( uint i = 0; i < npasswords; ++i ){
	  assert( guesses[ i ] != nullptr );
	  std::string const guess = guesses[ i ]->text().toUTF8();
	  if( guess != passwords[i] ) return;
	}

	assert( parent != nullptr );
	parent->removeChild( this );
      }
    );

  }

  LazyAuthDialog(
    std::vector< std::pair< std::string, std::string > > const & passwords,
    Wt::WContainerWidget * parent
  ){
    setMinimumSize( 500, 500 );

    auto layout = contents()->setLayout( std::make_unique< Wt::WGridLayout >() );
 
    uint const npasswords = passwords.size();
    std::vector< Wt::WTextArea * > guesses( npasswords );

    for( uint i = 0; i < npasswords; ++i ){
      layout->addWidget< Wt::WText >( std::make_unique< Wt::WText >( passwords[i].first ), i, 0 );
      guesses[ i ] = layout->addWidget< Wt::WTextArea >( std::make_unique< Wt::WTextArea >( "???" ), i, 1 );
    }


    Wt::WPushButton * const button = layout->addWidget< Wt::WPushButton >( std::make_unique< Wt::WPushButton >( "Log In" ), npasswords, 1 );
    button->setMinimumSize( 10, 40 );
    button->clicked().connect(
      [=]{
	for( uint i = 0; i < npasswords; ++i ){
	  assert( guesses[ i ] != nullptr );
	  std::string const guess = guesses[ i ]->text().toUTF8();
	  if( guess != passwords[i].second ) return;
	}

	assert( parent != nullptr );
	parent->removeChild( this );
      }
    );

  }

  static
  void
  protect(
    std::vector< std::string > const & passwords,
    Wt::WContainerWidget * parent
  ){
    parent->addChild( std::make_unique< LazyAuthDialog >( passwords, parent ) )->show();    
  }

  static
  void
  protect(
    std::vector< std::pair< std::string, std::string > > const & passwords,
    Wt::WContainerWidget * parent
  ){
    parent->addChild( std::make_unique< LazyAuthDialog >( passwords, parent ) )->show();    
  }

};

}
