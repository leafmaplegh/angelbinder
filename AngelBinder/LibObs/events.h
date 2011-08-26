#pragma once

/************************************************************************

Copyright (c) 2010, Jo�o Francisco Biondo Trinca
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright 
notice, this list of conditions and the following disclaimer in the 
documentation and/or other materials provided with the distribution.

* Neither the name of DarK TeaM Softwares nor the names of its 
contributors may be used to endorse or promote products derived from 
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.

************************************************************************/

/**
* Great thanks to Magnus Norddahl for its implementation on ClanLib.org
* this module is all based on his work there.
**/

#include <vector>
#include <memory>

#include <LibObs/Delegates.h>
#include <LibObs/Observers.h>



/**
* This is the base event, which means that all Event_vX inherits this 
* class to get access to the delegate list, and its cleanup algorithm
**/
class EventBase
{
public:
	typedef std::vector<std::shared_ptr<ObserverHolder> >	ObserverList;
	typedef ObserverList::size_type							ObserverListSize;
	typedef ObserverList::iterator							ObserverListIt;

protected:
	EventBase()
	{
	}

	void cleanup()
	{
		EventBase::ObserverListSize i, size = this->observers.size();
		for(i = 0; i < size; i++)
		{
			if(!this->observers[i]->get_delegate()->isValid())
			{
				this->observers.erase(this->observers.begin()+i);
				i--; size--;
			}
		}
	}

	EventBase::ObserverList observers;

};

//// ######## GENERATED BY SCRIPT - START ######## ////

/**************************************************************************
* Call dispatcher for functions with 0 arguments          
**************************************************************************/

/**
* Event class for 0 arguments
**/
class Event_v0
	: public EventBase
{
public:
	Event_v0() { }
	~Event_v0() { }


	Observer connect ( void (*function)() )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v0_StaticCall ( function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Data>
	Observer connect ( void (*function)(Data), const Data& data )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v0_StaticUserCall<Data> ( function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class>
	Observer connect ( Class* instance, void (Class::*function)())
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v0_MemberCall<Class> ( instance, function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class, class Data>
	Observer connect ( Class* instance, void (Class::*function)(Data), const Data& data)
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v0_MemberUserCall<Class, Data> ( instance, function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}


	void invoke (  )
	{
		EventBase::ObserverList del_list = this->observers;
		EventBase::ObserverListSize i, size = del_list.size();
		for(i = 0; i < size; i++)
		{
			if(del_list[i]->get_delegate()->isValid() && del_list[i]->get_delegate()->isEnabled())
			{
				((Delegate_v0_Generic  *)del_list[i].get()->get_delegate_ptr())->invoke();
			}
		}
	}

};

/**************************************************************************
* Call dispatcher for functions with 1 arguments          
**************************************************************************/

/**
* Event class for 1 arguments
**/
template<class A1>
class Event_v1
	: public EventBase
{
public:
	Event_v1() { }
	~Event_v1() { }

	Observer connect ( void (*function)(A1 a1) )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v1_StaticCall<A1> ( function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Data>
	Observer connect ( void (*function)(A1 a1, Data), const Data& data )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v1_StaticUserCall<A1, Data> ( function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1))
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v1_MemberCall<Class, A1> ( instance, function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class, class Data>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, Data), const Data& data)
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v1_MemberUserCall<Class, A1, Data> ( instance, function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	void invoke ( A1 a1 )
	{
		EventBase::ObserverList del_list = this->observers;
		EventBase::ObserverListSize i, size = del_list.size();
		for(i = 0; i < size; i++)
		{
			if(del_list[i]->get_delegate_ptr()->isValid() && del_list[i]->get_delegate_ptr()->isEnabled())
			{
				((Delegate_v1_Generic <A1> *)del_list[i].get()->get_delegate_ptr())->invoke(a1);
			}
		}
	}

};

/**************************************************************************
* Call dispatcher for functions with 2 arguments          
**************************************************************************/

/**
* Event class for 2 arguments
**/
template<class A1, class A2>
class Event_v2
	: public EventBase
{
public:
	Event_v2() { }
	~Event_v2() { }

	Observer connect ( void (*function)(A1 a1, A2 a2) )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v2_StaticCall<A1, A2> ( function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Data>
	Observer connect ( void (*function)(A1 a1, A2 a2, Data), const Data& data )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v2_StaticUserCall<A1, A2, Data> ( function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2))
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v2_MemberCall<Class, A1, A2> ( instance, function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class, class Data>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, Data), const Data& data)
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v2_MemberUserCall<Class, A1, A2, Data> ( instance, function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	void invoke ( A1 a1, A2 a2 )
	{
		EventBase::ObserverList del_list = this->observers;
		EventBase::ObserverListSize i, size = del_list.size();
		for(i = 0; i < size; i++)
		{
			if(del_list[i]->get_delegate_ptr()->isValid() && del_list[i]->get_delegate_ptr()->isEnabled())
			{
				((Delegate_v2_Generic <A1, A2> *)del_list[i].get()->get_delegate_ptr())->invoke(a1, a2);
			}
		}
	}

};

/**************************************************************************
* Call dispatcher for functions with 3 arguments          
**************************************************************************/

/**
* Event class for 3 arguments
**/
template<class A1, class A2, class A3>
class Event_v3
	: public EventBase
{
public:
	Event_v3() { }
	~Event_v3() { }

	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3) )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v3_StaticCall<A1, A2, A3> ( function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Data>
	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, Data), const Data& data )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v3_StaticUserCall<A1, A2, A3, Data> ( function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3))
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v3_MemberCall<Class, A1, A2, A3> ( instance, function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class, class Data>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, Data), const Data& data)
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v3_MemberUserCall<Class, A1, A2, A3, Data> ( instance, function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	void invoke ( A1 a1, A2 a2, A3 a3 )
	{
		EventBase::ObserverList del_list = this->observers;
		EventBase::ObserverListSize i, size = del_list.size();
		for(i = 0; i < size; i++)
		{
			if(del_list[i]->get_delegate_ptr()->isValid() && del_list[i]->get_delegate_ptr()->isEnabled())
			{
				((Delegate_v3_Generic <A1, A2, A3> *)del_list[i].get()->get_delegate_ptr())->invoke(a1, a2, a3);
			}
		}
	}

};

