{
  "targets": [
    {
      "target_name": "index",
      "sources": [
        "src/cpp/addon.cpp",
        "src/cpp/frame.cpp",
        "src/cpp/pdu/ethernet_ii.cpp",
        "src/cpp/pdu/radiotap.cpp",
        "src/cpp/pcap_wrapper.cpp",
        "src/cpp/util.cpp"
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
