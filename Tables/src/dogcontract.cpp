#include <eosio/eosio.hpp>
#include <eosio/contract.hpp>



using namespace eosio;

class [[eosio::contract("dogcontract")]] dogcontract : public eosio::contract {

public:


  /*Constructor  
  *
  *
  */
  dogcontract(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

  [[eosio::action]]
  void insert(name owner, std::string dog_name, int age) {
    require_auth( owner );
    dog_index dogs( get_self(), get_first_receiver().value );
    dogs.emplace(owner, [&]( auto& row ) {
     row.id = dogs.available_primary_key ();
     row.dog_name = dog_name;
     row.age = age;
     row.owner = owner;
    });
  }

  [[eosio::action]]
  void erase(int dog_id) {

    dog_index dogs( get_self(), get_first_receiver().value);

    auto dog = dogs.get(dog_id, "Unable to find dog");
    require_auth(dog.owner);
    
    auto iterator = dogs.find(dog_id);
    dogs.erase(iterator);
  }

private:
  struct [[eosio::table]] dog {
    int id;
    std::string dog_name;
    int age;
    name owner;
  
    uint64_t primary_key() const { return id; }
  };
  
  typedef eosio::multi_index<"dogs"_n, dog> dog_index;

};  