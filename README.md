# Musnake

Just a game to complete my homework.

---

// 唉，本来想转转英语，结果还是放弃了。

### 简介

这个算是游戏的东西就是我们的课程设计。老师要求做一个贪吃蛇这种的小游戏，但是啊，就一个贪吃蛇未免也太无趣了吧，于是联系上我之前玩过的一款名为节奏地牢的游戏，我想把这只贪吃蛇做成节奏型游戏。

大概玩法是这样的：

我们的小蛇会随着玩家的上下左右指令而在一个类似地牢的空间里蠕动，蠕动过程中要避开墙壁和自己的身体，并尽力吃食物。小蛇初始5滴血，最高10滴血。如果撞到了墙壁或自己的身体，就扣3滴血，如果吃到了食物，就加1滴血，并获得100分数。血量扣到0点或更低，就死亡；血量出现溢出，就进入FEVER，双倍得分。游戏中会有背景音乐，如果按着节奏移动的话，会获得10分；如果在连续的BEAT都能踩点移动，则计算连击，连击数除以10并舍弃小数部分后的数将加成到当步得分中；如果MISS了，就会自动向蛇头前方前进，但是不加分，同时也视为断连；如果向蛇头相反的方向移动，那么强制向前移动且断连；如果超速行驶，在BEAT判定内的移动会计入得分，但多余的移动将会判为断连并影响TP。

嗯嗯，就这样吧，虽然有些抽象，但值得一搞。

虽然我只跟着学校学了半年C语言，不过，有志者事竟成嘛。

### 环境准备

#### Windows
>项目本身是用Visual Studio基于SDL2开发的，需要导入许多库，比如SDL2本体啊、SDL_TTF啊之类的。
>
>SDL2链接-> http://libsdl.org/download-2.0.php
>
>SDL_Image链接-> http://www.libsdl.org/projects/SDL_image/ （附有zlib、libpng等库的下载链接）
>
>SDL_TTF链接-> http://www.libsdl.org/projects/SDL_ttf/ （附有FreeType库下载链接）
>
>SDL_Mixer链接-> http://www.libsdl.org/projects/SDL_mixer/
>
>如果要在VS项目中导入上述这些库，需要下载对应机型的开发库，并在VS中通过添加`.lib`文件以及“附加包含目录”来导入。
>
>之后，不论是运行还是在VS等IDE中调试，都要将上述库的动态链接库文件（`.dll`），添加到与`.exe`游戏文件或VS工程文件同目录的位置。可以直接从上述链接中下载它们的`.dll`文件，也可以下载了工程文件后自行编译出`.lib`文件和`.dll`文件。

#### Linux
>
>是的，项目也兼容Linux系统（虽然目前只在Ubuntu上测试过）~
>
>和上面一样，需要先安装需要的SDL库。
>
>```bash
>sudo apt-get install libsdl2-dev
>sudo apt-get install libsdl2-image-dev
>sudo apt-get install libsdl2-ttf-dev
>sudo apt-get install libsdl2-mixer-dev
>```
>
>然后`g++`编译即可：
>
>```bash
>g++ main.cpp jsoncpp.cpp -I/usr/include/SDL2 -L/usr/lib -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -oMusnake.out
>```
>
>最后的运行就简单了：
>
>
> ```bash
> ./Musnake.out
> ```
> 

对于`Cmake`党而言，注意在写`makefile`时安装导入对应的库。

### 其他说明

#### 参考文档


SDL2文档-> https://wiki.libsdl.org/

另外，项目使用了JSONCPP，文档-> https://github.com/open-source-parsers/jsoncpp/wiki

会看文档就不怕不怕辣。

#### 许可

虽然代码写的不咋滴，但是好歹也是辛辛苦苦写出来的。所以，想以学习为目的嫖代码，但取无妨；不过不要魔改成商用的，否则请私自传播者自行解决版权等问题以及由此带来的纠纷。

本软件遵循BSD 3协议，该协议内容在`LISENCE`文件中。此外，游戏用到的库的许可在`COPYING/`目录下。在Fork我们的代码时，请记得复制保留这些许可协议。

#### 其他信息

由于开发日志体量较大，现已迁移至另一文件->`doc/diary.md`

此外，关于开发思路和游戏构成，我们将其整理到了这个文件中->`doc/document.md`

抽离出当时的思路后，我整理出了一版算是小2D游戏引擎吧，虽然说引擎有点夸大了，毕竟开发起来也全靠继承然后各种coding。回过头来看，感觉写的东西属实是屎山，真亏它当时能跑得起来。我现在没有用新框架实现它的打算了，所以这个repo大概率不会再更新。如果有人真的看上了我的作业，感觉有哪里不好懂的，可以issue，可能我现在也看不懂，但会尽量回想。

