# BCreator.io

## 简介

## Build

### 第三方库及工具
#### 第三方库
- 包括字体、物理（Physx）、纹理资源、导入库用到文件格式（图片，FBX模型等）、图形库、声音处理、压缩、网络协议、光影及空间算法（lightmapper、nvmesh、waveworks等）、一般算法和容器（插值、数学、内存池等）、脚本环境（mono）、基础库（stl等）、调试（Log等）。
#### 第三方工具
- 调试、发布

### Visual Studio Community 2017

### Emscripten

### CMake

## Quick Start

### Try in Console

### Check examples

### Try the Creator

## Organization Glossary
- Asset、Prefab、Templet等概念在Runtime里并不存在，Unity3D里好像也是如此。在应用开发工序流中存在的一种术语。

## 注意
- 本框架伪代码暂时未考虑线程安全。
- SpaceTree的优化问题。转化为二叉树优化遍历，对我们有用吗？

## 模块关系
- 依赖关系是一个DAG，尽量按近似树的方式进行绘制，方便人类脑理解。

## Thinking In Nature
- Simpler, simpler and simpler! More simple, more easy. 而且也要考虑团队成员良莠不齐这个现实问题。
- Engineering Structure: Human brain & Code architecture.
- Glossary里的概念也尽量按最朴素的无专业领域人为感染的自然思维来进行定义。
- 依赖关系（h文件、包的依赖）最好是个TREE，顶多只能是DAG，绝对不能有环。
- 消除二义性、歧义。
- Part可以成为别人的零件。有两种方式自己被造出来，一种compile期coding写的，另外一种是运行期由别的零件运行构建出来。
- Part都可以被重用、并且序列持久化。
- 只有两种被使用的方式：A被调用；B被继承。本来在现代语言代表GO中，舍弃了继承这个特性，而我们仅为了类似脚本的字符串化操作（创建、属性等）而是用了继承（于Part，从这一点看Part很像很多工程里的Object)。针对被继承性质的抽象类，放入meta中，极力避免“继承”这一特性污染我们的工程。Part互相之间的关联关系：包含、引用、继承，同OOD的概念类似。“包含”、“引用”在运行期对应着前面的A被调用。其中“引用”意味着“实例”内数据会在别处被修改。Part零件元对象（类）兼具着可被继承，同时也是“引用和包含”行为的客体和主体。而Node（继承于Part）兼具这些特性的前提，而断绝了被继承的可能，实质相当于Unity3D的GameObject，胶水黏着物的底料。Node可以胶水联合上任何Part，例如某个Part也可是个自成体系的某种Space（例如Terrain/BSP/骨骼动画等）
- 在Unity3D中，Prefab同Scene没有实质区别，所以在我们这里概念统一为SpaceTree，SpaceTree的一部分可以被存储起来（相当于Prefab），SpaceTree里面实际上就是Node组成的树。关于Uniy3D中“Prefab”（我们还是叫SpaceTree）同来源实例及使用入"Scene"内的实例之间的关联关系，好像是一旦Scene内的实例自行修改过值后，Prefab数据才会断开联动，否则Prefab的修改会带动Scene内的实例修改。也就是说，似乎有一种隐性的从包含关系到引用关系的转变。我们于此的规则也不完全相同，我们沿用New3D时代的Cluster节点性质，生成Prefab（SpaceTree）时，实际上是从原SpaceTree树的一个分支作为源进行拷贝整个分支的所有节点成为一个“SpaceTree树枝预制件”，同之前的“源树枝”就再无关系。后面被使用，会有两种性质的使用操作，一种是作为一个Part零件黏合到别的Node或者Part对象上去，其本身可作为一个整体，是一种引用关系。一种是往某“新SpaceTree”中放入，这相当于是把此“SpaceTree树枝预制件”做了拆解操作而拷贝融入“新SpaceTree”中，与“SpaceTree树枝预制件”再无关系。
- Package、Module等抽象概念，各人内心自定义的含义都不一样。这种模糊的用词并不科学，实质也无非想表达一个段落层次。但是现实中一般人对层次的理解同样局限于"事不过四"，所以仍旧是可选用朴素的自然思维名词来规范组织语言。我们使用包的概念，包可以有路径，也就是说包可以有层次，但是为了简便起见，在我们自己项目的范畴，也并不打算把包同多层namespace挂钩。

## 约定
- 项目的逻辑结构同磁盘物理路径结构。
- 命名中含有直属上一层种类命名的缩写。
- 有些模块是上层应用程序若需要某项功能而必须使用的到的，某些未必。对于前者头文件名及目录名加上C2前缀以明示。后者（例如一些内部数学函数、mempool等），我们并不主动推荐使用，能够在C层面开发的人一般都有自己熟悉的一套库，不要给别人太多信息以及选择，未必是好事。当然作为功能辅助库，他要用也是可以的。无论怎样，我们的包间依赖关系都需要尽量低耦合。除了以上谈到的使用问题，还有一种情况是被扩展。
- 每个包有一个专门对外h文件，就好比com/combra/java/go里的接口思想一样，内部实现和对外接口的物理实现的形式可以没有关系。此h里定义好自己的依赖，用宏把h和lib装起来。详见Runtime/Meta/_defined.h。
