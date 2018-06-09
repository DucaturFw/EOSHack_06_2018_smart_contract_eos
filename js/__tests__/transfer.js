"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var eos_1 = require("../lib/eos");
it('should load contract', function (done) {
    var p = eos_1.eos.contract("l2dex.code");
    expect(p).toBeDefined();
    p.then(function (hello) {
        expect(hello).toBeDefined();
        console.log(hello);
        var p = hello.hi('loh', { authorization: "tester1", sign: true });
        expect(p).toBeDefined();
        p.then(function (x) { return done(); }).catch(done);
    }).catch(done);
}, 25000);
it('should send tx', function (done) {
    eos_1.eos.transaction({
        actions: [{
                account: 'l2dex.code',
                name: 'hi',
                authorization: [{
                        actor: "tester1",
                        permission: 'active'
                    }],
                data: { user: "huy" }
            }]
    }).then(function (x) { return done(); }).catch(done);
}, 15000);
