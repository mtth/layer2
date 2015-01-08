{
  "targets": [
    {
      "target_name": "index",
      "sources": [
        "src/cpp/addon.cpp",
        "src/cpp/util.cpp",
        "src/cpp/pcap_wrapper.cpp"
      ],
      "link_settings": {
        "libraries": [
          "-lpcap"
        ]
      }
    }
  ]
}
