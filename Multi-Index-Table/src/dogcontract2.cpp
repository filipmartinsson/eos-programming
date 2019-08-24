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
    }

    ACTION erase(int dog_id) {

      dog_index dogs( get_self(), get_self().value);

      auto dog = dogs.get(dog_id, "Unable to find dog");
      require_auth(dog.owner);

      auto iterator = dogs.find(dog_id);
      dogs.erase(iterator);
    }

    ACTION removeall(name user) {

      dog_index dogs( get_first_receiver(), get_first_receiver().value);

      auto owner_index = dogs.get_index<"byowner"_n>();
      auto iterator = owner_index.find(user.value);

      while(iterator != owner_index.end()){
        owner_index.erase(iterator);
        iterator = owner_index.find(user.value);
      }
    }

private:
  TABLE dog {
    int id;
    std::string dog_name;
    int age;
    name owner;

    uint64_t primary_key() const { return id; }
    uint64_t get_secondary_1() const { return owner.value;}

  };

  typedef eosio::multi_index<"dogs"_n, dog, indexed_by<"byowner"_n, const_mem_fun<dog, uint64_t, &dog::get_secondary_1>>> dog_index;


};
