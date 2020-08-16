# Musnake

Just a game for my homework.

---

// 唉，本来想转转英语，结果还是放弃了。

### 简介

这个算是游戏的东西就是我们的课程设计。要求做一个贪吃蛇这种的小游戏，但是啊，就一个贪吃蛇未免也太无趣了吧，于是联系上我之前玩过的一款名为节奏地牢的游戏，我想把这只贪吃蛇做成节奏型游戏。

大概玩法是这样的：

我们的小蛇会随着玩家的上下左右指令而在一个类似地牢的空间里蠕动，蠕动过程中要避开墙壁和自己的身体，并尽力吃食物。小蛇初始5滴血，最高10滴血。如果撞到了墙壁或自己的身体，就扣3滴血，如果吃到了食物，就加1滴血，并获得200分数。血量扣到0点或更低，就死亡；血量达到8点以上，就进入FEVER，双倍得分。游戏中会有背景音乐，如果卡着点移动的话，会获得20分；如果在连续的BEAT都能踩点移动，则计算连击，连击数除以10并舍弃小数部分后的数将加成到当步得分中；如果MISS了，就会自动向蛇头前方前进，但是不加分，同时也视为断连；如果向蛇头相反的方向移动，那么蛇不会移动且断连；如果超速行驶，在BEAT判定内的移动会计入得分，但多余的移动将会增加MISS，同时，限定最大Move Per Second为10。

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

> ***Sieroy***:
> 
> >嗯，开了个题，顺便写了写readme，然后开了工程+做了做环境的配置，试了试感觉也没什么问题。感觉不赖。

##### 2020年7月21日

> ***Sieroy***:
> 
> > 画图画图+画图。哇，原来像素画这么恶心的嘛。想想以后还要画地图块的Sprite图、连击上瘾的小蛇皮肤，我就有点害怕了。今天只画了蛇身、蛇摆尾的Sprite图（蛇摆尾的甚至还没画完），明天还有蛇摇头和剩下的摆尾动画。做完这些后应该就可以着手Coding了吧。那希望明天能搞完吧。

##### 2020年7月22日

> ***Sieroy***:
> 
> > 尾巴已经好了，可以烤了。头的话，图已经OK了，接下来就是时间问题了。是的！我今天摸鱼了……不过，Coding方面倒是有些进展，一些基本元素的定义都好了，但还要等测试。明天吧，明天把图搞完，然后对我的小小蛇测试一下。另外，可能要把这些图都放进一大张图里，方便程序的提取，今天已经完成了尾巴图的部分图的Layout，明天可以做做。（明日复明日）

##### 2020年7月23日 上午

> ***Sieroy***:
> 
> > 终于，劳资把图搞完了（难得没有摸鱼），分文件的图和大张的图都搞好了，PSD文件也Lay好了，方便后期再改或继承。然后，看了一个`SDL_mixer`模块，忽然想试一试，毕竟SDL自带的Audio支持太麻烦了哈哈。Mixer的链接已经更新到上文中去了，搞BGM和音效的时候可以学学。下午就，肝测试程序咯（草）。

##### 2020年7月23日 下午

> ***Sieroy***:
> 
> > 肝，都可以肝。都肝。但是没肝完。具体来说，下午主要整了整`Game`的初始化，并在初始化里载入Sprite图。载入时主要得关注图的顺序问题（毕竟要循环嘛，总不能让我一条一条写吧，那500+行瞬间就有了），同时还得让每一个情况都能得到助记的效果，于是，排布这些个东西就很肝了。再加上处理帧组间的关联、以及时不时为了某个功能而加函数，于是今天下午是刚刚把`Game`的载入搞完的样子。明天搞`Game::run`吧，赶紧把测试做了然后填填补补de de bug搞搞逻辑部分了。
> > 
> > // 刚刚发现日期写错了，尴尬……改了改了。

##### 2020年7月24日 上午

> ***Sieroy***:
> 
> > 上午还没run好。为了优化一下结构，加了一个`declaration.h`头，希望能程序部分能整齐起来吧。至于`Game::run`，已经把框搭起来了，顺便围绕着它完成了许多功能性函数。不过事件处理还没怎么写。下午就把事件处理搞了吧。尽量把一测搞了好开展下一步工作。

##### 2020年7月24日 下午

