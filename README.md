# Unix File System


   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;文件系统是OS中一个非常重要且庞大的模块,实现一个完整的文件系统并非是几千行代码可以完成的,这是一个简单的对UNIX文件系统的模拟,通过该实例可以帮助您对UNIX文件系统的组织方式及实现机制有一个较为清晰的认识.在本系统中,对于空闲盘块的组织采用UNIX System V的成组连接法,对于索引结点的组织采用增量式索引组织方式. 对于磁盘的模拟是通过建立一个写满空格的20971520KB的$DISK文件实现的,通过fseek函数来模拟磁盘磁头的移动.

## 使用
	$ git clone git@github.com:Sun-yq/UnixFileSystem.git
	$ gcc main.c -o test
	$ ./test

## 源文件描述
1. <code>data_structure.h</code>           定义文件系统的数据结构,包含系统的所有全局变量
2. <code>format.h</code>                   定义文件系统执行格式化操作时所需要的函数
3. <code>func.h</code>                     定义本文件系统的全部函数
4. <code>userInterface.h</code>            控制台界面
5. <code>main.c</code>                     主函数

## 概述
* 0# 系统引导扇区,该盘块记录空闲盘块号栈、系统可用索引节点数、系统文件总数等信息
* 1#-20# 系统索引节点区
* 21#-30# 系统根目录区
* 31#-20480# 系统文件区
* 本系统默认磁盘块大小为1KB

## 注意
* 退出程序时如果您希望您刚刚在这个模拟的文件系统中建立的文件得以保存的话,请您通过先执行‘关机’后再关闭界面
* 如果您发现本文件系统出现了一些错误您可以通过‘格式化’操作来还原整个文件系统

## License
MIT
