#ifndef RAIIBOOKING__BOOKING_H_
#define RAIIBOOKING__BOOKING_H_

namespace RAII {

template<class Provider>
class Booking {
 public:
  Booking(Provider *provider, int id) : provider_(provider), id_(id) {}

  Booking(const Booking &) = delete;
  Booking &operator=(const Booking &) = delete;

  Booking(Booking &&other) noexcept {
    if (this != &other) {
      provider_ = other.provider_;
      other.provider_ = nullptr;

      id_ = other.id_;
    }
  }

  Booking &operator=(Booking &&other) noexcept {
    if (this != &other) {
      provider_ = other.provider_;
      other.provider_ = nullptr;

      id_ = other.id_;
    }
    return *this;
  }

  ~Booking() {
    if (provider_)
      provider_->CancelOrComplete(*this);
  }

 private:
  Provider *provider_;
  int id_;
};

} //namespace RAII

#endif //RAIIBOOKING__BOOKING_H_
