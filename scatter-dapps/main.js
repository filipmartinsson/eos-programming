ScatterJS.plugins( new ScatterEOS() );

const network = ScatterJS.Network.fromJson({
    blockchain:'eos',
    chainId:'cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f',
    host:'127.0.0.1',
    port:8888,
    protocol:'http'
});
ScatterJS.connect('TestDapp', {network}).then(connected => {
    if(!connected) return false;
    const scatter = ScatterJS.scatter;
    //window.ScatterJS = null;

    scatter.login({accounts: [network]}).then(function(a){
      console.log(a);
      const account = scatter.account('eos')
      console.log(account);
      const eosOptions = { expireInSeconds:60 };

      const rpc = new eosjs_jsonrpc.JsonRpc(network.fullhost());
      //const api = new eosjs_api.Api({ rpc, signatureProvider });

      const eos = ScatterJS.eos(network, eosjs_api.Api, {rpc});
      const transactionOptions = { authorization:[`${account.name}@${account.authority}`] };
      /*eos.getTableRows({json:true, scope: 'payable', code: 'payable', table: 'dogs'}).then(res => {
         console.log(res.rows);
       })*/

       eos.transact({
             actions: [{
                 account: 'eosio.token',
                 name: 'transfer',
                 authorization: [{
                     actor: account.name,
                     permission: account.authority,
                 }],
                 data: {
                     from: account.name,
                     to: 'payable',
                     quantity: '10 DOGCOIN',
                     memo: "mem",
                 },
             }]
         }, {
             blocksBehind: 3,
             expireSeconds: 30,
         }).then(function(res){
             console.log('sent: ', res);
         }).catch(function(err){
             console.error('error: ', err);
         });

    });

});

/*
    const network = {
      blockchain:'eos',
      protocol:'https',
      host:'nodes.get-scatter.com',
      port:443,
      chainId:'aca376f206b8fc25a6ed44dbdc66547c36c6c33e3a119ffbeaef943642f0e906'
  }

    document.addEventListener("DOMContentLoaded", function() {
      ScatterJS.scatter.connect("TestDapp").then(connected => {
       // User does not have Scatter Desktop, Mobile or Classic installed.
       if(!connected) return false;
       const scatter = ScatterJS.scatter;
       window.ScatterJS = null;

       const requiredFields = {accounts:[network]};
       scatter.getIdentity(requiredFields, function(){
         const account = scatter.identity.accounts.find(function(x){
           x.blockchain === 'eos';
         });
         console.log(account);
       });


    });
    });
*/
