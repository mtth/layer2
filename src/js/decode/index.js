/* jshint node: true */

/**
 * Import all decoders inside this folder. The name of the file should
 * correspond to the link type it decodes.
 *
 * A decoder is a function that takes in a buffer and returns an object.
 *
 */
(function (root) {
  'use strict';

  var stream = require('stream'),
      util = require('util'),
      utils = require('../utils');

  var decoders = utils.requireDirectory(__dirname);

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

      var decodeFn;
      try {
        decodeFn = getDecodeFn(linkType);
      } catch (err) {
        return self.emit('error', err);
      }

      self._transform = function (data, encoding, callback) {

        var frame;
        try {
          frame = decodeFn(data);
        } catch (err) {
          return callback(err);
        }
        if (frame === null) {
          this.emit('invalid', data);
          callback();
        } else {
          callback(null, frame);
        }

      };

    }

  }
  util.inherits(Decoder, stream.Transform);

  Decoder.decode = function (linkType, buf) {

    var decodeFn = getDecodeFn(linkType);
    return decodeFn(buf);

  };

  function getDecodeFn(linkType) {

    if (!linkType) {
      throw new Error('No link type specified.');
    } else if (!(linkType in decoders)) {
      throw new Error('Unsupported link type: ' + linkType);
    }
    return decoders[linkType];

  }

  root.exports = {
    Decoder: Decoder
  };

})(module);
