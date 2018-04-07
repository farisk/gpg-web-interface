open Lwt;


let path = "/Users/noor/Documents/projects/friends-only/cache";

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
};

/* todo: replace with non block lwt call */
let write_file = (data) => {
  let time_stamp = Unix.time() |> int_of_float |> string_of_int;
  let oc = open_out(path ++ "/" ++ time_stamp);
  Printf.fprintf(oc, "%s\n", data);
  close_out(oc);
  path ++ "/" ++time_stamp;
};


let main = (text) => {
  let file = write_file(text);
  let command = ("", [|"gpg", "--verify", file|]);
  Lwt_process.pread(command) >|=
    (out) => {
      print_string(out);  
      print_string((FKStr.contains(out, "gpg: Good Signature") ? "yes" : "no"));
      FKStr.contains(out, "gpg: Good Signature");
    };
};