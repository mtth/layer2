/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var utils = require('../src/js/utils'),
      assert = require('assert');

  describe('Utils', function () {

    describe('Addon', function () {

      var addon = utils.requireAddon();

      it('can parse MAC addresses', function () {

        var buf = new Buffer('0123456789ab001122334455', 'hex');
        assert.equal(addon.readMacAddr(buf, 0), '01:23:45:67:89:ab');
        assert.equal(addon.readMacAddr(buf, 6), '00:11:22:33:44:55');
        assert.throws(function () { addon.readMacAddr(buf, 8); });

      });

      it('can pretend decode', function () {

        var buf = new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c142411b4007c013ce072e6612bcc03fadc202a719fe3d6', 'hex');
        // assert.equal(addon.decoders.IEEE802_11_RADIO(buf, 0));
        buf = new Buffer('000019006f0800009627433d00000000560c1c164001d0a4010862', 'hex');
        // assert.equal(addon.decoders.IEEE802_11_RADIO(buf, 0));

      });

    });

  });


})();
