/* jshint node: true */

// TODO: Use two rotating buffers in the Sniffer class (allowing triggering the
// next capture before all PDUs are decoded). This will improve latency (though
// not throughput).

'use strict';

var utils = require('./utils'),
    events = require('events'),
    util = require('util');


// Convenience function to load the correct build of the addon (release or
// debug) based on availability and environment variables.
var addon = utils.requireAddon();

/**
 * Sniffer, event emitter used to capture PDUs (i.e. ~frames/packets).
 *
 * Not meant to be instantiated directly though, only through the two factory
 * functions below (so as not to expose the CPP-defined `Wrapper`).
 *
 */
function Sniffer(wrapper, bufferSize) {
  events.EventEmitter.call(this);

  bufferSize = bufferSize || 65536;
  this._bufs = [new Buffer(bufferSize), new Buffer(bufferSize)];
  this._bufIndex = 0;
  this._wrapper = wrapper;
  this._destroyed = false;

  var self = this;
  utils.loadProtocol(function (err, protocol) {
    if (err) {
      self.emit('error', err);
      return;
    }

    var type = protocol.getType('Pdu');
    (function sniff() {
      var buf = self._bufs[self._bufIndex];
      self._bufIndex = 1 - self._bufIndex;
      self._wrapper.getPdus(buf, function (err, n) {
        if (err) {
          self.emit('error', err);
          return;
        }

        self.emit('batch', n);
        if (!self._destroyed) {
          sniff();
        }

        try {
          var pos = 0;
          while (n--) {
            var obj = type.decode(buf, pos);
            self.emit('pdu', obj.value.pdu);
            pos = obj.offset;
          }
        } catch (err) {
          self.emit('error', err);
          return;
        }

        if (self._destroyed) {
          self.emit('end');
        }
      });
    })();
  });
}
util.inherits(Sniffer, events.EventEmitter);

// A few more PDUs might be emitted after this is called.
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
  var exhausted = false;
  return new Sniffer(wrapper, opts.bufferSize)
    .on('batch', function (n) {
      // We must do this in two passes because libtin's `FileSniffer` will
      // sometimes return an empty batch even though the file isn't exhausted
      // (for example at the very beginning).
      if (exhausted) {
        this.destroy();
      }
      exhausted = !n;
    });
}


module.exports = {
  createInterfaceSniffer: createInterfaceSniffer,
  createFileSniffer: createFileSniffer
};
