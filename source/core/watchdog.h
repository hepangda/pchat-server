#define EXTERN_WATCHDOGS extern bool WATCHDOG_DPKG; \
                         extern bool WATCHDOG_SPKG; \
                         extern bool WATCHDOG_RVPKG;
#define WHILE_WATCHDOG(dog) while (WATCHDOG_##dog)


template <int Type>
class Watchdog {
 public:
  void feed() {}

  bool isAlive() {}

  Watchdog<Type> &get() {
    static Watchdog<Type> instance;
    return instance;
  }
};
