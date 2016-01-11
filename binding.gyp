# TODO: Let Avro have its own binding.gyp file.
{
  'targets': [
    {
      'target_name': 'index',
      'sources': [
        'src/index.cpp',
        # 'src/dispatch.cpp',
        'src/utils.cpp'
      ],
      'link_settings': {
        'libraries': [
          '-lavrocpp',
          '-ltins'
        ]
      },
      'cflags!': ['-fno-exceptions'],
      'cflags_cc!': ['-fno-exceptions'],
      'conditions': [
        [
          'OS=="mac"', {
            'include_dirs': ['/usr/local/include'],
            'libraries': ['-L/usr/local/lib', '-L<(module_root_dir)/etc/deps/avro/lang/c++/build'],
            'xcode_settings': {
              'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
              'MACOSX_DEPLOYMENT_TARGET': '10.7',
              'OTHER_CPLUSPLUSFLAGS': [
                '-stdlib=libc++'
              ]
            }
          }
        ]
      ],
      'include_dirs' : [
        '<!(node -e \'require("nan")\')'
      ]
    }
  ]
}
