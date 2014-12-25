{
  "targets": [
    {
      "target_name": "pcap",
      "sources": [
        "src/pcap.cpp",
        "src/pcap_reader.cpp",
        "src/pcap_writer.cpp"
      ],
      "link_settings": {
        "libraries": [
          "-lpcap"
        ]
      }
    }
  ]
}
