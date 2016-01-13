/* jshint node: true */

// TODO: Use two rotating buffers in the Sniffer class (allowing triggering the
// next capture before all PDUs are decoded). This will improve latency (though
// not throughput).

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
function Sniffer(wrapper, bufferSize) {
  events.EventEmitter.call(this);
  this._buf = new Buffer(bufferSize || 65536);
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
 * For more information see:
 *
 * + http://www.tcpdump.org/manpages/pcap.3pcap.html
 * + http://libtins.github.io/docs/latest/da/d53/classTins_1_1SnifferConfiguration.html
 *
 */
function createInterfaceSniffer(dev, opts) {
  opts = opts || {};
  var wrapper = new addon.Wrapper().fromInterface(
    dev,
    opts.snaplen,
    opts.promisc,
    opts.rfmon,
    opts.timeout,
    opts.bufferSize,
    opts.filter
  );
  // We use the same size for both PCAP's buffer and ours. It is an
  // approximation though (we still need to handle overflows) because the
  // encodings are different in each, so data size will vary.
  return new Sniffer(wrapper, opts.bufferSize);
}

/**
 * Factory method for replaying captures from PCAP files.
 *
 */
function createFileSniffer(path, opts) {
  opts = opts || {};
  var wrapper = new addon.Wrapper().fromFile(path, opts.filter);
  return new Sniffer(wrapper, opts.bufferSize);
}


module.exports = {
  createInterfaceSniffer: createInterfaceSniffer,
  createFileSniffer: createFileSniffer
};
