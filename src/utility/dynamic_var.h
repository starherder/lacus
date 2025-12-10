#pragma once

#include <variant>
#include <string>
#include <vector>
#include <assert.h>

namespace utility
{
	enum class VariantType : char
	{
		VT_Unknow,
		VT_Bool,
		VT_Char,
		VT_Int,
		VT_Int64,
		VT_Size,
		VT_Float,
		VT_Double,
		VT_CString,
		VT_String,
	};

	class Var
	{
		using VariantClass = std::variant<
			bool,
			char,
			int,
			int64_t,
			size_t,
			float,
			double,
			const char*,
			std::string
		>;

		friend class VarList;

	public:
		Var()
		{
		}

		Var(bool b) : m_Variant(b)
		{
		}

		Var(char c) : m_Variant(c)
		{
		}

		Var(int i) : m_Variant(i)
		{
		}

		Var(int64_t ii) : m_Variant(ii)
		{
		}

		Var(size_t sz) : m_Variant(sz)
		{
		}

		Var(float f) : m_Variant(f)
		{
		}

		Var(double d) : m_Variant(d)
		{
		}

		Var(const char* cc) : m_Variant(cc)
		{
		}

		Var(const std::string& s) : m_Variant(s)
		{
		}

		Var(std::string& s) : m_Variant(s)
		{
		}

		Var& operator = (const Var& var)
		{
			m_Variant = var.m_Variant;
			return *this;
		}

		template<typename T>
		operator T() const
		{
			return convert<T>();
		}

		template<typename T>
		bool is_type() const
		{
			return std::get_if<T>(&m_Variant) != nullptr;
		}

		template<typename T>
		T convert() const
		{
			try {
				return std::get<T>(m_Variant);
			}
			catch (const std::exception& exp) {
				assert(false && exp.what());
				return T{};
			}
		}
		
		VariantType type() const
		{
			switch (m_Variant.index())
			{
			case 0: return VariantType::VT_Bool;
			case 1: return VariantType::VT_Char;
			case 2: return VariantType::VT_Int;
			case 3: return VariantType::VT_Int64;
			case 4: return VariantType::VT_Size;
			case 5: return VariantType::VT_Float;
			case 6: return VariantType::VT_Double;
			case 7: return VariantType::VT_CString;;
			case 8: return VariantType::VT_String;;
			default: return VariantType::VT_Unknow;
			}
		}
		
		size_t byteCount() const
		{
			size_t used_len = 1;

			switch (m_Variant.index())
			{
			case 0: { used_len += sizeof(bool); } break;
			case 1: { used_len += sizeof(char); } break;
			case 2: { used_len += sizeof(int); } break;
			case 3: { used_len += sizeof(int64_t); } break;
			case 4: { used_len += sizeof(size_t); } break;
			case 5: { used_len += sizeof(float); } break;
			case 6: { used_len += sizeof(double); } break;
			case 7: {
				auto str = std::get<const char*>(m_Variant);
				auto len = strlen(str);
				used_len += len + 1;
			}break;
			case 8: {
				auto str = std::get<std::string>(m_Variant);
				int len = (int)str.length();
				used_len += sizeof(int) + len;
			}break;
			default: {}break;
			}

			return used_len;
		}

		std::string toBytes() const
		{
			std::string result(byteCount(), 0);
			serialization(result.data());

			return result;
		}

		bool fromBytes(const std::string& str)
		{
			auto sz = deserialization(str.c_str());
			return sz == str.length();
		}

		size_t serialization(char* buff) const
		{
			*buff = (char)m_Variant.index();

			buff++;

			size_t used_len = 1;

			switch (m_Variant.index())
			{
			case 0: {
				bool res = std::get<bool>(m_Variant);
				*buff = (char)res;
				used_len += sizeof(bool);
			}break;
			case 1: {
				char c = std::get<char>(m_Variant);
				*buff = c;
				used_len += sizeof(char);
			}break;
			case 2: {
				*((int*)buff) = std::get<int>(m_Variant);
				used_len += sizeof(int);
			}break;
			case 3: {
				*((int64_t*)buff) = std::get<int64_t>(m_Variant);
				used_len += sizeof(int64_t);
			}break;
			case 4: {
				*((size_t*)buff) = std::get<size_t>(m_Variant);
				used_len += sizeof(size_t);
			}break;
			case 5: {
				*((float*)buff) = std::get<float>(m_Variant);
				used_len += sizeof(float);
			}break;
			case 6: {
				*((double*)buff) = std::get<double>(m_Variant);
				used_len += sizeof(double);
			}break;
			case 7: {
				auto str = std::get<const char*>(m_Variant);
				auto len = strlen(str);
				memcpy_s(buff, len, str, len);

				*(buff + len) = 0;
				used_len += len + 1;
			}break;
			case 8: {
				auto str = std::get<std::string>(m_Variant);
				int len = (int)str.length();

				int* idata = (int*)buff;
				*idata = (int)len;

				char* data = (char*)(idata + 1);
				memcpy_s(data, len, str.data(), len);

				used_len += sizeof(int) + len;
			}break;
			default:
			{
			}break;
			}

			return used_len;
		}

