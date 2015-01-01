{
  "targets": [
    {
      "target_name": "pcap",
      "sources": [
        "src/cpp/pcap.cpp",
        "src/cpp/pcap_util.cpp",
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
