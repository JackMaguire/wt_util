#pragma once

#include <string>
#include <vector>

#include <Wt/WDialog.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTextEdit.h>
#include <Wt/WGridLayout.h>

#include <cassert>

namespace wt_util {

class LazyAuthDialog : public Wt::WDialog {
public:
  LazyAuthDialog( std::vector< std::string > const & passwords, Wt::WContainerWidget * parent ){
    setMinimumSize( 500, 500 );

    Wt::WContainerWidget * const my_contents = contents();

    auto layout = contents()->setLayout( std::make_unique< Wt::WGridLayout >() );
 
    uint const npasswords = passwords.size();
    std::vector< Wt::WTextEdit * > guesses( npasswords );

    for( uint i = 0; i < npasswords; ++i ){
      guesses[ i ] = layout->addWidget< Wt::WText >( std::make_unique< Wt::WPushButton >( "Password #" + std::to_string(i) ), i, 0 );
      layout->addWidget< Wt::WText >( std::make_unique< Wt::WTextEdit >( "???" ), i, 1 );
    }


    Wt::WPushButton * const button = my_contents->addWidget< Wt::WPushButton >( std::make_unique< Wt::WPushButton >( "Log In" ), npasswords, 1 );
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
};

}
