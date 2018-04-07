
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

type history = list(string);


module type Config = {
  let friends: list((string,string));
  let history: string;
};

module Make = (Conf: Config) => {
  
  let check_signature = (key, message) => {
    Ok("all is well?");
  };
 
  let get_friend_key = (name: string) => List.assoc_opt(name, Conf.friends);

  let check_message = (message: message) => {
    message.from |>
    get_friend_key |>
    fun 
      | Some (key) => key |> check_signature(message)
      | None => Error("no such friend")
  };

};
