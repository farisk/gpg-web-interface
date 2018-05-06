type error_kind = Conns | Bad_Input;
type post_state = Pending | Success | Failed(error_kind);
type state = {
  email: string,
  send_state: option(post_state),
};

type actions = SET_EMAIL(string) | SEND | SEND_SUCCESS | ERROR(error_kind);

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
      then_(res => (self.ReasonReact.send(res |> Fetch.Response.ok ? SEND_SUCCESS : ERROR(Bad_Input)) |> resolve)) |>
      catch(res => self.ReasonReact.send(ERROR(Conns)) |> resolve) |>
      ignore
  );
};


let component = ReasonReact.reducerComponent("Authenticator");


let make = (_) => {
  ...component,
  initialState: () => {email: "", send_state: None},
  reducer: (action: actions, state:state) => switch (action) {
   | SET_EMAIL(email) => ReasonReact.Update({...state, email: email})
   | SEND => ReasonReact.UpdateWithSideEffects({...state,
      send_state: Some(Pending)
     }, challenge_request)
   | SEND_SUCCESS => ReasonReact.Update({...state, send_state: Some(Success)})
  },
  render: self => <div>
    <h2>(ReasonReact.stringToElement("Request a challenge to authenticate yourself."))</h2>
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
    ( switch (self.state.send_state) {
      | Some(Pending) => "Loading..."
      | Some(Success) => "Complte"
      | Some(Failed(_)) => "Failed"
      | None => ""
    } |> ReasonReact.stringToElement)
  </div>
};



