{
  "targets": [
    {
      "target_name": "index",
      "sources": [
        "src/addon.cpp",
        "src/frame.cpp",
        "src/pdu/ethernet_ii.cpp",
        "src/pdu/radiotap.cpp",
        "src/pcap_wrapper.cpp",
        "src/util.cpp"
      ],
      "link_settings": {
        "libraries": [
          "-ltins",
          "-lpcap"
        ]
      },
      'cflags!': ['-fno-exceptions'],
      'cflags_cc!': ['-fno-exceptions'],
      'conditions': [
        [
          'OS=="mac"',
          {'xcode_settings': {'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'}}
        ]
      ],
      "include_dirs" : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
