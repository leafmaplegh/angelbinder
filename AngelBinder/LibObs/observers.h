#pragma once

/************************************************************************

Copyright (c) 2010, João Francisco Biondo Trinca
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

#include <vector>
#include <memory>

#include <LibObs/Delegates.h>

class ObserverContainer;

class ObserverHolder
{

	friend class Observer;

private:
	std::shared_ptr<DelegateBase> _delegate;

public:
	ObserverHolder()
		: _delegate(NULL)
	{

	}

	ObserverHolder(DelegateBase* base)
	{
		_delegate = std::shared_ptr<DelegateBase>(base);
	}

	~ObserverHolder()
	{
		if(this->get_delegate())
		{
			this->get_delegate()->setValid(false);
		}
	}

	std::shared_ptr<DelegateBase>& get_delegate()
	{
		return this->_delegate;				
	}

	DelegateBase* get_delegate_ptr()
	{
		return this->_delegate.get();				
	}

protected:
	void set_delegate(std::shared_ptr<DelegateBase>& delegate)
	{
		this->_delegate = delegate;
	}

};

/**
* An instance of this class represents a connection between
* and event, and a delegate, which will stay alive until the 
* instance is destroyed
**/
class Observer
{
protected:
	/// Our delegate instance
	std::shared_ptr<ObserverHolder> _holder;

public:
	/**
	* Initializes the observer
	**/
	Observer ( std::shared_ptr<ObserverHolder> &other ) 
		: _holder ( new ObserverHolder() )
	{
		this->get_holder()->set_delegate ( other.get()->get_delegate() ); 
	}

	/**
	* Destroys the observer
	**/
	~Observer() 
	{
	}

	/**
	* Destroy the observer, making the delegate not to be called
	**/
	void destroy()
	{
		if(this->get_holder() && this->get_holder()->get_delegate())
		{
			this->get_holder()->get_delegate()->setValid(false);
		}
	}

	/**
	* Enables the observer for getting callbacks
	**/
	void enable()
	{
		if(this->get_holder() && this->get_holder()->get_delegate())
		{
			this->get_holder()->get_delegate()->setEnabled(true);
		}
	}

	/**
	* Disables the observer for getting callbacks
	**/
	void disable()
	{
		if(this->get_holder() && this->get_holder()->get_delegate())
		{
			this->get_holder()->get_delegate()->setEnabled(false);
		}
	}

	/**
	* Gets the current associated delegate pointer
	**/
	ObserverHolder* get_holder()
	{
		return this->_holder.get();
	}

};

/**
* This is a container to store all the observers
**/
class ObserverContainer
{
protected:
	std::vector<Observer> observers;

public:
	ObserverContainer() 
	{ 
	}

	~ObserverContainer() 
	{ 
		std::vector<Observer>::size_type i, size = this->observers.size();
		for(i = 0; i < size; i++)
		{
			this->observers.at(i).destroy();
		}
	}

	template<class EventClass>
	void connect ( EventClass& event, void (*function)())
	{
		this->observers.push_back ( event.connect ( function ) );
	}

	template<class EventClass, class Data>
	void connect ( EventClass& event, void (*function)(Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( function, data ) );
	}

	template<class EventClass, class Class>
	void connect ( EventClass& event, Class* instance, void (Class::*function)())
	{
		this->observers.push_back ( event.connect ( instance, function ) );
	}

