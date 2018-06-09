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

      /// @abi table channels
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
        // print for sanity
        print("Opening ", opener, " ", respondent, " for ", quantity.symbol.name(), quantity.amount, " - ", pair);

        // check input arguments correctness
        eosio_assert(quantity.symbol.is_valid(), "invalid symbol name" );
        eosio_assert(quantity.amount > 0, "invalid token quantity");
        eosio_assert(is_account(opener), "invalid opener!");
        eosio_assert(is_account(respondent), "invalid respondent!");
        eosio_assert(opener != respondent, "can't open channel to self!");
        eosio_assert(!pair.empty(), "invalid pair!");
        
        // check opener authorization to prevent malicious use
        require_auth(opener);

        require_recipient(opener);
        require_recipient(respondent);

        // calculate channel hash to check existence
        std::string hash_source = std::to_string(opener)
          + std::to_string(respondent)
          + std::to_string(quantity.symbol.name())
          + pair;
        
        checksum256 id_hash;
        sha256(const_cast<char*>(hash_source.c_str()), hash_source.length(), &id_hash);

        // check if the channel between these two accounts in the selected currency already exists
        channels channel( _self, N(l2dex.code) );
        auto chid = reinterpret_cast<uint64_t*>(id_hash.hash)[0];
        auto ch2 = channel.find(chid);

        eosio_assert(ch2 == channel.end(), "channel already exists!");

        // print(" ",hash_source," ");
        // print(" ",std::string(reinterpret_cast<char*>(id_hash.hash)).c_str()," ");
        // SEND_INLINE_ACTION(eosio::token, get_balance, ("owner", opener)("sym", quantity.symbol.name()));
        // INLINE_ACTION_SENDER(eosio::token, transfer)( N(eosio.token), {payer, N(opener)},
        //   { opener, N(eosio.ramfee), fee, std::string("") } )

        // check that opener has enough money
        accounts acc( N(eosio.token), opener );
        auto balance = acc.get(quantity.symbol.name());
        // print(balance.balance);

        eosio_assert(balance.balance.amount >= quantity.amount, "not enough money!");

        // raise permissions level and transfer tokens (currency/symbol) from opener to this contract
        action(
          permission_level{ opener, N(active) },
          N(eosio.token), N(transfer),
          std::make_tuple(opener, _self, quantity, pair)
        ).send();

        // put channel data into contract table
        channel.emplace(opener, [&]( auto& a ) {
          a.id = chid;
          a.allowance = quantity;
          a.opener = opener;
          a.respondent = respondent;
          a.pub_key = pub_key;
          a.pair = pair;
        });
         
        // transfer tokens
        // sub_balance(opener, quantity);
        // add_balance(_self, quantity, opener);
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
