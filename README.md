# AVIF Susie plugin

This plugin allows reading of AVIF images in Susie plugin-compatible image viewers.

## Downloads

The following binaries are available:  
* [Win32 (Intel 32-bit GCC)](https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.intel32.gcc.7z)  
* [Win32 (Intel 64-bit GCC)](https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.intel64.gcc.7z)  
* [Win32 (Intel 32-bit Clang)](https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.intel32.clang.7z)  
* [Win32 (Intel 64-bit Clang)](https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.intel64.clang.7z)  
* [Win32 (ARM 32-bit Clang)](https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.arm32.clang.7z)  
* [Win32 (ARM 64-bit Clang)](https://github.com/uyjulian/ifavif/releases/latest/download/ifavif.arm64.clang.7z)  

## Preparing the environment

* Ubuntu
```
$ sudo apt install mingw-w64 meson nasm
```

## Building

After cloning submodules and preparing the environment, a simple invocation of `make` will generate `ifavif.spi`.  

## How to use

Susie plugins are compatible with many programs, including these:

- [A to B converter](http://www.asahi-net.or.jp/~KH4S-SMZ/spi/abc/index.html)  
- [DYNA](https://hp.vector.co.jp/authors/VA004117/dyna.html)  
- [Linar](http://hp.vector.co.jp/authors/VA015839/)  
- [Susie](http://www.digitalpad.co.jp/~takechin/betasue.html#susie32)  
- [picture effecter](http://www.asahi-net.or.jp/~DS8H-WTNB/software/index.html)  
- [stereophotomaker](http://stereo.jpn.org/eng/stphmkr/)  
- [vix](http://www.forest.impress.co.jp/library/software/vix/)  

## License

This project is licensed under the MIT license. Please read the `LICENSE` file for more information.
