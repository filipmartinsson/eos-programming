#include <authenticate.hpp>

ACTION authenticate::hi(name user) {
  require_auth(name{"filip"});
  print("Hello, ", name{user});
}