/**************************************************************************
* Call dispatcher for functions with 4 arguments          
**************************************************************************/

/**
* Event class for 4 arguments
**/
template<class A1, class A2, class A3, class A4>
class Event_v4
	: public EventBase
{
public:
	Event_v4() { }
	~Event_v4() { }

	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, A4 a4) )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v4_StaticCall<A1, A2, A3, A4> ( function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Data>
	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, A4 a4, Data), const Data& data )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v4_StaticUserCall<A1, A2, A3, A4, Data> ( function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, A4 a4))
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v4_MemberCall<Class, A1, A2, A3, A4> ( instance, function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class, class Data>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, A4 a4, Data), const Data& data)
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v4_MemberUserCall<Class, A1, A2, A3, A4, Data> ( instance, function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	void invoke ( A1 a1, A2 a2, A3 a3, A4 a4 )
	{
		EventBase::ObserverList del_list = this->observers;
		EventBase::ObserverListSize i, size = del_list.size();
		for(i = 0; i < size; i++)
		{
			if(del_list[i]->get_delegate_ptr()->isValid() && del_list[i]->get_delegate_ptr()->isEnabled())
			{
				((Delegate_v4_Generic <A1, A2, A3, A4> *)del_list[i].get()->get_delegate_ptr())->invoke(a1, a2, a3, a4);
			}
		}
	}

};

/**************************************************************************
* Call dispatcher for functions with 5 arguments          
**************************************************************************/

