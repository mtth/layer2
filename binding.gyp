{
  'targets': [
    {
      'target_name': 'index',
      'sources': [
        'src/index.cpp',
        'src/codecs.cpp',
        'src/utils.cpp',
        'src/wrapper.cpp'
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
        '<!(node -e \'require("nan")\')'
      ]
    }
  ]
}
