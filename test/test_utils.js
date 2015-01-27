/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var assert = require('assert'),
      utils = require('../lib/utils');

  describe('Utils', function () {

    describe('BatchReadable', function () {

      var BatchReadable = utils.BatchReadable;

      it('works when fn is sync', function (done) {

        var readable = new BatchReadable(fn, {objectMode: true});
        var nBatches = 3;
        var nReads = 0;

        readable
          .on('data', function () { nReads++; })
          .on('end', function () {
            assert.equal(nReads, 30);
            done();
          });

        function fn(cb) {

          if (nBatches--) {
            cb(null, new Iterator(10));
          } else {
            cb(null, null);
          }

        }

      });

      it('works when fn is async', function (done) {

        var readable = new BatchReadable(fn, {
          objectMode: true,
          concurrency: 1
        });
        var nBatches = 3;
        var nReads = 0;
        var called = false;

        readable
          .on('data', function () { nReads++; })
          .on('end', function () {
            assert.equal(nReads, 30);
            done();
          });

        function fn(cb) {

          assert.ok(!called);
          called = true;

          setTimeout(function () {
            called = false;
            if (nBatches--) {
              cb(null, new Iterator(10));
            } else {
              cb(null, null);
            }
          }, 5);

        }

      });

      it('supports parallel batches', function (done) {

        var readable = new BatchReadable(fn, {
          objectMode: true,
          concurrency: 5
        });
        var nBatches = 10;
        var concurrency = 0;
        var maxConcurrency = 0;

        readable
          .on('data', function () {})
          .on('end', function () {
            assert.equal(maxConcurrency, 5);
            done();
          });

        function fn(cb) {

          if (++concurrency > maxConcurrency) {
            maxConcurrency = concurrency;
          }

          setTimeout(function () {
            concurrency--;
            if (nBatches--) {
              cb(null, new Iterator(10));
            } else {
              cb(null, null);
            }
          }, 5);

        }

      });

      it('continues through empty iterables', function (done) {

        var readable = new BatchReadable(fn, {
          objectMode: true,
          concurrency: 1
        });
        var nBatches = 3;
        var nReads = 0;
        var called = false;

        readable
          .on('data', function () { nReads++; })
          .on('end', function () {
            assert.equal(nReads, 20);
            done();
          });

        function fn(cb) {

          assert.ok(!called);
          called = true;

          setTimeout(function () {
            called = false;
            if (nBatches--) {
              cb(null, new Iterator(nBatches % 2 ? 0 : 10));
            } else {
              cb(null, null);
            }
          }, 5);

        }

      });

      it('transmits errors', function (done) {

        var readable = new BatchReadable(fn, {objectMode: true});
        var nBatches = 3;
        var nErrors = 0;
        var called = false;

        readable
          .on('data', function () {})
          .on('error', function () { nErrors++; })
          .on('end', function () {
            assert.equal(nErrors, 3);
            done();
          });

        function fn(cb) {

          assert.ok(!called);
          called = true;

          setTimeout(function () {
            called = false;
            if (nBatches--) {
              cb(new Error('foo'), new Iterator(10));
            } else {
              cb(null, null);
            }
          }, 5);

        }

      });

      // Helpers.

      function Iterator(n) {

        this.next = function () { return n-- || null; };

      }

    });

  });

})();
