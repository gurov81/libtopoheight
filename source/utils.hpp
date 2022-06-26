#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "CDT.h"

namespace utils {

struct CustomEdge
{
    std::pair<std::size_t, std::size_t> vertices;
};

struct Relief {
  std::vector<double> coords;
  std::vector<double> altitudes;
  std::vector<CustomEdge> edges;
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

inline void get_altitude(const rapidjson::Value& arr,Relief& relief, const double* prop_value ) {
  if( prop_value ) {
    relief.altitudes.push_back( *prop_value );
    return;
  }
  double alt=0;
  if( arr.Size()==3 && get_double(arr[2],alt) ) {
    relief.altitudes.push_back( alt );
    return;
  }
  throw std::runtime_error("No altitude data");
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
    relief.coords.push_back(x);
    relief.coords.push_back(y);    
    get_altitude(arr,relief,prop_value);
    return;
  }  
  if(arr[0].IsArray()) {
    for(rapidjson::SizeType i = 0; i < arr.Size(); i++) {
      get_coords(arr[i],relief,prop_value);
    }
  }
}

inline void add_points(const rapidjson::Value& arr,Relief& relief, const double* prop_value ) {
  if(!arr.IsArray()) return;
  double X=0.0, Y=0.0;
  if( get_double(arr[0],X) && get_double(arr[1],Y)) {
    relief.coords.push_back(X);
    relief.coords.push_back(Y);
    get_altitude(arr,relief,prop_value);
    return;
  }
  
  if(arr.Size() == 1){
    add_points(arr[0],relief,prop_value);
    return;
  }
  
  double tmp_x[arr.Size()], tmp_y[arr.Size()],tmp_alt[arr.Size()];
  for(int i = 0; i < arr.Size(); i++) {
    get_double(arr[i][0],tmp_x[i]);
    get_double(arr[i][1],tmp_y[i]);
    get_double(arr[i][2],tmp_alt[i]);
  }
  
  int num_add = 0;
  
  const int n = num_add+1;
  int k = 0;  
  double x[(arr.Size()-1)*(n-1)+arr.Size()], y[(arr.Size()-1)*(n-1)+arr.Size()], alt[(arr.Size()-1)*(n-1)+arr.Size()], dx,dy,dalt;
  for(rapidjson::SizeType  i = 0; i < arr.Size()-1; i++) {
    dx = (tmp_x[i+1] - tmp_x[i])/n;
    dy = (tmp_y[i+1] - tmp_y[i])/n;
    dalt = (tmp_alt[i+1] - tmp_alt[i])/n;
    for(rapidjson::SizeType j = 0; j < n; j++) {
      x[k] = tmp_x[i]+j*dx;
      y[k] = tmp_y[i]+j*dy;
      alt[k] = tmp_alt[i]+j*dalt;
      k++;
    }  
  }
    
  x[k] = tmp_x[arr.Size()-1];
  y[k] = tmp_y[arr.Size()-1];
  alt[k] = tmp_alt[arr.Size()-1];

  for(rapidjson::SizeType  i = 0; i < (arr.Size()-1)*(n-1)+arr.Size(); i++) {
    relief.coords.push_back(x[i]);
    relief.coords.push_back(y[i]);
    relief.altitudes.push_back(alt[i]);
  }
}

inline void get_edges(const rapidjson::Value& arr,Relief& relief) {
  CustomEdge edge;
  
  if(arr.Size() == 1){
    get_edges(arr[0],relief);
    return;
  }
  
  double tmp_x[arr.Size()], tmp_y[arr.Size()];
  for(int i = 0; i < arr.Size()/*-1*/; i++ /*i+=2*/) {
    get_double(arr[i][0],tmp_x[i]);
    get_double(arr[i][1],tmp_y[i]);
    //get_double(arr[i+1][0],tmp_x[i+1]);
    //get_double(arr[i+1][1],tmp_y[i+1]);
    
    edge.vertices.first = tmp_x[i];
    edge.vertices.second = tmp_y[i];
    //edge.vertices.first[0] = tmp_x[i];
    //edge.vertices.first[1] = tmp_y[i];
    //edge.vertices.second[0] = tmp_x[i+1];
    //edge.vertices.second[1] = tmp_y[i+1];
    relief.edges.push_back(edge);
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
	
        if( feature["geometry"] ["type"] == "Polygon" || feature["geometry"] ["type"] == "LineString") {
		const rapidjson::Value& coordinates = feature["geometry"]["coordinates"];
		if( has_prop || prop.empty() ) {
		  //add_points(coordinates,relief,has_prop ? &prop_value : NULL);
		  get_coords(coordinates,relief,has_prop ? &prop_value : NULL);
		  get_edges(coordinates,relief);
		}          
        } else {
        	//MultiPoint && Point
		const rapidjson::Value& coordinates = feature["geometry"]["coordinates"];
		if( has_prop || prop.empty() ) {
		  get_coords(coordinates,relief,has_prop ? &prop_value : NULL);
		}
        } 
    }
    return relief;
}

} // end ns utils
