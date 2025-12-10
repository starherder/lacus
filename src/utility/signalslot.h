/*
  * sigslot.h
  *
  * Copyright (c) 2014, Leon Colijn. All rights reserved.
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  * MA 02110-1301  USA
  */

  #pragma once

#include <memory>
#include <functional>
#include <algorithm>

#include <list>

//#define USE_MULTI_THREAD 

#ifdef USE_MULTI_THREAD
#	include <mutex>
#	define THREAD_BLOCK_LOCKER std::lock_guard<std::mutex> locker(_mutex);
#	define THREAD_DEFINE_MUTEX std::mutex _mutex;
#else
#	define THREAD_BLOCK_LOCKER 
#	define THREAD_DEFINE_MUTEX
#endif


namespace utility
{

namespace sigslot
{
	using BindingPtr = std::shared_ptr<class Binding>;

	/*
	 * @brief decoupling the binding and the SigSlotBase
	 */
	class SigSlotInterface
	{
	public:
		virtual void add_binding(const BindingPtr& b) = 0;
		virtual void erase_binding(const BindingPtr& b) = 0;

	protected:
		THREAD_DEFINE_MUTEX
	};


	/*
	 * @brief bridge between signal and slots
	 */
	class Binding : public std::enable_shared_from_this<Binding>
	{
	public:
		Binding() = delete;
		Binding(const Binding& o) = delete;
		Binding& operator=(const Binding& other) = delete;

		virtual ~Binding()
		{
			unbind();
		}

		static BindingPtr create(SigSlotInterface* em, SigSlotInterface* recv)
		{
			return BindingPtr(new Binding(em, recv));
		}

		void unbind()
		{
			if (_emitter) {
				SigSlotInterface* em = _emitter;
				_emitter = nullptr;
				em->erase_binding(shared_from_this());
			}
			if (_receiver) {
				SigSlotInterface* recv = _receiver;
				_receiver = nullptr;
				recv->erase_binding(shared_from_this());
			}
		}

	private:
		Binding(SigSlotInterface* emitter, SigSlotInterface* receiver)
			: _emitter(emitter), _receiver(receiver)
		{
		}

		SigSlotInterface* _emitter;
		SigSlotInterface* _receiver;
	};


	/*
	 * @brief Base class for objects wishing to receive signals (i.e. have slots)
	 */
	class SigSlotBase : public SigSlotInterface
	{
	public:
		virtual ~SigSlotBase()
		{
			while (!_bindings.empty()) {
				_bindings.front()->unbind();
			}
		}

		void add_binding(const BindingPtr& b) override
		{
			_bindings.push_back(b);
		}

		void erase_binding(const BindingPtr& b) override
		{
			auto pos = std::find(_bindings.begin(), _bindings.end(), b);
			if (pos == _bindings.end()) {
				// throw std::runtime_error("Specified binding was not found");
				return;
			}

			_bindings.erase(pos);
		}

	private:
		std::list<BindingPtr> _bindings;
	};


	/*
	 * @brief signal can be connected and emit
	 */
	template <typename... _ArgTypes>
	class Signal : public SigSlotBase
	{
		typedef std::function<void(_ArgTypes...)> _Fun;
		struct _Slot
		{
			int prio;
			_Fun func;
		};
		typedef std::pair<BindingPtr, _Slot> _BindingRef;

	public:
		template <typename _Class>
		void connect(_Class* inst, void(_Class::* func)(_ArgTypes...), int prio = 0)
		{
			THREAD_BLOCK_LOCKER;

			_Slot slot = { prio, [=](_ArgTypes... args) {return (inst->*func)(args...); } };
			BindingPtr binding = Binding::create(this, inst);

			add_slot(binding, slot);

			inst->add_binding(binding);
			add_binding(binding);
		}

		void connect(_Fun fun, int prio = 0)
		{
			THREAD_BLOCK_LOCKER;

			_Slot slot = { prio, fun };
			BindingPtr binding = Binding::create(nullptr, nullptr);

			add_slot(binding, slot);
		}

		void emit(_ArgTypes... args)
		{
			THREAD_BLOCK_LOCKER;

			for (auto& slot : _slots) {
				std::get<1>(slot).func(args...);
			}
		}

		void operator()(_ArgTypes... args)
		{
			emit(args...);
		}

		void disconnect()
		{
			THREAD_BLOCK_LOCKER;

			_slots.clear();
		}

	protected:
		void erase_binding(const BindingPtr& b)
		{
			THREAD_BLOCK_LOCKER;

			SigSlotBase::erase_binding(b);

			auto it = std::find_if(_slots.begin(), _slots.end(), [&b](_BindingRef r) -> bool {
				return std::get<0>(r) == b; });
			_slots.erase(it);
		}

		void add_slot(const BindingPtr& binding, const _Slot& slot)
		{
			int prio = slot.prio;
			for (auto it = _slots.begin(); it != _slots.end(); it++)
			{
				auto& b = *it;
				if (b.second.prio > prio)
				{
					_slots.insert(it, std::make_pair(binding, slot));
					return;
				}
			}

			_slots.push_back(std::make_pair(binding, slot));
		}

	private:
		std::list<_BindingRef> _slots;
	};

	using SlotHandler = SigSlotBase;
} // namespace sigslot
} // namespace utility