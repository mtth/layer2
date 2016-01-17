/* jshint mocha: true, node: true */

'use strict';

var addon = require('../lib/utils').addon,
    assert = require('assert');


suite('Addon', function () {

  suite('Utilities', function () {

    test('can stringify addresses', function () {
      var buf;
      buf = new Buffer('0123456789ab', 'hex');
      assert.equal(addon.stringifyAddress(buf), '01:23:45:67:89:ab');
      buf = new Buffer('001122334455', 'hex');
      assert.equal(addon.stringifyAddress(buf), '00:11:22:33:44:55');
      buf = new Buffer('', 'hex');
      assert.throws(function () { addon.stringifyAddress(buf); });
    });

  });


});
