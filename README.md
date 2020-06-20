# QT_httpserver
it's a simple httpserver on QT 

由于经常需要在ubuntu下进行软件开发设计，但是虚拟机又非常卡顿，所以就给电脑安装了ubuntu，win10的双系统。这个时候，出现了一个问题，我使用win10的时候，可以轻松把别人电脑上或者手机上的的一些文件通过QQ等工具，网址等传到电脑。但是ubuntu下缺少相关软件，于是我准备自己开发一个相关的软件。

由于既要考虑跨平台(linux、windows、安卓)，又要考虑使用的便捷性。使用我采用服务端使用QT，客户端使用web的方案。

QT本身是一个C/S架构的软件，所以也没有提供http服务器相关的类，但是http协议是一个基于TCP协议的应用层协议，QT提供了底层的TCP的一系列接口，所以可以在TCP的基础上自己写一个http server。
