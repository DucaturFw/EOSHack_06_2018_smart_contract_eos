"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
var eosjs_1 = __importDefault(require("eosjs"));
var confjs = require("../../data/config.json");
console.assert(confjs.pk, "pk not found in config");
console.assert(confjs.rpc, "rpc not found in config");
var config = {
    // chainId: "cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f", // 32 byte (64 char) hex string
    chainId: null,
    keyProvider: confjs.pk,
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
};
exports.eos = eosjs_1.default(config);
exports.getTableRows = function (code, scope, table, json) {
    if (json === void 0) { json = true; }
    return exports.eos.getTableRows({
        code: code,
        scope: scope,
        table: table,
        json: json.toString()
    });
};
