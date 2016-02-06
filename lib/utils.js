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


// CPP binding.
//
// Either the debug or release version will be returned depending on the
// `LAYER2_DEBUG` environment variable (defaulting to the existing one if none
// is specified). An error is raised if the variable is specified and the
// corresponding addon isn't found.
var ADDON = (function () {
  var override = process.env.LAYER2_DEBUG;
  var debug = override && parseInt(override);
  var dpath = path.join(__dirname, '..', 'build', debug ? 'Debug' : 'Release');
  if (!fs.existsSync(dpath)) {
    throw new Error('No addon found at: ' + dpath);
  } else {
    return require(dpath);
  }
})();


/**
 * Load PDU Avro type from IDL files.
 *
 */
function loadProtocol(cb) {
  var opts = {
    logicalTypes: {
      address: AddressType,
      'timestamp-millis': DateType
    },
    registry: {},
    typeHook: createTypeHook()
  };

  var fpath = path.join(__dirname, '..', 'etc', 'idls', 'Pdu.avdl');
  avro.assemble(fpath, function (err, attrs) {
    if (err) {
      cb(err);
      return;
    }
    try {
      var protocol = avro.parse(attrs, opts);
    } catch (err) {
      cb(err);
      return;
    }

    // Augment all types.
    Object.keys(opts.registry).forEach(function (name) {
      var type = opts.registry[name];
      if (type.getName(true) === 'record') {
        if (/^dot11\./.test(type.getName())) {
          util.inherits(type.getRecordConstructor(), Dot11Frame);
        }
      }
    });

    cb(null, protocol);
  });
}

/**
 * Generate typehook which does the following:
 *
 *  + Unwraps "optional" unions (of the form `["null", ???]`).
 *  + Inlines fields with an `inline` annotation.
 *
 */
function createTypeHook() {
  var visited = [];

  return function typeHook(attrs, opts) {
    if (~visited.indexOf(attrs)) {
      return;
    }
    visited.push(attrs);

    if (attrs instanceof Array && attrs[0] === 'null' && attrs.length === 2) {
      return new OptionalType(attrs, opts);
    }

    if (attrs.type !== 'record') {
      // Following inlining only applies to records.
      return;
    }

    var fields = [];
    var namespace = opts.namespace;
    opts.namespace = attrs.namespace || opts.namespace;

    attrs.fields.forEach(function (fieldAttrs) {
      if (!fieldAttrs.inline) {
        // Default behavior.
        fields.push(fieldAttrs);
        return;
      }

      var type;
      if (typeof fieldAttrs.type === 'string') {
        // This is a reference, we must obtain the underlying type.
        var name = fieldAttrs.type;
        if (!~name.indexOf('.') && opts.namespace) {
          name = opts.namespace + '.' + name;
        }
        type = opts.registry[name];
        if (!type) {
          // Just add the undefined reference so that `parse` can throw an
          // appropriate "undefined type" error.
          fields.push(fieldAttrs);
          return;
        }
      } else {
        type = avro.parse(fieldAttrs.type, opts);
      }

      if (type.getName(true) !== 'record') {
        throw new Error('only record fields can be inlined');
      }

      type.getFields().forEach(function (inlinedField) {
        // Note that we don't need to check for duplicates here since that will
        // be done by `parse` when it creates the type after the hook returns.
        fields.push({
          aliases: inlinedField.getAliases(),
          'default': inlinedField.getDefault(),
          name: inlinedField.getName(),
          order: computeOrder(inlinedField.getOrder()),
          type: inlinedField.getType()
        });
      });

      function computeOrder(order) {
        // If the inlined field has a descending order, we must "invert" all
        // its fields' orders. Similarly for ignored.
        if (order === 'ignore' || fieldAttrs.order === 'ignore') {
          return 'ignore';
        }
        switch (fieldAttrs.order || 'ascending') {
          case 'descending':
            return order === 'ascending' ? 'descending' : 'ascending';
          case 'ascending':
            return order;
          default:
            throw new Error('invalid order: ' + fieldAttrs.order);
        }
      }
    });

    opts.namespace = namespace;
    attrs.fields = fields;
  };
}

/**
 * Custom type to unwrap certain unions.
 *
 * Unlike the other logical types below, it cannot be applied via `parse`'s
 * `logicalTypes` option since unions cannot be annotated.
 *
 */
function OptionalType(attrs, opts) {
  avro.types.LogicalType.call(this, attrs, opts, [avro.types.UnionType]);
  var type = this.getUnderlyingType().getTypes()[1];
  this._name = type.getName() || type.getName(true);
}
util.inherits(OptionalType, avro.types.LogicalType);

OptionalType.prototype._fromValue = function (val) {
  return val === null ? null : val[this._name];
};

OptionalType.prototype._toValue = function (any) {
  if (any === null) {
    return null;
  }
  var obj = {};
  obj[this._name] = any;
  return obj;
};

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
  this._size = this.getUnderlyingType().getSize();
}
util.inherits(AddressType, avro.types.LogicalType);

AddressType.prototype._fromValue = function (val) {
  if (val.length !== this._size) {
    throw new Error('invalid address');
  }
  return new Address(val);
};

AddressType.prototype._toValue = function (addr) { return addr.toBuffer(); };

/**
 * Hardware address.
 *
 */
function Address(buf) {
  this._buf = buf;
  this._str = undefined; // Lazily instantiated.
}

Address.prototype.isMulticast = function () { return !!(this._buf[0] & 1); };

Address.prototype.toBuffer = function () { return this._buf; };

Address.prototype.toString = function () {
  if (!this._str) {
    this._str = ADDON.stringifyAddress(this._buf);
  }
  return this._str;
};

Address.prototype.toJSON = Address.prototype.toString;

Address.prototype.inspect = Address.prototype.toString;

/**
 * Base class to augment all 802.11 frames.
 *
 */
function Dot11Frame() {}

Dot11Frame.prototype.getSrcAddr = function () {
  switch (this.toDs + 2 * this.fromDs) {
    case 0:
    case 1:
      return this.addr2;
    case 2:
      return this.addr3;
    case 3:
      return this.addr4;
  }
};

Dot11Frame.prototype.getDstAddr = function () {
  switch (this.toDs + 2 * this.fromDs) {
    case 0:
    case 2:
      return this.addr1;
    case 1:
      return this.addr3;
    case 3:
      // TODO: Check this.
      return this.addr3;
  }
};

Dot11Frame.prototype.getApAddr = function () {
  switch (this.toDs + 2 * this.fromDs) {
    case 0:
      return this.addr3;
    case 1:
      return this.addr1;
    case 2:
      return this.addr2;
    case 3:
      // Not clear which one to return, so return none.
  }
};


module.exports = {
  addon: ADDON, // For tests.
  loadProtocol: loadProtocol
};
