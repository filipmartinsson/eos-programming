#include <eosio/eosio.hpp>
#include <eosio/print.hpp>

using namespace eosio;

CONTRACT helloworld : public contract {
  public:
    using contract::contract;
    
    helloworld(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds) {}

    ACTION hi(name user) {
      require_auth(name{"bob"});
      print("Hello, ", name{user});
    }
  
};