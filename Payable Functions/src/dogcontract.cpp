#include <eosio/eosio.hpp>
#include <eosio/contract.hpp>
#include <eosio/asset.hpp>
#include <eosio/print.hpp>


using namespace eosio;

CONTRACT dogcontract : public contract {

  using contract::contract;
  public:
    dogcontract(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds),currency_symbol("DOGCOIN", 0) {}


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
    
    [[eosio::on_notify("eosio.token::transfer")]]
    void pay(name from, name to, asset quantity, std::string memo){
      print("DEBUG");
      if (from == get_self() || to != get_self())
      {
        return;
      }

      check(quantity.amount > 0, "Not enough Dogecoin");
      check(quantity.symbol == currency_symbol, "We only accept DOGECOIN.");
      
      balance_index balances(get_self(), from.value);
      auto iterator = balances.find(currency_symbol.raw());

      if (iterator != balances.end())
        balances.modify(iterator, get_self(), [&](auto &row) {
          row.funds += quantity;
      });
      else
        balances.emplace(get_self(), [&](auto &row) {
          row.funds = quantity;
      });

    }
    

private:

    const symbol currency_symbol;
    
  void check_balance(name user){
    balance_index balances(get_self(), user.value);
    auto row = balances.get(currency_symbol.raw(), "No balance");
    check(row.funds.amount >= 10, "Not enough balance");
  }  
  void reduce_balance(name user){
    balance_index balances(get_self(), user.value);
    auto iterator = balances.find(currency_symbol.raw());
    if (iterator != balances.end())
        balances.modify(iterator, get_self(), [&](auto &row) {
          row.funds.set_amount(row.funds.amount-10);
        });
  } 

  TABLE dog {
    int id;
    std::string dog_name;
    int age;
    name owner;

    uint64_t primary_key() const { return id; }

  };
  TABLE balance {
    asset funds;
    uint64_t primary_key() const { return funds.symbol.raw(); }

  };

  typedef multi_index<"dogs"_n, dog> dog_index;
  typedef multi_index<"balances"_n, balance> balance_index;


};
