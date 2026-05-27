#pragma once

#include <variant>
#include <string>
#include <vector>
#include <cstring>
#include <cassert>

namespace utility
{
	enum class VariantType : int8_t
	{
		VT_Unknown,
		VT_Bool,
		VT_Int8,
		VT_UInt8,
		VT_Int16,
		VT_UInt16,
		VT_Int32,
		VT_UInt32,
		VT_Int64,
		VT_UInt64,
		VT_Float,
		VT_Double,
		VT_CString,
		VT_String,
	};

	class Var
	{
		using VariantClass = std::variant<
			bool,
			int8_t,
			uint8_t,
			int16_t,
			uint16_t,
			int32_t,
			uint32_t,
			int64_t,
			uint64_t,
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

		Var(int8_t c) : _variant(c)
		{
		}

		Var(uint8_t c) : _variant(c)
		{
		}

		Var(int16_t i) : _variant(i)
		{
		}

		Var(uint16_t i) : _variant(i)
		{
		}

		Var(int32_t i) : _variant(i)
		{
		}

		Var(uint32_t i) : _variant(i)
		{
		}

		Var(int64_t ii) : _variant(ii)
		{
		}

		Var(uint64_t sz) : _variant(sz)
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
			if (_variant.index() + 1 > (int)VariantType::VT_String)
			{
				return VariantType::VT_Unknown;
			}

			return VariantType(_variant.index() + 1);
		}