> ***Sieroy***:
> 
> > 一测完成，现在小蛇它会爬了，看着贼鸡儿顺畅的动态效果，我赶紧把这个版本Push了。虽然小蛇的爬是要按方向键的那种，时间催爬我还没写，不过好歹蛇的UI层被证明正常了。有UI输出了，接下来写写贪吃蛇的逻辑吧。后面还有地格的Sprite，讲真的地格Sprite我想直接嫖节奏地牢的了；然后还有节奏判定的实现。明天有课，能把基础逻辑写完就好（省的再明日复明日）。

##### 2020年7月26日 凌晨

> ***Sieroy***:
> 
> > 逻辑？没写。但是啊，我啊，已经把节拍支持+催动+BGM播放大致写好了。不过现在有个匪夷所思的BUG，就是小蛇移动锁定时间莫名其妙地长，本来设置0.1秒，但实际测试时感觉有0.2到0.3秒之多，这就很不妙。希望起床后能De掉这个bug。

##### 2020年7月27日 凌晨

> ***Sieroy***:
> 
> > 唉，今天干了什么呢？没多少，毕竟花了好几个小时的时间忽悠学弟来我们学校嘛。具体做的，就是把食物加上了，包括食物的刷新和被吃，但也就这，弄好了一个框，更高级的食物操作还没有实际地搞。此外，耗时一半天把昨晚遗留的BUG排了。所以今天主要是排BUG咯……明天把HP系统、连击系统、分数系统搞出来吧，如果能把可视化的Note搞出来就更好了~
> > 
> > 其实到现在，除了蛇还没有死亡判定，可以说基本的贪吃蛇已经完成了。可以在HP系统弄好之后分出一个Branch，如果到时候其他的系统没弄完，就这个基本版的贪吃蛇就够交作业的了哈哈哈，虽然尽量避免这样吧。

##### 2020年7月28日 凌晨

> ***Sieroy***:
> 
> > 今早一起床，就发现，已经11点了。看来昨天忽悠学弟忽悠得有点晚。于是，今天本应完成得工作还差一些。今天搞完了HP系统，HP现在可以正常显示、并随着蛇行而舞动了；此外还弄了简易的音符线帮助玩家找节拍（可以再绘制得更精美）；另外，为了不让开局即音乐的紧迫场面出现，我开局加了2秒的延时播放，并提前半秒开始接收输入。
> > 
> >至于连击系统和积分系统，因为原本代码结构就给它们留了位置，所以写起来并不麻烦，关键是输出。现在正打算把可打印字符打包到一个图片文件中，不然就SDL的TTF低渲染速度，恐怕会造成不小的卡顿，而且矩形也不好控制。只能复明日了。
> > 
> > 此外，在现有成果的细节方面，也有一些可优化/加强的地方，一方面是注释中多次提到的实体对象的绘图基类问题，这个等重构时再说叭，先修修补补把成品做出来再考虑这些结构问题；另一方面是蛇转体/前进碰到物体或吃到食物，这方面还没有特效用来强调，单有血量变化作为反馈。此外，还有启动页面、菜单、单局游戏结束页面之类的。
> > 
> > 现阶段嘛，就先放一放地格，尽量在8月前搞完上面这些，至少游戏看起来像个游戏吧。当然，地格的实现也不复杂，关键是图片问题，在8月前也能把地格给搞了固然不错，但囿于画图的工作量，所以先推后叭。

##### 2020年7月28日 下午

> ***Sieroy***:
> 
> >现在呢，连击和积分以及相关的IO已经好了，所以我想存个档，然后开始搞菜单。毕竟连菜单都没有，蛇的死亡就没有复现意义，玩家玩着也没有一个闭合完整的体验对吧。而且`Game`在析构阶段的内存释放还没有写完+debug，所以，赶紧肝菜单咯~

##### 2020年7月29日 凌晨

> ***Sieroy***:
> 
> > 菜单搞了一半，忽然想再加一首曲子试试多曲目的选择效果，于是花了点时间搞了搞金蛇狂舞的节拍（444.4的间隔，是在搞我？）。现在正在写显示用的函数，估计明天能写完。此外，较大的改动是，把一局游戏内的载入帧组的操作挪到了程序开头。毕竟只有20+的内存占用，搁到全局好像也没什么。

##### 2020年7月30日 凌晨

