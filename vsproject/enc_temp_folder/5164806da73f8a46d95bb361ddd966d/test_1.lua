

print("\n\t hello, this is lua \n")

function greeting_in_lua(name)
	print("+++++ hello "..", welcome to lua +++++")

end

greeting_in_cpp();



function test_warrior()
	print(">>>> test_warrior")
	local warrior = Warrior("zhang-sam", 10,20);
	local enemy = Warrior("li-sil", 30, 10);
	
	warrior:desc()
	enemy:desc()

	warrior:attack(enemy)
end




---------------------------------------------
test_warrior()