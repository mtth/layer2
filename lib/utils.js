/* jshint node: true */

// TODO: Use an OptionalType (like UnionType but unwrapped), for all unions
// with only two types, one of which is null.

'use strict';

/**
 * Various utilities.
 *
 */

var avro = require('avsc'),
    fs = require('fs'),
    path = require('path'),
    util = require('util');


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

  var opts = {
    logicalTypes: {'timestamp-millis': DateType},
    registry: {}
  };

  var fpath = path.join(__dirname, '..', 'etc', 'idls', 'Pdu.avdl');
  avro.assemble(fpath, function (err, attrs) {
    if (err) {
      cb(err);
      return;
    }
    try {
      PROTOCOL = avro.parse(attrs, opts);
    } catch (err) {
      cb(err);
      return;
    }

    var types = [];
    Object.keys(opts.registry).forEach(function (name) {
      var type = opts.registry[name];
      if (type.getName(true) === 'record') {
        types.push(type);
      }
    });
    augmentTypes(types);

    cb(null, PROTOCOL);
  });
}

function augmentTypes(types) {

  // 802.11

  var DOT11_TYPES = ['MGMT', 'CTRL', 'DATA'];

  var DOT11_MGMT_SUBTYPES = [
    'ASSOC_REQ',
    'ASSOC_RES',
    'REASSOC_REQ',
    'REASSOC_RES',
    'PROBE_REQ',
    'PROBE_RESP',
    'RESERVED',
    'RESERVED',
    'BEACON',
    'ATIM',
    'DISASSOC',
    'AUTH',
    'DEAUTH',
    'ACTION'
  ];

  var DOT11_CTRL_SUBTYPES = [
    'BLOCK_ACK_REQ',
    'BLOCK_ACK',
    'PS',
    'RTS',
    'CTS',
    'ACK',
    'CF_END',
    'CF_END_ACK'
  ];

  var DOT11_DATA_SUBTYPES = [
    'DATA',
    'DATA_CF_ACK',
    'DATA_CF_POLL',
    'DATA_CF_ACK_CF_POLL',
    'NULL',
    'CF_ACK',
    'CF_POLL',
    'CF_ACK_CF_POLL',
    'QOS_DATA',
    'QOS_DATA_CF_ACK',
    'QOS_DATA_CF_POLL',
    'QOS_DATA_CF_ACK_CF_POLL',
    'QOS_NULL',
    'RESERVED',
    'QOS_CF_POLL',
    'QOS_CF_ACK_CF_POLL'
  ];

  function Dot11Pdu() {}

  Dot11Pdu.prototype.getType = function () {
    return DOT11_TYPES[this.header.type];
  };

  Dot11Pdu.prototype.getSubtype = function () {
    switch (this.header.type) {
      case 0: return DOT11_MGMT_SUBTYPES[this.header.subtype];
      case 1: return DOT11_CTRL_SUBTYPES[this.header.subtype - 8];
      case 2: return DOT11_DATA_SUBTYPES[this.header.subtype];
    }
  };

  Dot11Pdu.prototype._getSubheader = function () {
    switch (this.header.type) {
      case 0: return this.mgmtHeader;
      case 2: return this.dataHeader;
      default: return; // No subheader in control frames.
    }
  };

  Dot11Pdu.prototype.getSrcAddr = function () {
    var subheader = this._getSubheader();
    if (subheader) {
      switch (this.header.toDs + 2 * this.header.fromDs) {
        case 0:
        case 1:
          return subheader.addr2;
        case 2:
          return subheader.addr3;
        case 3:
          return subheader.addr4;
      }
    }
  };

  Dot11Pdu.prototype.getDstAddr = function () {
    var subheader = this._getSubheader();
    if (subheader) {
      switch (this.header.toDs + 2 * this.header.fromDs) {
        case 0:
        case 2:
          return this.header.addr1;
        case 1:
          return subheader.addr3;
        case 3:
          return subheader.addr3;
      }
    }
  };

  Dot11Pdu.prototype.getApAddr = function () {
    var subheader = this._getSubheader();
    if (subheader) {
      switch (this.header.toDs + 2 * this.header.fromDs) {
        case 0:
          return subheader.addr3;
        case 1:
          return this.header.addr1;
        case 2:
          return subheader.addr3;
        case 3:
          // Not clear which one to return, so return none.
      }
    }
  };

  types.forEach(function (type) {
    if (/^dot11\./.test(type.getName())) {
      var Record = type.getRecordConstructor();
      Record.prototype.getType = Dot11Pdu.prototype.getType;
      util.inherits(Record, Dot11Pdu);
    }
  });

}


function DateType(attrs, opts) {
  avro.types.LogicalType.call(this, attrs, opts, [avro.types.LongType]);
}
util.inherits(DateType, avro.types.LogicalType);

DateType.prototype._fromValue = function (val) { return new Date(val); };
DateType.prototype._toValue = function (date) { return +date; };


module.exports = {
  loadProtocol: loadProtocol,
  requireAddon: requireAddon
};