> ***Sieroy***:
> 
> >唉，真就不到半夜就零生产力呗……这次把菜单的基础跳转逻辑和简陋能看的UI写好了，于是，程序有了能看的入场，入场可以听各个曲目的歌曲预览、看各个曲目的封面。此外，实装了FEVER系统，吃食物后，溢出10血的部分将会为包括当前一步的接下来5步提供2倍分数（当然以后会延长），这也是为了抵制一条蛇长时间绕圈刷分而加的一个系统吧。
> > 
> > 接下来把游戏暂停功能搞了、把菜单绘制模块化、把游戏结束回到菜单debug一下。然后，把菜单、判定线等的UI该完善的完善。感觉8月前达成2期目标有希望嗯。
> > 
> > 不知不觉这个项目已经立下10天了，感觉主线进度几乎全在第3到第7天。虽然其他时间摸鱼情况严重，但能半拖半做地弄到现在感觉还是挺GOOD的。再加上前几天看到鹅厂一款手游的某个模式（BV14A411v7kR），我TM信心大振，甚至出现了“给我一个美工我就能进鹅厂”的谜之膨胀。哈哈哈，行行行，自己夸自己。不过夸归夸，与一些游戏创新比赛里那些3天肝出一个大DEMO的参赛者相比我还是有够拉跨的。接下来好好搞吧，毕竟到最后还要搞成2.5D的类节奏地牢的视角才行，而且还要去写LEVEL的自定义，不能就靠一幅15X20的静态盒子图混嘛。
> > 
> > （说实话我开始担心写报告的问题了。做的太酷炫会不会最后报告跟不上啊……）

##### 2020年7月31日 凌晨

> ***Sieroy***:
> 
> > 夜晚有催人高效工作的魔力。今天把菜单、游戏结束、游戏暂停之间的跳转逻辑做完了，剩下的就是UI了。讲真现在的UI真的是丑得1P，我自己看了就觉得叭太行，硬插图吧，又贼怪，唉，果然我这儿缺一个美工（确信）。唯一能让我舒心的就是1000~2000的FPS了。
> > 
> > 逻辑写好了，新的BUG又出现了——在暂停回复后，我们的小蛇会不知为何出现错帧BUG，而且在游戏暂停追踪一次后就再也没能触发过，感觉是颗不小的定时炸弹；此外，暂停回复后，音乐与节拍会发生不定的错位。而且更奇怪的是，在《金蛇狂舞》中这个现象贼明显，而在《Tombtorial》中就几乎莫得问题。奇了。真就逼我在游戏对象里也加一个计时变量呗。

##### 2020年8月1日 凌晨

> ***Sieroy***:
> 
> > 好了现在8月了。昨天提到的两个BUG，前者已经完美解决了。想来BUG的原因还是我在冲凉时灵机一动想出来的，真就穿着衣服就想不出灵感呗——那是次底层对蛇的实现，在更新帧组时没有考虑到暂停时间，而只是调用了一下`getTimeDelta`这个全局的时计函数，再加之更新时只更了一次帧，并没有按照当前帧计时的实际情况来构成循环，所以就，唉……至于第二个，应该是`SDL_mixer`库函数`Mix_ResumeMusic`用时过长的问题，我正在考虑添加`updateTime`来达到低偏移的目的。
> > 
> > 那么毕竟也到8月了，来总结一下吧。到目前为止，基本的节奏贪吃蛇已经构建出来了，但地格处理的进度几乎为零，而且之后还要搞自定义地格的支持，所以时间并不宽裕——因为程序相对会比较复杂，所以为了处理报告，要预留一周左右的时间来回忆我的工作~~以及瞎编~~，那么算来，留给我的时间也就不到三周。此外还有UI的精美化，精灵图的绘制和品质提高，所以，啊希望我能高效一点吧。
> > 
> > 对了，Stdcat那只鸽子不知道怎样了，这不得去时不时骚扰骚扰他？

##### 2020年8月2日 凌晨

