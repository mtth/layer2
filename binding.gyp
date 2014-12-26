{
  "targets": [
    {
      "target_name": "pcap",
      "sources": [
        "src/pcap.cpp",
        "src/pcap_wrapper.cpp"
      ],
      "link_settings": {
        "libraries": [
          "-lpcap"
        ]
      }
    }
  ]
}
