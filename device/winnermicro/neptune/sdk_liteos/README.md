# vendor_w800

编译说明：
 cd vendor/w800
 执行
 make 编译出w800.elf, w800.bin 镜像。

libs 更新：
 w800的库在lib/w800 下面。
执行make 命令默认不会编译这些库。如果库有更新。可以先执行make lib
编译生成库到bin/build/w800 下面，同时会拷贝到lib/w800 下面。
