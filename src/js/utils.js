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
      path = require('path');

  function requireAddon() {
    // Either debug or release depending on environment variable (defaulting to
    // the existing one if no override).

    var buildRoot = path.join(__dirname, '..', '..', 'build');
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

  root.exports = {
    requireAddon: requireAddon
  };

})(module);
