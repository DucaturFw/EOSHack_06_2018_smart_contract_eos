import { eos } from "../lib/eos"

it('should load contract', done =>
{
	let p = eos.contract("l2dex.code")
	expect(p).toBeDefined()
	p.then((hello: any) =>
	{
		expect(hello).toBeDefined()
		console.log(hello)
		let p = hello.hi('loh', { authorization: "tester1", sign: true })
		expect(p).toBeDefined()
		p.then((x: any) => done()).catch(done)
	}).catch(done)
}, 25000)

it('should send tx', done =>
{
	eos.transaction({
		actions: [{
			account: 'l2dex.code',
			name: 'hi',
			authorization: [{
				actor: "tester1",
				permission: 'active'
			}],
			data: { user: "huy" }
		}]
	}).then((x: any) => done()).catch(done)
}, 15000)