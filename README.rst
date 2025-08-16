AVIF Susie plugin
=================

This plugin allows reading of AVIF images in Susie plugin-compatible
image viewers.

Downloads
---------

| The following binaries are available:
* `Win32 (Intel 32-bit
  GCC) <https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.intel32.gcc.7z>`__
* `Win32 (Intel 64-bit
  GCC) <https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.intel64.gcc.7z>`__
* `Win32 (Intel 32-bit
  Clang) <https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.intel32.clang.7z>`__
* `Win32 (Intel 64-bit
  Clang) <https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.intel64.clang.7z>`__
* `Win32 (ARM 32-bit
  Clang) <https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.arm32.clang.7z>`__
* `Win32 (ARM 64-bit
  Clang) <https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.arm64.clang.7z>`__

Preparing the environment
-------------------------

Install the ``act`` tool: https://nektosact.com/installation/index.html

Building
--------

::

   $ git clone https://github.com/uyjulian/ifpng.git
   $ cd ifpng
   $ act run --artifact-server-path $PWD/build-artifacts

Output artifacts will be in ``build-artifacts`` folder.

How to use
----------

Susie plugins are compatible with many programs, including these:

- `A to B
  converter <http://www.asahi-net.or.jp/~KH4S-SMZ/spi/abc/index.html>`__
- `DYNA <https://hp.vector.co.jp/authors/VA004117/dyna.html>`__
- `Linar <http://hp.vector.co.jp/authors/VA015839/>`__
- `Susie <http://www.digitalpad.co.jp/~takechin/betasue.html#susie32>`__
- `picture
  effecter <http://www.asahi-net.or.jp/~DS8H-WTNB/software/index.html>`__
- `stereophotomaker <http://stereo.jpn.org/eng/stphmkr/>`__
- `vix <http://www.forest.impress.co.jp/library/software/vix/>`__

License
-------

This project is licensed under the MIT license. Please read the
``LICENSE`` file for more information.
