#include <eosio/eosio.hpp>
#include <eosio/contract.hpp>

using namespace eosio;

CONTRACT dogcontract : public contract {

  using contract::contract;
  public:
    dogcontract(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}


    ACTION insert(name owner, std::string dog_name, int age) {
      require_auth( owner );
      dog_index dogs(get_first_receiver(), get_first_receiver().value);
      dogs.emplace(owner, [&]( auto& row ) {
       row.id = dogs.available_primary_key ();
       row.dog_name = dog_name;
       row.age = age;
       row.owner = owner;
      });
      send_summary(owner, "successfully added new dog");
    }

    ACTION erase(int dog_id) {

      dog_index dogs( get_self(), get_self().value);

      auto dog = dogs.get(dog_id, "Unable to find dog");
      require_auth(dog.owner);

      auto iterator = dogs.find(dog_id);
      dogs.erase(iterator);
      send_summary(dog.owner, "successfully removed dog");
    }

    ACTION notify(name user, std::string msg) {
      require_auth(get_self());
      require_recipient(user);
  }

private:

        //permission_level,
        //code,
        //action,
        //data
  void send_summary(name user, std::string message){
    action(
      permission_level{get_self(),"active"_n},
      get_self(),
      "notify"_n,
      std::make_tuple(user, name{user}.to_string() + message)
    ).send();
  };


  TABLE dog {
    int id;
    std::string dog_name;
    int age;
    name owner;

    uint64_t primary_key() const { return id; }

  };

  typedef multi_index<"dogs"_n, dog> dog_index;


};