		size_t byteCount() const
		{
			size_t used_len = 1;

			switch (type())
			{
			case VariantType::VT_Bool: { used_len += sizeof(bool); } break;
			case VariantType::VT_Int8: { used_len += sizeof(int8_t); } break;
			case VariantType::VT_UInt8: { used_len += sizeof(uint8_t); } break;
			case VariantType::VT_Int16: { used_len += sizeof(int16_t); } break;
			case VariantType::VT_UInt16: { used_len += sizeof(uint16_t); } break;
			case VariantType::VT_Int32: { used_len += sizeof(int32_t); } break;
			case VariantType::VT_UInt32: { used_len += sizeof(uint32_t); } break;
			case VariantType::VT_Int64: { used_len += sizeof(int64_t); } break;
			case VariantType::VT_UInt64: { used_len += sizeof(uint64_t); } break;
			case VariantType::VT_Float: { used_len += sizeof(float); } break;
			case VariantType::VT_Double: { used_len += sizeof(double); } break;
			case VariantType::VT_CString: {
				auto str = std::get<const char*>(_variant);
				auto len = strlen(str);
				used_len += len + 1;
			}break;
			case VariantType::VT_String: {
				auto& str = std::get<std::string>(_variant);
				used_len += sizeof(int) + str.length();
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
			*buff = (char)type();

			buff++;

			size_t used_len = 1;

			switch (type())
			{
			case VariantType::VT_Bool: {
				bool res = std::get<bool>(_variant);
				*buff = (char)res;
				used_len += sizeof(bool);
			}break;
			case VariantType::VT_Int8: {
				auto val = std::get<int8_t>(_variant);
				std::memcpy(buff, &val, sizeof(val));
				used_len += sizeof(int8_t);
			}break;
			case VariantType::VT_UInt8: {
				auto val = std::get<uint8_t>(_variant);
				std::memcpy(buff, &val, sizeof(val));
				used_len += sizeof(uint8_t);
			}break;
			case VariantType::VT_Int16: {
				auto val = std::get<int16_t>(_variant);
				std::memcpy(buff, &val, sizeof(val));
				used_len += sizeof(int16_t);
			}break;
			case VariantType::VT_UInt16: {
				auto val = std::get<uint16_t>(_variant);
				std::memcpy(buff, &val, sizeof(val));
				used_len += sizeof(uint16_t);
			}break;
			case VariantType::VT_Int32: {
				auto val = std::get<int32_t>(_variant);
				std::memcpy(buff, &val, sizeof(val));
				used_len += sizeof(int32_t);
			}break;
			case VariantType::VT_UInt32: {
				auto val = std::get<uint32_t>(_variant);
				std::memcpy(buff, &val, sizeof(val));
				used_len += sizeof(uint32_t);
			}break;
			case VariantType::VT_Int64: {
				auto val = std::get<int64_t>(_variant);
				std::memcpy(buff, &val, sizeof(val));
				used_len += sizeof(int64_t);
			}break;
			case VariantType::VT_UInt64: {
				auto val = std::get<uint64_t>(_variant);
				std::memcpy(buff, &val, sizeof(val));
				used_len += sizeof(uint64_t);
			}break;
			case VariantType::VT_Float: {
				auto val = std::get<float>(_variant);
				std::memcpy(buff, &val, sizeof(val));
				used_len += sizeof(float);
			}break;
			case VariantType::VT_Double: {
				auto val = std::get<double>(_variant);
				std::memcpy(buff, &val, sizeof(val));
				used_len += sizeof(double);
			}break;
			case VariantType::VT_CString: {
				auto str = std::get<const char*>(_variant);
				auto len = strlen(str);
				std::memcpy(buff, str, len);
				*(buff + len) = 0;
				used_len += len + 1;
			}break;
			case VariantType::VT_String: {
				auto& str = std::get<std::string>(_variant);
				auto len = str.length();

				std::memcpy(buff, &len, sizeof(len));
				std::memcpy(buff + sizeof(len), str.data(), len);

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
			const char* data = str;

			char index = *data;
			const char* rawdata = ++data;

			size_t used = 1;

			switch ((VariantType)index)
			{
			case VariantType::VT_Bool: { _variant = (bool)(*rawdata); used += sizeof(char); } break;
			case VariantType::VT_Int8: { int8_t val; std::memcpy(&val, rawdata, sizeof(val)); _variant = val; used += sizeof(int8_t); } break;
			case VariantType::VT_UInt8: { uint8_t val; std::memcpy(&val, rawdata, sizeof(val)); _variant = val; used += sizeof(uint8_t); } break;
			case VariantType::VT_Int16: { int16_t val; std::memcpy(&val, rawdata, sizeof(val)); _variant = val; used += sizeof(int16_t); } break;
			case VariantType::VT_UInt16: { uint16_t val; std::memcpy(&val, rawdata, sizeof(val)); _variant = val; used += sizeof(uint16_t); } break;
			case VariantType::VT_Int32: { int32_t val; std::memcpy(&val, rawdata, sizeof(val)); _variant = val; used += sizeof(int32_t); } break;
			case VariantType::VT_UInt32: { uint32_t val; std::memcpy(&val, rawdata, sizeof(val)); _variant = val; used += sizeof(uint32_t); } break;
			case VariantType::VT_Int64: { int64_t val; std::memcpy(&val, rawdata, sizeof(val)); _variant = val; used += sizeof(int64_t); } break;
			case VariantType::VT_UInt64: { uint64_t val; std::memcpy(&val, rawdata, sizeof(val)); _variant = val; used += sizeof(uint64_t); } break;
			case VariantType::VT_Float: { float val; std::memcpy(&val, rawdata, sizeof(val)); _variant = val; used += sizeof(float); } break;
			case VariantType::VT_Double: { double val; std::memcpy(&val, rawdata, sizeof(val)); _variant = val; used += sizeof(double); } break;
			case VariantType::VT_CString: { _variant = rawdata; used += strlen(rawdata) + 1; } break;
			case VariantType::VT_String: {
				int len;
				std::memcpy(&len, rawdata, sizeof(len));
				const char* data = rawdata + sizeof(len);
				_variant = std::string(data, len);
				used += sizeof(int) + len;
			}break;
			default: return 0;
			}

			return used;
		}

		std::string toString() const
		{
			switch (type())
			{
			case VariantType::VT_Bool: return std::get<bool>(_variant) ? "true" : "false";
			case VariantType::VT_Int8: return std::to_string(std::get<int8_t>(_variant));
			case VariantType::VT_UInt8: return std::to_string(std::get<uint8_t>(_variant));
			case VariantType::VT_Int16: return std::to_string(std::get<int16_t>(_variant));
			case VariantType::VT_UInt16: return std::to_string(std::get<uint16_t>(_variant));
			case VariantType::VT_Int32: return std::to_string(std::get<int32_t>(_variant));
			case VariantType::VT_UInt32: return std::to_string(std::get<uint32_t>(_variant));
			case VariantType::VT_Int64: return std::to_string(std::get<int64_t>(_variant));
			case VariantType::VT_UInt64: return std::to_string(std::get<uint64_t>(_variant));
			case VariantType::VT_Float: return std::to_string(std::get<float>(_variant));
			case VariantType::VT_Double: return std::to_string(std::get<double>(_variant));
			case VariantType::VT_CString: return std::get<const char*>(_variant);
			case VariantType::VT_String: return std::get<std::string>(_variant);
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
			if (_vars.empty())
			{
				return *this;
			}
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

			const char* data = str.data();
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
