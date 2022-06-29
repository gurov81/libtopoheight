#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "ctl.h"

namespace utils {

struct Relief {
  ctl::PointList points;
  std::vector<ctl::PointList> lineStrings;
  std::vector<ctl::PointList> polygons;
};

inline std::string read_file(const char* filename) {
  std::ifstream input_file(filename);
  if(input_file.good()) {
    std::string json_str(
      (std::istreambuf_iterator<char>(input_file)),
      std::istreambuf_iterator<char>()
    );
    return json_str;
  }
  printf("Error reading file %s", filename);
  throw std::runtime_error("Error reading file");
}


inline bool get_double(const rapidjson::Value& val, double& out_val) {
  if(val.IsDouble()) {
    out_val = val.GetDouble();
    return true;
  }
  if(val.IsInt()) {
    out_val = static_cast<double>(val.GetInt());
    return true;
  }
  return false;
}

inline bool feature_get_altitude_property(const rapidjson::Value& feature,std::string const& prop,double& out_val) {
  if(prop.empty() || !feature.HasMember("properties")) {
    return false;
  }
  const rapidjson::Value& props = feature["properties"];
  if(props.HasMember(prop.c_str())) {
    return get_double(props[prop.c_str()],out_val);
  }
  return false;
}

inline void get_point(const rapidjson::Value& arr,Relief& relief, const double* prop_value ) {
  if(!arr.IsArray()) return;
  double x=0.0, y=0.0, z=0.0;
  if( get_double(arr[0],x) && get_double(arr[1],y) ) {
    if( prop_value ) {
      relief.points.push_back(ctl::Point(x,y,*prop_value)); 
      return;
    }
    get_double(arr[2],z);
    relief.points.push_back(ctl::Point(x,y,z));    
    return;
  }  
  if(arr[0].IsArray()) {
    for(rapidjson::SizeType i = 0; i < arr.Size(); i++) {
      get_point(arr[i],relief,prop_value);
    }
  }
}

inline void get_lineString(const rapidjson::Value& arr,Relief& relief, const double* prop_value ) {
  if(!arr.IsArray()) return; 
  if(arr.Size() == 1){
    get_lineString(arr[0],relief,prop_value);
    return;
  }
  
  ctl::PointList lineString;
  double x=0.0, y=0.0, z=0.0;
  for(int i = 0; i < arr.Size(); i++) {
    get_double(arr[i][0],x);
    get_double(arr[i][1],y);
    if( prop_value ) {
      lineString.push_back(ctl::Point(x,y,*prop_value));
    } else {
      get_double(arr[i][2],z);
      lineString.push_back(ctl::Point(x,y,z));
    }
  }
  
  relief.lineStrings.push_back(lineString);
  return;
}

inline void get_polygon(const rapidjson::Value& arr,Relief& relief, const double* prop_value ) {
  if(!arr.IsArray()) return; 
  if(arr.Size() == 1){
    get_polygon(arr[0],relief,prop_value);
    return;
  }
  
  ctl::PointList polygon;
  double x=0.0, y=0.0, z=0.0;
  for(int i = 0; i < arr.Size(); i++) {
    get_double(arr[i][0],x);
    get_double(arr[i][1],y);
    if( prop_value ) {
      polygon.push_back(ctl::Point(x,y,*prop_value));
    } else {
      get_double(arr[i][2],z);
      polygon.push_back(ctl::Point(x,y,z));
    }
  }
    
  relief.polygons.push_back(polygon);
  return;
}

inline Relief get_geo_json_points(std::string const& json, std::string const& prop) {
    rapidjson::Document document;
    if(document.Parse(json.c_str()).HasParseError()) {
        throw std::runtime_error("Cannot parse JSON");
    }
    const rapidjson::Value& features = document["features"];
    Relief relief;
    // vector<double> y_vector;
    for(rapidjson::SizeType i = 0; i < features.Size(); i++) {
        const rapidjson::Value& feature = features[i];
	double prop_value = 0;
	const bool has_prop = feature_get_altitude_property(feature,prop,prop_value);
	const rapidjson::Value& coordinates = feature["geometry"]["coordinates"];
        if( feature["geometry"] ["type"] == "Polygon"){
          if( has_prop || prop.empty() ) {
	  //add_points(coordinates,relief,has_prop ? &prop_value : NULL);
	  get_polygon(coordinates,relief,has_prop ? &prop_value : NULL);
	  }          
        } else if( feature["geometry"] ["type"] == "LineString") {
          if( has_prop || prop.empty()){
	  //add_points(coordinates,relief,has_prop ? &prop_value : NULL);
	  get_lineString(coordinates,relief,has_prop ? &prop_value : NULL);
	  }
        } else {
          //MultiPoint && Point
	  if( has_prop || prop.empty() ) {
	    get_point(coordinates,relief,has_prop ? &prop_value : NULL);
	  }
        } 
    }
    return relief;
}

} // end ns utils