> ***Sieroy***:
> 
> > 你们可能不知道，一天时间看完两部番是……好吧我错了，我面壁。
> > 
> > 但是，也是有成果来着，比如，构出了绘图元素的基类并应用到现有的几个类上啊；比如，重构了Flame的构造函数，使它在实际调用时可以更方便啊；再比如，补上了好多个位置的初始化，以免再出现一编译就蹦出一大堆Warnings的情况啊。
> > 
> > 此外就是在脑内构思了一下选歌界面、暂停界面、结算界面的UI背景。还有，顺便修了一处`SDL_Surface`没有及时Free的小bug。
> > 
> >明天的任务嘛，能用TTF的就不要再截`char.png`了吧，所以我要在这方面再重构一下。如果渲染太费时间了，那就得考虑统一加载以及加载画面的UI咯。

##### 2020年8月5日 凌晨

> ***Sieroy***:
>
> > 咕咕咕。
> >
> > 前两天有志愿者活动所以咕咯……今天则继续工作（虽然也没多少）。
> >
> > 大体上有两处更新——部分字体由TTF库来绘制而不是继续依靠字符图，接下来准备逐渐改用这种模式；此外标题页加了个BGM。其他也有一些进展，比如绘制了胜利界面的UI背景，绘制了一些场景的键位提示。
> >
> > 还有一个重要的进展——叫醒了StdCat老咕！！！

##### 2020年8月5日 晚上

> ***Sieroy***:
>
> > 今天进度蛮大的。当然，都是UI上的进度，游戏地图方面还是没怎么动。
> >
> > UI上，又把一些地方的文字改成了TTF绘制，同时，首页、选歌页、暂停页增加了操作提示以及背景；游戏胜利页增加了背景；游戏中暂时标上了判定线（是的，暂时，以后要换用鬼畜的精灵图2333）。
> >
> > 那么接下来就是继续搞UI，并准备切入地图部分。

##### 2020年8月7日 晚上

> ***Sieroy***:
>
> > 昨天就放在今天一起更吧hhh
> >
> > 这两天主要是优化了一下关卡的结构，在Level的基础上新增了LevelClass，以方便玩家找到要打的歌。为此，我给程序加上了JSON支持，这也方便后期搞用户最高分之类的存档工作。同时有了JSON支持，我也把关卡文件夹改了个更隐秘简短的名字。然后就是加上了LevelClass的UI表现，现在可以在菜单界面通过左右键切换LevelClass了。
> >
> > 然后修复了个小BUG，现在随机数生成更加随机了。当然，随机数roll食物这块还有待改动。
> >
> > // 顺便地，美化了一下Readme的样式。
> >
> > // 然后发现，StdCat老咕是不是没有更新过…… /沧桑

##### 2020年8月9日 晚上

> ***Stdcat:*** 
>
> > 来了来了，他来了，金光鸽王来了🕊🕊🕊🕊🕊🕊🕊
> >
> > 加入了自定义蛇和食物初始位置的功能，并且加入了鼠标支持
> >
> > 在一阵暗牧鬼畜后完成了大地图支持 Sieroy真是有先见之明啊哈哈哈哈哈哈
> >
> > // 没意外应该可以继续写，咕咕
>
> ***Sieroy***:
> > StdCat NB！！！
> >
> > 一上来就加入了好多高大上的功能，这只鸽子不鸣则咕，一鸣惊人！
> >
> > 我嘛，我就做做幕后善后工作了，比如给歌名下方增加了原曲作者的标注，以及为绘制大地图做了些平滑处理~ 顺便地，优化了几处数据结构，现在程序占用内存更少咯（虽然只少了不到1M）~ 再顺便地，更改了一下关卡评级的算法，专治各种手速帝~
> >
> > 所以还有用户自定义设置支持，还有用户信息的JSON支持，还有地格背景图和障碍块的图，还有几处UI。淦！

##### 2020年8月12日 凌晨

> ***Sieroy***:
>
> > 唉，罪恶，太罪恶了。
> >
> > 先说说这两天我干了什么吧。先是补完进击的巨人1-59集，接着又疯狂看漫画，所以……唉。
> >
> > 不过到目前为止，小蛇也算是有了很大的进展——新增了读档存档功能，现在小蛇能保存最高分咯；从《节奏地牢》目录下找到了一只可爱的exe文件，能把歌曲offset和BPM解析出来并生成与`note.mu`结构相近的文件；新增了一个分类“二次元”和一首用上面提到的那个程序解析的歌《SPEAK》（寄生兽OST里的，“谷歌谷歌淦了谷歌”）；决定嫖节奏地牢里的Sprite图，地格、墙壁的图有咯。
> >
> > 于是，忽然资源文件就快凑齐了。那还不加紧开发？！
> >
> > 先把读档存档功能以及衍生UI搞出来，接着弄其他整块的UI，再接着，我想实现歌曲的Toast功能，毕竟新手教学要用。
> >
> > // 因为在存档机制上偷了个懒，所以内存占用量又提高了……