/**
* Event class for 5 arguments
**/
template<class A1, class A2, class A3, class A4, class A5>
class Event_v5
	: public EventBase
{
public:
	Event_v5() { }
	~Event_v5() { }

	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v5_StaticCall<A1, A2, A3, A4, A5> ( function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Data>
	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, Data), const Data& data )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v5_StaticUserCall<A1, A2, A3, A4, A5, Data> ( function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5))
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v5_MemberCall<Class, A1, A2, A3, A4, A5> ( instance, function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class, class Data>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, Data), const Data& data)
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v5_MemberUserCall<Class, A1, A2, A3, A4, A5, Data> ( instance, function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	void invoke ( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
	{
		EventBase::ObserverList del_list = this->observers;
		EventBase::ObserverListSize i, size = del_list.size();
		for(i = 0; i < size; i++)
		{
			if(del_list[i]->get_delegate_ptr()->isValid() && del_list[i]->get_delegate_ptr()->isEnabled())
			{
				((Delegate_v5_Generic <A1, A2, A3, A4, A5> *)del_list[i].get()->get_delegate_ptr())->invoke(a1, a2, a3, a4, a5);
			}
		}
	}

};

/**************************************************************************
* Call dispatcher for functions with 6 arguments          
**************************************************************************/

/**
* Event class for 6 arguments
**/
template<class A1, class A2, class A3, class A4, class A5, class A6>
class Event_v6
	: public EventBase
{
public:
	Event_v6() { }
	~Event_v6() { }

	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v6_StaticCall<A1, A2, A3, A4, A5, A6> ( function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Data>
	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, Data), const Data& data )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v6_StaticUserCall<A1, A2, A3, A4, A5, A6, Data> ( function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6))
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v6_MemberCall<Class, A1, A2, A3, A4, A5, A6> ( instance, function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class, class Data>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, Data), const Data& data)
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v6_MemberUserCall<Class, A1, A2, A3, A4, A5, A6, Data> ( instance, function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	void invoke ( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
	{
		EventBase::ObserverList del_list = this->observers;
		EventBase::ObserverListSize i, size = del_list.size();
		for(i = 0; i < size; i++)
		{
			if(del_list[i]->get_delegate_ptr()->isValid() && del_list[i]->get_delegate_ptr()->isEnabled())
			{
				((Delegate_v6_Generic <A1, A2, A3, A4, A5, A6> *)del_list[i].get()->get_delegate_ptr())->invoke(a1, a2, a3, a4, a5, a6);
			}
		}
	}

};

/**************************************************************************
* Call dispatcher for functions with 7 arguments          
**************************************************************************/

/**
* Event class for 7 arguments
**/
template<class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Event_v7
	: public EventBase
{
public:
	Event_v7() { }
	~Event_v7() { }

	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v7_StaticCall<A1, A2, A3, A4, A5, A6, A7> ( function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Data>
	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, Data), const Data& data )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v7_StaticUserCall<A1, A2, A3, A4, A5, A6, A7, Data> ( function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7))
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v7_MemberCall<Class, A1, A2, A3, A4, A5, A6, A7> ( instance, function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class, class Data>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, Data), const Data& data)
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v7_MemberUserCall<Class, A1, A2, A3, A4, A5, A6, A7, Data> ( instance, function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	void invoke ( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 )
	{
		EventBase::ObserverList del_list = this->observers;
		EventBase::ObserverListSize i, size = del_list.size();
		for(i = 0; i < size; i++)
		{
			if(del_list[i]->get_delegate_ptr()->isValid() && del_list[i]->get_delegate_ptr()->isEnabled())
			{
				((Delegate_v7_Generic <A1, A2, A3, A4, A5, A6, A7> *)del_list[i].get()->get_delegate_ptr())->invoke(a1, a2, a3, a4, a5, a6, a7);
			}
		}
	}

};

/**************************************************************************
* Call dispatcher for functions with 8 arguments          
**************************************************************************/

/**
* Event class for 8 arguments
**/
template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class Event_v8
	: public EventBase
{
public:
	Event_v8() { }
	~Event_v8() { }

	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v8_StaticCall<A1, A2, A3, A4, A5, A6, A7, A8> ( function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Data>
	Observer connect ( void (*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, Data), const Data& data )
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v8_StaticUserCall<A1, A2, A3, A4, A5, A6, A7, A8, Data> ( function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8))
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v8_MemberCall<Class, A1, A2, A3, A4, A5, A6, A7, A8> ( instance, function ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	template<class Class, class Data>
	Observer connect ( Class* instance, void (Class::*function)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, Data), const Data& data)
	{
		this->cleanup();
		std::shared_ptr<ObserverHolder> delegate ( new ObserverHolder ( new Delegate_v8_MemberUserCall<Class, A1, A2, A3, A4, A5, A6, A7, A8, Data> ( instance, function, data ) ) );
		this->observers.push_back(delegate);
		return Observer(delegate);
	}

	void invoke ( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 )
	{
		EventBase::ObserverList del_list = this->observers;
		EventBase::ObserverListSize i, size = del_list.size();
		for(i = 0; i < size; i++)
		{
			if(del_list[i]->get_delegate_ptr()->isValid() && del_list[i]->get_delegate_ptr()->isEnabled())
			{
				((Delegate_v8_Generic <A1, A2, A3, A4, A5, A6, A7, A8> *)del_list[i].get()->get_delegate_ptr())->invoke(a1, a2, a3, a4, a5, a6, a7, a8);
			}
		}
	}

};

//// ######## GENERATED BY SCRIPT - FINISH ######## ////


