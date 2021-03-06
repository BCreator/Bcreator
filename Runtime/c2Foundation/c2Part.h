#ifndef C2_PART_H_
#define C2_PART_H_

#include<assert.h>
#include<string>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include<hash_map>
#include<mutex>
#include<boost/smart_ptr/intrusive_ptr.hpp>
#include<boost/atomic.hpp>

#define USE_intrusive_ptr

////////////////////////////////////////////////////////////////////////////////

#define C2DefineClass(classname)	\
		public:\
			static c2Part* _create() {\
				return new classname;\
			}

/******************************************************************************/
//除了继承外，c2Part不可以在用户态使用。
//TODO： 只是一个临时的原型级实现，方便往下进行项目其他方面的原型建模。1）线程安全并不
//完备，原子操作的lockfree并不完备。2）自我约束也还不完备。例如还需要让更严谨的做法是
//把赋值等也都私有化。“还应该只申明，不定义其实现方法”？
class c2Part {
public:
	using ARPart = boost::intrusive_ptr<c2Part>;
	C2DefineClass(c2Part)
private:
	typedef	int			GUID;		//FIXME
	GUID _GUID;						//？既然对象名称有关，通过对象名称字符串生成？
	/*------------------------------------------------------------------------*/
private:
	mutable boost::atomic<int>	_Ref;
	friend void			intrusive_ptr_add_ref(const c2Part *pP) {
		if (!pP)			return;
		pP->_Ref.fetch_add(1, boost::memory_order_relaxed);
	}
	friend void			intrusive_ptr_release(const c2Part *pP)
	{
		if (!pP)			return;
		if (pP->_Ref.fetch_sub(1, boost::memory_order_release) == 1) {
			boost::atomic_thread_fence(boost::memory_order_acquire);
			delete pP;
		}
	}
protected:
	explicit c2Part() : _Ref(0), _GUID(0) {}
	virtual ~c2Part() { _Ref = 0; _GUID = 0; }
	c2Part(c2Part &&other) = delete;
	//c2Part& operator=(c2Part &&rhs);	//TODO
	/*------------------------------------------------------------------------*/
public:
	using CreationFunc		= c2Part* (*)();
	typedef stdext::hash_map<std::string, CreationFunc>	CreationDict;
	static CreationDict		_CreationDict;
	//friend bool _c2RegistPartClass(const char *sClass, c2Part::CreationFunc C);
	//friend ARPart c2CreatePart(const char *sClass, const char *sName = nullptr);
/*============================================================================*/
private:
	std::vector<ARPart>		_Parts;
};


////////////////////////////////////////////////////////////////////////////////
#endif//C2_PART_H_
