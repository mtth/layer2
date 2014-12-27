/* jshint node: true */

/**
 * Packet decoding functions and associated streams.
 *
 */
(function (root) {
  'use strict';

  var stream = require('stream'),
      util = require('util'),
      decoders = require('./decoders');

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

  // Extractors.

  var extractors = {}; // Link type specific implementations.

  extractors.IEEE802_11_RADIO = {}; // Radiotap.

  extractors.IEEE802_11_RADIO.IEEE802_11_FRAME = function (opts) {
    // Basic implementation of a sync transform object stream (i.e. where the
    // `_transform` function is always synchronous).

    var ignoreErrors = opts.ignoreErrors || false;

    var needPacket = false;
    var packet = null;

    this._read = function () {

      if (packet) {
        var buf = packet;
        var frame;
        packet = null;
        needPacket = false;
        try {
          frame = extract(buf);
        } catch (err) {
          if (ignoreErrors) {
            needPacket = true;
          } else {
            this.emit(err);
          }
        }
        this.push(frame);
      } else {
        needPacket = true;
      }

    };

    this._write = function (data, encoding, callback) {

      packet = data;
      if (needPacket) {
        this._read();
      }
      return callback();

    };

    function extract(buf) {

      var headerLength = buf[2] + 256 * buf[3];
      var body = new Buffer(buf.length - headerLength);
      buf.copy(body, 0, headerLength);
      return body;

    }

  };

  /**
   * Extractor stream class.
   *
   * This 
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

  // Helpers.


  root.exports = {
    Decoder: Decoder,
    Extractor: Extractor
  };

})(module);
