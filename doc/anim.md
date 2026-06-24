# 动画功能

帧动画、多个材质轮流播放形成动画效果

- engine目录中实现Animation类，Animation类内含Frame类，记录材质（texture）和此帧持续时间(duration)以及帧事件(event)
- Animation有成员变量记录了帧数据列表；有AddFrame方法，在指定索引添加帧；
- Animation类还有loop属性、rate属性及相关get\set方法； 有获取当前帧的材质的方法，有update(float deltatime)方法，此方法根据时间计算当前显示的材质。
- 同一个文件中有AnimationManager类，继承自IResManager，用于读取和管理Animation。
- ResourceManager类中会集成AnimationManager成员，并在自身的update方法里调用该成员的update，Application类中update ResourceManger.
- Animation有on_frame，on_frame_event信号(signals::Signal)，在update的过程中，每个帧持续时间结束就切换到下一帧，每帧刚开始的瞬间，触发on_frame，如果有帧事件，触发on_frame_event
- 在gameLogic类中，AnimationManger读取res/animations/目录下所有xml文件，读取每个动画的配置，tex\dur\evt分别是材质、时间和事件。
