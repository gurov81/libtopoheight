#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "CDT.h"

namespace utils {

struct Relief {
  double maxAlt = 0;
  std::vector<CDT::V2d<double>> points;
  std::vector<CDT::Edge> edges;
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

inline void get_coords(const rapidjson::Value& arr,Relief& relief, const double* prop_value ) {
  if(!arr.IsArray()) return;
  double x=0.0, y=0.0;
  if( get_double(arr[0],x) && get_double(arr[1],y) ) {      
    //std::cout << "\n" << x << " " << y;
    CDT::V2d<double> tmp_point;
    tmp_point.x = x;
    tmp_point.y = y;
    if( prop_value ) {
      tmp_point.z = *prop_value;
      if (relief.maxAlt < tmp_point.z){
        relief.maxAlt = tmp_point.z;
      }
      relief.points.push_back(tmp_point);
      return;
    }
    double alt=0;
    if( arr.Size()==3 && get_double(arr[2],alt) ) {
      tmp_point.z = alt;
      if (relief.maxAlt < tmp_point.z){
        relief.maxAlt = tmp_point.z;
      }
      relief.points.push_back(tmp_point);
      return;
    }
    throw std::runtime_error("No altitude data");
  }  
  if(arr[0].IsArray()) {
    for(rapidjson::SizeType i = 0; i < arr.Size(); i++) {
      get_coords(arr[i],relief,prop_value);
    }
  }
}

inline void get_edges(const rapidjson::Value& arr,Relief& relief) { 
  if(arr.Size() == 1){
    get_edges(arr[0],relief);
    return;
  }
  
  CDT::VertInd v1 = 0, v2 = 0;
  
  int tmp = relief.points.size() - arr.Size();
  
  for(int i = 0; i < arr.Size() -1 /*-1*/; i++ /*i+=2*/) {
    v1 = tmp + i;
    v2 = tmp + i + 1;    

    relief.edges.push_back(CDT::Edge(v1, v2));
  }
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
    if( feature["geometry"] ["type"] == "Polygon" || feature["geometry"] ["type"] == "LineString") {
      if( has_prop || prop.empty() ) {
        get_coords(coordinates,relief,has_prop ? &prop_value : NULL);
        get_edges(coordinates,relief);
      }          
    } else {
    //MultiPoint && Point
      if( has_prop || prop.empty() ) {
        get_coords(coordinates,relief,has_prop ? &prop_value : NULL);
      }
    }
  }
  return relief;
}

} // end ns utils
