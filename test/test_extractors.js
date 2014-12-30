/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var assert = require('assert'),
      extractors = require('../lib/extractors'); // Not exposed via dot11.

  describe('Extractor', function () {

    describe('IEEE802_11_RADIO -> IEEE802_11', function () {

      var extract = extractors.IEEE802_11_RADIO.IEEE802_11.call({});

      it('extracts frames', function () {

        assert.deepEqual(
          extract(
            new Buffer('000020006708040054c6b82400000000220cdaa002000000400100003c142411aa', 'hex')
          ),
          new Buffer('aa', 'hex')
        );

      });

    });

    describe('EN10MB -> RAW', function () {

      var extract = extractors.EN10MB.RAW.call({});

      it('extracts ipv4 frames', function () {

        assert.deepEqual(
          extract(
            new Buffer('8438355f8e8a08863b3b39c70800aaaa01234567', 'hex')
          ),
          new Buffer('aaaa', 'hex')
        );

      });

      it('skips arp frames', function () {

        assert.equal(
          extract(
            new Buffer('8438355f8e8a08863b3b39c70806000108000604000108863b3b39c7c0a80201000000000000c0a80225', 'hex')
          ),
          null
        );

      });

    });

  });

})();
