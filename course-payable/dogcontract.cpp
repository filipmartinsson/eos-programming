#include <eosio/eosio.hpp>
#include <eosio/contract.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT dogcontract : public contract {

  using contract::contract;
  public:
    dogcontract(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds),
    currency_symbol("DOGCOIN", 0) {}

    ACTION insert(name owner, std::string dog_name, int age) {
      require_auth( owner );
      check_balance(owner);
      reduce_balance(owner);
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

    [[eosio::on_notify("eosio.token::transfer")]]
    void pay(name from, name to, asset quantity, std::string memo){
      if(from == get_self() || to != get_self()){
        return;
      }

      check(quantity.amount > 0, "Not enough coins bro");
      check(quantity.symbol == currency_symbol, "Not the right coin bro");

      balance_index balances(get_self(), from.value);
      auto iterator = balances.find(currency_symbol.raw());

      if(iterator != balances.end()){
        balances.modify(iterator, get_self(), [&](auto &row){
          row.funds += quantity;
        });
      }
      else{
        balances.emplace(get_self(), [&](auto &row){
          row.funds = quantity;
        });
      }
    }

private:
  const symbol currency_symbol;

  TABLE dog {
    int id;
    std::string dog_name;
    int age;
    name owner;

    uint64_t primary_key() const { return id; }
    uint64_t get_secondary_1() const { return owner.value;}
  };

  TABLE balance {
    asset funds;

    uint64_t primary_key() const {return funds.symbol.raw();}
  };

  typedef multi_index<"dogs"_n, dog, indexed_by<"byowner"_n, const_mem_fun<dog, uint64_t, &dog::get_secondary_1>>> dog_index;
  typedef multi_index<"balances"_n, balance> balance_index;

  void check_balance(name user){
      balance_index balances(get_self(), user.value);
      auto row = balances.get(currency_symbol.raw(), "No DOGCOIN Balance");
      check(row.funds.amount >= 10, "Not Enough DOGCOINS Deposited");
  }
  void reduce_balance(name user){
      balance_index balances(get_self(), user.value);
      auto iterator = balances.find(currency_symbol.raw());
      if(iterator != balances.end()){
        balances.modify(iterator, get_self(), [&](auto &row){
          row.funds.set_amount(row.funds.amount-10);
        });
      }
  }

};
