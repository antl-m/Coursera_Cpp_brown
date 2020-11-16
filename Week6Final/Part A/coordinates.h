//
// Created by ilya on 25.11.2019.
//

#ifndef YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_COORDINATES_H
#define YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_COORDINATES_H

class Coordinates {
public:
  Coordinates() = default;
  explicit Coordinates(double latitude, double longitude) : latitude_(latitude), longitude_(longitude) {}

  double GetLatitude() const;
  double& GetLatitude();
  double GetLongitude() const;
  double& GetLongitude();

  static double Distance(const Coordinates& from, const Coordinates& to);

private:
  constexpr static double PI = 3.1415926535;
  constexpr static int RADIUS = 6371;
  double latitude_ = 0;
  double longitude_ = 0;
};

class CoordinatesBuilder {
public:
  CoordinatesBuilder() = default;
  CoordinatesBuilder& SetLatitude(double lat);
  CoordinatesBuilder& SetLongitude(double longi);
  Coordinates Build() const;
private:
  double latitude_;
  double longitude_;
};

#endif //YANDEXCPLUSPLUS_4_BROWN_FINAL_PROJECT_PART_A_COORDINATES_H
