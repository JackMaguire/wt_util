#pragma once

#include <string>

#include <Wt/WDialog.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>

#include <cassert>

namespace wt_util {

class SingleStringDialog : public Wt::WDialog {
public:
  SingleStringDialog( std::string const & text, Wt::WContainerWidget * parent ){
    setMinimumSize( 500, 500 );

    Wt::WContainerWidget * const my_contents = contents();    
    my_contents->addWidget( std::make_unique< Wt::WText >( text ) );

    // Close Button
    Wt::WPushButton * const close_button = my_contents->addWidget< Wt::WPushButton >( std::make_unique< Wt::WPushButton >( "Close" ) );
    close_button->setMinimumSize( 10, 40 );
    close_button->clicked().connect(
      [=]{
	assert( parent != nullptr );
	parent->removeChild( this );
      }
    );

  }

static
void
add(
  Wt::WContainerWidget * parent,
  std::string const & message
){
  parent->addChild( std::make_unique< wt_util::SingleStringDialog >( message, parent ) )->show();
}
};

}
