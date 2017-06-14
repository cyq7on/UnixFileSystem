# Unix File System


   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;这是一个简单的UNIX文件系统的模拟,这个实例可以帮助您对UNIX文件系统的组织方式及实现机制上有一个较为清晰的认识.在本系统中,对于空闲盘块的组织采用UNIX System V的成组连接法,对于索引结点的组织采用增量式索引组织方式.

## 使用
	$ git clone git@github.com:Sun-yq/UnixFileSystem.git
	$ gcc main.c -o test
	$ ./test

## 概述
* 0# 作为系统引导扇区,该盘块记录空闲盘块号栈、系统可用索引节点数、系统文件总数等信息
* 1#-20# 是系统索引节点区
* 21#-30# 系统根目录区
* 31# 开始是系统的文件区


