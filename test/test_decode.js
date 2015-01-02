/* jshint mocha: true, node: true */

(function () {
  'use strict';

  var assert = require('assert'),
      diff = require('deep-diff'),
      dot11 = require('../src/js');

  describe('Decoder', function () {

    describe('IEEE802_11_RADIO', function () {

      var helper = new Helper('IEEE802_11_RADIO');

      it('decodes radiotap frames', function () {

        helper.compare(
          '000020006708040054c6b82400000000220cdaa002000000400100003c142411aa',
          {
            'headerRevision': 0,
            'headerPad': 0,
            'headerLength': 32,
            'body': new Buffer('aa', 'hex')
          }
        );

      });

    });

    describe('IEEE802_11', function () {

      var helper = new Helper('IEEE802_11');

      // Uncomment this to be able to analyze the frames used for testing in
      // this section from Tcpdump or Wireshark.
      // after(function () { helper.save('dat/IEEE802_11.pcap'); });

      it('checks the fcs', function () {

        assert.throws(function () {
          helper.decode('88422c008438355f8e8a08863b3b39c708863b3b39c7e00001001e00002000000000aaaa03000000080045200034556040003a06854448155b1dc0a802250050dc5e1c104c0cb3fce7928010001f03c800000101080a4fb0196c36b68ca9a334bb90ca345b56');
        });

      });

      it('decodes beacon frames', function () {

        helper.compare(
          '80000000ffffffffffff5a238c2e1f0f5a238c2e1f0f4071100506c74501000064002104000b7866696e69747977696669010882848b9624b0486c0301010504000100002a01042f010432048c1298602d1abd181bffffff00000000000000000000000000000000000000003d1601081500000000000000000000000000000000000000dd090010180200001c0000dd180050f2020101840003a4000027a4000042435e0062322f003deb9577',
          {
            'version': 0,
            'type': 'mgmt',
            'subType': 'beacon',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'duration': 0,
            'fragN': 0,
            'seqN': 1812,
            'ra': 'ff:ff:ff:ff:ff:ff',
            'ta': '5a:23:8c:2e:1f:0f',
            'da': 'ff:ff:ff:ff:ff:ff',
            'sa': '5a:23:8c:2e:1f:0f',
            'bssid': '5a:23:8c:2e:1f:0f'
          }
        );

      });

      it('decodes rts frames', function () {

        helper.compare(
          'b4007c013ce072e6612bcc03fadc202a719fe3d6',
          {
            'version': 0,
            'type': 'ctrl',
            'subType': 'rts',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'duration': 380,
            'ra': '3c:e0:72:e6:61:2b',
            'ta': 'cc:03:fa:dc:20:2a'
          }
        );

      });

      it('decodes cts frames', function () {

        helper.compare(
          'c40072008438355f8e8a2c4ba360',
          {
            'version': 0,
            'type': 'ctrl',
            'subType': 'cts',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'duration': 114,
            'ra': '84:38:35:5f:8e:8a'
          }
        );

      });

      it('decodes data frames', function () {

        helper.compare(
          '08492c0088f7c7ce57bbfc0fe61dc60888f7c7ce57bad0c83c2a0020000000002de4be0d52e42c5e4ed1c2e7b4239581feb3d9c0e55efe267fc42ae986922a6aa77e09340f23e502548ac514d7cb978501568ae7da1d36e6965fad260dca52d048b0b44fd70b0087ffc3ce35139ce3e9d2c95ed0',
          {
            'version': 0,
            'type': 'data',
            'subType': 'data',
            'toDs': 1,
            'fromDs': 0,
            'retry': 1,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'fragN': 0,
            'seqN': 3213,
            'duration': 44,
            'ra': '88:f7:c7:ce:57:bb',
            'ta': 'fc:0f:e6:1d:c6:08',
            'da': '88:f7:c7:ce:57:ba',
            'sa': 'fc:0f:e6:1d:c6:08',
            'bssid': '88:f7:c7:ce:57:bb',
            'encryption': 'aes',
            'body': new Buffer('2de4be0d52e42c5e4ed1c2e7b4239581feb3d9c0e55efe267fc42ae986922a6aa77e09340f23e502548ac514d7cb978501568ae7da1d36e6965fad260dca52d048b0b44fd70b0087ffc3ce35139ce3e9', 'hex')
          }
        );

      });

      it('decodes qos frames', function () {

        helper.compare(
          'c8023a01001f5bd11f5788f7c7ce57bb88f7c7ce57bb100c0000068ef9e0',
          {
            'version': 0,
            'type': 'data',
            'subType': 'qos',
            'toDs': 0,
            'fromDs': 1,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'fragN': 0,
            'seqN': 193,
            'duration': 314,
            'ra': '00:1f:5b:d1:1f:57',
            'ta': '88:f7:c7:ce:57:bb',
            'da': '00:1f:5b:d1:1f:57',
            'sa': '88:f7:c7:ce:57:bb',
            'bssid': '88:f7:c7:ce:57:bb'
          }
        );

      });

      it('decodes ack frames', function () {

        helper.compare(
        'd40000008438355f8e8a295c78a9',
          {
            'version': 0,
            'type': 'ctrl',
            'subType': 'ack',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'duration': 0,
            'ra': '84:38:35:5f:8e:8a'
          }
        );

      });

      it('decodes block-ack frames', function () {

        helper.compare(
          '940000008438355f8e8a08863b3b39c70510a0a40100000000000000ed91e950',
          {
            'version': 0,
            'type': 'ctrl',
            'subType': 'block-ack',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'duration': 0,
            'ra': '84:38:35:5f:8e:8a',
            'ta': '08:86:3b:3b:39:c7'
          }
        );

      });

      it('decodes probe responses', function () {

        helper.compare(
          '50003a01002258957e368af7c7ce57bd8af7c7ce57bd002b928953137a03000064002104000b7866696e69747977696669010882848b9624b0486c0301012a01042f010432048c1298602d1abd181bffffff00000000000000000000000000000000000000003d1601001300000000000000000000000000000000000000dd090010180200001c0000dd180050f2020101840003a4000027a4000042435e0062322f00d9b873b7',
          {
            'version': 0,
            'type': 'mgmt',
            'subType': 'probe-res',
            'toDs': 0,
            'fromDs': 0,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'fragN': 0,
            'seqN': 688,
            'duration': 314,
            'ra': '00:22:58:95:7e:36',
            'ta': '8a:f7:c7:ce:57:bd',
            'da': '00:22:58:95:7e:36',
            'sa': '8a:f7:c7:ce:57:bd',
            'bssid': '8a:f7:c7:ce:57:bd'
            // 'body': new Buffer('928953137a03000064002104', 'hex')
          }
        );

      });

      it('decodes data frames (tkip, from ds)', function () {

        helper.compare(
          '0842000001005e7ffffa58238c2e1f0d78f7bef4e0664091a828bc60120000001bf80989e473411d0f1ace8d1a00ad52aaa4d88cc857de1297f7b6252f2bb9cbeb3eada18ed09065443369454b47ea701199a825299f811117767d8799f9de34dced537e8d0c7044e5e0881b7f9333b95b36938da2ab1120603a116e387e0e507f0ab9f567c83bd6bd43119c0b50b84cd6ec956468ddc95fa2865d2595d1a56fa0c69b8e754c9f5205e6f4cb79218259188286f584f82c4908787d08100830341d218fd22593997eab4187a015ba05972a',
          {
            'version': 0,
            'type': 'data',
            'subType': 'data',
            'toDs': 0,
            'fromDs': 1,
            'retry': 0,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'fragN': 0,
            'seqN': 2324,
            'duration': 0,
            'encryption': 'tkip',
            'ra': '01:00:5e:7f:ff:fa',
            'ta': '58:23:8c:2e:1f:0d',
            'da': '01:00:5e:7f:ff:fa',
            'sa': '78:f7:be:f4:e0:66',
            'bssid': '58:23:8c:2e:1f:0d',
            'body': new Buffer('1bf80989e473411d0f1ace8d1a00ad52aaa4d88cc857de1297f7b6252f2bb9cbeb3eada18ed09065443369454b47ea701199a825299f811117767d8799f9de34dced537e8d0c7044e5e0881b7f9333b95b36938da2ab1120603a116e387e0e507f0ab9f567c83bd6bd43119c0b50b84cd6ec956468ddc95fa2865d2595d1a56fa0c69b8e754c9f5205e6f4cb79218259188286f584f82c4908787d08100830341d218fd22593997eab4187a015', 'hex')
          }
        );

      });

      it('decodes qos-data frames (aes, to ds)', function () {

        helper.compare(
          '88493a0100259c58c39e784b87dfc32801005e7ffffac00a0600090b002000000000d6272209e6095f853df52eea857514e3cb71afab524140c858497d54365cae9b5ebb2ccb449b22df2e56487d5992ebb61e056100',
          {
            'version': 0,
            'type': 'data',
            'subType': 'qos-data',
            'toDs': 1,
            'fromDs': 0,
            'retry': 1,
            'powerMgmt': 0,
            'moreFrag': 0,
            'moreData': 0,
            'fragN': 0,
            'seqN': 172,
            'duration': 314,
            'encryption': 'aes',
            'ra': '00:25:9c:58:c3:9e',
            'ta': '78:4b:87:df:c3:28',
            'da': '01:00:5e:7f:ff:fa',
            'sa': '78:4b:87:df:c3:28',
            'bssid': '00:25:9c:58:c3:9e',
            'body': new Buffer('d6272209e6095f853df52eea857514e3cb71afab524140c858497d54365cae9b5ebb2ccb449b22df2e56487d5992ebb6', 'hex')
          }
        );

      });

    });

    describe('EN10MB', function () {

      var helper = new Helper('EN10MB');

      it('decodes ip frames', function () {

        helper.compare(
          '8438355f8e8a08863b3b39c70800aaaa6e4d2ad1',
          {
            'type': 'ipv4',
            'da': '84:38:35:5f:8e:8a',
            'sa': '08:86:3b:3b:39:c7',
            'body': new Buffer('aaaa', 'hex')
          }
        );

      });

    });

  });

  /**
   * Helper to view output sample frames to a capture file.
   * This lets us easily view them from Wireshark or Tcpdump (convenient when
   * changing the format of the decoder, especially when adding a field).
   *
   */
  function Helper(linkType) {
    this._buffers = [];
    this._linkType = linkType;
    this.decode = require('../src/js/decode/' + linkType);
  }

  Helper.prototype.compare = function (actualString, expectedObject) {
    var buf = new Buffer(actualString, 'hex');
    this._buffers.push(buf); // Save the buffer.
    var actualObject = this.decode(buf);
    assert.deepEqual(
      actualObject,
      expectedObject,
      JSON.stringify(diff(actualObject, expectedObject), null, 2)
    );
  };

  Helper.prototype.save = function (path) {
    var save = new dot11.capture.Save(path, {linkType: this._linkType});
    var i;
    for (i = 0; i < this._buffers.length; i++) {
      save.write(this._buffers[i]);
    }
    save.end();
  };

})();

// vim: set nowrap
