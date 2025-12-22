<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.11.2" name="wild" tilewidth="16" tileheight="16" spacing="1" tilecount="1767" columns="57">
 <properties>
  <property name="tileset_prop" value="1234567890"/>
 </properties>
 <image source="roguelike_wild.png" width="968" height="526"/>
 <tile id="0">
  <properties>
   <property name="prop_int" type="int" value="666"/>
   <property name="prop_str" value="water"/>
  </properties>
 </tile>
 <tile id="12">
  <properties>
   <property name="prop" value="ssssssssss"/>
   <property name="prop_int" value="222"/>
  </properties>
  <animation>
   <frame tileid="12" duration="300"/>
   <frame tileid="13" duration="300"/>
   <frame tileid="14" duration="300"/>
  </animation>
 </tile>
 <tile id="415">
  <animation>
   <frame tileid="415" duration="300"/>
   <frame tileid="417" duration="300"/>
   <frame tileid="416" duration="300"/>
  </animation>
 </tile>
 <tile id="469">
  <animation>
   <frame tileid="469" duration="300"/>
   <frame tileid="470" duration="300"/>
   <frame tileid="471" duration="300"/>
  </animation>
 </tile>
 <tile id="1251">
  <properties>
   <property name="prop_int" type="int" value="33"/>
   <property name="prop_obj" type="object" value="0"/>
  </properties>
 </tile>
 <wangsets>
  <wangset name="unamed" type="corner" tile="-1">
   <wangcolor name="" color="#ff0000" tile="-1" probability="1"/>
   <wangcolor name="water" color="#00ff00" tile="-1" probability="1"/>
   <wangcolor name="solid" color="#0000ff" tile="-1" probability="1"/>
   <wangtile tileid="2" wangid="0,0,0,2,0,0,0,0"/>
   <wangtile tileid="3" wangid="0,0,0,2,0,2,0,0"/>
   <wangtile tileid="4" wangid="0,0,0,0,0,2,0,0"/>
   <wangtile tileid="59" wangid="0,2,0,2,0,0,0,0"/>
   <wangtile tileid="61" wangid="0,0,0,0,0,2,0,2"/>
   <wangtile tileid="116" wangid="0,2,0,0,0,0,0,0"/>
   <wangtile tileid="117" wangid="0,2,0,0,0,0,0,2"/>
   <wangtile tileid="118" wangid="0,0,0,0,0,0,0,2"/>
   <wangtile tileid="520" wangid="0,0,0,3,0,0,0,0"/>
   <wangtile tileid="521" wangid="0,0,0,3,0,3,0,0"/>
   <wangtile tileid="522" wangid="0,0,0,0,0,3,0,0"/>
   <wangtile tileid="577" wangid="0,3,0,3,0,0,0,0"/>
   <wangtile tileid="579" wangid="0,0,0,0,0,3,0,3"/>
   <wangtile tileid="634" wangid="0,3,0,0,0,0,0,0"/>
   <wangtile tileid="635" wangid="0,3,0,0,0,0,0,3"/>
   <wangtile tileid="636" wangid="0,0,0,0,0,0,0,3"/>
  </wangset>
 </wangsets>
</tileset>
