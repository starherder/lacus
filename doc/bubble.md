# 头顶气泡

头顶气泡是用于显示角色的状态、情绪、语言的弹出框，框内会显示图标、文字等；

调用显示气泡方法时，会在角色头顶显示一个气泡，显示参数传入的文字、表情图标等；持续一段时间淡出消失。

## 实现：

- 使用ECS模式，需要显示气泡时发送一个ECS事件。
- 在BubbleSystem的system类中，收到该事件时，给entity上加一个BubbleComponent组件，改组件上记录了气泡类型（语言、表情），语言需要配置文字、表情需要配置图标
- BubbleComponent中的语言对应的文字要使用译文系统(Trans宏)，表情要使用BubbleManager
- 在BubbleSystem的update方法中定时淡出气泡（气泡的存在、淡出时间在GameConfig里配置）。
- 在RenderSystem中增加绘制气泡及其中文字和表情的方法。

## 配置	

- 文字使用译文，通过Trans宏来获取实际的文字
- 增加个表情管理器EmotionManager，用于读取res/data/emotion/emotions.json，提供接口根据名字获取材质

## 测试

在ImFormDebug中像drawSkyWindow一样，遍历所有表情管理器中的表情，写入Combobox，选择表情，即给已选择的角色发送显示气泡事件，让他显示气泡
