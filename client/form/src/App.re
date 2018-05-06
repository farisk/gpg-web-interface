let component = ReasonReact.reducerComponent("App");

let make = (_) => {
  ...component,
  initialState: () => [],
  reducer: (action: list(string), _state: list(string)) => {
    ReasonReact.Update(action);
  },
  subscriptions: (self) => [
    Sub(
        () => ReasonReact.Router.watchUrl((url) => {
          self.send(url.path); 
        }), ReasonReact.Router.unwatchUrl)
  ],
  render: ({state: path}) => <div>
    <div>
      <Link href="/">(ReasonReact.stringToElement("Home"))</Link>
      <Link href="send_message">(ReasonReact.stringToElement("Send Message"))</Link>
      <Link href="authenticate">(ReasonReact.stringToElement("Authenticate"))</Link>
    </div>
    (
      switch (path) {
        | [] => <div>(ReasonReact.stringToElement("nothing to see"))</div> 
        | ["send_message"] => <MessageSender />
        | ["authenticate"] => <Authenticator />
        | _ => <div>("Not found!" |> ReasonReact.stringToElement)</div>
      }
    )
  </div>,
      
};
