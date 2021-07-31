#pragma once

#include <Wt/WContainerWidget.h>

#include <map>
#include <unordered_map>
#include <string>
#include <cassert>
#include <list>

namespace wt_util {

static auto const ALIGN = Wt::AlignmentFlag::Right | Wt::AlignmentFlag::Middle;

class SerializableSubForm : public Wt::WContainerWidget {
public:

  struct LabeledValue {
    Wt::WText * label;
    Wt::WLineEdit * value;
  };

public:
  SerializableSubForm(){
    items_.max_load_factor( 0.2 );

    layout_ = setLayout( std::make_unique< Wt::WGridLayout >() );

    //setMinimumSize( 1000, 300 );
    /*
    int row = 0;
    int col = -1;
    add_element( "Project Name", row, ++col, "TODO" );
    add_element( "Target Name",  row, ++col, "TODO" );
    add_element( "Site Name",    row, ++col, "TODO" );

    ++row; col=-1;
    add_element( "min_loop_length", row, ++col, "7" );
    add_element( "max_loop_length", row, ++col, "9" );
    add_element( "min_internal_hbonds", row, ++col, "2" );

    ++row; col=-1;
    add_element( "hours", row, ++col, "20" );
    add_element( "nruns", row, ++col, "2" );
    add_element( "optimizer", row, ++col, "DoubleFastGADiscreteOnePlusOne" );*/
  }

  virtual
  std::string
  unique_prefix() const = 0;
  
  void
  serialize( std::map< std::string, std::string > & all_data ) const {
    for( auto const & pair : items_ ){
      std::string const & token = pair.first;
      //LabeledValue const & lv = pair.second;
      std::string const key = unique_prefix() + '_' + token;
      assert( all_data.find( key ) == all_data.end() );
      all_data[ key ] = get( token );
    }
  }

  void
  deserialize(
    std::map< std::string, std::string > const & all_data
    //, std::list & missing_items
  ) {
    for( auto & pair : items_ ){
      std::string const & token = pair.first;
      LabeledValue & lv = pair.second;
      std::string const key = unique_prefix() + '_' + token;

      auto const iter = all_data.find( key );

      if( iter != all_data.end() ){
	std::string const & value = iter->second;
	lv.value->setText( value );
      } else {
	//missing_items.push_back( key );
      }
    }
  }

protected:

  std::string
  get( std::string const & id ) const {
    assert( items_.find( id ) != items_.end() );
    return std::string( items_.at( id ).value->text().toUTF8() );
  }

  void
  add_element(
    std::string const & name,
    int const row,
    int const col,
    std::string const & starting_value
  ){
    LabeledValue lv;

    lv.label = layout_->addWidget( std::make_unique< Wt::WText >( name ), row, 2*col, ALIGN );

    lv.value = layout_->addWidget( std::make_unique< Wt::WLineEdit >( starting_value ), row, 2*col + 1, ALIGN );

    items_[ name ] = lv;
  }

private:
  Wt::WGridLayout * layout_;
  std::unordered_map< std::string, LabeledValue > items_;

};


}
