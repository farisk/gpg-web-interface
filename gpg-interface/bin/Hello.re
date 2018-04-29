module GPGChallenger = Challenger.MakeChallenger(Unixcrypto.GPGChecker); 

module Service = Restinterface.Make(Unixcrypto.BasicFriendsonly, GPGChallenger); 

Service.runner();
