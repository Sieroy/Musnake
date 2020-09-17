# 项目构成



（为了使文件中的流程图等能正常显示，请确保当前阅读器支持[Mermaid图表](https://mermaid-js.github.io/mermaid/#/flowchart)。特别地，对于网页端的读者，你可以为浏览器安装相关拓展应用，以使浏览器能自动绘制Mermaid图。）

## 程序构成

### 路径处理

程序里的文件路径都是相对路径，为了保证所有的文件都能正常读入，我们把这些相对路径转为了绝对路径并进行后续处理。

在程序开始时，我们先将`main`函数拿到的`argv[0]`字符串处理成程序文件所在的目录路径，并保存给一个变量。在以后使用相对路径时，利用`catPath`函数就能依据这个目录字符串来处理出绝对路径。

```mermaid
classDiagram

class PATH
<<var>> PATH

class initPath
<<function>> initPath
initPath : args:
initPath : - char* 程序所在的绝对路径

class catPath
<<function>> catPath
catPath : args:
catPath : - char* 目标字符串
catPath : - char* 给定的相对路径

class someFunction
<<function>> someFunction

someFunction *-- catPath : 绝对路径转换
catPath *-- PATH
PATH <|-- initPath : 存储程序所在目录的绝对路径
```

由于Windows和Linux文件路径分隔符不同，所以在`initPath`和`catPath`中有相应的变通。

### 时间控制

游戏没有机械地使用`SDL_Delay`函数来达到延时效果，而是利用当前帧和上一帧之间所差的时间来计算各种时间效果。

首先，我们有两个变量`timeVal`和`timeDelta`，分别用来储存程序当前运行的时间和距离上一帧间隔的时间差值，这两个变量由`updateTime`函数来更新；之后，其他函数可以通过`getTimeVal`和`getTimeDelta`函数来获取这两个变量的值，从而执行各种时间性的效果。

```mermaid
classDiagram

class timeVal
<<var>> timeVal

class timeDelta
<<var>> timeDelta

class fps
<<var>> fps

class updateTime
<<function>> updateTime

class SDL_GetTicks
<<lib_function>> SDL_GetTicks

class someFunction
<<function>> someFunction

class getTimeVal
<<function>> getTimeVal

class getTimeDelta
<<function>> getTimeDelta

someFunction *-- getTimeVal
someFunction *-- getTimeDelta
someFunction *--* fps

getTimeVal *-- timeVal : 对外只读
getTimeDelta *-- timeDelta : 对外只读

timeVal <|-- updateTime : 储存时间值
timeDelta <|-- updateTime : 计算出上一次更新后增加的时间值
fps <|-- updateTime : 计算出FPS

updateTime *-- SDL_GetTicks : 获取时间值
```

除了正常的按值处理的时间效果，为了达到“在某个时间执行某个函数”的效果，我们还构建了一种`DelayFunc`对象。这是一种由时间决定顺序的单向链表结构。其中的`(*func)`字段是所需要执行的函数，`timeVal`字段储存函数运行的时间值，`arg`字段储存需要传递给函数的参数，需要转为`Uint32`类型，其值将在待执行函数中被转为其原类型。

围绕着这个延时函数表结构，我们封装了几个工具函数——

`addDelayFunc`：用来构建并在延时函数表中添加节点；

`triggerDelayFunc`：用来触发指定的延时函数表中的所有已达到时间的函数，并将其释放；

`clearDelayFunc`：用来清空指定的延时函数表；

`removeDelayFuncByFuncArg`：查找指定延时函数表中的某个节点，并将其释放。

```mermaid
classDiagram

class DelayFunc{
	<<struct>>
	*func 结束后要运行的函数
	int 执行被延时函数时应传给它的参数
	int 延时的时间量
	DelayFunc* 下一个延时队列块
}

class addDelayFunc{
	<<function>>
	args:
	- DelayFunc** 延时队列表
	- *func 要延时执行的函数
	- int 延时执行函数所需的参数
	- int 延时时间
}

class triggerDelayFunc{
	<<function>>
	args:
	- DelayFunc** 延时队列表
}

class clearDelayFunc{
	<<function>>
	args:
	- DelayFunc** 延时队列表
}

class removeDelayFuncByFuncArg{
	<<function>>
	args:
	- DelayFunc** 延时队列表
	- *func 延时函数
	- int 函数参数
}

class getTimeVal{
	<<function>>
}

DelayFunc <|-- addDelayFunc : 按时间顺序增添元素 
DelayFunc <|-- triggerDelayFunc : 按时间触发
DelayFunc <|-- clearDelayFunc : 清空延时队列表
DelayFunc <|-- removeDelayFuncByFuncArg : 撤除指定元素
addDelayFunc *-- getTimeVal : 获取添加时的时间
triggerDelayFunc *-- getTimeVal : 获取触发时的时间
```

这个函数表结构可以有多个。

### 绘图对象

程序里的图片绘制都是借由一个名为`Flame`的对象操作的。`Flame`对象储存图片的`SDL_Texture`对象，和纹理原图片的长和宽。`Flame`对象有三个主要的方法，`setAlpha`方法用来设置所储存纹理的透明度；`draw`方法用来在指定的位置绘制`Flame`所存储的图像，可以用矩形指定长宽以达到缩放的效果，但至少应告知绘制的位置（`top`和`left`）；`draw_centered`用来以所给位置为中心点进行绘制，可以额外指定角度以达到图像旋转的效果。

```mermaid
classDiagram

class Flame{
	<<class>>
	SDL_Texture* 绑定的SDL纹理
	int 应当持续的时间
	int 纹理的长、宽
	Flame* 如果有持续时间，则指向下一个图帧
	setAlpha(Alpha值) 设置纹理的alpha值
	draw(位置指定) 绘制
	draw_centered(位置指定) 绘制
}

class draw{
	<<method>>
	args:
	- 绘制的位置信息
}

class draw_centered{
	<<method>>
	args:
	- 绘制的位置信息和旋转角度
}

class SDL_RenderCopy{
	<<lib_function>>
}

class SDL_RenderCopy_Ex{
	<<lib_function>>
}

class setAlpha{
	<<method>>
	args:
	- 设置alpha应成为的值
}

class SDL_SetTextureAlphaMod{
	<<lib_function>>
}

Flame .. setAlpha
Flame .. draw
Flame .. draw_centered

setAlpha *-- SDL_SetTextureAlphaMod
draw *-- SDL_RenderCopy
draw_centered *-- SDL_RenderCopy_Ex
```

`Flame`对象可以由构造函数生成，指定一个`SDL_Surface`即可，可以额外指定裁剪区域。但在构造时`Flame`仅会复制并使用由`SDL_Surface`生成的`SDL_Texture`对象，所以请注意及时释放这个`SDL_Surface`对象。此外，构造时也可以指定`Flame`对象的`duration`字段，这个属性可以用来控制这个`Flame`对象绘图持续的时间，在计时结束后，可以改为绘制`next`字段所指的`Flame`对象。

除非需要特别地裁切，一般地，可以使用封装好的几个函数来方便地构建`Flame`对象而非使用它的构造函数——

`loadFlameFromFile`：这个函数能接受一个相对路径，并将该路径下的图片文件读取到一个`Flame`对象；

`loadFlameFromText`：这个函数能接受一段ASCII字符串，并用指定的字体和颜色来绘制图片并读取给一个`Flame`对象；

`loadFlameFromUTF8`：这个函数能接受一段UTF-8字符串，并用指定的字体和颜色来绘制图片并读取给一个`Flame`对象。

```mermaid
classDiagram

class Flame{
	<<class>>
}

class loadFlameFromFile
<<function>> loadFlameFromFile
loadFlameFromFile : args:
loadFlameFromFile : - char* 图片文件相对路径
loadFlameFromFile : return(Flame*) 创建的Flame

class loadFlameFromText
<<function>> loadFlameFromText
loadFlameFromText : args:
loadFlameFromText : - TTF_Font* 绘制文字的字体
loadFlameFromText : - char* 要绘制的Ascii文字
loadFlameFromText : - SDL_Color* 绘制文字的颜色
loadFlameFromText : return(Flame*) 创建的Flame

class loadFlameFromUTF8
<<function>> loadFlameFromUTF8
loadFlameFromUTF8 : args:
loadFlameFromUTF8 : - TTF_Font* 绘制文字的字体
loadFlameFromUTF8 : - char* 要绘制的Ascii文字
loadFlameFromUTF8 : - SDL_Color* 绘制文字的颜色
loadFlameFromUTF8 : return(Flame*) 创建的Flame

class catPath
<<function>> catPath
catPath : args:
catPath : - char* 目标字符串
catPath : - char* 给定的相对路径

class IMG_Load
<<lib_function>> IMG_Load

class TTF_RenderText_Blended
<<lib_function>> TTF_RenderText_Blended

class TTF_RenderUTF8_Blended
<<lib_function>> TTF_RenderUTF8_Blended

class SDL_CreateTextureFromSurface
<<lib_function>> SDL_CreateTextureFromSurface


loadFlameFromFile *-- catPath : 绝对路径转换
loadFlameFromFile *-- IMG_Load : 读取出SDL表面
loadFlameFromText *-- TTF_RenderText_Blended : 渲染出SDL图面
loadFlameFromUTF8 *-- TTF_RenderUTF8_Blended : 渲染出SDL图面

loadFlameFromFile *-- SDL_CreateTextureFromSurface : 将图面转为纹理
loadFlameFromText *-- SDL_CreateTextureFromSurface : 将图面转为纹理
loadFlameFromUTF8 *-- SDL_CreateTextureFromSurface : 将图面转为纹理

Flame <|-- loadFlameFromFile : 纹理、长宽
Flame <|-- loadFlameFromText : 纹理、长宽
Flame <|-- loadFlameFromUTF8 : 纹理、长宽
```

### 游戏要素

游戏里有一些基本的对象。这些对象不包括连击数、分数等构成游戏机制的组成部分，这一部分在“程序流程”段的“游戏过程”分段下有介绍。

目前我们主要构建出了三种对象——`Snake`、`Food`和`Grid`。他们都从一个基类`Element`派生而来。

```mermaid
classDiagram

class Element{
	<<class>>
	Flame* 游戏图像元素的图帧
	SDL_Rect 图像元素所在的矩形
	int 图形元素当前图帧的计时
    update() 更新元素的样式
    draw(指定基点) 根据基点位置绘制元素
}

class Grid~Element~{
	<<class>>
	int 地格对象在地格网图中的x、y坐标
	int 指明当前地格内对象的类型
	union [Snake*, Food*] 地格内对象
	draw(指定基点) 根据基点位置绘制地格和地格上对象
}

class Snake~Element~{
	<<class>>
	int 蛇段在蛇头方向和蛇尾方向的朝向
	Snake* 蛇段的前一个和后一个蛇段
	Grid* 蛇所在的地格
	turnBody() 蛇头段因蛇的移动而作为蛇身段
	turnTail() 蛇身段因蛇的移动而作为蛇尾段
	shakeTail() 蛇尾因蛇吃食物或碰撞而摆动
	endTail() 蛇尾段因蛇的移动而消失
}

class Food~Element~{
	<<class>>
	Grid* 食物所在的地格
}

class update{
	<<method>>
}

class draw{
	<<method>>
}

class Flame__draw{
	<<method>>
}

class getTimeDelta{
	<<function>>
}

Grid *-- Element
Snake *-- Element
Food *-- Element

Element .. update
Element .. draw

draw *-- Flame__draw
update *-- getTimeDelta : 确定时间
```

#### 基类（`Element`）

`Element`类储存了对象图像的矩形，从而使对象被绘制时能够恒定图像的大小，此外`Element`类还封装了对象的时间性自更新功能以及绘制功能，其中，绘制功能还可以选择性指定0号地格相对于游戏窗口的偏移量，从而确保蛇头能始终绘制在窗口的中间部分。

关于对象的时间性自更新机制，它大概是这样的——

> 每一帧获取距离上一帧经过的时间；
>
> 用这个时间值更新对象的计时；
>
> 如果计时达到了某个值，使得对象的属性或图片需要改变，则执行。

当然，这都只是游戏内对象的基本特性，具体的功能还是要具体写写。

#### 蛇段（`Snake`）

`Snake`类

<img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAApQAAAEsCAYAAAB9viO5AAAgAElEQVR4nOzde4xk130f+O+5r7rVVdWvGQ1JkSL1IG1JdKzYQ683RiyLxtAJLCtaJ0s6gh0YEGJ2jCxgLGCBjBew4f3H4mqxWCeLtZubNTZIdgVzsLATi0DWYlaU5fXa8DCCrZVEx+RIokSRHM5MT3fX477P4nfuqZ6anrrTXd317P5+7FL1o95zWfXr3znfc0BERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERCTUmz/29/e+Se+7D70HH0L33nvROXc32mfOor26hjisz/RBDrEC4H4A7wLwgD2/3/58FNsAXgPwbQDfsuev2Z+fCJ94sHFSngoRERHNKYf/MERERER0HCwoiYiIiOhYvDl5+RQA1xa47sDXatiFtdbLWuuG1kVdFzrQWnu6KBSgR71bpRzHU0oFylF1pZyGUmpZqaF3O07yQAsAuT31vx71CRARERHN3LwUlFJA1gCE+86HPj4pJvMsO5dn2Vqepc08zWpZljpFUYx0p47jOJ7n11zfa7qev+Z6Xup6npSZq+N6YhUyADGAaN95NuH7JSIiIhq7eSkoHVtMNu2pZc+DYReWzqQUk2kcrSdR3EriKEyiyM3SdKQ79XzfDcIwDGphKwhrqTwEx3V8wOmN64lVSAC0AezaczHagyciIiKaE/PWoZQict2e1gAMjZfLMLd0JqWY7HXazV67Xevu7rpJFI10p0EYukutVq3ebMr9wnFd3/P9BlxT8E2SFKxbAHx7H1JMdid8n0REREQTMU8FZWg7k1JMnrOnoWveyJxJGeaWzqQUk7tbW+GNq1edXrsz0p3Wmw0nz7LQPADP8/0gqOtQr0xh6Lmzr5js2deAiIiIaOHM05B3v0O5ZovJewEsD7uwBHBkzqQMc0tnUorJN7/1bXfryrWR7nTt3BnTGZViMgjDpVq9nutyIuakwzE79jy1xeU2E/dERES0qKZdUA5Nc2utV8pTsaIL+3VRrAB6aEEpQ9tJ1EMSd5DEbaTJDtL0GtLszeH3quX/tXQ2zTl0+XU9zlQSt7w4XkIc1VGLakhqtSnUkyZd3lZKtZWjdpWyX08m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">



#### 地格（`Grid`）

#### 食物（`Food`）



## 程序流程

### 总体流程

### 标题页面

### 选单页面

### 设置页面

### 游戏过程



## 文件构成



## 附带工具