//
// Created by ilya on 25.11.2019.
//

#include <cmath>
#include "coordinates.h"

double Coordinates::GetLatitude() const {
  return latitude_;
}

double& Coordinates::GetLatitude() {
  return latitude_;
}

double Coordinates::GetLongitude() const {
  return longitude_;
}

double& Coordinates::GetLongitude() {
  return longitude_;
}

double Coordinates::Distance(const Coordinates& from, const Coordinates& to) {
  double lat_1 = from.latitude_ * PI/180.0;
  double long_1 = from.longitude_ * PI/180.0;
  double lat_2 = to.latitude_ * PI/180.0;
  double long_2 = to.longitude_ * PI/180.0;

  double delta = std::abs(long_1 - long_2);
  double cos_delta = std::cos(delta);
  double sin_delta = std::sin(delta);

  double cos_lat_1 = std::cos(lat_1);
  double sin_lat_1 = std::sin(lat_1);
  double cos_lat_2 = std::cos(lat_2);
  double sin_lat_2 = std::sin(lat_2);

  double part1 = std::pow(cos_lat_2 * sin_delta, 2);
  double part2 = std::pow(cos_lat_1 * sin_lat_2 - sin_lat_1 * cos_lat_2 * cos_delta, 2);
  double part3 = std::sqrt(part1 + part2);
  double part4 = sin_lat_1 * sin_lat_2 + cos_lat_1 * cos_lat_2 * cos_delta;
  double d = std::atan(part3 / part4);
  // Distance in meters
  double distance = d * RADIUS * 1000;
  return distance;
}

CoordinatesBuilder& CoordinatesBuilder::SetLatitude(double lat) {
  latitude_ = lat;
  return *this;
}
CoordinatesBuilder& CoordinatesBuilder::SetLongitude(double longi) {
  longitude_ = longi;
  return *this;
}
Coordinates CoordinatesBuilder::Build() const {
  return Coordinates(latitude_, longitude_);
}