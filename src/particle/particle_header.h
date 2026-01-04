#pragma once


#include "engine/wrapper.h"


namespace particle
{
	using namespace engine;

	using Color4f = Color;

#define fail_return(exp) if (!(exp)) return;
#define fail_return_result(exp, res) if(!(exp)) return (res);
#define fail_return_zero(exp) if(!(exp)) return 0;
#define fail_return_false(exp) if(!(exp)) return false;
#define fail_return_null(exp) if(!(exp)) return nullptr;
#define fail_continue(exp) if(!(exp)) continue;
#define fail_break(exp) if(!(exp)) break;

#define safe_new new
#define safe_delete(ptr) if((ptr)) {delete (ptr);(ptr)=nullptr;}
#define safe_free(p) if(p){free(p);p=nullptr;}


#define DefineGetter(vtype, name, mem) \
vtype Get##name() {return mem;}							

#define DefineSetter(vtype, name, mem) \
void Set##name(const vtype& val){mem=val;}			

#define DefineGetterSetter(vtype, name, mem)	DefineGetter(vtype, name, mem); DefineSetter(vtype, name, mem);

#define MemberFunction(type, funcname, valname)		\
	public:												\
	void Set##funcname(type val) { valname = val; }		\
	type Get##funcname() const { return valname; }		\
	protected:											\
	type valname

#define VirtualMemberFunction(type, funcname, valname)			\
	public:														\
	virtual void Set##funcname(type val) { valname = val; }		\
	virtual type Get##funcname() const { return valname; }		\
	protected:													\
	type valname


	static float FloatEqual(float lval, float rval, float epsilon = 0.000001)
	{
		return lval >= (rval - epsilon) && lval <= (rval + epsilon);
	}

	inline float DegreeToRadian(float angle)
	{
		return angle * 0.0174532925f;
	}

	inline float RadianToDegree(float radian)
	{
		return radian * 57.29577951f;
	}


	using StringStringMap = std::map<std::string, std::string>;






}