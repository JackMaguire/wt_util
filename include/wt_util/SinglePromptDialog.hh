#pragma once

#include <string>

#include <Wt/WDialog.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>

#include <cassert>

namespace wt_util {

class SinglePromptDialog : public Wt::WDialog {
public:
  SinglePromptDialog(
    std::string const & text,
    std::string const & suggestion,
    Wt::WContainerWidget * parent,
    std::function< void(std::string const &) > && operation
  ){
    setMinimumSize( 500, 500 );

    Wt::WContainerWidget * const my_contents = contents();
    my_contents->addWidget( std::make_unique< Wt::WText >( text ) );
    my_contents->addWidget( std::make_unique< Wt::WBreak >() );
    Wt::WTextArea * area = my_contents->addWidget( std::make_unique< Wt::WTextArea >( suggestion ) );


    Wt::WPushButton * const cancel_button = my_contents->addWidget< Wt::WPushButton >( std::make_unique< Wt::WPushButton >( "Cancel" ) );
    cancel_button->setMinimumSize( 10, 40 );
    cancel_button->clicked().connect(
      [=]{
	assert( parent != nullptr );
	parent->removeChild( this );
      }
    );


    Wt::WPushButton * const close_button = my_contents->addWidget< Wt::WPushButton >( std::make_unique< Wt::WPushButton >( "Done" ) );
    close_button->setMinimumSize( 10, 40 );
    close_button->clicked().connect(
      [=]{
	operation( area->text().toUTF8() );

	assert( parent != nullptr );
	parent->removeChild( this );
      }
    );

  }

  static
  void
  add(
    Wt::WContainerWidget * parent,
    std::string const & text,
    std::string const & suggestion,
    std::function< void(std::string const &) > && operation
  ){
    parent->addChild( std::make_unique< wt_util::SinglePromptDialog >( text, suggestion, parent, std::move( operation ) ) )->show();
  }


};

}
