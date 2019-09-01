ScatterJS.plugins( new ScatterEOS() );

const network = ScatterJS.Network.fromJson({
    blockchain:'eos',
    chainId:'cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f',
    host:'127.0.0.1',
    port:8888,
    protocol:'http'
});

const contractConfig = {
  code: "dogcontract",
  scope: "dogcontract",
  dogTableName: "dogs",
  balancesTableName: "balances",
  symbol: "DOGCOIN"
}

var eos;
var rpc;
var account;

ScatterJS.connect('DogDapp', {network}).then(connected => {
  if(!connected) return alert("No Scatter Detected");
  console.log("Scatter Connected");

  const scatter = ScatterJS.scatter;
  window.ScatterJS = null;

  scatter.login({accounts: [network]}).then(function(){
    account = scatter.account('eos');
    rpc = new eosjs_jsonrpc.JsonRpc(network.fullhost());
    eos = scatter.eos(network, eosjs_api.Api, {rpc});
    getDogs();
    getBalances();
  });

});

function getDogs(){
  rpc.get_table_rows({
    json: true,
    code: contractConfig.code,
    scope: contractConfig.scope,
    table: contractConfig.dogTableName,
    index_position: 2,
    key_type: "name",
    lower_bound: account.name,
    upper_bound: account.name
  }).then(function(res){
    console.log(res);
    populateDogList(res.rows);
  })
}
function getBalances(){
  rpc.get_table_rows({
    json: true,
    code: contractConfig.code,
    scope: account.name,
    table: contractConfig.balancesTableName,
  }).then(function(res){
    console.log(res);
    populateBalanceList(res.rows);
  })
}

function populateDogList(dogs){
  $("#doglist").empty();
  var ul = document.getElementById("doglist");
  for (var i = 0; i < dogs.length; i++) {
    var li = document.createElement("li");
    li.appendChild(document.createTextNode(dogs[i].id + ": " + dogs[i].dog_name + ", " + dogs[i].age));
    ul.appendChild(li);
  }
}
function populateBalanceList(balances){
  $("#balance_list").empty();
  var ul = document.getElementById("balance_list");
  for (var i = 0; i < balances.length; i++) {
    var li = document.createElement("li");
    li.appendChild(document.createTextNode(balances[i].funds));
    ul.appendChild(li);
  }
}
function addDog(){
  var dogName = $("#dog_name").val();
  var dogAge = $("#dog_age").val();

  eos.transact({
    actions: [{
      account: contractConfig.code,
      name: 'insert',
      authorization: [{
        actor: account.name,
        permission: account.authority
      }],
      data: {
        owner: account.name,
        dog_name: dogName,
        age: dogAge
      }
    }]
  }, {
    blocksBehind: 3,
    expireSeconds: 30
  }).then(function(res){
    getDogs();
    getBalances();
  }).catch(function(err){
    alert(err);
  })
}
function deposit(){
  var quantity = $("#depositvalue").val();
  var asset = quantity + " " + contractConfig.symbol;

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
                to: contractConfig.code,
                quantity: asset,
                memo: "DogDapp Deposit"
            },
        }]
    }, {
        blocksBehind: 3,
        expireSeconds: 30,
    }).then(function(res){
        getBalances();
    }).catch(function(err){
        alert('error: ', err);
    });
}
function removeDog(){
  var dogId = $("#dog_id").val();
  eos.transact({
        actions: [{
            account: contractConfig.code,
            name: 'erase',
            authorization: [{
                actor: account.name,
                permission: account.authority,
            }],
            data: {
                dog_id: dogId
            },
        }]
    }, {
        blocksBehind: 3,
        expireSeconds: 30,
    }).then(function(res){
        getDogs();
    }).catch(function(err){
        alert('error: ', err);
    });
}
function removeAllMyDogs(){
  eos.transact({
        actions: [{
            account: contractConfig.code,
            name: 'removeall',
            authorization: [{
                actor: account.name,
                permission: account.authority,
            }],
            data: {
                user: account.name
            },
        }]
    }, {
        blocksBehind: 3,
        expireSeconds: 30,
    }).then(function(res){
        getDogs();
    }).catch(function(err){
        alert('error: ', err);
    });
}
$(document).ready(function() {
  $("#add_dog_button").click(addDog);
  $("#removeButton").click(removeDog);
  $("#removeAllMyDogsButton").click(removeAllMyDogs);
  $("#depositButton").click(deposit);

});
