
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

module type Config = {
  type key = string;
  type name = string;
  type signature = string;

  let get_friend_key: (key) => option(key);
  let check_signature: (message, key) => bool;
  let store: (message) => result(string, friend_error);
};


module Make = (Conf: Config) => {
  let store_message = (message: message) => {
    message.from |>
    Conf.get_friend_key |>
    fun 
      | Some (key) => key |> Conf.check_signature(message) ? Conf.store(message) : Error(BAD_SIGNATURE)
      | None => Error(NO_SUCH_FRIEND)
  };

};
