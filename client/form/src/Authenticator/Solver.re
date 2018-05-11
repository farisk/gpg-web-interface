type state = {
    solution: string,
};

type actions = SET_SOLUTION(string);

let component = ReasonReact.reducerComponent("Solver");


let make = (~challenge, ~solve, ~post_state, _children) => {
  ...component,
  initialState: () => {
      solution: "",
  },
  reducer: (action: actions, state:state) => switch(action) {
    | SET_SOLUTION(solution) => ReasonReact.Update({...state, solution: solution})
  },
  render: (self) => <div>
    <h2>(ReasonReact.stringToElement("2. Solve this: " ++ challenge))</h2>
    (switch(post_state) {
      | Some(Fetcher.Pending) => "Loading..." |> ReasonReact.stringToElement
      | Some(Fetcher.Success(msg)) =>  msg |> ReasonReact.stringToElement
      | Some(Failed(_)) => "FAILED" |> ReasonReact.stringToElement
    })
    <textarea value=self.state.solution onChange=Util.onChangeToAction((new_value) => self.send(SET_SOLUTION(new_value))) ></textarea>
    <button onClick=((_) => solve(self.state.solution))>(Util.ste("SOLVE"))</button>
  </div>
};

