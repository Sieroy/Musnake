# Musnake

Just a game for my homework.

---

// 唉，本来想转转英语，结果还是放弃了。

### 简介

这个算是游戏的东西就是我们的课程设计。要求做一个贪吃蛇这种的小游戏，但是啊，就一个贪吃蛇未免也太无趣了吧，于是联系上我之前玩过的一款名为节奏地牢的游戏，我想把这只贪吃蛇做成节奏型游戏。

大概玩法是这样的：

我们的小蛇会随着玩家的上下左右指令而在一个类似地牢的空间里蠕动，蠕动过程中要避开墙壁和自己的身体，并尽力吃食物。小蛇初始5滴血，最高10滴血。如果撞到了墙壁或自己的身体，就扣4滴血，如果吃到了食物，就加1滴血，并获得200分数。血量扣到0点或更低，就死亡；血量达到8点以上，就进入FEVER，双倍得分。游戏中会有背景音乐，如果卡着点移动的话，会获得20分；如果在连续的BEAT都能踩点移动，则计算连击，连击数除以10并舍弃小数部分后的数将加成到当步得分中；如果MISS了，就会自动向蛇头前方前进，但是不加分，同时也视为断连；如果向蛇头相反的方向移动，那么蛇不会移动且断连；如果超速行驶，在BEAT判定内的移动会计入得分，但多余的移动将会增加MISS，同时，限定最大Move Per Second为5。

嗯嗯，就这样吧，虽然有些抽象，但值得一搞。

### 环境准备

项目本身是用Visual Studio基于SDL2开发的，需要导入许多库，比如SDL2本体啊、SDL_TTF啊之类的。

SDL2链接-> http://libsdl.org/download-2.0.php

SDL_Image链接-> http://www.libsdl.org/projects/SDL_image/ （附有zlib、libpng等库的下载链接）

SDL_TTF链接-> http://www.libsdl.org/projects/SDL_ttf/ （附有FreeType库下载链接）

SDL_Mixer链接-> http://www.libsdl.org/projects/SDL_mixer/

可以直接下载它们的dll文件，也可以下载了工程文件后自行编译出lib文件和dll文件。

SDL2文档-> https://wiki.libsdl.org/

会看文档就不怕不怕辣。

### 开发日志

##### 2020年7月20日

嗯，开了个题，顺便写了写readme，然后开了工程+做了做环境的配置，试了试感觉也没什么问题。感觉不赖。

##### 2020年7月21日

画图画图+画图。哇，原来像素画这么恶心的嘛。想想以后还要画地图块的Sprite图、连击上瘾的小蛇皮肤，我就有点害怕了。今天只画了蛇身、蛇摆尾的Sprite图（蛇摆尾的甚至还没画完），明天还有蛇摇头和剩下的摆尾动画。做完这些后应该就可以着手Coding了吧。那希望明天能搞完吧。

##### 2020年7月22日

尾巴已经好了，可以烤了。头的话，图已经OK了，接下来就是时间问题了。是的！我今天摸鱼了……不过，Coding方面倒是有些进展，一些基本元素的定义都好了，但还要等测试。明天吧，明天把图搞完，然后对我的小小蛇测试一下。另外，可能要把这些图都放进一大张图里，方便程序的提取，今天已经完成了尾巴图的部分图的Layout，明天可以做做。（明日复明日）

##### 2020年7月23日 上午

终于，劳资把图搞完了（难得没有摸鱼），分文件的图和大张的图都搞好了，PSD文件也Lay好了，方便后期再改或继承。然后，看了一个SDL_Mixer模块，忽然想试一试，毕竟SDL自带的Audio支持太麻烦了哈哈。Mixer的链接已经更新到上文中去了，搞BGM和音效的时候可以学学。下午就，肝测试程序咯（草）。

##### 2020年7月23日 下午

肝，都可以肝。都肝。但是没肝完。具体来说，下午主要整了整Game的初始化，并在初始化里载入Sprite图。载入时主要得关注图的顺序问题（毕竟要循环嘛，总不能让我一条一条写吧，那500+行瞬间就有了），同时还得让每一个情况都能得到助记的效果，于是，排布这些个东西就很肝了。再加上处理帧组间的关联、以及时不时为了某个功能而加函数，于是今天下午是刚刚把Game的载入搞完的样子。明天搞run吧，赶紧把测试做了然后填填补补de de bug搞搞逻辑部分了。

// 刚刚发现日期写错了，尴尬……改了改了。

##### 2020年7月24日 上午

上午还没run好。为了优化一下结构，加了一个declaration头，希望能程序部分能整齐起来吧。至于run，已经把框搭起来了，顺便围绕着它完成了许多功能性函数。不过事件处理还没怎么写。下午就把事件处理搞了吧。尽量把一测搞了好开展下一步工作。