/* jshint node: true */

/**
 * Packet decoding functions and associated streams.
 *
 */
(function (root) {
  'use strict';

  var stream = require('stream'),
      util = require('util'),
      decoders = require('./decoders'),
      extractors = require('./extractors');

  /**
   * Decoder stream class.
   *
   */
  function Decoder(opts) {

    opts = opts || {};
    var ignoreErrors = opts.ignoreErrors || false;
    var linkType = opts.linkType; // Inferred below.
    var stringify = opts.stringify || false;

    stream.Transform.call(this, {objectMode: true});

    var decode = null;

    this.on('pipe', function (src) {
      // Infer link type from first capture stream piped.

      if (!linkType) {
        linkType = src.getLinkType();
      } else if (linkType !== src.getLinkType()) {
        return this.emit('error', new Error('Inconsistent link type.'));
      }

    });

    this._transform = function (data, encoding, callback) {

      if (!decode) {
        if (!linkType) {
          return this.emit('error', new Error('No link type specified.'));
        } else if (!(linkType in decoders)) {
          return this.emit('error', new Error('Unsupported link type.'));
        }
        decode = decoders[linkType].decode;
      }

      var packet;
      try {
        packet = decode(data);
        if (stringify) {
          packet = JSON.stringify(packet);
        }
      } catch (err) {
        return ignoreErrors ? callback() : callback(err);
      }
      callback(null, packet);

    };

    this.getLinkType = function () { return linkType; };

  }
  util.inherits(Decoder, stream.Transform);

  /**
   * Extractor stream class.
   *
   * To be functional, this duplex stream must be activated first (see method
   * below).
   *
   */
  function Extractor(opts) {

    opts = opts || {};
    var fromLinkType = opts.fromLinkType;
    var toLinkType = opts.toLinkType;

    stream.Duplex.call(this, {objectMode: true});

    this.getLinkType = function (incoming) {

      return incoming ? fromLinkType : toLinkType;

    };

    // Setup the internals once we can infer the link types.

    this.on('pipe', function (src) {
      // The best case is on pipe, which gives us access to the source stream.

      if (!fromLinkType) {
        fromLinkType = src.getLinkType();
      } else if (fromLinkType !== src.getLinkType()) {
        return this.emit('error', new Error('Inconsistent source link type.'));
      }

      if (!toLinkType) {
        if (fromLinkType in extractors) {
          var toLinkTypes = Object.keys(extractors[fromLinkType]);
          if (toLinkTypes.length === 1) {
            toLinkType = toLinkTypes[0];
          } else {
            return this.emit('error', new Error('Ambiguous target link type.'));
          }
        } else {
          return this.emit('error', new Error('No valid target link type.'));
        }
      }

      activate();

    });

    // Emit 'end' event when writable sides finishes.
    this.once('finish', function () { this.push(null); });

    // If pipe wasn't called, we must still ensure that activate gets called,
    // so we proxy both `_read` and `_write` (which will then be overwritten by
    // `activate`).

    this._read = function () {

      activate();
      this._read();

    };

    this._write = function (data, encoding, callback) {

      activate();
      this._write(data, encoding, callback);

    };

    var self = this;

    function activate() {
      // Fill in the correct `_read` and `_write` functions depending on the
      // link types.

      if (fromLinkType && toLinkType) {
        var srcExtractors = extractors[fromLinkType];
        if (srcExtractors && toLinkType in srcExtractors) {
          srcExtractors[toLinkType].call(self, opts);
        } else {
          self.emit('error', new Error('Unsupported link types.'));
        }
      } else {
        self.emit('error', new Error('Missing link types.'));
      }

    }

  }
  util.inherits(Extractor, stream.Duplex);

  // Export things.

  root.exports = {
    Decoder: Decoder,
    Extractor: Extractor
  };

})(module);