	template<class EventClass, class Class, class Data>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( instance, function, data ) );
	}


	template<class EventClass, class A1>
	void connect ( EventClass& event, void (*function)(A1))
	{
		this->observers.push_back ( event.connect ( function ) );
	}

	template<class EventClass, class Data, class A1>
	void connect ( EventClass& event, void (*function)(A1, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( function, data ) );
	}

	template<class EventClass, class Class, class A1>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1))
	{
		this->observers.push_back ( event.connect ( instance, function ) );
	}

	template<class EventClass, class Class, class Data, class A1>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( instance, function, data ) );
	}


	template<class EventClass, class A1, class A2>
	void connect ( EventClass& event, void (*function)(A1, A2))
	{
		this->observers.push_back ( event.connect ( function ) );
	}

	template<class EventClass, class Data, class A1, class A2>
	void connect ( EventClass& event, void (*function)(A1, A2, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( function, data ) );
	}

	template<class EventClass, class Class, class A1, class A2>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2))
	{
		this->observers.push_back ( event.connect ( instance, function ) );
	}

	template<class EventClass, class Class, class Data, class A1, class A2>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( instance, function, data ) );
	}


	template<class EventClass, class A1, class A2, class A3>
	void connect ( EventClass& event, void (*function)(A1, A2, A3))
	{
		this->observers.push_back ( event.connect ( function ) );
	}

	template<class EventClass, class Data, class A1, class A2, class A3>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( function, data ) );
	}

	template<class EventClass, class Class, class A1, class A2, class A3>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3))
	{
		this->observers.push_back ( event.connect ( instance, function ) );
	}

	template<class EventClass, class Class, class Data, class A1, class A2, class A3>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( instance, function, data ) );
	}


	template<class EventClass, class A1, class A2, class A3, class A4>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, A4))
	{
		this->observers.push_back ( event.connect ( function ) );
	}

	template<class EventClass, class Data, class A1, class A2, class A3, class A4>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, A4, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( function, data ) );
	}

	template<class EventClass, class Class, class A1, class A2, class A3, class A4>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, A4))
	{
		this->observers.push_back ( event.connect ( instance, function ) );
	}

	template<class EventClass, class Class, class Data, class A1, class A2, class A3, class A4>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, A4, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( instance, function, data ) );
	}


	template<class EventClass, class A1, class A2, class A3, class A4, class A5>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, A4, A5))
	{
		this->observers.push_back ( event.connect ( function ) );
	}

	template<class EventClass, class Data, class A1, class A2, class A3, class A4, class A5>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, A4, A5, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( function, data ) );
	}

	template<class EventClass, class Class, class A1, class A2, class A3, class A4, class A5>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, A4, A5))
	{
		this->observers.push_back ( event.connect ( instance, function ) );
	}

	template<class EventClass, class Class, class Data, class A1, class A2, class A3, class A4, class A5>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, A4, A5, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( instance, function, data ) );
	}


	template<class EventClass, class A1, class A2, class A3, class A4, class A5, class A6>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, A4, A5, A6))
	{
		this->observers.push_back ( event.connect ( function ) );
	}

	template<class EventClass, class Data, class A1, class A2, class A3, class A4, class A5, class A6>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, A4, A5, A6, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( function, data ) );
	}

	template<class EventClass, class Class, class A1, class A2, class A3, class A4, class A5, class A6>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, A4, A5, A6))
	{
		this->observers.push_back ( event.connect ( instance, function ) );
	}

	template<class EventClass, class Class, class Data, class A1, class A2, class A3, class A4, class A5, class A6>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, A4, A5, A6, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( instance, function, data ) );
	}


	template<class EventClass, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, A4, A5, A6, A7))
	{
		this->observers.push_back ( event.connect ( function ) );
	}

	template<class EventClass, class Data, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, A4, A5, A6, A7, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( function, data ) );
	}

	template<class EventClass, class Class, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, A4, A5, A6, A7))
	{
		this->observers.push_back ( event.connect ( instance, function ) );
	}

	template<class EventClass, class Class, class Data, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, A4, A5, A6, A7, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( instance, function, data ) );
	}


	template<class EventClass, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, A4, A5, A6, A7, A8))
	{
		this->observers.push_back ( event.connect ( function ) );
	}

	template<class EventClass, class Data, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	void connect ( EventClass& event, void (*function)(A1, A2, A3, A4, A5, A6, A7, A8, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( function, data ) );
	}

	template<class EventClass, class Class, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, A4, A5, A6, A7, A8))
	{
		this->observers.push_back ( event.connect ( instance, function ) );
	}

	template<class EventClass, class Class, class Data, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	void connect ( EventClass& event, Class* instance, void (Class::*function)(A1, A2, A3, A4, A5, A6, A7, A8, Data), const Data& data )
	{
		this->observers.push_back ( event.connect ( instance, function, data ) );
	}


};

