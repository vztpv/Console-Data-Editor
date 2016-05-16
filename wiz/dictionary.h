
#ifndef DICTIONARY_H_INCLUDED
#define DICTIONARY_H_INCLUDED


#include <wiz/global.h>
#include <wiz/wizardError.h>


namespace wiz {
		// Dictionary Class? <- push_back + insertSorting + binary Search
	template < class T, class COMP = ASC<T>, class EE = EE<T> > // T must have operator=().
	class Dictionary {
	public:
		friend ostream& operator<<(ostream& stream, const Dictionary<T, COMP, EE>& dic) {
			for (int i = 0; i < dic.count; i++)
				stream << dic.arr[i] << " ";
			stream << "\n";
			return stream;
		}
	private:
		COMP comp;
		std::vector<T> arr;
	public:
		// size ÁöÁ¤? const int size = 1
		explicit Dictionary() {
		}
		Dictionary(const Dictionary<T, COMP, EE>& dic) {
			arr = dic.arr;
		}
		Dictionary(Dictionary<T, COMP, EE>&& dic){
			arr = std::move(dic.arr);
		}
		Dictionary<T, COMP, EE>& operator=(const Dictionary<T, COMP, EE>& dic) {
			if (this == &dic ) {
				return *this;
			}
			arr = dic.arr;
			return *this;
		}
		Dictionary<T, COMP, EE>& operator=(Dictionary<T, COMP, EE>&& dic)
		{
			if (arr == dic.arr) {
				return *this;
			}

			arr = move(dic.arr);
			return *this;
		}
	public:
		void operator=(const vector<T>& arr)
		{
		    Remove();
			if( arr.empty() ) { return; }
			for( int i=0; i < arr.size(); i++ )
            {
                PushBack( arr[i] );
            }
		}

		void PushBack(const T& val) {
			// if already exist then pass??
			if (Search(val)) { /// to linear search?
				return;
			}
			arr.push_back(val);
			// not sort!
			///mini_insertSort(); // sorted items + new item.
		}
		void PushBack(const T&& val) {
			// if already exist then pass??
			if (Search(val)) { /// to linear search?
				return;
			}
			arr.push_back(val);
			// not sort!
			///mini_insertSort(); // sorted items + new item.
		}
		bool IsFull() const{
			return arr.size() == GetCount();
		}
		bool Search(const T&& key, int* index = NULL) const {
			/// linear serach
			for (int i = 0; i < GetCount(); ++i) {
				if (arr[i] == key)
				{
					if (index) { *index = i; }
					return true;
				}
			}
			return false;
		}
		bool Search(const T& key, int* index = NULL) const {
			/// linear serach
			for (int i = 0; i < GetCount(); ++i) {
				if (arr[i] == key)
				{
					if (index) { *index = i;   }
					return true; 
				}
			}
			return false;
			//return BinarySearch< T, COMP, EE >::search(arr, key, 0, count - 1, index); // return existence
		}
		int GetCount() const {
			return arr.size();
		}
		bool RemoveItem(const T& key) {
			int index = -1;

			if (Search(key, &index)) {
				// shift.. and count--
				for (int i = index + 1; i < count; i++) {
					arr[i - 1] = std::move(arr[i]);
				}
				arr[count - 1] = T();
				count--;
				return true;
			}
			else {
				return false;
			}
		}
		void Remove()  { /// chk....
			arr.clear();
		}
	public:
		const T& operator[](const int i) const {
			/// idx chk...
			return arr[i];
		}
		T& operator[] (const int i) {
			/// idx chk...
			return arr[i];
		}
	private:
		void mini_insertSort() {
			const T temp = std::move(arr[count - 1]);

			int current = count - 1;
			for (; (current > 0) && comp(temp, arr[current - 1]); current--) {
				arr[current] = std::move(arr[current - 1]);
			}
			arr[current] = std::move(temp);
		}
	};

}
#endif
