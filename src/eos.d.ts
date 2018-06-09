declare module "eosjs"
{
	export default function Eos(config: any): EosInstance
	interface ITableResult<T>
	{
		rows: T[]
		more: boolean
	}
	class EosInstance
	{
		fc: any
		api: any
		getCurrencyBalance(...args: any[]): any
		getCurrencyStats(...args: any[]): any
		getProducers(...args: any[]): any
		getInfo(...args: any[]): any
		getBlock(...args: any[]): any
		getAccount(...args: any[]): any
		getCode(...args: any[]): any
		getTableRows(config: { code: string, scope: string, table: string, json?: string }): Promise<ITableResult<any>>
		abiJsonToBin(...args: any[]): any
		abiBinToJson(...args: any[]): any
		getRequiredKeys(...args: any[]): any
		pushBlock(...args: any[]): any
		pushTransaction(...args: any[]): any
		pushTransactions(...args: any[]): any
		getActions(...args: any[]): any
		getControlledAccounts(...args: any[]): any
		getKeyAccounts(...args: any[]): any
		getTransaction(...args: any[]): any
		createTransaction(...args: any[]): any
		transaction(...args: any[]): any
		bidname(...args: any[]): any
		buyram(...args: any[]): any
		buyrambytes(...args: any[]): any
		canceldelay(...args: any[]): any
		claimrewards(...args: any[]): any
		delegatebw(...args: any[]): any
		deleteauth(...args: any[]): any
		linkauth(...args: any[]): any
		newaccount(...args: any[]): any
		onerror(...args: any[]): any
		refund(...args: any[]): any
		regproducer(...args: any[]): any
		regproxy(...args: any[]): any
		reqauth(...args: any[]): any
		rmvproducer(...args: any[]): any
		sellram(...args: any[]): any
		setalimits(...args: any[]): any
		setglimits(...args: any[]): any
		setprods(...args: any[]): any
		setabi(...args: any[]): any
		setcode(...args: any[]): any
		setparams(...args: any[]): any
		setpriv(...args: any[]): any
		setram(...args: any[]): any
		undelegatebw(...args: any[]): any
		unlinkauth(...args: any[]): any
		unregprod(...args: any[]): any
		updateauth(...args: any[]): any
		voteproducer(...args: any[]): any
		create(...args: any[]): any
		issue(...args: any[]): any
		transfer(...args: any[]): any
		contract(...args: any[]): any
	}
}