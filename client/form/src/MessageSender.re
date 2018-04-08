let url = "http://127.0.0.1:3000";

type error_kind = Conns | Bad_Input;
type post_state = Chilling | Pending | Success | Failed(error_kind);
type state = { 
  save_fetcher: post_state, 
  message: string,
};


type actions = UPDATE_TEXT(string) | SEND | SEND_SUCCESS | ERROR(error_kind);
let component = ReasonReact.reducerComponent("Sender");

let send_message = (self) => {
  let payload = Js.Dict.empty();
  Js.Dict.set(payload, "message", Js.Json.string(self.ReasonReact.state.message));
  Js.Promise.(
    Fetch.fetchWithInit(url ++ "/message",
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

let get_status_message = fun 
  | Pending => "Loading" |> ReasonReact.stringToElement
  | Failed(error_kind) => switch(error_kind) {
    | Conns => "Connection Error" 
    | Bad_Input => "Your signature is bad or I do not know you!"
  } |> ReasonReact.stringToElement
  | Success => "Success" |> ReasonReact.stringToElement
  | _ => ReasonReact.nullElement;

let make = (_children) => {
  ...component,
  initialState: () => { save_fetcher: Chilling, message: "" },
  reducer: (action, state: state) => {
    switch (action) {
      | UPDATE_TEXT(message) => ReasonReact.Update({...state, message: message})
      | SEND => ReasonReact.UpdateWithSideEffects({...state, save_fetcher: Pending}, send_message)
      | SEND_SUCCESS => ReasonReact.Update({...state, save_fetcher: Success})
      | ERROR(err) => ReasonReact.Update({...state, save_fetcher: Failed(err)})
    };
  },
  render: self => <div>
      <textarea
            value=(self.state.message) 
            onChange=((evt) => self.send(UPDATE_TEXT(
                    ReactDOMRe.domElementToObj(
                      ReactEventRe.Form.target(evt),
                    )##value 
            ))) ></textarea>
      {get_status_message(self.state.save_fetcher)}
      <button disabled=(self.state.save_fetcher == Pending |> Js.Boolean.to_js_boolean) onClick=((_) => self.send(SEND))>
        (ReasonReact.stringToElement("Send"))
      </button>
    </div>,
};
