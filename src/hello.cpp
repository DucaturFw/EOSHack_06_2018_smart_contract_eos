#include <eosiolib/eosio.hpp>
// #include <eosiolib/print.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/crypto.h>
#include <eosio.token/eosio.token.hpp>

#include <string>
#include <sstream>

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
        public_key pub_key_resp;
        std::string pair;

        int64_t openerAmount;
        uint64_t n;
        time_t t = 0;
        bool opener_agreed = false;

        uint64_t primary_key() const { return id; }
      };
      struct account {
        asset balance;

        uint64_t primary_key()const { return balance.symbol.name(); }
      };

      typedef eosio::multi_index<N(accounts), account> accounts;
      typedef eosio::multi_index<N(channels), channel> channels;

      /// @abi action
      void open(account_name opener, public_key pub_key, account_name respondent, public_key resp_key, asset quantity, std::string pair)
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

        // check if the channel between these two accounts in the selected currency already exists
        channels channel( _self, N(l2dex.code) );
        auto chid = calcChannelId(opener, respondent, quantity, pair);
        auto ch2 = channel.find(chid);

        eosio_assert(ch2 == channel.end(), "channel already exists!");

        // print(" ",hash_source," ");
        // print(" ",std::string(reinterpret_cast<char*>(id_hash.hash)).c_str()," ");
        // SEND_INLINE_ACTION(eosio::token, get_balance, ("owner", opener)("sym", quantity.symbol.name()));
        // INLINE_ACTION_SENDER(eosio::token, transfer)( N(eosio.token), {payer, N(opener)},
        //   { opener, N(eosio.ramfee), fee, std::string("") } )

        // check that opener has enough money
        eosio_assert(enoughMoney(opener, quantity), "not enough money!");

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
          a.pub_key_resp = resp_key;
        });
         
        // transfer tokens
        // sub_balance(opener, quantity);
        // add_balance(_self, quantity, opener);
      }
      /// @abi action
      void extend(account_name opener, public_key pub_key, account_name respondent, asset quantity, std::string pair)
      {
        print("Extending: ", opener, " ", respondent, " with ", quantity, " ", pair);
        
        require_auth(opener);
        eosio_assert(enoughMoney(opener, quantity), "not enough money!");

        channels channel( _self, N(l2dex.code) );
        auto id = calcChannelId(opener, respondent, quantity, pair);
        auto ch2 = channel.find(id);
        eosio_assert(ch2 != channel.end(), "channel doesn't exist!");
        const auto& ch = *ch2;

        eosio_assert(ch.allowance.amount < quantity.amount, "not enough quantity to extend!");
        channel.modify(ch, 0, [&](auto& x)
        {
          x.allowance = quantity;
        });
      }
      /// @abi action
      void validate(uint64_t chid, int64_t openerAmount, uint64_t n, signature& sign, bool isOpener)
      {
        channels channel( _self, N(l2dex.code) );
        const auto& ch = channel.get(chid, "channel doesn't exist!");

        // print("pre-auth");

        if (!has_auth(ch.respondent))
          require_auth(ch.opener);
        
        // print("post-auth");
        
        public_key key = has_auth(ch.respondent) ? ch.pub_key : ch.pub_key_resp;

        // eosio_assert(openerAmount <= ch.allowance.amount, "not enough allowance!");
        eosio_assert(openerAmount >= 0, "can't go below zero!");
        // print("tak-to vse vrode zaebis");

        std::string str = std::to_string(chid) + ";" + std::to_string(n) + ";" + std::to_string(openerAmount);

        checksum256 hash;
        sha256(const_cast<char*>(str.c_str()), str.length(), &hash);
        assert_recover_key(&hash, reinterpret_cast<char *>(sign.data), 66, key.data, 34);
      }
      /// @abi action
      void close(uint64_t chid, int64_t openerAmount, uint64_t n, signature& sign, bool isOpener)
      {
        validate(chid, openerAmount, n, sign, isOpener);
        
        channels channel( _self, N(l2dex.code) );
        const auto& ch = channel.get(chid, "channel doesn't exist!");

        eosio_assert(n >= ch.n, "nonce is lower than last tx!");

        bool challengeOpen = ch.t > 0;

        if (challengeOpen)
        {
          if (isOpener != ch.opener_agreed)
          {
            // finalize
            finalizeChannel(chid, openerAmount);
          }
          else
          {
            if (channelTimedout(ch.t))
            {
              // single-party finalize on timeout
              finalizeChannel(chid, openerAmount);
            }
            else
            {
              // discard
              eosio_assert(false, "this party already submitted their close request!");
            }
          }
        }
        else
        {
          // open challenge
          channel.modify(ch, ch.opener, [&](auto& ch)
          {
            ch.n = n;
            ch.openerAmount = openerAmount;
            ch.opener_agreed = isOpener;
            ch.t = now();
          });
        }
      }

      /// @abi action 
      void hi( account_name user ) {
         print( "Hello, ", name{user} );
      }
  private:
      bool channelTimedout(time_t t)
      {
        if (!t)
          return false;
        
        return (t < (now() - 300));
      }
      void finalizeChannel(uint64_t chid, int64_t openerAmount)
      {
        channels channel( _self, N(l2dex.code) );
        const auto& ch = channel.get(chid, "channel doesn't exist!");

        asset newBalance(openerAmount, ch.allowance.symbol);
        
        // SEND_INLINE_ACTION(eosio::token, transfer, ("from", _self)("to", opener)("quantity", newBalance)("memo", chid));
        action(
          permission_level{ _self, N(active) },
          N(eosio.token), N(transfer),
          std::make_tuple(_self, ch.opener, newBalance, std::to_string(chid))
        ).send();

        channel.erase(ch);
      }
      bool enoughMoney(account_name opener, asset quantity)
      {
        return getUserBalance(opener, quantity).amount >= quantity.amount;
      }
      asset getUserBalance(account_name opener, asset quantity)
      {
        accounts acc( N(eosio.token), opener );
        auto balance = acc.get(quantity.symbol.name());
        return balance.balance;
      }
      uint64_t calcChannelId(account_name opener, account_name respondent, asset quantity, std::string pair)
      {
        auto hash = calcChannelHash(opener, respondent, quantity, pair);
        return reinterpret_cast<uint64_t*>(hash.hash)[0];
      }
      checksum256 calcChannelHash(account_name opener, account_name respondent, asset quantity, std::string pair)
      {
        std::string hash_source = std::to_string(opener)
          + std::to_string(respondent)
          + std::to_string(quantity.symbol.name())
          + pair;
        
        checksum256 id_hash;
        sha256(const_cast<char*>(hash_source.c_str()), hash_source.length(), &id_hash);
        return id_hash;
      }
};

EOSIO_ABI( l2dex, (hi)(open)(validate)(close)(extend) )
