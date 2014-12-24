{
  "targets": [
    {
      "target_name": "dot11",
      "sources": [
        "src/dot11.cpp",
        "src/pcap.cpp"
      ],
      "link_settings": {
        "libraries": [
          "-lpcap"
        ]
      }
    }
  ]
}
