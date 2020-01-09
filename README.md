# Npfs

A 9p2000 server framework in C++ for microcontrollers, inspired by the spfs library.

It is intended to create a virtual file server, which can present C++ data (memory, variables and classes) as files 
and directories in a virtual file system, which is network mountable (even via loopback interface) or 
via other stream oriented channels using the v9fs file system (9fs in Linux). 
It is not limited to the 9fs in Linux, as the direct access to the resources via network or other means of 
packet or stream channels using i.e. the spfs library is possible as well and has mostly a higher throughput 
performance than the Linux VFS access.

It was already successfully used for local and network presentation (HaaS: hardware as a service) of:
* debugging of microcontrollers (file I/O based access to memory pages of microcontrollers) 
* remote control of hardware resources (HaaS) of microcontrollers: file I/O based access to memory mapped 
  peripheral resources 
* remote controlled rolling gates of fire trucks garages
* character/graphic LCD displays as windowed and layered network display server for multiple 
  simultaneous operating clients
* complicated hardware resources of realtime controller frontends for Laser scanning and cutting systems
* complicated hardware resources of microcontroller frontends of realtime highspeed data sampling systems

more to come...

## Credits

The library is heavily inspired by the spfs library, which was available at http://sourceforge.net/projects/npfs/.

## License

Npfs is published under a double license at your option: zlib license or GPLv3 with linking exception.

## TODO
* English translation of some German comments
* examples of usage
* put interfaces and utilities to several external modules to avoid repeated copies of them
* check of writable parent directory for `Orclose`
* remove operation for the case of multiple open references, probably catching this case via 
  the base class for `RemovableResource`, `RemovableDirectory`
* nice to have: Auth, but it might be insecure, because fid/afid spoofing might get around this,
  would prefer to encapsulate the TCP stream into TLS
* rework of file and directory resources in a way that static logical structures may be compiled 
  as const/constexpr into r/o memory (Flash)

 