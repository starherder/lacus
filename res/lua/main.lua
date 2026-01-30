
log_info("-------------- lua main --------------")

-- 
function get_levelup_exp(lv)
	return lv*100 + 10
end 

function get_total_exp(lv)

    local total = 0
    for l = 1, lv do
        total = total + get_levelup_exp(l);
    end

    return total
end

function get_kill_award_exp(enemy_lv)
	return get_total_exp(enemy_lv) / 2
end


function get_level_baseprop(props)

    --log_info("props: lv="..props.lv.." exp="..props.exp.." cst="..props.cst.." str="..props.str)

    local result = BaseProps()

    result.lv = props.lv
    result.exp = props.exp
    result.cst = props.cst + props.lv * 10
    result.str = props.str + props.lv * 10
    result.dex = props.dex + props.lv * 10
    result.met = props.met + props.lv * 10

    --log_info("result: lv="..result.lv.." exp="..result.exp.." cst="..result.cst.." str="..result.str)
    return result
end


function get_fight_prop(base)

    local fight = FightProps()

    fight.hpm = base.cst * 10; -- hp max
    fight.hpr = base.dex / 10; -- hp increase ratio

    fight.atk = base.cst / 3 + base.str;
    fight.def = base.cst + base.str / 3;
    fight.mvs = base.dex;
    fight.ats = base.dex;
    fight.atd = base.dex + base.str;
    fight.crt = base.dex / 10000;
    fight.par = base.dex / 10000;

    --fight.xxx += buf.xxx // buf
    --fight.xxx += equip.xxx // ×°±¸
    --fight.xxx + carrier.xxx // ÔØ¾ß

    return fight
end