/* jshint node: true */

/**
 * Various utilities.
 *
 * (If only the built-in`util` package had a different name...)
 *
 */
(function (root) {
  'use strict';

  var fs = require('fs'),
      path = require('path'),
      stream = require('stream'),
      util = require('util');

  /**
   * Require CPP addon.
   *
   * Either the debug or release version will be returned depending on the
   * `LAYER2_DEBUG` environment variable (defaulting to the existing one if
   * none is specified). An error is raised if the variable is specified and
   * the corresponding addon isn't found.
   *
   */
  function requireAddon() {

    var buildRoot = path.join(__dirname, '..', 'build');
    var override = process.env.LAYER2_DEBUG;
    var debug;

    if (override) {
      debug = parseInt(override);
    } else {
      debug = fs.existsSync(path.join(buildRoot, 'Debug'));
    }

    var buildFolder = debug ?
      path.join(buildRoot, 'Debug') :
      path.join(buildRoot, 'Release');

    if (!fs.existsSync(buildFolder)) {
      throw new Error('No addon found at: ' + buildFolder);
    } else {
      return require(buildFolder);
    }

  }

  /**
   * Batch loaded readable stream.
   *
   * @param {Function} `fn(cb)`, where `cb(err, iter)`. Function used to feed
   * the stream. It should pass an object with a `.next()` method to its
   * callback or `null` to signal EOT. The iterator is assumed to be exhausted
   * the first time it returns `null`. This function is guaranteed to only be
   * called once at a time (i.e. it won't be called again until it returns).
   * @param {Object} `opts` Options forwarded to `stream.Readable`, along with
   * the following: `batchHighWatermark`, the maximum number of batches
   * preemptively fetched [default: `2`]; `concurrency`, the maximum number of
   * parallel batch calls [default: `1`]. Note that order isn't guaranteed when
   * multiple requests are made in parallel.
   *
   * This is useful when feeding a stream from a secondary thread (e.g. when
   * writing a CPP addon).
   *
   * Example:
   *
   *    var nBatches = 10;
   *
   *    function Iterator(n) {
   *      this.next = function () { return n-- || null; };
   *    }
   *
   *    function fetchBatch(cb) {
   *      setTimeout(function () {
   *        if (nBatches--) {
   *          cb(null, new Iterator(5));
   *        } else {
   *          cb(null, null);
   *        }
   *      }, 50);
   *    }
   *
   *    var stream = new BatchReadable(fn, {objectMode: true});
   *
   */
  function BatchReadable(fn, opts) {

    opts = opts || {};
    var batchHighWatermark = opts.batchHighWatermark || 2;
    var concurrency = opts.concurrency || 1;
    stream.Readable.call(this, opts);

    var self = this;
    var iters = [];
    var needPush = false;
    var finishing = false;

    this._read = function () {

      var elem = null;
      while (iters.length && (elem = iters[0].next()) === null) {
        // Consume all iterators until we find something, or run out.
        iters.shift();
      }

      if (!iters.length && !finishing) {
        // `elem` must be null too. We wait for the next batch to arrive.
        needPush = true;
      } else {
        // `elem` is null if finishing and not otherwise. Push it either way!
        this.push(elem);
      }

      if (concurrency > 0 && !finishing && iters.length < batchHighWatermark) {
        concurrency--;
        fn(batchCb);
      }

    };

    function batchCb(err, iter) {

      concurrency++;
      if (err) {
        self.emit('error', err);
        // Don't return though.
      }
      if (!iter) {
        finishing = true;
      } else {
        iters.push(iter);
      }
      if (needPush) {
        needPush = false;
        self._read();
      }

    }

  }
  util.inherits(BatchReadable, stream.Readable);

  root.exports = {
    BatchReadable: BatchReadable,
    requireAddon: requireAddon
  };

})(module);
