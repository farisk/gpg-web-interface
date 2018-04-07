type message = {
  subject: string,
  from: string,
  signature: string,
  body: string,
};

type config = {
  keys_path: string,
  history_path: string,
};

type friend_error = BAD_SIGNATURE | NO_SUCH_FRIEND;

type key = string;
type name = string;
type signature = string;

module type StoreConfig = {
  let get_friend_key: (key) => option(key);
  let store: (message) => result(string, friend_error);
};

module type Crypto = {
  let check_signature: (message, key) => bool;
};

module Make = (Store: StoreConfig, Crypto: Crypto) => {
  let store_message = (message: message) => {
    message.from |>
    Store.get_friend_key |>
    fun 
      | Some (key) => key |> Crypto.check_signature(message) ? Store.store(message) : Error(BAD_SIGNATURE)
      | None => Error(NO_SUCH_FRIEND)
  };
};

let version = "0.1";
