# Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
#
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file in the root of the source
# tree. An additional intellectual property rights grant can be found
# in the file PATENTS.  All contributing project authors may
# be found in the AUTHORS file in the root of the source tree.

{
  'includes': [
    '../../../../build/common.gypi',
  ],
  'targets': [
    {
      'target_name': 'webrtc_h264',
      'type': 'static_library',
      'dependencies': [
        '<(webrtc_root)/common_video/common_video.gyp:common_video',
        '<(webrtc_root)/modules/video_coding/utility/video_coding_utility.gyp:video_coding_utility',
        '<(webrtc_root)/system_wrappers/source/system_wrappers.gyp:system_wrappers',
      ],
      'include_dirs': [
        '/Users/kxie/openh264-master/codec/api/svc',
        '/Users/kxie/openh264-master/codec/common',
        '/Users/kxie/openh264-master/codec/encoder/core/inc',
        '/Users/kxie/openh264-master/codec/encoder/plus/inc',
        '/Users/kxie/openh264-master/codec/processing/interface',        
        'include',
        '<(webrtc_root)/common_video/interface',
        '<(webrtc_root)/modules/video_coding/codecs/interface',
        '<(webrtc_root)/modules/interface',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          'include',
          '<(webrtc_root)/common_video/interface',
          '<(webrtc_root)/modules/video_coding/codecs/interface',
        ],
      },
      'link_settings': {
        'libraries': [
          '-L/Users/kxie/openh264-master/',
          '-lwels',
          '-lencoder',
          '-ldecoder',
          '-lprocessing',
          '-lh264common',
          '-m32',
          '-read_only_relocs suppress',
        ],
      },
      'sources': [
        'h264_impl.h',
        'h264_impl.cc',
      ],
    },
  ], # targets
}
