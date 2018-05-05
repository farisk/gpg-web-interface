open Opium.Std;
open Lwt.Infix;

[@deriving (yojson)]
type new_message = {
  message: string,
};


[@deriving (yojson)]
type challenge_resolve = {
  solution: string,
  email: string,
};

[@deriving (yojson)]
type challenge_request = {
  email: string,
};

[@deriving (yojson)]
type messages = {
  messages: array(string),
};

let accept_options = App.options("**", (_) => respond'(`String("OK")));

/* TODO: handle decoding exception */
let req_to_obj = (convertor, on_decode, req) => {
  Lwt.(
    App.string_of_body_exn(req) >>= 
      (raw) => raw |>
        Yojson.Safe.from_string |>
        convertor |>
        fun
          | Ok(data) => on_decode(data) 
          | Error(err) => `String("Bad input:" ++ err) |> respond'(~code=Cohttp.Code.status_of_code(400))
  );
};

module Make = (FriendsOnly: Friendsonly.FriendsOnly,
               Challenger: Challenger.Challenger) => {

  let my_auth = Auth.m(Challenger.authenticate);

  let execute_new_message = (new_message) => {
    let s: Friendsonly.message = {body: new_message.message};
    FriendsOnly.store_message({s});
  };

  
  let runner = () => {

    let get_challenge = req_to_obj(challenge_request_of_yojson, ({email}) => {
       Challenger.get_challenge(email) >>=
          (challenge) => `String(challenge) |> respond'
    }) |> post("/get_challenge");


    let solve_challenge = req_to_obj(challenge_resolve_of_yojson, ({solution, email}) => {
      Challenger.solve_challenge(email,solution) >>=
                   fun
                    | Some(email) => `String(email) |> respond'
                    | None => `String("Bad Solution. Challenge destroyed.") |> respond'(~code=Cohttp.Code.status_of_code(403))
    }) |> post("/challenge");


    let get_messages = 
      get("/messages", req => {
        switch (Auth.user(req)) {
          | Some(_) => 
                messages_to_yojson({messages: FriendsOnly.get_messages()}) |>
                Yojson.Safe.to_string |>
                (o) => `String(o) |> respond'
          | None => `String("not logged in!") |> respond'(~code=Cohttp.Code.status_of_code(403))
        }
      });

    let who_am_i =
      get("/who_am_i", req => {
        switch (Auth.user(req)) {
          | Some({username}) => `String(username) |> respond'
          | None => `String("not authenticated!") |> respond'
        };
      });

    let make_message = req_to_obj(new_message_of_yojson,
        (new_message) => {
          new_message |>
            execute_new_message >>=
            fun
              | Ok(who_be) => `String(who_be) |> respond'
              | Error(_) => `String("Bad") |> respond'(~code=Cohttp.Code.status_of_code(403))
          }
      ) |> post("/message");

    let my_logging_middleware = {
     let logger = (handler, req: Request.t) => {
             Lwt_io.printlf("connection on %S", req |> Request.uri |> Uri.to_string);
             handler(req);
     };
     Rock.Middleware.create(~filter=logger, ~name="logging middleware");
    };


    App.empty |> 
    middleware(Opium_allow_cors.allow_cors) |> 
    accept_options |>
    middleware(Opium.Middleware.debug) |>
    middleware(Opium.Middleware.trace) |> 
    middleware(my_logging_middleware) |>
    middleware(my_auth) |>
    make_message |>
    who_am_i |>
    get_messages |>
    get_challenge |>
    solve_challenge |>
    App.run_command;
  };
};
