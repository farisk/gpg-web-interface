open Opium.Std;

open Sexplib.Std;

/* ... */
[@deriving sexp]
type user = {username: string};

/* My convention is to stick the keys inside an Env sub module. By
   not exposing this module in the mli we are preventing the user or other
   middleware from meddling with our values by not using our interface */
module Env = {
  /* or use type nonrec */
  let key: Opium.Hmap.key(user) = Opium.Hmap.Key.create(("user", [%sexp_of : user]));
};

/*
    Usually middleware gets its own module so the middleware constructor function
    is usually shortened to m. For example, [Auth.m] is obvious enough.
    The auth param (auth : username:string -> password:string -> user option)
    would represent our database model. E.g. it would do some lookup in the db
    and fetch the user.
 */
let m = (auth) => {
  let filter = (handler, req) =>
    switch (req |> Request.headers |> Cohttp.Header.get_authorization) {
    | None =>
      /* could redirect here, but we return user as an option type */
      handler(req)
    | Some(`Other(token)) => {
      Lwt_io.print("yo yo" ++ token);
      switch((auth(token))) {
       | None => failwith("your token is stale bro")
       | Some(username) => 
        /* we have a user. let's add him to req */
        let user = { username: username };
        let env = Opium.Hmap.add(Env.key, user, Request.env(req));
        let req = {...req, Request.env};
        handler(req);
      }
    }
    | Some(`Basic(username, password)) => failwith("We do not accept that authentication type old man") 
    };
  Rock.Middleware.create(~name="http basic auth", ~filter);
};


let my_auth = (token: string) => switch (token) {
  | "hello" => Some({ username: "foo@foo.com" })
  | _ => None
};

let user = (req) => Opium.Hmap.find(Env.key, Request.env(req));

