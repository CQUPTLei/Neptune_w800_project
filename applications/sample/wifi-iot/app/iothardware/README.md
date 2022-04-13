## 如何编译

使用本方法可以编译此目录下的样例程序

1. 进入openharmony源码的`applications\sample\wifi-iot\app\iothardware`目录下；

   1. 修改`applications\sample\wifi-iot\app\iothardware\BUILD.gn`文件，决定需要编译哪个`.c`文件；
   2. 再修改`neptune-harmony-os1.1-iot/device/winnermicro/neptune/sdk_liteos/Makefile`文件，添加`$(OHOS_LIBS_DIR)/libgpio_example.a`
   如图
   ![输入图片说明](https://images.gitee.com/uploads/images/2021/0515/135237_b3fd4948_8343063.png "屏幕截图 2021-05-15 135037.png")
   3. 在openharmony源码顶层目录执行：`hb build`

