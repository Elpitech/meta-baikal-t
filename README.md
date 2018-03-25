# OpenEmbedded/Yocto layer for the Baikal-T(1) SoC

## Dependencies

- [openembedded-core](https://github.com/openembedded/openembedded-core)
  layer, with a matching branch (i.e. master of oe-core and master of
  meta-sourcery).
- [bitbake](https://github.com/openembedded/bitbake), with a matching branch.

## Usage & Instructions

- Add meta-baikal-t layer to your yocto project `BBLAYERS` in `conf/bblayers.conf`.

- Make sure you got internet connection ready so to fetch the kernel and
  bootloader sources

- Add your Baikal-T based hardware settings to a separate machine conf-file under
  `conf/machine/<machine name>.conf`

### Optional Functionality

- There is an interface, which allows to create a ready-to-use U-boot environment blob.
  It can be easily configured by setting corresponding variables, which are described
  in details in `classes/uboot-env.bbclass` file.

- U-boot and kernel deployed symbolic links are changed so to have them looking
  unified and to clean up the images deploy directory.

## Behavior

Tha layer provides two main recipes, which describes linux and u-boot build
procedures for sources fetched from T-platforms repositories and directes.
Primarily U-boot 2014.10 and Linux kernel 4.4, 4.9 and 4.14 recipes are 
added, but it is easy to extend the versions just by copying the corresdponing
files.
Note also, that both linux-baikal and u-boot-baikal mangle the files and symbolic
links deployed to the images directory, so to have them better ordered.

## Contributing

To contribute to this layer, please fork and submit pull requests to the
github [repository](https://github.com/T-Platforms/linux-kernel), or open
issues for any bugs you find, or feature requests you have.

## Maintainer

This layer is maintained by [T-platforms](https://www.t-platforms.ru/).
Please direct all support requests for this layer to the GitHub repository
issues interface. Optionally you can try to communicate with primary
developer: Serge Semin <fancer.lancer@gmail.com>

## To Do List

See [TODO.md](TODO.md).