##### 2020年8月12日 晚上

> ***Sieroy***:
>
> > 今天还行，只摸了几个小时的鱼，通了《Steins;Gate》。
> >
> > 然后蛇这边，嫖来了节奏地牢的部分贴图，并成功地加入到了游戏中；实现了用户最高分数的显示和打破最高分的提示显示功能。
> >
> > 接下来准备搞一搞Toaster，给新手教程铺个垫。有时间的话，再研究一下食物指示牌要怎么安排；以及设置面板的问题。另外，还要记得赶紧把TTF的推行工作做掉。

##### 2020年8月14日 凌晨

> ***Sieroy***:
>
> > 如果不往大了写，就永远不会知道在底层还有什么奇葩BUG等着我。
> >
> > 在通关《Steins;Gate》所有线、并刷了好几集《人渣的本愿》后，我开始了今天的工作（咕）。
> >
> > 这次还挺顺利，Toast功能已经做好了（在1小时设计+Coding，外加2小时Debug之后。这Bug也是奇葩，源于底层对延时函数表的实现——有一个我怎么也想不到我会写错的指针弄错了，艹），找时间上新手教程吧。教程使用曲暂定为《游戏人生》OST里面的《TETO》。
> >
> > 此外，修复了胜利界面打破纪录显示异常的BUG；给游戏时画面的水增加了动态效果；用Python为《节奏地牢》自带的`beattracker.exe`程序增加了Musnake格式匹配程序。

##### 2020年8月14日 晚上

> ***Sieroy***:
>
> > 很好，今天除了把《人渣的本愿》补完了之外没有怎么摸鱼！
> >
> > 今天把《TETO》的谱手动扒出来了，并做进了游戏里；另外，配合昨天完成的Toast功能，把新手引导做了，配曲就选了《TETO》；用《节奏地牢》自带的`beattracher`把《Bad Apple》解出了谱子，奈何与曲子对不上，所以先搁置着，准备日后手动扒谱；修复了之前关闭程序时有概率导致程序跑飞的问题；修复了Stdcat留下来的一处点击事件处理Bug。
> >
> > 忽然想把《Vitality》或《骨折り損》做进去了，要不难度太小蛤蛤蛤。估计这样的话，移蛇锁就要缩短时间了……
> >
> > // 22点25分
> >
> > 我真的把《骨折り損》加上了，然后为了保证效果，我把锁的时长减到了86（为了防止在低FPS时发生内存炸裂……）。结果，我发现，我手跟不上。这TM就尴尬了。反正为了不死绕圈圈也是能过关，但就是不爽……唉……

##### 2020年8月16日 凌晨

> ***Sieroy***:
>
> > 怎么说，肝得有点猛，一看表，3点半了卧槽。还好7点到9点睡了一小会儿。
> >
> > 今天把《Steins;Gate》白金了，开始《Steins;Gate 0》，めでたしめでたし...じゃなくて！这都是次要的嗯嗯。
> >
> > 今天把蛇锁给解了，大大小小的改动几乎给底层小规模地回顾了一遍，现在《骨折り損》容错率没那么地狱级了；扩大食物生成区域为整个地图；增加场上同时存在食物数量为3个，以免小蛇跑半天还没吃的；此外，增加了食物指示标（初中三角函数玩爽了），在食物位于窗口以外的位置时给予食物位置提示，现在游戏时帧率成功降到了600以下。
> >
> > 另外，部分贴图的配色有所改动。
> >
> > // Stdcat又双咕了，小本本记仇。

##### 2020年8月16日 中午

> ***Sieroy***:
>
> > 昨晚肝得太狠了，早上一起床发现已经12点了……
> >
> > 趁着吃午饭，我把积攒了快一个月的Warnings清掉了，顺便改了一处大的内存泄漏BUG。然后又给地面贴图加了点灰度。
> >
> > 下午努力把设定界面或信息界面肝出来吧，或者，把《Vitality》谱扒出来做进去~