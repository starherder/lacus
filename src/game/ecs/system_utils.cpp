#include "system_utils.h"



namespace game
{



    SystemUtils::FuncFactorOpt SystemUtils::parseFightFunc(const std::string& str)
    {
        // hp-,hp+20,hp-2*,hp+30%,buf+bleed,buf-poison

        std::vector<FuncFactor> result;

        const auto& views = utility::StringUtil::split(str, ',');
        for (auto& svitem : views)
        {
            std::string sitem{ svitem.data(), svitem.size() };

            FuncFactor fac;
            size_t kpos = 0;
            size_t mpos = 0;
            size_t ppos = 0;

            for (size_t i = 0; i < sitem.size(); i++) {

                if (sitem[i] == '+' || sitem[i] == '-') {
                    kpos = i;
                    fac.operate = (sitem[i] == '+') ? FuncOperate::Plus : FuncOperate::Minus;
                }

                if (sitem[i] == '*') {
                    mpos = i;
                    fac.unit = FuncUnitType::Multi;
                }

                if (sitem[i] == '%') {
                    ppos = i;
                    fac.unit = FuncUnitType::Percent;
                }
            }

            if (kpos == 0) {
                return std::nullopt;
            }

            size_t vpos = 0;
            if (mpos != 0) {
                vpos = mpos;
            }
            else if (ppos != 0) {
                vpos = ppos;
            }
            else {
                vpos = sitem.size();
                fac.unit = FuncUnitType::Value;
            }

            fac.key = sitem.substr(0, kpos);

            std::string sval = sitem.substr(kpos + 1, vpos - kpos - 1);
            if (utility::StringUtil::is_number(sval)) {
                fac.fval = std::stof(sval);
            }
            else {
                fac.sval = sval;
            }

            result.push_back(fac);
        }

        return result;
    }



}