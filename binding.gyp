# TODO: Let Avro have its own binding.gyp file.
{
  'targets': [
    {
      'target_name': 'index',
      'sources': [
        'src/index.cpp',
        'src/codecs.cpp',
        'src/utils.cpp',
        'src/wrapper.cpp'
        # 'src/dispatch.cpp',
      ],
      'link_settings': {
        'libraries': [
          '-lavrocpp',
          '-ltins'
        ]
      },
      'cflags!': ['-fno-exceptions'],
      'cflags_cc!': ['-fno-exceptions', '-fno-rtti'],
      'conditions': [
        [
          'OS=="mac"', {
            'include_dirs': [
              '/usr/local/include' # TODO: Remove this.
            ],
            'libraries': [
              # '-L/usr/local/lib',
              '-L<(module_root_dir)/etc/deps/avro/lang/c++/build',
              '-L<(module_root_dir)/etc/deps/libtins/build/lib'
            ],
            'xcode_settings': {
              'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
              'GCC_ENABLE_CPP_RTTI': 'YES',
              'MACOSX_DEPLOYMENT_TARGET': '10.7',
              'OTHER_CPLUSPLUSFLAGS': [
                '-stdlib=libc++'
              ]
            }
          }
        ]
      ],
      'include_dirs' : [
        '<(module_root_dir)/etc/deps',
        '<(module_root_dir)/etc/deps/libtins/include',
        '<!(node -e \'require("nan")\')'
      ]
    }
  ]
}
