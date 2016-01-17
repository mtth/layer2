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

// CPP binding.
//
// Either the debug or release version will be returned depending on the
// `LAYER2_DEBUG` environment variable (defaulting to the existing one if none
// is specified). An error is raised if the variable is specified and the
// corresponding addon isn't found.
var ADDON = (function () {
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
})();


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
    logicalTypes: {
      address: AddressType,
      'timestamp-millis': DateType
    },
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

  function Dot11Pdu() {}

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
      util.inherits(type.getRecordConstructor(), Dot11Pdu);
    }
  });

}


/**
 * Custom type to parse timestamps as dates.
 *
 */
function DateType(attrs, opts) {
  avro.types.LogicalType.call(this, attrs, opts, [avro.types.LongType]);
}
util.inherits(DateType, avro.types.LogicalType);

DateType.prototype._fromValue = function (val) { return new Date(val); };

DateType.prototype._toValue = function (date) { return +date; };

/**
 * Custom type to better display addresses (e.g. MAC).
 *
 */
function AddressType(attrs, opts) {
  avro.types.LogicalType.call(this, attrs, opts, [avro.types.FixedType]);

  var size = this.getUnderlyingType().getSize();
  this._constructor = Address;

  function Address(buf) {
    if (buf.length !== size) {
      throw new Error('invalid address');
    }
    this._buf = buf;
  }

  Address.prototype.toString = function () {
    return ADDON.stringifyAddress(this._buf);
  };

  Address.prototype.isMulticast = function () {
    var buf = this._buf;
    if (buf[0] === 51 && buf[1] === 51) {
      return true;
    }
    if (buf[0] === 1 && (!buf[1] && buf[2] === 94)) {
      return true;
    }
    var i, l;
    for (i = 0, l = buf.length; i < l; i++) {
      if (buf[i] !== 255) {
        return false;
      }
    }
    return true;
  };

  Address.prototype.inspect = Address.prototype.toString;
}
util.inherits(AddressType, avro.types.LogicalType);

AddressType.prototype._fromValue = function (val) {
  return new this._constructor(val);
};

AddressType.prototype._toValue = function (addr) { return addr._buf; };


module.exports = {
  addon: ADDON,
  loadProtocol: loadProtocol
};
