/*
* 公共对象基类
* 在此基础上实现动态事件机制
*/

#ifndef TObjectH
#define TObjectH

/**
* @defgroup 宏定义
* @{
*/
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x500
#endif

#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <windows.h>


#ifdef __BORLANDC__
#ifdef __DLL__
#define __EXPORT__  __declspec(dllexport)
#else
#define __EXPORT__  __declspec(dllimport)
#endif
#else
#ifdef _DLL
#define __EXPORT__  __declspec(dllexport)
#else
#define __EXPORT__  __declspec(dllimport)
#endif
#endif

#ifndef _DEBUG
#define __ALWAYS_INLINE	inline
#else
#define __ALWAYS_INLINE
#endif


/// 结构异常指针，此处省略
#define DECL_EXCEPTION_POINTERS()
#define ASSERT(x)
#define __TRY try
#define __EXCEPT() \
	catch(...) \
	{ }


/**
* @name 属性定义宏
* @{
*/
#define INIT_PROPERTY(name, val)    m_##name = (val);

#define INIT_PROP(name, val)    INIT_PROPERTY(name, val)

#define DECL_PROPERTY(type, property)						\
	private:												\
		/**　属性 */										\
		type	m_##property;								\
	public:													\
		/** 属性的set方法 */								\
		void set##property(type value)						\
		{													\
			if ( m_##property != value ) m_##property = value;\
		}													\
		/** 属性的get方法 */								\
		type get##property()								\
		{													\
			return m_##property;								\
		}

#define DECL_PROP(type, property) DECL_PROPERTY(type, property)

#define DECL_PROPERTY_RO(type, property)					\
	private:												\
		/**　属性 */										\
		type	m_##property;								\
	private:												\
		/** 属性的set方法 */								\
        void set##property(type value)						\
		{													\
			if ( m_##property != value ) m_##property = value;\
		}													\
    public:													\
		/** 属性的get方法 */								\
		type get##property()								\
		{													\
			return m_##property;								\
		}

#define DECL_PROP_RO(type, property) DECL_PROPERTY_RO(type, property)

#define DECL_LOCK_PROPERTY(type, property)					\
	private:												\
		/**　带临界区锁的属性 */							\
		TLockVar<type>	m_##property;						\
	public:													\
		void set##property(type value)						\
		{													\
			if ( m_##property != value ) m_##property = value;\
		}													\
		type get##property()								\
		{													\
			return m_##property;								\
		}

#define DECL_LOCK_PROP(type, property) DECL_LOCK_PROPERTY(type, property)

#define DECL_LOCK_PROPERTY_RO(type, property)				\
	private:												\
		/**　只读的带临界区锁的属性 */						\
		TLockVar<type>	m_##property;						\
	private:												\
        void set##property(type value)						\
		{													\
			if ( m_##property != value ) m_##property = value;\
		}													\
    public:													\
		type get##property() const							\
		{													\
			return m_##property;								\
		}

#define DECL_LOCK_PROP_RO(type, property) DECL_LOCK_PROPERTY_RO(type, property)

#define DECL_PROPERTY_INT(type, property)					\
	private:												\
		/**　私有属性 */									\
		##type	m_##property;								\
															\
		void set##property(type value)						\
		{													\
			if ( m_##property != value ) m_##property = value;\
		}													\
                                                            \
		##type& get##property()								\
		{													\
			return m_##property;								\
		}

#define DECL_PROP_INT(type, property)   DECL_PROPERTY_INT(type, property)

// property proxy
#define DECL_PROPERTY_PROXY(type, prop, dstobj, prototype)			\
	public:															\
		/**　属性代理的set方法 */									\
		void set##prop(type value)									\
		{															\
			ASSERT(get##dstobj());									\
			get##dstobj()->set##prototype(value);					\
		}															\
		/** 属性代理的get方法*/										\
		type get##prop()											\
		{															\
			ASSERT(get##dstobj());									\
			return get##dstobj()->get##prototype();					\
		}

#define DECL_PROP_PROXY(type, prop, dstobj, prototype)  DECL_PROPERTY_PROXY(type, prop, dstobj, prototype)

#define DECL_PROPERTY_PROXY_RO(type, prop, dstobj, prototype)		\
	public:															\
		/** 只读属性代理的get方法 */								\
		type get##prop()											\
		{															\
			ASSERT(get##dstobj());									\
			return get##dstobj()->get##prototype();					\
		}

#define DECL_PROP_PROXY_RO(type, prop, dstobj, prototype)   DECL_PROPERTY_PROXY_RO(type, prop, dstobj, prototype)

#define DECL_PROPERTY_PROXY_WO(type, prop, dstobj, prototype)	\
	public: \
    	/** 只写属性代理的get方法 */							\
        void set##prop(type val) \
        { \
            get##dstobj()->set##prototype(val); \
        }

#define DECL_PROP_PROXY_WO(type, prop, dstobj, prototype) DECL_PROPERTY_PROXY_WO(type, prop, dstobj, prototype)

#define DECL_LOCK_PROPERTY_PROXY(type, prop, dstobj, prototype)	\
	private:													\
		/** 具有临界区锁的属性代理 */							\
		TLockVar<type> m_##prop;									\
	public:														\
		/** 具有临界区锁的属性代理的set方法*/					\
		void set##prop(type value)								\
		{														\
			if ( m_##prop != value )								\
			{													\
				ASSERT(get##dstobj());							\
				get##dstobj()->set##prototype(value);			\
				m_##prop = get##dstobj()->get##prototype();		\
			}													\
		}														\
		/** 具有临界区锁的属性代理的get方法*/					\
		type get##prop()										\
		{														\
			return get##dstobj()->get##prototype();				\
		}

#define DECL_LOCK_PROP_PROXY(type, prop, dstobj, prototype) DECL_LOCK_PROPERTY_PROXY(type, prop, dstobj, prototype)


#define DECL_LOCK_PROPERTY_PROXY_CONDITION(type, prop, dstobj, prorotype, condition)	\
	private:																			\
		/** 具有临界锁、条件判断的属性代理*/											\
		TLockVar<type> m_##prop;															\
	public:																				\
		/** 具有临界锁、条件判断的属性代理的set方法*/									\
		void set##prop(type value)														\
		{																				\
			if ( m_##prop != value )														\
			{																			\
				if ( (condition) )														\
				{																		\
					ASSERT(get##dstobj());												\
					get##dstobj()->set##prorotype(value);								\
					m_##prop = get##dstobj()->get##prorotype();							\
				}																		\
				else																	\
				{																		\
					m_##prop = value;													\
				}																		\
			}																			\
		}																				\
		/** 具有临界锁、条件判断的属性代理的get方法*/									\
		type get##prop()																\
		{																				\
			return m_##prop;																\
		}

#define DECL_LOCK_PROP_PROXY_CONDITION(type, prop, dstobj, prorotype, condition)    DECL_LOCK_PROPERTY_PROXY_CONDITION(type, prop, dstobj, prorotype, condition)

#define DECL_PROPERTY_PROTECTED(type, property)				\
	protected:												\
		/**　属性 */										\
		type	m_##property;								\
	public:													\
		/** 属性的set方法 */								\
		void set##property(type value)						\
		{													\
			if ( m_##property != value ) m_##property = value;\
		}													\
		/** 属性的get方法 */								\
		type get##property()								\
		{													\
			return m_##property;								\
		}

#define DECL_PROP_PROTECTED(type, property) DECL_PROPERTY_PROTECTED(type, property)

/**
* @}
*/ // name

/**
* @}
*/ // defgroup
namespace igame
{
    /**
	* @class TObject
	* @brief 所有对象的基类
	*/
	class TObject
	{
	public:
		/// ctor
		TObject() { }

		/// dtor
		virtual ~TObject() { }
	};
};

#endif