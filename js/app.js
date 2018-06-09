"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var eos_1 = require("./lib/eos");
/* getTableRows("l2dex.code", "huylo", "channels", true).then(x =>
{
    console.log(x)
}) */
eos_1.eos.contract("l2dex.code", {}, function (err, res) {
    if (err)
        return console.error(err);
    console.log(res);
});
