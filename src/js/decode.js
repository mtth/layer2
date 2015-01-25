/* jshint node: true */

/**
 * Import all decoders inside this folder. The name of the file should
 * correspond to the link type it decodes.
 *
 * A decoder is a function that abides by the following contract:
 *
 * + Its signature is `decode(buf, assumeValid)`, where `buf` is the buffer to
 *   decode and `assumeValid` is a flag to skip the CRC check.
 * + It should return `null` when a frame is invalid. Throwing errors should be
 *   avoided (e.g. only happen on actual parsing error).
 *
 */
(function (root) {
  'use strict';

  var stream = require('stream'),
      util = require('util'),
      addon = require('./utils').requireAddon();

  /**
   * Decoder stream class.
   *
   * This class will "dynamically" look up the correct transform function based
   * on the link type of the source stream. This is done by having a
   * placeholder `_transform` method replace itself when it is first called.
   *
   */
  function Decoder(opts) {

    opts = opts || {};
    var linkType = opts.linkType || null; // Inferred below.

    stream.Transform.call(this, {objectMode: true});

    this.on('pipe', function (src) {
      // Infer link type from first capture stream piped.

      if (!linkType) {
        linkType = src.getLinkType();
      } else if (linkType !== src.getLinkType()) {
        return this.emit('error', new Error('Inconsistent link type.'));
      }

    });

    this.getLinkType = function () { return linkType; };

    this._transform = function (data, encoding, callback) {

      activate(this);
      return this._transform(data, encoding, callback);

    };

    function activate(self) {

      if (!linkType) {
        throw new Error('No link type specified.');
      }

      self._transform = function (data, encoding, callback) {

        var frame;
        try {
          frame = new addon.Frame(linkType, data);
        } catch (err) {
          err.data = data;
          return callback(err);
        }
        if (frame.isValid()) {
          callback(null, frame);
        } else {
          this.emit('invalid', frame);
          callback();
        }

      };

    }

  }
  util.inherits(Decoder, stream.Transform);

  Decoder.decode = function (linkType, buf) {

    return new addon.Frame(linkType, buf);

  };

  root.exports = {
    Decoder: Decoder
  };

})(module);
