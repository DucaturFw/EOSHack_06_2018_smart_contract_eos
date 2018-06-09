import { eos, getTableRows } from "./lib/eos"

/* getTableRows("l2dex.code", "huylo", "channels", true).then(x =>
{
	console.log(x)
}) */
eos.contract("l2dex.code", {}, (err: any, res: any) =>
{
	if (err)
		return console.error(err)
	
	console.log(res)
})