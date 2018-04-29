open Lwt;


let path = "../cache";
let save = "../save";

/* TODO: replace with a function from CORE if anything
   becomes of this */
module FKStr = {
  let cut_head = (str) => String.sub(str, 1, String.length(str) - 1);

  let contains = (container: string, contains: string) => {
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

  let yank_from_until = (input, from, until) => {
    /* TODO: handle a fucking exception being throw and make it return a NONE */
    let from_pos = String.index(input, from);
    let to_pos = String.index(input, until);
    String.sub(input, from_pos+1, to_pos - from_pos - 1);
  };
};

/* todo: replace with non block lwt call, and then replace with a DB kek */
let write_file = (~save_path=path, data) => {
  let time_stamp = Unix.time() |> int_of_float |> string_of_int;
  let oc = open_out(save_path ++ "/" ++ time_stamp);
  Printf.fprintf(oc, "%s\n", data);
  close_out(oc);
  save_path ++ "/" ++time_stamp;
};

/* todo: do something non-blocking with Lwt */
let read_all_dir = (~path="../save", ()) => {
  Sys.readdir(path) |>
  Array.map((file) => {
    let full_path = path ++ "/" ++ file;
    let oi = open_in(full_path);
    
    let rec take_it_all = (~message="", oi) => {
      switch (input_line(oi)) {
        | new_line => take_it_all(~message=message ++ "\n" ++ new_line, oi)
        | exception End_of_file => message
      };  
    };

    take_it_all(oi); 
  });

};


/* TODO: unsure why this lives here? */
module Storer: Friendsonly.StoreConfig = {
  let store = (message: Friendsonly.message) => write_file(message.body, ~save_path=save);
  let read_all = (unit) => read_all_dir(~path=save, ());
};


module GPGChecker: Friendsonly.Crypto = {
  let check_signature = (text) => {
    let file = write_file(text);
    print_string(file);
    let command = ("", [|"sh", "gpg_runner.sh", file|]); 
    Lwt_process.pread(command) >|=
      (out) => {
        FKStr.contains(out, "gpg: Good signature") ? Ok(FKStr.yank_from_until(out, '<','>')) : Error(Friendsonly.BAD_SIGNATURE)
      };
  }; 
};




module BasicFriendsonly = Friendsonly.Make(Storer, GPGChecker);


