/* jshint node: true */

// TODO: OptionalType (like UnionType but unwrapped).
// TODO: Figure out best way to save IDLs and schemas. Do we use `parse` from
// JavaScript or `assemble`?

'use strict';

/**
 * Various utilities.
 *
 */

var avro = require('avsc'),
    fs = require('fs'),
    path = require('path');


// Cache this.
var PROTOCOL = null;

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
 * Load PDU Avro type from IDL files.
 *
 */
function loadProtocol(cb) {
  if (PROTOCOL) {
    process.nextTick(function () { cb(null, PROTOCOL); });
    return;
  }

  var fpath = path.join(__dirname, '..', 'etc', 'idls', 'Pdu.avdl');
  avro.assemble(fpath, function (err, attrs) {
    if (err) {
      cb(err);
      return;
    }
    try {
      PROTOCOL = avro.parse(attrs);
    } catch (err) {
      cb(err);
      return;
    }
    cb(null, PROTOCOL);
  });
}


module.exports = {
  loadProtocol: loadProtocol,
  requireAddon: requireAddon
};
