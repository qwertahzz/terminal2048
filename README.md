# terminal2048

C++的一个跑在**Linux**终端上的2048。

开发环境： Ubuntu 20.04，依赖ncurses库

参考教程：https://www.nowcoder.com/project/index/8

在教程基础框架上有所修改：

- 可以分开设置长和宽（长宽可以不等）
- 增添了有颜色的版本

# 游戏界面
![image](https://user-images.githubusercontent.com/72243450/233133010-be0157e2-34c7-4f84-802d-b004d1add627.png)
![}5@SXI(QN%WG~RE$VNJBL3H](https://user-images.githubusercontent.com/72243450/233131979-d91263ad-5b26-47f4-aefd-9fd51b63b385.png)


# 使用指北
1. 安装ncurses库
```
  sudo apt-get install ncurses
```
2. 编译cpp文件
```
  g++ 2048.cpp -o 2048 -l ncurses; ./2048
  或
  g++ 2048_color.cpp -o 2048_color -l ncurses; ./2048_color
```
3. 运行可执行文件
```
  ./2048
  或
  ./2048_color
```
