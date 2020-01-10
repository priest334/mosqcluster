#ifndef __XDB_RECORD_H__
#define __XDB_RECORD_H__

#include <vector>

namespace hlp {
	using std::vector;

	template <class Type>
	class TypeDelete {
	public:
		void operator()(Type* pointer) {
			delete pointer;
		}
	};

	template <class Type>
	class TypeNew {
	public:
		Type* operator()() {
			return new Type();
		}
	};

	template <class Type, class Constructor = TypeNew<Type>, class Destructor = TypeDelete<Type> >
	class Record {
		typedef Type* TypePtr;
		typedef struct value_s {
			bool delete_;
			TypePtr value_;
		}Value;
		typedef vector<Value> Container;
		typedef typename Container::iterator Iterator;
		typedef typename Container::const_iterator ConstIterator;
	public:
		static TypePtr New() { Constructor c; return c(); }
		static void Destroy(TypePtr pointer) { Destructor d; d(pointer); }
	public:
		Record() {}
		~Record() {
			Clear();
		}

		void Clear() {
			Destructor d;
			Iterator iter = container_.begin();
			for (; iter != container_.end(); ++iter) {
				if ((*iter).delete_)
					d((*iter).value_);
			}
			container_.clear();
		}

		void Append(TypePtr value, bool auto_delete = true) {
			Value v;
			v.delete_ = auto_delete;
			v.value_ = value;
			container_.push_back(v);
		}

		Iterator Begin() {
			return container_.begin();
		}

		Iterator End() {
			return container_.end();
		}

		const Type& operator[](int index) const {
			int size = container_.size();
			if (index < 0 || index >= size)
				throw "array index out of bounds!";
			return *(container_[index].value_);
		}

		Type& operator[](int index) {
			int size = container_.size();
			if (index < 0 || index >= size)
				throw "array index out of bounds!";
			return *(container_[index].value_);
		}

		int Size() const {
			return container_.size();
		}

	private:
		Container container_;
	};
} // namespace hlp


#endif //__XDB_RECORD_H__
