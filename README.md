> 当前版本：0.24

# OneKeyD2D

封装了Direct 2D窗口创建相关函数，用于快速创建Direct 2D窗口。  
是纯头文件的库，方便学习使用  
教程[OneKeyD2D开发文档](https://thisminibox.github.io/)

## 下载库

将项目克隆至本地：

```bash
git clone https://github.com/JCU-AoC/OneKeyD2D.git
```

请确保你已正确安装[git](https://git-scm.com/)。若未安装，请从官网下载安装包安装即可。

或者，你也可以直接下载zip包，但需要解压。

## Visual Studio Code 配置环境

将include文件夹放入您的项目文件夹后，可能需要配置调试环境（这里以g++为例）。

打开`tasks.json`文件,找到`args`（表示编译时的后缀）:

```json
    "args": [
        "-fdiagnostics-color=always",    //以彩色字符输出警告（非必要）
        "-g",    //生成调试信息
        "${file}",    //vs自动添加文件
        "-o",    //输出
        "${fileDirname}\\${fileBasenameNoExtension}.exe",    //当前项目地址

        //以下是库必要的链接库
        "-ld2d1",
        "-ldwrite",
        "-lole32",
        "-lwindowscodecs",
        "-lWinmm",
        //结束
        "-finput-charset=UTF-8",    //指定输入的文件格式
    ],
```
## Visual Studio 配置环境

将include文件夹放入您的项目文件夹后，在“项目——属性——C/C++——命令行”中添加`/utf-8`，并在高级文件保存中设置文件保存格式为`Unicode`(UTF-8 无签名)，或把所有库文件改成GBK格式。

## xmake 项目配置

将include文件夹放入项目文件夹，打开`xmake.lua`，在`add_files`下面添加该语句：

```lua
add_headerfiles("/src/include/*.hpp")
```

这样头文件就可以正常包含在源文件里了。

若尚未创建xmake项目，可以使用该命令快速创建：

```bash
xmake create <project_name>
```

## 更新日志
### 0.23

**新增：**
* RoomObj::RoomObjectSet
    * 将允许多个元素作为一个集合渲染
    * 保证正确的渲染顺序，避免经常更新的场景对象覆盖了ui的渲染，并且管理渲染层次更加方便
* d2dGeometry允许设置填充色
**修改：**
* Room::AddObject
    * 现在此函数后面允许加入整数参数来控制对象的渲染队列位置
    * 位置参数允许为负数表示倒数第几个，如果超出大小会被忽略
    * 原有的代码任然有效
**bug修复：**
* 修复了GDI与D2D窗口的简单绘制函数绘制效果不一致的问题
* 修复d2dGeometry的填充无效的问题
* 修复了d2dGeometry的渲染位置设置无效的问题
### 0.23

**新增：**
* RoomObj::Text
    * 添加场景文本
* d2dEdit（编辑框控件）
    * 添加点击选择光标
    * 添加限制输入数字
    * 添加限制最大长度

**bug修复：**
* SimpleSound
    * 修复当环境字节码为窄字符时出现内存错误
* 消除了右值引用错误使用警告

### 0.22

**新增：**
* d2dClickDetection（窗口的点击检测元素）
    * 新增椭圆功能（刷新效率几乎不变）
* d2dEdit（编辑框控件）
    * 完全进行了重构
    * 选中状态与默认状态背景颜色设置
    * 光标颜色设置
    * 字体颜色设置
    * 移除回调功能

**bug修复：**
* 代码优化


### 0.21

**新增：**
* 添加文件OneKeyD2D.hpp
    * 用作快速引用组件
* RoomManage添加用户数据函数
* Room可以直接调用GetRoomManage获取管理器实现内部场景跳转，可以获取当前窗口
* 添加类Sound
    * 可以进行更加精细的音频播放操作，包括暂停跳转音量等功能，可读取mp3
* RoomObjectUI新增相当UI设置控件位置
    * 实现控件自动跟随窗口移动，避免重载size函数
    * 旧的代码依旧适用 
* RoomObject新增Button控件

**bug修复：**

* 修复了OneKeyD2D在多文件编程中会出现重定义的错误

### 0.2

**新增：**
* 添加文件RoomManage
    * 包含`Room`
    * 包含`RoomManage`
    * 包含`RoomObject`
    * 包含`Camera2D`
* 添加命名空间`RoomObject`
    * 包含`RoomUI`纯虚类
    * 包含`RoomScence`纯虚类
    * 包含`ImageUI`
    * 包含`Image`
    * 包含`Animation`
    * 包含`AnimationUI`
    * 包含`TextUI`
    * 包含`TileMap`

**bug修复：**

* 修复了Edit控件反复绑定到窗口会导致内存泄漏的bug

### 0.12

**新增：**

* 添加命名空间`Vector`
    * 包含`Vector2`模板类和`Vector3`模板类

* 添加类`SimpleSound`
    * 用于播放wav音频文件

* 添加元素`d2dElliptic`
    * 用于绘制椭圆

* 添加元素功能`d2dRectangle`
    * 新增圆角绘制与填充绘制

* 添加控件`d2dPictureAnimation`
    * 用于实现单图片切割关键帧播放

**bug修复：**

* 修复了鼠标移动消息与鼠标滚轮消息无法正常截取的bug
