/* jshint mocha: true, node: true */

'use strict';

var utils = require('../lib/utils'),
    assert = require('assert');


suite('utils', function () {

  test('stringify addresses', function () {
    var buf;
    buf = new Buffer('0123456789ab', 'hex');
    assert.equal(utils.stringifyAddress(buf), '01:23:45:67:89:ab');
    buf = new Buffer('001122334455', 'hex');
    assert.equal(utils.stringifyAddress(buf), '00:11:22:33:44:55');
    buf = new Buffer('', 'hex');
    assert.throws(function () { utils.stringifyAddress(buf); });
  });

});