		size_t deserialization(const char* str)
		{
			char* data = (char*)str;

			char index = *data;
			char* rawdata = ++data;

			size_t used = 1;

			switch (index)
			{
			case 0: { m_Variant = (bool)(*rawdata); used += sizeof(char); } break;
			case 1: { m_Variant = *rawdata; used += sizeof(char); } break;
			case 2: { m_Variant = *((int*)(rawdata)); used += sizeof(int); } break;
			case 3: { m_Variant = *((int64_t*)(rawdata)); used += sizeof(int64_t); } break;
			case 4: { m_Variant = *((size_t*)(rawdata)); used += sizeof(size_t); } break;
			case 5: { m_Variant = *((float*)(rawdata)); used += sizeof(float); } break;
			case 6: { m_Variant = *((double*)(rawdata)); used += sizeof(double); } break;
			case 7: { m_Variant = rawdata; used += strlen(rawdata) + 1; } break;
			case 8: {
				auto szdata = (int*)rawdata;
				int len = *szdata;
				char* data = (char*)(szdata + 1);
				m_Variant = std::string(data, len);
				used += sizeof(int) + len;
			}break;
			default: return 0;
			}

			return used;
		}

		std::string toString() const
		{
			switch (m_Variant.index())
			{
			case 0: return std::get<bool>(m_Variant) ? "true" : "false";
			case 1: return std::to_string(std::get<char>(m_Variant));
			case 2: return std::to_string(std::get<int>(m_Variant));
			case 3: return std::to_string(std::get<int64_t>(m_Variant));
			case 4: return std::to_string(std::get<size_t>(m_Variant));
			case 5: return std::to_string(std::get<float>(m_Variant));
			case 6: return std::to_string(std::get<double>(m_Variant));
			case 7: return std::get<const char*>(m_Variant);
			case 8: return std::get<std::string>(m_Variant);
			default: return "[ERR_FMT]";
			}
		}

	private:
		VariantClass m_Variant;
	};



	///////////////////////////////////////////////////////////////////////////////////////////


	class VarList
	{
	public:
		using Iterator = std::vector<Var>::iterator;
		using ConstIterator = std::vector<Var>::const_iterator;

	public:
		VarList() {}
		VarList(const VarList& varlist) noexcept
			: m_Vars(varlist.m_Vars) {}
		VarList(VarList&& varlist) noexcept
			: m_Vars(std::move(varlist.m_Vars)) {}
		VarList(const std::vector<Var>& varlist) noexcept
			: m_Vars(varlist) {}
		VarList(std::vector<Var>&& varlist) noexcept
			: m_Vars(std::move(varlist)) {}
		VarList(std::initializer_list<Var>&& varlist) noexcept
			: m_Vars(std::move(varlist)) {}
		~VarList() {}

		Iterator begin()
		{
			return m_Vars.begin();
		}

		ConstIterator begin() const
		{
			return m_Vars.begin();
		}

		Iterator end()
		{
			return m_Vars.end();
		}

		ConstIterator end() const
		{
			return m_Vars.end();
		}

		void add(const Var& var)
		{
			m_Vars.push_back(var);
		}

		Var& operator [](size_t index)
		{
			return m_Vars[index];
		}

		const Var& operator [](size_t index) const
		{
			return m_Vars[index];
		}

		bool empty() const
		{
			return m_Vars.empty();
		}

		size_t size() const
		{
			return m_Vars.size();
		}

		VarList& operator << (const Var& var)
		{
			add(var);
			return *this;
		}

		VarList& operator >> (Var& var)
		{
			var = m_Vars.back();
			m_Vars.pop_back();
			return *this;
		}

		void clear()
		{
			m_Vars.clear();
		}

		size_t byteCount() const
		{
			size_t bytes = 0;
			for (auto& var : m_Vars)
			{
				bytes += var.byteCount();
			}
			return bytes;
		}

		std::string toBytes() const
		{
			size_t pos = 0;
			std::string result(byteCount(), 0);

			for (auto& var : m_Vars)
			{
				pos += var.serialization(result.data() + pos);
			}

			return result;
		}

		bool fromBytes(const std::string& str)
		{
			m_Vars.clear();

			char* data = (char*)str.data();
			size_t len = str.length();

			Var var;
			size_t pos = 0;

			while (pos < len)
			{
				pos += var.deserialization(data + pos);
				m_Vars.push_back(var);
			}

			return true;
		}

		std::string toString() const
		{
			std::string result = "[";

			for (auto& var : m_Vars)
			{
				result += var.toString();
				result += ",";
			}

			if (m_Vars.empty())
			{
				result += ']';
			}
			else
			{
				result[result.size() - 1] = ']';
			}

			return result;
		}

	private:
		std::vector<Var> m_Vars;
	};

}
