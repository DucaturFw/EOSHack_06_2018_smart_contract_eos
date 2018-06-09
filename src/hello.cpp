#include <eosiolib/eosio.hpp>
// #include <eosiolib/print.hpp>
#include <eosiolib/asset.hpp>
#include <eosio.token/eosio.token.hpp>

#include <string>

using namespace eosio;

// using string;

class l2dex : public eosio::contract {
  public:
      using contract::contract;

      struct channel {
        asset allowance;
        account_name opener;
        account_name respondent;

        // uint64_t primary_key() const { return opener + respondent; }
      };
      struct account {
        asset    balance;

        uint64_t primary_key()const { return balance.symbol.name(); }
      };

      /// @abi action 
      void open(account_name opener, account_name respondent, asset quantity, std::string pair)
      {
        print("Opening ", opener, " ", respondent, " for ", quantity.symbol.name(), quantity.amount, " - ", pair);
        // SEND_INLINE_ACTION(eosio::token, get_balance, ("owner", opener)("sym", quantity.symbol.name()));
        // INLINE_ACTION_SENDER(eosio::token, get_balance)( N(eosio.token), {payer, N(opener)},
        //   { opener, N(eosio.ramfee), fee, std::string("") } )
        eosio::multi_index<N(accounts), account> acc( N(eosio.token), opener );
        auto balance = acc.get(quantity.symbol.name());
        print(balance.balance);
      }
      /// @abi action
      void close(account_name opener, account_name respondent, std::string tx)
      {
        print("Closing: ", opener, respondent, " with ", tx);
      }

      /// @abi action 
      void hi( account_name user ) {
         print( "Hello, ", name{user} );
      }
};

EOSIO_ABI( l2dex, (hi)(open)(close) )
