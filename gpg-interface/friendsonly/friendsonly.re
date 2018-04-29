open Lwt.Infix;

type message = {
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
  let store: (message) => string;
  let read_all: (unit) => array(string);
};

module type Crypto = {
  let check_signature: (string) => Lwt.t(result(string,friend_error));
};

module type FriendsOnly = {
  let store_message: (message) => Lwt.t(result(string, friend_error));
  let get_messages: (unit) => array(string);
};


module Make = (Store: StoreConfig, Crypto: Crypto) => {
  let store_message = (message: message) => {
      message.body |> 
      Crypto.check_signature >|=
        fun
          | Ok(who_be) => {Store.store(message); Ok(who_be);}
          | Error(err) => Error(err)
  };

  let get_messages = () => Store.read_all();

};


