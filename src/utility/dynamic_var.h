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

		Var(bool b) : _variant(b)
		{
		}

		Var(char c) : _variant(c)
		{
		}

		Var(int i) : _variant(i)
		{
		}

		Var(int64_t ii) : _variant(ii)
		{
		}

		Var(size_t sz) : _variant(sz)
		{
		}

		Var(float f) : _variant(f)
		{
		}

		Var(double d) : _variant(d)
		{
		}

		Var(const char* cc) : _variant(cc)
		{
		}

		Var(const std::string& s) : _variant(s)
		{
		}

		Var(std::string& s) : _variant(s)
		{
		}

		Var(const Var& var) : _variant(var._variant)
		{
		}

		Var(Var&& var) noexcept : _variant(std::move(var._variant))
		{
		}

		Var& operator = (const Var& var)
		{
			_variant = var._variant;
			return *this;
		}

		Var& operator = (Var&& var) noexcept
		{
			_variant = std::move(var._variant);
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
			return std::get_if<T>(&_variant) != nullptr;
		}

		template<typename T>
		T convert() const
		{
			try {
				return std::get<T>(_variant);
			}
			catch (const std::exception& exp) {
				assert(false && exp.what());
				return T{};
			}
		}
		
		VariantType type() const
		{
			switch (_variant.index())
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

			switch (_variant.index())
			{
			case 0: { used_len += sizeof(bool); } break;
			case 1: { used_len += sizeof(char); } break;
			case 2: { used_len += sizeof(int); } break;
			case 3: { used_len += sizeof(int64_t); } break;
			case 4: { used_len += sizeof(size_t); } break;
			case 5: { used_len += sizeof(float); } break;
			case 6: { used_len += sizeof(double); } break;
			case 7: {
				auto str = std::get<const char*>(_variant);
				auto len = strlen(str);
				used_len += len + 1;
			}break;
			case 8: {
				auto str = std::get<std::string>(_variant);
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
			*buff = (char)_variant.index();

			buff++;

			size_t used_len = 1;

			switch (_variant.index())
			{
			case 0: {
				bool res = std::get<bool>(_variant);
				*buff = (char)res;
				used_len += sizeof(bool);
			}break;
			case 1: {
				char c = std::get<char>(_variant);
				*buff = c;
				used_len += sizeof(char);
			}break;
			case 2: {
				*((int*)buff) = std::get<int>(_variant);
				used_len += sizeof(int);
			}break;
			case 3: {
				*((int64_t*)buff) = std::get<int64_t>(_variant);
				used_len += sizeof(int64_t);
			}break;
			case 4: {
				*((size_t*)buff) = std::get<size_t>(_variant);
				used_len += sizeof(size_t);
			}break;
			case 5: {
				*((float*)buff) = std::get<float>(_variant);
				used_len += sizeof(float);
			}break;
			case 6: {
				*((double*)buff) = std::get<double>(_variant);
				used_len += sizeof(double);
			}break;
			case 7: {
				auto str = std::get<const char*>(_variant);
				auto len = strlen(str);
				memcpy_s(buff, len, str, len);

				*(buff + len) = 0;
				used_len += len + 1;
			}break;
			case 8: {
				auto str = std::get<std::string>(_variant);
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
			case 0: { _variant = (bool)(*rawdata); used += sizeof(char); } break;
			case 1: { _variant = *rawdata; used += sizeof(char); } break;
			case 2: { _variant = *((int*)(rawdata)); used += sizeof(int); } break;
			case 3: { _variant = *((int64_t*)(rawdata)); used += sizeof(int64_t); } break;
			case 4: { _variant = *((size_t*)(rawdata)); used += sizeof(size_t); } break;
			case 5: { _variant = *((float*)(rawdata)); used += sizeof(float); } break;
			case 6: { _variant = *((double*)(rawdata)); used += sizeof(double); } break;
			case 7: { _variant = rawdata; used += strlen(rawdata) + 1; } break;
			case 8: {
				auto szdata = (int*)rawdata;
				int len = *szdata;
				char* data = (char*)(szdata + 1);
				_variant = std::string(data, len);
				used += sizeof(int) + len;
			}break;
			default: return 0;
			}

			return used;
		}

		std::string toString() const
		{
			switch (_variant.index())
			{
			case 0: return std::get<bool>(_variant) ? "true" : "false";
			case 1: return std::to_string(std::get<char>(_variant));
			case 2: return std::to_string(std::get<int>(_variant));
			case 3: return std::to_string(std::get<int64_t>(_variant));
			case 4: return std::to_string(std::get<size_t>(_variant));
			case 5: return std::to_string(std::get<float>(_variant));
			case 6: return std::to_string(std::get<double>(_variant));
			case 7: return std::get<const char*>(_variant);
			case 8: return std::get<std::string>(_variant);
			default: return "[ERR_FMT]";
			}
		}

	private:
		VariantClass _variant;
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
			: _vars(varlist._vars) {}
		VarList(VarList&& varlist) noexcept
			: _vars(std::move(varlist._vars)) {}
		VarList(const std::vector<Var>& varlist) noexcept
			: _vars(varlist) {}
		VarList(std::vector<Var>&& varlist) noexcept
			: _vars(std::move(varlist)) {}
		VarList(std::initializer_list<Var>&& varlist) noexcept
			: _vars(std::move(varlist)) {}
		~VarList() {}

		Iterator begin()
		{
			return _vars.begin();
		}

		ConstIterator begin() const
		{
			return _vars.begin();
		}

		Iterator end()
		{
			return _vars.end();
		}

		ConstIterator end() const
		{
			return _vars.end();
		}

		void add(const Var& var)
		{
			_vars.push_back(var);
		}

		Var& operator [](size_t index)
		{
			return _vars[index];
		}

		const Var& operator [](size_t index) const
		{
			return _vars[index];
		}

		bool empty() const
		{
			return _vars.empty();
		}

		size_t size() const
		{
			return _vars.size();
		}

		VarList& operator << (const Var& var)
		{
			add(var);
			return *this;
		}

		VarList& operator >> (Var& var)
		{
			var = _vars.back();
			_vars.pop_back();
			return *this;
		}

		void clear()
		{
			_vars.clear();
		}

		size_t byteCount() const
		{
			size_t bytes = 0;
			for (auto& var : _vars)
			{
				bytes += var.byteCount();
			}
			return bytes;
		}

		std::string toBytes() const
		{
			size_t pos = 0;
			std::string result(byteCount(), 0);

			for (auto& var : _vars)
			{
				pos += var.serialization(result.data() + pos);
			}

			return result;
		}

		bool fromBytes(const std::string& str)
		{
			_vars.clear();

			char* data = (char*)str.data();
			size_t len = str.length();

			Var var;
			size_t pos = 0;

			while (pos < len)
			{
				pos += var.deserialization(data + pos);
				_vars.push_back(var);
			}

			return true;
		}

		std::string toString() const
		{
			std::string result = "[";

			for (auto& var : _vars)
			{
				result += var.toString();
				result += ",";
			}

			if (_vars.empty())
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
		std::vector<Var> _vars;
	};

}
