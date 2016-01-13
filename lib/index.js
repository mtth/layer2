/* jshint node: true */

  'use strict';

var utils = require('./utils'),
    avro = require('avsc'),
    events = require('events'),
    util = require('util');


var addon = utils.requireAddon();

var registry = {};
avro.parse('./pdus.avsc', {registry: registry});
var pduType = registry.Pdu;


/**
 * Sniffer, event emitter used to capture PDUs (i.e. ~frames/packets).
 *
 * Not meant to be instantiated directly though, only through the two factory
 * functions below (so as not to expose the CPP-defined `Wrapper`).
 *
 */
function Sniffer(wrapper, opts) {
  events.EventEmitter.call(this);
  this._buf = new Buffer(opts.bufferSize || 1024);
  this._wrapper = wrapper;
  this._destroyed = false;

  var self = this;
  (function sniff(err, n) {
    if (err) {
      self.emit('error', err);
      return;
    }
    try {
      var pos = 0;
      var buf = self._buf;
      while (n--) {
        var obj = pduType.decode(buf, pos);
        self.emit('pdu', obj.value);
        pos = obj.offset;
      }
      if (!self._destroyed) {
        setTimeout(function () { self._wrapper.getPdus(buf, sniff); }, 0);
      }
    } catch (err) {
      self.emit('error', err);
    }
  })();
}
util.inherits(Sniffer, events.EventEmitter);

Sniffer.prototype.destroy = function () { this._destroyed = true; };

/**
 * Factory method for live captures.
 *
 */
function createInterfaceSniffer(dev, opts) {
  opts = opts || {};
  return new Sniffer(new addon.Wrapper().fromInterface(dev, opts), opts);
}

/**
 * Factory method for replaying captures from PCAP files.
 *
 */
function createFileSniffer(path, opts) {
  opts = opts || {};
  return new Sniffer(new addon.Wrapper().fromFile(path, opts), opts);
}


module.exports = {
  createInterfaceSniffer: createInterfaceSniffer,
  createFileSniffer: createFileSniffer
};
