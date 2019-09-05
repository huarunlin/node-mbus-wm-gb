{
    "targets": [
        {
            "target_name": "mbus_wm_gb",
            "sources": [
                "src/CMbus.cpp",
                "src/main.cpp"
            ],
            'include_dirs': [
                'mbus',
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            'libraries': [],
            'dependencies': [
                'libmbus',
                "<!(node -p \"require('node-addon-api').gyp\")"
            ],
            'defines': [
                'NAPI_DISABLE_CPP_EXCEPTIONS'
            ]
        },
        {
			'target_name': 'libmbus',
            'type': 'static_library',
			'cflags': [
                '-w'
			],
			'include_dirs': [
			],
			'sources': [
				'mbus/mbus-protocol.c',
				'mbus/mbus-serial.c',
				'mbus/mbus.c'
			],
            'conditions': [
                ['OS=="mac"', {
                    'xcode_settings': {
                        'OTHER_CFLAGS': [
                            '-w'
                        ],
                    }
                }],
                ['OS=="win"', {
			        'sources': [
                        'mbus/win/termiWin.c'
                    ]
                }]
            ]
		},
    ],
    "defines": [],
    "include_dirs": []
}