
let component = ReasonReact.statelessComponent("Link");


let handle_click = (href, event) => {
  if(!ReactEventRe.Mouse.defaultPrevented(event)) {
    ReactEventRe.Mouse.preventDefault(event);
    Js.log("pushing" ++ href);
    ReasonReact.Router.push(href);
  }
};

let make = (~href, children) => {
  ...component,
  render: (_self) => ReasonReact.createDomElement("a", ~props={"href": href, "onClick": handle_click(href)}, children) 
};

