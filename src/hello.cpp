#include <eosiolib/eosio.hpp>
// #include <eosiolib/print.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/crypto.h>
#include <eosio.token/eosio.token.hpp>

#include <string>

using namespace eosio;

// using string;

class l2dex : public eosio::contract {
  public:
      using contract::contract;

      struct channel {
        uint64_t id;

        asset allowance;
        account_name opener;
        account_name respondent;
        public_key pub_key;
        std::string pair;

        uint64_t primary_key() const { return id; }
      };
      struct account {
        asset balance;

        uint64_t primary_key()const { return balance.symbol.name(); }
      };

      typedef eosio::multi_index<N(accounts), account> accounts;
      typedef eosio::multi_index<N(channels), channel> channels;

      /// @abi action
      void open(account_name opener, public_key pub_key, account_name respondent, asset quantity, std::string pair)
      {
        print("Opening ", opener, " ", respondent, " for ", quantity.symbol.name(), quantity.amount, " - ", pair);

        eosio_assert(quantity.amount > 0, "invalid token quantity");
        eosio_assert(opener, "invalid opener!");
        eosio_assert(respondent, "invalid respondent!");
        eosio_assert(opener != respondent, "can't open channel to self!");
        eosio_assert(!pair.empty(), "invalid pair!");

        std::string hash_source = std::to_string(opener)
          + std::to_string(respondent)
          + std::to_string(quantity.symbol.name())
          + pair;
        
        checksum256 id;
        sha256(const_cast<char*>(hash_source.c_str()), hash_source.length(), &id);

        channels channel( N(this), N(l2dex.code) );
        auto ch2 = channel.find(reinterpret_cast<uint64_t*>(id.hash)[0]);

        eosio_assert(ch2 == channel.end(), "channel already exists!");

        // print(" ",hash_source," ");
        // print(" ",std::string(reinterpret_cast<char*>(id.hash)).c_str()," ");
        // SEND_INLINE_ACTION(eosio::token, get_balance, ("owner", opener)("sym", quantity.symbol.name()));
        // INLINE_ACTION_SENDER(eosio::token, transfer)( N(eosio.token), {payer, N(opener)},
        //   { opener, N(eosio.ramfee), fee, std::string("") } )
        accounts acc( N(eosio.token), opener );
        auto balance = acc.get(quantity.symbol.name());
        print(balance.balance);

        eosio_assert(balance.balance.amount >= quantity.amount, "not enough money!");
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
