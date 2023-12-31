> 当前版本：0.31

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
    "defines": [
                "UNICODE",
            ],
```
## Visual Studio 配置环境

将include文件夹放入您的项目文件夹后，在“项目——属性——C/C++——命令行”中添加`/utf-8`，并在高级文件保存中设置文件保存格式为`Unicode`(UTF-8 无签名)，或把所有库文件改成GBK格式。

## xmake 项目配置

将include文件夹放入项目文件夹，打开`xmake.lua`，在`add_files`下面添加该语句：

```lua
add_headerfiles("/src/include/*.hpp")
```
链接库
```lua
add_syslinks("user32","gdi32","d2d1","dwrite","ole32","windowscodecs","Winmm")
```
定义字符串类型（必要的）
```lua
add_defines("UNICODE")
```
设置文件编译类型
```lua
set_encodings("utf-8")
```
这样头文件就可以正常包含在源文件里了。

若尚未创建xmake项目，可以使用该命令快速创建：

```bash
xmake create <project_name>
```

## 更新日志
### 0.31
**新增：**
* Text支持设置字符颜色效果
* Text新增DrawEx，支持多色绘制
* Text新增SetDrawingEffect，自定义绘制样式
* 所有控件新增统一大小调整函数SetShowSize、SetShowPosition、SetShowRect以及获取这些信息的函数，支持Vector::Vec2、D2D1_POINT_2F、float等类型
* 允许设置窗口点击消息的激活次数（当检测区域重叠时，默认仅激活一次，可设置为都激活，之前是都激活）
* 新增复选框控件
* 新增拖动控件，可以通过鼠标拖动，可设置绘制的窗口元素，设置限制x轴或y轴滑动


**修改：**
* 编辑框仅数字模式有了更加严格的限制（无法在第一个位置以外输入负号，且只允许存在一个小数点）

**bug修复：**
* ButtonUI大小设置异常
* d2dText存在内存泄漏
* d2dEdit控件在调整位置时光标没有随之移动

### 0.30
**新增：**
* 全新GameTools组件，包含碰撞体以及触发工具

**修改：**
* 现在可以单独设置场景ui显示位置与宽度是以比例还是固定像素

**bug修复：**
* Text不是居中绘制
* 设置字体大小或样式后导致对齐样式丢失
* 场景的ui宽度和位置可以分别设置为固定或者比例

### 0.26
**新增：**
* 现在所有窗口元素都支持进行旋转操作
* 添加场景对象无序集合，拥有更快的添加于删除对象的速度，但是无法指定集合内部的渲染顺序

**修改：**
* MianWind.hpp补全了其中所有函数的注释
* WindElement.hpp补齐了其中所有函数的注释

### 0.25
**修改：**
* RoomObject的场景元素的绘制，全部改成了中心点坐标
* 现在能通过修改椭圆的显示大小，显示左上角坐标轴，显示矩形来调整参数

**bug修复：**
* 修复函数控件的字符坐标轴字符与线没对齐的bug
* 修复矩形元素旋转后移动导致坐标轴翻转的问题

### 0.24

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
