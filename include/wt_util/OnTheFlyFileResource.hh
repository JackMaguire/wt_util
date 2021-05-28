#pragma once

#include <Wt/WStreamResource.h>
#include <Wt/WMessageBox.h>

#include <sstream>
#include <fstream>
#include <memory>
#include <filesystem> //remove

namespace wt_util {

class OnTheFlyFileResource : public Wt::WStreamResource {
public:

  struct Result {
    std::string message;
    bool success;
  };

  OnTheFlyFileResource(
    Wt::WContainerWidget * root,
  ) :
    root_( root )
  {}

  ~OnTheFlyFileResource() = default;

  // This is the only thing you need to implement
  virtual
  Result run() = 0;

  void handleRequest(
    Wt::Http::Request const & request,
    Wt::Http::Response & response
  ) override {

    if( ! request.continuation() ){
      Result const result = run();
      if( result.success ){
	iss_for_most_recent_request_ = std::make_unique< std::istringstream >( result.message );
      } else {
	iss_for_most_recent_request_ = std::make_unique< std::istringstream >( "" );
	handleFailure( result.message );
	return;
      }
    }

    handleRequestPiecewise( request, response, * iss_for_most_recent_request_ );
  }

  void
  handleFailure( std::string const & message ){
    Wt::WMessageBox * const messageBox = root_->addChild(
      std::make_unique< Wt::WMessageBox >(
	"Error",
	"Compilation failed with message: " + message,
	Wt::Icon::Critical,
	Wt::StandardButton::Ok
      )
    );
    messageBox->setModal( false );
    messageBox->buttonClicked().connect(
      [=] {
	root_->removeChild( messageBox );
      }
    );
    messageBox->show();
  }

  static
  std::string
  load_file_contents( std::string const & filename, bool const delete_file ) {
    std::ifstream in( filename, std::ios::in | std::ios::binary );
    assert( in.is_open() );
    std::ostringstream contents;
    contents << in.rdbuf();
    in.close();

    if( delete_file ){
      std::filesystem::remove( filename );
    }

    return contents.str();
  }

private:
  Wt::WContainerWidget * root_;
  std::unique_ptr< std::istringstream > iss_for_most_recent_request_;
};

} // namespace
