"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var eos_1 = require("../lib/eos");
it.skip('should return empty rows on random id', function (done) {
    var rndid = Math.floor(Math.random() * 1000);
    eos_1.getTableRows("l2dex.code", "huylo" + rndid, "channels", true).then(function (x) {
        // console.log(x)
        expect(x).toBeDefined();
        expect(x.rows).toBeDefined();
        expect(x.rows).toHaveLength(0);
        done();
    });
});
it('should error on non-existant id', function (done) {
    var rndid = Math.floor(Math.random() * 1000);
    eos_1.getTableRows("l2dex.code", "huylo" + rndid, "channels", true).then(function (x) {
        done("eos should not have returned successfully! " + rndid);
    }).catch(function (err) {
        done();
    });
});
