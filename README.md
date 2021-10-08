# UILib

#### 项目介绍
Windows平台基于ATL的DirectUI界面库，可以只改1个UIWindow.h文件就能给MFC用，也可以继承CUIRootView并改几行代码给windowless模式用。
这是我2012年开始自创的界面库，最开始只有Button控件，根据项目需要不断添加新控件，5年期间用它做了4个大型客户端产品，现分享给有需要的小伙伴。
得益于巧妙的设计，每个控件代码都极其精简，基本都在100行左右，使用也非常简单，怎么用都不会出问题，而且功能强大、性能极高，可以实现任何复杂界面，比DuiLib更更好用。

#### 使用说明
TestLib是ATL用的例子，TestMFC是MFC用的例子，MakeSkin是专用的皮肤打包器，打包后可以是单个文件，也可以嵌入到项目资源中。例子最开始，都有UILib::LoadSkin先加载皮肤，一般Debug下指定路径，加载路径下的图片文件，便于开发调试，Release下指定打包后的整个皮肤文件或资源，提高效率，解包算法完爆用不压缩的.zip。

先从基本的介绍，CImagex是公用图像类，有2个成员std::shared_ptr&lt;CImage&gt;、CRect，分别是图像的指针、和自己在图中占的区域。比如一个3态的按钮，每态宽高为40、20，则图像总宽高为120、20，加载时指定“按钮.png:3”，就加载了第1态，用SetFrameIndex(n)切换其它态，从源码可以看到，切换只是把自己的CRect改了一下，图像是共享的。

接下来是CUIBase、CUIView、CUIControl、CUIRootView，这4个类把整个UI库驱动起来了，其它控件只是继承CUIView或CUIControl，并特化自己的绘制、键鼠消息。主要看下这4个类的源码，看懂之后，其它控件就都非常简单了。

我简单介绍一下：CUIBase是所有的祖先类，里面方法很少，一看就懂；CUIView继承自CUIBase，它是一个容器类，有成员vector&lt;CUIBase*&gt; m_vecChilds，方法更少更好懂；CUIControl继承自CUIView，看名字就知道是它控件的基类，像CUIButton、CUICheckBox等都是继承自它，这个类提供的方法同样很少；CUIRootView继承自CUIView，它是顶层视图，用于和window、windowless交接。整个界面是树形结构，顶层根是RootView，往下是各层各个View、Control，每个控件都能GetRootView使用它。

我实现了一些基本控件，和几个高级控件（CUISlider、CUIVScroll等），高级控件都可以由基本控件组合而成，就像原子核+电子组成原子，原子组成分子一样。举个简单的例子，一个按钮的右上角想放个小标，CUIButton继承自CUIControl，CUIControl又继承自CUIView，所以CUIButton可以直接Add一个子CUIImage。

处理鼠标消息时，树形结构最下层Control先处理，也就是视觉的最上层控件，它会独享鼠标点击消息，即使它没有响应，也不给其它控件机会（这就像windows上的窗口行为一样），如果想让鼠标点击时穿透它，比如CUIImage等，则设置m_bClickable = false。

MouseEnter、MouseLeave这2个消息特殊，如果它的Parent是CUIControl，则会让Parent也同步Enter、Leave，比如浏览器标签，标签是按钮，右边的×号是它的子按钮，你试试鼠标放在标签的×号上，是不是标签和×号都亮了。

下面讲**最核心**的部分，把一个窗口的所有控件都摆好，算法在CUIView的RecalcLayout和CUIBase的CalcRect。对于每个View（Control也是View），包括RootView，当它的区域改变时，会先计算出自己的rect，然后再布局所有子控件，每个子控件布局后，如果和之前相同则返回，不同就又会布局它的子控件，这样依次递归。

RecalcLayout算法：初始rect为自己的m_rect，依次把这个rect传给子控件，让它们按照约束，CalcRect出自己需要的部分，并把剩下的部分返回。

CalcRect算法：传进来的rect，对于左右方向，有左、右、宽共3个属性，都不设置的话：left=rect.left、right=rect.right。只设置1个属性的情况：只设置左则left=rect.left+左、right=rect.right，只设置右则left=rect.left、right=rect.right-右，只设置宽则left、right就在rect的居中。同时设置2个属性的情况：设置左、右就定下来了，设置左/右、宽就能算右/左。如果同时设置3个属性，则会使用左、右而忽略宽。上下方向同理。
如果指定了左、宽，可以同时指定左剪裁，就是划分完myRect后，剩余rect.left=myRect.right并返回，注意看SetLeft第2个参数，xml中对应的是数字后加*号，右、宽，上/下、高都同理。
如果控件使用了图片，比如Button、Image等，可以不用设置宽高，初始宽高就是图片的宽高，也可以根据需要SetWidth、SetHeight。

例如构建一个简单窗口，先在RootView上用top、height划出标题栏并剪裁，剩下的就是客户区。然后再在标题栏View上用right划出关闭按钮并剪裁（不用指定width），再用right划出最小化按钮，再用left划出窗口名等等。

关于RecalcLayout效率，使用了延迟布局算法，任何控件添加、删除、改变上下左右宽高，都会InvalidateLayout记录一下，并在最后需WM_PAINT前一次性布局所有需要的。

未完待续……