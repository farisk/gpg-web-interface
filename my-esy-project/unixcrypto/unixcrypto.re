open Lwt;


let path = "/Users/noor/Documents/projects/friends-only/cache";
let save = "/Users/noor/Documents/projects/friends-only/save";

/* TODO: replace with a function from CORE if anything
   becomes of this */
module FKStr = {
  let cut_head = (str) => String.sub(str, 1, String.length(str) - 1);

  let contains = (container: string, contains: string) => {
    print_string("\nfunction input " ++ container ++ " \n");
    let contains_length = String.length(contains);

    let rec finder = (~found=0, ~searching=container, ()) => {
     if(found >= contains_length) {
        true;
      }
      else if(searching == "") {
        false;
      }
      else {
        let new_stack = searching.[0] == contains.[found] ? (found + 1) : 0;
        finder(
          ~found=new_stack,
          ~searching=cut_head(searching),
          ()
        );
      };
    };
    
    finder();
  };
};

/* todo: replace with non block lwt call */
let write_file = (~save_path=path, data) => {
  let time_stamp = Unix.time() |> int_of_float |> string_of_int;
  let oc = open_out(save_path ++ "/" ++ time_stamp);
  Printf.fprintf(oc, "%s\n", data);
  close_out(oc);
  save_path ++ "/" ++time_stamp;
};


module Storer: Friendsonly.StoreConfig = {
  let store = (message: Friendsonly.message) => write_file(message.body, ~save_path=save);
};


module GPGChecker: Friendsonly.Crypto = {
  let check_signature = (text) => {
    let file = write_file(text);
    print_string(file);
    let command = ("", [|"sh", "gpg_runner.sh", file|]); 
    Lwt_process.pread(command) >|=
      (out) => {
        print_string("\n=======");
        print_string(out);  
        print_string("\n=======");
        print_string((FKStr.contains(out, "gpg: Good signature") ? "yes" : "no"));
        FKStr.contains(out, "gpg: Good signature") ? Ok(text) : Error(Friendsonly.BAD_SIGNATURE)
      };
  }; 
};

module BasicFriendsonly = Friendsonly.Make(Storer, GPGChecker);


