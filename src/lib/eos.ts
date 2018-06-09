import Eos from "eosjs"

let confjs = require(`../../data/config.json`)
console.assert(confjs.pk, "pk not found in config")
console.assert(confjs.rpc, "rpc not found in config")

let config = {
	// chainId: "cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f", // 32 byte (64 char) hex string
	chainId: null,
	keyProvider: confjs.pk, // WIF string or array of keys..
	httpEndpoint: confjs.rpc,
	// mockTransactions: () => 'pass', // or 'fail'
	/* transactionHeaders: (expireInSeconds: any, callback: (error: any, headers: any)) => {
		callback(undefined, headers)
	}, */
	expireInSeconds: 60,
	broadcast: false,
	// debug: false, // API and transactions
	// debug: true,
	sign: true,
}

export let eos = Eos(config)

export let getTableRows = (code: string, scope: string, table: string, json: boolean = true) => eos.getTableRows({
	code,
	scope,
	table,
	json: json.toString()
})