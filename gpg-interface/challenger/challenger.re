open Lwt.Infix;
/* challenger stuff, to extract */
module RandomChallenge = {
  let random_challenge = () => {
    let command = ("", [|"sh", "random.sh"|]); 
    Lwt_process.pread(command)
  };
};

/* Make exception throwing stuff into optional returns! */
module ChallengeMemory = {
    let memory = Hashtbl.create(1000);
    let store = (email: string, challenge: string) => {
      Hashtbl.replace(memory, email, challenge);
    };
    let get = (email) => {
      Hashtbl.find(memory, email);
    };
    let destroy = (email) => {
      Hashtbl.remove(memory, email);
    };
  };

module AuthMemory = {
  let memory = Hashtbl.create(1000);
  /* TODO: store date created, delete on expire*/

  let store = (email: string, token: string) => {
    Hashtbl.replace(memory, token, email);
  };

  let get_email = (token: string) => {
    switch(Hashtbl.find(memory, token)) {
      | email => Some(email)
      | exception Not_found => None
    };
  };
};

module type Challenger = {
  let get_challenge: (string) => Lwt.t(string);
  let solve_challenge: (string, string) => Lwt.t(option(string));
  let authenticate: (string) => option(string);
};

module MakeChallenger = (Crypto: Friendsonly.Crypto) => {

  let get_challenge = (email) => {
    RandomChallenge.random_challenge() >|=
    (rand) => {
      ChallengeMemory.store(email, rand);
      rand;
    };
  };

  let solve_challenge = (email:string, solution: string) => {
    let challenge = ChallengeMemory.get(email); /* TODO: handle optional */

    let form_gpg_input = "-----BEGIN PGP SIGNED MESSAGE-----\nHash: SHA256\n\n" ++ challenge ++ "\n" ++ solution;
    Lwt_io.print("the challenge is " ++ form_gpg_input);
    form_gpg_input |> 
    Crypto.check_signature >|=
      fun 
        | Ok(signed_with) => { 
          ChallengeMemory.destroy(email);
          signed_with == email ? {
            AuthMemory.store(email, challenge);
            Some("Welcome " ++ signed_with);
          }: None;
        }
        | Error(_) => None
  };

  let authenticate = (token) => {
    AuthMemory.get_email(token);
  };

};




