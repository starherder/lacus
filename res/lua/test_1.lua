

print("\n\t ------------- hello, this is lua -------------\n")

function greeting_in_lua(name)
	print("+++++ hello "..name..", welcome to lua +++++")
end

greeting_in_cpp();


function calc_add(a, b)
	return a + b
end

function test_warrior()
	print(">>>> test_warrior")
	local warrior = Warrior("zhang-sam", 10,20);
	local enemy = Warrior("li-sil", 30, 10);
	
	warrior:desc()
	enemy:desc()

	warrior:attack(enemy)
end

function calc_props(prop)

	print("prop: lv="..prop.lv..", str="..prop.str..", def="..prop.def)

	local com = CompFight()
	com.lv = prop.lv
	com.str = prop.str + prop.lv * 3
	com.def = prop.def + prop.lv * 4
	
	print("com: lv="..com.lv..", str="..com.str..", def="..com.def)

	return com
end


---------------------------------------------
test_warrior()