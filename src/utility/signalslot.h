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

	struct SlotContext
	{
		void setBreak(bool v) { flow = (v ? Flow::Break : Flow::Continue); }
		bool isBreak() const { return flow == Flow::Break; }

	private:
		enum class Flow
		{
			Continue = 0,
			Break = 1,
		};
		Flow flow = Flow::Continue;
	};

	/*
	 * @brief decoupling the binding and the SigSlotBase
	 */
	class SigSlotInterface
	{
	public:
		virtual void addBinding(const BindingPtr& b) = 0;
		virtual void eraseBinding(const BindingPtr& b) = 0;

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
				em->eraseBinding(shared_from_this());
			}
			if (_receiver) {
				SigSlotInterface* recv = _receiver;
				_receiver = nullptr;
				recv->eraseBinding(shared_from_this());
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

		void addBinding(const BindingPtr& b) override
		{
			_bindings.push_back(b);
		}

		void eraseBinding(const BindingPtr& b) override
		{
			auto pos = std::find(_bindings.begin(), _bindings.end(), b);
			if (pos == _bindings.end()) {
				return;
			}

			_bindings.erase(pos);
		}

		static SlotContext& slotContext()
		{
			thread_local SlotContext context;
			return context;
		}

	private:
		std::list<BindingPtr> _bindings;
	};


	/*
	 * @brief signal can be connected and emit
	 */
	template <typename... ArgTypes>
	class Signal : public SigSlotBase
	{
		using Fun = std::function<void(ArgTypes...)>;
		struct Slot
		{
			int prio;
			Fun func;
		};
		using BindingRef = std::pair<BindingPtr, Slot>;

	public:
		template <typename ClassType>
		void connect(ClassType* inst, void(ClassType::* func)(ArgTypes...), int prio = 0)
		{
			THREAD_BLOCK_LOCKER;

			Slot slot = { prio, [=](ArgTypes... args) {return (inst->*func)(args...); } };
			BindingPtr binding = Binding::create(this, inst);

			addSlot(binding, slot);

			inst->addBinding(binding);
			addBinding(binding);
		}

		void connect(Fun fun, int prio = 0)
		{
			THREAD_BLOCK_LOCKER;

			Slot slot = { prio, fun };
			BindingPtr binding = Binding::create(nullptr, nullptr);

			addSlot(binding, slot);
		}

		void emit(ArgTypes... args)
		{
			THREAD_BLOCK_LOCKER;

			auto& ctx = slotContext();
			bool savedBreak = ctx.isBreak();
			ctx.setBreak(false);

			for (auto& slotBind : _slotBinds)
			{
				auto& slot = std::get<1>(slotBind);
				slot.func(args...);

				if (ctx.isBreak())
				{
					break;
				}
			}

			ctx.setBreak(savedBreak);
		}

		void operator()(ArgTypes... args)
		{
			emit(args...);
		}

		void disconnect()
		{
			THREAD_BLOCK_LOCKER;

			_slotBinds.clear();
		}

	protected:
		void eraseBinding(const BindingPtr& b)
		{
			THREAD_BLOCK_LOCKER;

			SigSlotBase::eraseBinding(b);

			auto it = std::find_if(_slotBinds.begin(), _slotBinds.end(), [&b](BindingRef r) -> bool {
				return std::get<0>(r) == b; });
			if (it != _slotBinds.end())
			{
				_slotBinds.erase(it);
			}
		}

		void addSlot(const BindingPtr& binding, const Slot& slot)
		{
			int prio = slot.prio;
			for (auto it = _slotBinds.rbegin(); it != _slotBinds.rend(); it++)
			{
				auto& b = *it;
				if (b.second.prio < prio)
				{
					_slotBinds.insert(it.base(), std::make_pair(binding, slot));
					return;
				}
			}

			_slotBinds.push_front(std::make_pair(binding, slot));
		}

	private:
		std::list<BindingRef> _slotBinds;
	};

	using SlotHandler = SigSlotBase;
} // namespace sigslot
} // namespace utility
