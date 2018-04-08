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
};

module type Crypto = {
  let check_signature: (string) => Lwt.t(result(string,friend_error));
};


module Make = (Store: StoreConfig, Crypto: Crypto) => {
  let store_message = (message: message) => {
      message.body |> 
      Crypto.check_signature >|=
        fun
          | Ok(_) => Ok(Store.store(message))
          | Error(err) => Error(err)
  };
};

let version = "0.1";

