/* jshint node: true */

/**
 * Import all extractors inside this folder. The name of the file should
 * correspond to the link type it extracts _from_.
 *
 * An extractor is a "class" that provides `_read` and `_write` methods
 * (compatible with the duplex stream interface). It takes as constructor
 * arguments a single dictionary of options.
 *
 * TODO: Switch these to actual extractors (i.e. from link layer to IP layer).
 *
 */
(function (root) {
  'use strict';

  var stream = require('stream'),
      util = require('util'),
      utils = require('../utils');

  var extractors = utils.requireDirectory(__dirname);

  /**
   * Extractor stream class.
   *
   * Similarly to the decoder stream above, in order to be functional, this
   * duplex stream must be activated first (see method below). The first time
   * `_read` or `_write` is called, the extractor will look up the appropriate
   * extracting function based on the link type and override both methods.
   *
   */
  function Extractor(opts) {

    opts = opts || {};
    var linkType = opts.linkType;

    stream.Duplex.call(this, {objectMode: true});

    // TODO: extract method (from a list of buffers)?

    this.getLinkType = function () { return linkType; };

    this.on('pipe', function (src) {

      if (!linkType) {
        linkType = src.getLinkType();
      } else if (linkType !== src.getLinkType()) {
        return this.emit('error', new Error('Inconsistent link type.'));
      }

    });

    this.once('finish', function () { this.push(null); });
    // Emit 'end' event when writable sides finishes (duplex streams don't seem
    // to do so, unlike transform streams).

    this._read = function () {

      activate(this);
      return this._read();

    };

    this._write = function (data, encoding, callback) {

      activate(this);
      return this._write(data, encoding, callback);

    };

    function activate(self) {

      if (!linkType) {
        return self.emit('error', new Error('No link type specified.'));
      } else if (!(linkType in extractors)) {
        return self.emit(
          'error',
          new Error('Unsupported link type: ' + linkType)
        );
      }

      extractors[linkType].call(self);

    }

  }
  util.inherits(Extractor, stream.Duplex);

  root.exports = {
    Extractor: Extractor
  };

})(module);
