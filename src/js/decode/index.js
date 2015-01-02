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
   * Ha.
   *
   */
  function Frame(linkType, contents) {

    this.linkType = linkType;
    this.contents = contents;

  }

  Frame.fromBytes = function (linkType, bytes) {

    if (!(linkType in decoders)) {
      throw new Error('Unsupported link type: ' + linkType);
    }

    var contents = decoders[linkType](bytes);
    return new Frame(linkType, contents);

  };

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
    var stringify = opts.stringify || false;

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

      if (!linkType) {
        return this.emit('error', new Error('No link type specified.'));
      } else if (!(linkType in decoders)) {
        return this.emit(
          'error',
          new Error('Unsupported link type: ' + linkType)
        );
      }

      var decode = decoders[linkType];

      // Override function.
      this._transform = function (data, encoding, callback) {

        var contents;
        try {
          contents = decode(data);
        } catch (err) {
          this.emit('invalid', data, err);
          return callback();
        }

        if (stringify) {
          callback(null, JSON.stringify(contents));
        } else {
          callback(null, new Frame(linkType, contents));
        }

      };

      // And call it again now.
      this._transform(data, encoding, callback);

    };

  }
  util.inherits(Decoder, stream.Transform);

  root.exports = {
    Frame: Frame,
    Decoder: Decoder
  };

})(module);
