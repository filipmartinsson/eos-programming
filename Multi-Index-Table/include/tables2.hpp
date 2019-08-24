#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

CONTRACT tables2 : public contract {
  public:
    using contract::contract;
    tables2(eosio::name receiver, eosio::name code, datastream<const char*> ds):contract(receiver, code, ds) {}

    ACTION hi(name user);
  
  private:
    TABLE tableStruct {
      name key;
      std::string name;
    };
    typedef eosio::multi_index<"table"_n, tableStruct> table;
};

//EOSIO_DISPATCH(tables2, (hi))
