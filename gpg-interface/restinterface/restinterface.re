open Opium.Std;
open Lwt.Infix;

let add_cors_headers = (headers: Cohttp.Header.t) : Cohttp.Header.t =>
  Cohttp.Header.add_list(
    headers,
    [
      ("access-control-allow-origin", "*"),
      ("access-control-allow-headers", "Accept, Content-Type"),
      (
        "access-control-allow-methods",
        "GET, HEAD, POST, DELETE, OPTIONS, PUT, PATCH",
      ),
    ],
  );


let allow_cors = {
  let filter = (handler, req) =>
      handler(req) |> Lwt.map(
      response => {
        response
        |> Response.headers
        |> add_cors_headers
        |> (headers) => {...response, Response.headers: headers};
      }
    );
  Rock.Middleware.create(~name=("allow cors"), ~filter);
};

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
type messages = {
  messages: array(string),
};

let accept_options = App.options("**", (_) => respond'(`String("OK")));

module Make = (FriendsOnly: Friendsonly.FriendsOnly,
               Challenger: Challenger.Challenger) => {

  let my_auth = Auth.m(Challenger.authenticate);

  let execute_new_message = (new_message) => {
    let s: Friendsonly.message = {body: new_message.message};
    FriendsOnly.store_message({s});
  };

  let runner = () => {

    let get_challenge = 
      get("/challenge", req => {
         Challenger.get_challenge("bullshit@bullshit.com") >>=
          (challenge) => `String(challenge) |> respond'
      });


    let solve_challenge = 
      post("/challenge", req => {
        Lwt.(
          App.string_of_body_exn(req) >>= /*TODO: remove repition*/
            (raw) => raw |>
              Yojson.Safe.from_string |>
              challenge_resolve_of_yojson |>
              fun
                | Ok({solution, email}) => 
                  {Challenger.solve_challenge(email,solution) >>=
                   fun
                    | Some(email) => `String(email) |> respond'
                    | None => `String("Scum Scum Scum") |> respond'
                    } 
                | Error(err) => `String("Bad input:" ++ err) |> respond'(~code=Cohttp.Code.status_of_code(400))
        );
      });


    /* TODO: we should make a middleware to protect this route! */
    let get_messages = 
      get("/messages", req => {
        messages_to_yojson({messages: FriendsOnly.get_messages()}) |>
        Yojson.Safe.to_string |>
        (o) => `String(o) |> respond'
      });

    let who_am_i =
      get("/who_am_i", req => {
        switch (Auth.user(req)) {
          | Some({username}) => `String(username) |> respond'
          | None => `String("not logged in!") |> respond'
        };
      });

    let make_message =
      post("/message", req => {
        Lwt.(
          App.string_of_body_exn(req) >>= 
            (raw) => raw |> 
              Yojson.Safe.from_string |> 
              new_message_of_yojson |>
              fun
                | Ok(new_message) => {
                  new_message |>
                    execute_new_message >>=
                    fun
                      | Ok(who_be) => `String(who_be) |> respond'
                      | Error(_) => `String("Bad") |> respond'(~code=Cohttp.Code.status_of_code(403))
                  }
                | Error(err) => `String("Bad input:" ++ err) |> respond'(~code=Cohttp.Code.status_of_code(400))
        )});
    let my_logging_middleware = {
     let logger = (handler, req: Request.t) => {
             Lwt_io.printlf("connection on %S", req |> Request.uri |> Uri.to_string);
             handler(req);
     };
     Rock.Middleware.create(~filter=logger, ~name="logging middleware");
    };


    App.empty |> 
    middleware(allow_cors) |> 
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
