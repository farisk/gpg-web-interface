type error_kind = Fetcher.error_kind;
type post_state = Fetcher.post_state; 
type state = {
  email: string,
  send_state: option(post_state),
  send_challenge_state: option(post_state),
};

type actions = SET_EMAIL(string) | SEND | UPDATE_SEND_STATE(post_state) | UPDATE_SOLVE_CHALLENGE_STATE(post_state);

let challenge_request = (self) => {
  let payload = Js.Dict.empty();
  Js.Dict.set(payload, "email", Js.Json.string(self.ReasonReact.state.email));
  Js.Promise.(
    Fetch.fetchWithInit(Config.api_url ++ "/get_challenge",
                        Fetch.RequestInit.make(
                          ~method_=Fetch.Post,
                          ~body=Fetch.BodyInit.make(Js.Json.stringify(Js.Json.object_(payload))),
                          ~headers=Fetch.HeadersInit.make({"Content-Type": "application/json"}),
                          (),
                        )) |>
      then_(Fetch.Response.text) |>
      then_(text => self.ReasonReact.send(UPDATE_SEND_STATE(Success(text))) |> resolve) |>
      catch(res => self.ReasonReact.send(UPDATE_SEND_STATE(Failed(Conns))) |> resolve) |>
      ignore
  );
};


let solve_challenge = (self, solution: string) => {
  let payload = Js.Dict.empty();
  Js.Dict.set(payload, "email", Js.Json.string(self.ReasonReact.state.email));
  Js.Dict.set(payload, "solution", Js.Json.string(solution));
  Js.Promise.(
    Fetch.fetchWithInit(Config.api_url ++ "/solve_challenge",
                        Fetch.RequestInit.make(
                          ~method_=Fetch.Post,
                          ~body=Fetch.BodyInit.make(Js.Json.stringify(Js.Json.object_(payload))),
                          ~headers=Fetch.HeadersInit.make({"Content-Type": "application/json"}),
                          (),
                        )) |>
      then_(Fetch.Response.text) |>
      then_(text => self.ReasonReact.send(UPDATE_SOLVE_CHALLENGE_STATE(Success(text))) |> resolve) |>
      catch(res => self.ReasonReact.send(UPDATE_SOLVE_CHALLENGE_STATE(Failed(Conns))) |> resolve) |>
      ignore
  );
};

let component = ReasonReact.reducerComponent("Authenticator");


let make = (_) => {
  ...component,
  initialState: () => {email: "", send_state: None, send_challenge_state: None},
  reducer: (action: actions, state:state) => switch (action) {
   | SET_EMAIL(email) => ReasonReact.Update({...state, email: email})
   | SEND => ReasonReact.UpdateWithSideEffects({...state,
      send_state: Some(Pending)
     }, challenge_request)
   | UPDATE_SEND_STATE(output) => ReasonReact.Update({...state, send_state: Some(output)})
   | UPDATE_SOLVE_CHALLENGE_STATE(output) => ReasonReact.Update({...state, send_challenge_state: Some(output)}) },
  render: self => <div>
    (switch (self.state.send_state) {
      | Some(Pending) => "Loading..." |> ReasonReact.stringToElement
      | Some(Success(output)) => (<Solver post_state={self.state.send_challenge_state} solve=(solve_challenge(self)) challenge=output />)
      | Some(Failed(_)) => "Failed" |> ReasonReact.stringToElement
      | None => (<div>
                    <h2>(ReasonReact.stringToElement("1. Request a challenge to authenticate yourself."))</h2>
                        <label>(ReasonReact.stringToElement("Email:"))
                          <input value=(self.state.email) onChange={(evt) => {
                            let new_value = ReactDOMRe.domElementToObj(
                              ReactEventRe.Form.target(evt),
                            )##value;
                            self.send(SET_EMAIL(new_value));
                          }}></input>
                        </label>

                        <button onClick={(_) => self.send(SEND)}>
                          (ReasonReact.stringToElement("Request"))
                        </button>
                </div>)
    })
  </div>
};



