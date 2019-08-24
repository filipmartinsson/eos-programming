#include <eosio/eosio.hpp>
#include <eosio/print.hpp>

using namespace eosio;

CONTRACT hellotable : public contract {
  public:
    using contract::contract;
    
    hellotable(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds) {}
    
    ACTION insert(name owner, std::string dog_name, int age){
      require_auth(owner);
      dog_index dogs(get_self(), get_self().value);
      dogs.emplace(owner, [&](auto& row){
        row.id = dogs.available_primary_key();
        row.dog_name = dog_name;
        row.age = age;
        row.owner = owner;
      });
    }
    
    ACTION erase(int dog_id){
        dog_index dogs(get_self(), get_self().value);
        
        auto dog = dogs.get(dog_id, "Unable ot fetch dog.");
        require_auth(dog.owner);
        
        auto iterator = dogs.find(dog_id);
        dogs.erase(iterator);
    }
    
  private:
    TABLE dog{
      int id;
      std::string dog_name;
      int age;
      name owner;
      
      uint64_t primary_key() const {return id;}
      uint64_t by_owner() const {return owner.value;}
    };
    
    typedef multi_index<"dogs"_n, dog, indexed_by<"byowner"_n, const_mem_fun<dog, uint64_t, &dog::by_owner>>> dog_index;

};


















