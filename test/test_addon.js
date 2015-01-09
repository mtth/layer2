/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var utils = require('../src/js/utils'),
      assert = require('assert');

  describe('Addon', function () {

    var addon = utils.requireAddon();

    describe('Utils', function () {

      it('can parse MAC addresses', function () {

        var buf = new Buffer('0123456789ab001122334455', 'hex');
        assert.equal(addon.readMacAddr(buf, 0), '01:23:45:67:89:ab');
        assert.equal(addon.readMacAddr(buf, 6), '00:11:22:33:44:55');
        assert.throws(function () { addon.readMacAddr(buf, 8); });

      });

    });

  });


})();
