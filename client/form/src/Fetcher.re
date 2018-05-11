type error_kind = Conns | Bad_Input;
type post_state = Pending | Success(string) | Failed(error_kind);
