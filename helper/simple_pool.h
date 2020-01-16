#pragma once

#include <stddef.h>
#include <list>
#include <string>
#include "lock.h"


namespace hlp {
	using std::list;
	using std::string;


	template <class _Type>
	class SimpleAllocator {
	public:
		typedef _Type object;
		typedef _Type* object_pointer;
		
		object_pointer& Create() {
			return new _Type;
		}
		void Destroy(object_pointer& obj) {
			delete obj;
		}
	private:
	};

	template <class _Type, class _Allocator = SimpleAllocator<_Type> >
	class SimplePool {
	public:
		typedef _Allocator Allocator;
		typedef _Type* ObjectPointer;
		typedef _Type& ObjectRefrence;
		class RefObject {
		public:
			friend class SimplePool;
			RefObject(ObjectPointer object, SimplePool<_Type, _Allocator>* pool)
				: ref_count_(1), object_(object), pool_(pool) {
			}

			~RefObject() {
			}

			void AddRef() {
				++ref_count_;
			}

			void Release() {
				if (--ref_count_ == 0) {
					pool_->Release(this);
				}
			}

			ObjectPointer pointer() {
				return object_;
			}

			ObjectPointer pointer() const {
				return object_;
			}

		//private:
			int ref_count_;
			ObjectPointer object_;
			SimplePool* pool_;
		};

		class SimpleObject {
		public:
			SimpleObject() : object_(0) {
			}

			SimpleObject(RefObject* object) : object_(object) {
				if (object_) {
					object_->AddRef();
				}
			}

			~SimpleObject() {
				if (object_)
					object_->Release();
			}

			SimpleObject(const SimpleObject& src) {
				if (src.object_) {
					object_->AddRef();
					object_ = src.object_;
				}
			}

			SimpleObject& operator=(const SimpleObject& src) {
				if (src.object_) {
					object_->AddRef();
					object_ = src.object_;
				}
			}

			ObjectPointer operator->() {
				return object_->pointer();
			}

			operator bool() const {
				return (0 != object_);
			}

			operator ObjectPointer() {
				return object_->pointer();
			}

			operator ObjectPointer() const {
				return object_->pointer();
			}

			ObjectRefrence operator*() {
				return *object_->pointer();
			}

			_Type operator*() const {
				return *object_->pointer();
			}

		private:
			RefObject* object_;
		};

		SimplePool(_Allocator& allocator)
			: current_size_(0), idle_size_(0), init_size_(0), inc_size_(1), max_size_(-1), lock_(0), allocator_(allocator) {
		}

		SimplePool(_Allocator& allocator, int init_size, int inc_size = 1, int max_size = -1, ILock* lock = 0)
			: current_size_(0), idle_size_(0), init_size_(init_size), inc_size_(inc_size), max_size_(max_size), lock_(lock), allocator_(allocator) {
			if (init_size > 0 && (max_size_ < 0 || init_size < max_size_)) {
				Create(init_size);
			}
		}

		virtual ~SimplePool() {
		}

		RefObject*& Get() {
			AutoLock lock(lock_);
			if (idle_size_ == 0) {
				int size = 0;
				if (max_size_ < 0) {
					size = inc_size_;
				} else if (current_size_ < max_size_) {
					size = max_size_ - current_size_;
				}
				if (size > 0) {
					Create(size);
				}
			}
			RefObject*& pointer = list_.front();
			list_.pop_front();
			idle_size_--;
			return pointer;
		}

		void Release(RefObject*& obj, bool cleanup = false) {
			AutoLock lock(lock_);
			if (cleanup && idle_size_ > init_size_) {
				current_size_--;
				allocator_.Destroy(obj);
			} else {
				list_.push_back(obj);
				idle_size_++;
			}
		}

		ObjectRefrence GetObject() {
			RefObject*& wrapper = Get();
			return *(wrapper->pointer());
		}

		void ReleaseObject(ObjectRefrence obj, bool cleanup = false) {
			AutoLock lock(lock_);
			RefObject* wrapper = (RefObject*)((size_t)&obj - offsetof(RefObject, object_));
			if (cleanup && idle_size_ > init_size_) {
				current_size_--;
				allocator_.Destroy(&obj);
				delete wrapper;
			} else {
				list_.push_back(wrapper);
				idle_size_++;
			}
		}

		SimpleObject GetAutoObject() {
			return SimpleObject(Get());
		}

		void Tidy() {
			AutoLock lock(lock_);
			int rsize = idle_size_ - init_size_;
			for (int i = 0; i < rsize; i++) {
				RefObject*& wrapper = list_.back();
				allocator_.Destroy(wrapper->pointer());
				idle_size_--;
				current_size_--;
				list_.pop_back();
				delete wrapper;
			}
		}

	protected:
		void Create(int size) {
			for (int i = 0; i < size; i++) {
				RefObject* pointer = new RefObject(allocator_.Create(), this);
				if (pointer) {
					list_.push_back(pointer);
					idle_size_++;
					current_size_++;
				}
			}
		}
	private:
		int current_size_;
		int idle_size_;
		int init_size_;
		int inc_size_;
		int max_size_;
		list<RefObject*> list_;
		ILock* lock_;
		Allocator& allocator_;
	};
} // namespace hlp


