import { getTableRows } from "../lib/eos"

it.skip('should return empty rows on random id', (done) =>
{
	let rndid = Math.floor(Math.random() * 1000)
	getTableRows("l2dex.code", `huylo${rndid}`, "channels", true).then(x =>
	{
		// console.log(x)

		expect(x).toBeDefined()
		expect(x.rows).toBeDefined()
		expect(x.rows).toHaveLength(0)
		
		done()
	})
})
it('should error on non-existant id', done =>
{
	let rndid = Math.floor(Math.random() * 1000)
	getTableRows("l2dex.code", `huylo${rndid}`, "channels", true).then(x =>
	{
		done(`eos should not have returned successfully! ${rndid}`)
	}).catch(err =>
	{
		done()
	})
})