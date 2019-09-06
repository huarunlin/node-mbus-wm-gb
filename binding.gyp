{
    "targets": [
        {
            "target_name": "<(module_name)",
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
        {
            "target_name": "action_after_build",
            "type": "none",
            "dependencies": [ "<(module_name)" ],
            "copies": [{
                    "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
                    "destination": "<(module_path)"
            }]
        }
    ],
    "defines": [
        "NAPI_VERSION=<(napi_build_version)"
    ],
    "include_dirs": []
}