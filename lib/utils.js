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
      stream = require('stream');

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
   * Batch loaded stream.
   *
   * @param {Object} `opts` Options forwarded to `stream.Readable`, along with
   * the following: `batchHighWatermark`, the maximum number of batches
   * preemptively fetched [default: `2`]; `concurrency`, the maximum number of
   * parallel batch calls [default: `1`]. Note that order isn't guaranteed when
   * multiple requests are made in parallel.
   *
   * Instead of implementing a `_read` method, suffices now to implement a
   * `_batch(cb)` method, with `cb(err, iter)`.
   *
   * This is useful when feeding a stream from a secondary thread (e.g. when
   * writing a CPP addon).
   *
   */
  function BatchStream(opts) {

    opts = opts || {};
    var batchHighWaterMark = opts.batchHighWaterMark || 2;
    var concurrency = opts.concurrency || 1;
    var duplex = opts.duplex || false;

    if (duplex) {
      stream.Duplex.call(this, opts);
    } else {
      stream.Readable.call(this, opts);
    }

    var self = this;
    var iters = [];
    var needPush = false;
    var finishing = false;
    var pending = 0;

    this._read = function () {

      var elem = null;
      while (iters.length && (elem = iters[0].next()).done) {
        // Consume all iterators until we find something, or run out.
        iters.shift();
      }

      if (elem && elem.value) {
        this.push(elem.value);
      } else if (finishing && !pending) {
        this.push(null);
      } else {
        needPush = true;
      }

      if (
        pending < concurrency &&
        !finishing &&
        iters.length < batchHighWaterMark
      ) {
        pending++;
        this._batch(batchCb);
      }

    };

    this.finish = function () { finishing = true; };

    function batchCb(err, iter) {

      pending--;
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

  /**
   * Wrap raw iterator.
   *
   * Creating objects from CPP is very slow, so we wrap from JS.
   *
   */
  function Iterator(iter) {

    this.next = function () {

      var elem = iter.next();
      return elem === null ? {done: true} : {value: elem};

    };

  }

  root.exports = {
    BatchStream: BatchStream,
    Iterator: Iterator,
    requireAddon: requireAddon
  };

})(module);
