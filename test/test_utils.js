/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var utils = require('../src/js/utils'),
      assert = require('assert');

  describe('Utils', function () {

    describe ('crc32', function () {

      var crc32 = utils.crc32;

      it('computes a correct checksum on a buffer', function () {

        assert.equal(crc32(new Buffer('aa02', 'hex')), 4215220144);

      });

    });

  });


})();
