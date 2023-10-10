# OneD2D
封装了d2d窗口创建相关函数，快速创建d2d窗口

## 使用教程
首先，将项目克隆至本地：
```
git clone https://github.com/JCU-AoC/OneKeyD2D.git
```
请确保你已正确安装[git](https://git-scm.com/)。若未安装，请从官网下载安装包安装即可。

##visual studio code配置环境
将下载好的include文件放入您的项目文件夹后
可能需要配置调试环境（这里以g++为例）

打开tasks.json文件,找到"args"（表示编译时的后缀）
```
    "args": [
        "-fdiagnostics-color=always",//以彩色字符输出警告（非必要）
        "-g",//生成调试信息
        "${file}",//vs自动添加文件
        "-o",//输出
        "${fileDirname}\\${fileBasenameNoExtension}.exe",//当前项目地址

        //以下是库必要的链接库
        "-ld2d1",
        "-ldwrite",
        "-lole32",
        "-lwindowscodecs",
        //结束
        "-finput-charset=UTF-8",//指定输入的文件格式
    ],
```
