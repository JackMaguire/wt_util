#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>

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
  SerializableSubForm()
  {
    items_.max_load_factor( 0.2 );
    layout_ = setLayout( std::make_unique< Wt::WGridLayout >() );
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

class SerializableSubFormMenuItem : public Wt::WMenuItem {
public:

  struct LabeledValue {
    Wt::WText * label;
    Wt::WLineEdit * value;
  };

public:
  SerializableSubFormMenuItem( std::string const & name ):
    Wt::WMenuItem( name )
  {
    items_.max_load_factor( 0.2 );
    layout_ = setLayout( std::make_unique< Wt::WGridLayout >() );
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
