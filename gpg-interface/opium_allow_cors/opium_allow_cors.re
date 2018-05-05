open Opium.Std;

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
