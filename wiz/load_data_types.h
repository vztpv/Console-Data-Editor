
#ifndef LOAD_DATA_TYPES_H
#define LOAD_DATA_TYPES_H

#include <string>
#include <vector>
#include <memory>
using namespace std;

#include <wiz/newArrays.h>
#include <wiz/dictionary.h>

namespace wiz {
	namespace load_data {

		class Type {
		private:
			string name;
			bool isValid;
		public:
			bool valid()const { return isValid; }
			void setValid(const bool val) { isValid = val; }
		public:
			explicit Type(const string& name = "", const bool isValid = true) : name(name), isValid(isValid) { }
			Type(const Type& type)
				: name(type.name), isValid(type.isValid)
			{ }
			virtual ~Type() { }
			bool IsFail() const { // change body?
				return "" == name;
			}
			const string& GetName()const {
				return name;
			}
			void SetName(string name)
			{
				this->name = name;
			}
			bool operator==(const Type& t) const {
				return name == t.name;
			}
			bool operator<(const Type& t) const {
				return name < t.name;
			}
			Type& operator=(const Type& type)
			{
				name = type.name;
				isValid = type.isValid;
				return *this;
			}
		};

		template < class T >
		class TypeArray : public Type {
		private:
			std::vector<T> arr;
		public:
			TypeArray(const TypeArray<T>& ta) : Type(ta)
			{
				arr = ta.arr;
			}
			TypeArray(TypeArray<T>&& ta) : Type(ta)
			{
				arr = move( ta.arr );
			}
		public:
			explicit TypeArray(const string& name = "") : Type(name) { }
			virtual ~TypeArray() { }
		public:
			void Remove() {
				arr = std::vector<T>();
			}
			void Remove(int idx)
			{
				for (int i = idx + 1; i < arr.size(); ++i)
				{
					arr[i - 1] = arr[i];
				}
				arr.resize(arr.size() - 1);
			}
			void Push(const T& val) { /// do not change..!!
				arr.push_back(val);
			}
			void Push(T&& val) {
				arr.push_back(val);
			}
			T& Get(const int index) {
				return arr[index];
			}
			const T& Get(const int index) const {
				// #ifdef DEBUG ??
				/*if (index < 0 || index >= count || arr.size() <= 0) {
					cout << "index " << index << endl;
					throw "index not valid";
				}*/
				return arr[index];
			}
			void Set(const int index, const T& val) {
				//if (index >= 0 && index < count) {
					arr[index] = val;
				//}
				//else
			//	{
			//		cout << "error " << index << " " << count << endl;
			//	}
			}
			int size()const {
				return arr.size();
			}
		public:
			TypeArray<T>& operator=(const TypeArray<T>& ta)
			{
				Type::operator=(ta);
				TypeArray<T> temp = ta;

				arr = std::move(temp.arr);
				return *this;
			}
			TypeArray<T>& operator=(TypeArray<T>&& ta)
			{
				Type::operator=(ta);
				if (arr == ta.arr) { return *this; }

				arr = move( ta.arr );
				return *this;
			}
			string ToString()const
			{
				string temp;

				for (int i = 0; i < size(); ++i) {
					temp = temp + arr[i];
					if (i != size() - 1) {
						temp = temp + "/";
					}
				}

				return temp;
			}
		};

		class UserType : public Type {
		public:
			const vector<int>& GetIList() const { return ilist; }
			vector<int>& GetIList() { return ilist; }
			int GetItemListSize()const { return itemList.size(); }
			int GetUserTypeListSize()const { return userTypeList.size(); }
			TypeArray<string>& GetItemList(const int idx) { return itemList[idx]; }
			const TypeArray<string>& GetItemList(const int idx) const { return itemList[idx]; }
			TypeArray<UserType*>& GetUserTypeList(const int idx) { return userTypeList[idx]; }
			const TypeArray<UserType*>& GetUserTypeList(const int idx) const { return userTypeList[idx]; }
			void AddItemList(const TypeArray<string>& strTa)
			{
				for (int i = 0; i < strTa.size(); ++i) {
					this->AddItem(strTa.GetName(), strTa.Get(i));
				}
			}
			void AddUserTypeList(const TypeArray<UserType*>& utTa)
			{
				for (int i = 0; i < utTa.size(); ++i) {
					this->AddUserTypeItem(*utTa.Get(i));
				}
			}
		public:
			UserType* GetParent() { return parent; }
			const UserType* GetParent()const { return parent; }
		private:
			UserType* parent;
			std::vector<int> ilist;
			std::vector< TypeArray<string> > itemList;
			std::vector< TypeArray<UserType*>> userTypeList;
		public:
			explicit UserType(const string& name = "") : Type(name) , parent(NULL) { }
			UserType(const UserType& ut) : Type(ut.GetName()) {
				Reset(ut);  // Initial
			}
			UserType(UserType&& ut) : Type(ut.GetName()) {
				Reset2(move(ut));
			}
			virtual ~UserType() {
				_Remove();
			}
			UserType& operator=(const UserType& ut) {
				if (this == &ut) { return *this; }
				Type::operator=(ut);

				RemoveUserTypeList();
				Reset(ut);
				return *this;
			}
			UserType& operator=(UserType&& ut) {
				if (this == &ut) { return *this;  }

				Type::operator=(ut);
				RemoveUserTypeList();
				Reset2(move(ut));
				return *this;
			}
		private:
			void Reset(const UserType& ut) { /// UT 전체를 복사한다.
				ilist = ut.ilist;
				itemList = ut.itemList;
				parent = ut.parent;

				for (int i = 0; i < ut.userTypeList.size(); i++) {
					TypeArray<UserType*> temp(ut.userTypeList[i].GetName());
					for (int j = 0; j < ut.userTypeList[i].size(); j++) {
						temp.Push( new UserType(*ut.userTypeList[i].Get(j)) );
					}
					userTypeList.push_back(move(temp));
				}
			}
			void Reset2(UserType&& ut) {
				parent = ut.parent;
				ut.parent = NULL; /// chk..
				ilist = std::move(ut.ilist);
				itemList = std::move(ut.itemList);
				userTypeList = std::move(ut.userTypeList);
			}

			void _Remove()
			{
				ilist = vector<int>();
				itemList = vector< TypeArray<string> >();
				RemoveUserTypeList();
			}

		private:
			int _GetIndex(const vector<int>& ilist, const int val, const int start = 0)
			{
				for (int i = start; i < ilist.size(); ++i) {
					if (ilist[i] == val) { return i; }
				}
				return -1;
			}
		public:
			void RemoveItemList(const string& varName)
			{
				int k = _GetIndex(ilist, 1, 0);
				vector<TypeArray<string>> tempDic;
				for (int i = 0; i < itemList.size(); ++i) {
					if (varName != itemList[i].GetName()) {
						tempDic.push_back(itemList[i]);
						k = _GetIndex(ilist, 1, k + 1);
					}
					else {
						// remove item, ilist left shift 1.
						for (int j = k + 1; j < ilist.size(); ++j) {
							ilist[j - 1] = ilist[j];
						}
						ilist.resize(ilist.size() - 1);	
						k = _GetIndex(ilist, 1, k);
					}
				}
				itemList = ( tempDic );
			}
			void RemoveItemList() /// ALL
			{
				itemList = vector<TypeArray<string>>();
				//
				vector<int> temp;
				for (int i = 0; i < ilist.size(); ++i) {
					if (ilist[i] == 2)
					{
						temp.push_back(2);
					}
				}
				ilist = move( temp );
			}
			void RemoveEmptyItem() // fixed..
			{
				int k = _GetIndex(ilist, 1, 0);
				vector<TypeArray<string>> tempDic;
				for (int i = 0; i < itemList.size(); ++i) {
					if (itemList[i].size() > 0) {
						tempDic.push_back(itemList[i]);
					}
					else {
						// remove item, ilist left shift 1.
						for (int j = k + 1; j < ilist.size(); ++j) {
							ilist[j - 1] = ilist[j];
						}
						ilist.resize(ilist.size() - 1);
					}
					k = _GetIndex(ilist, 1, k + 1);
				}
				itemList = move(tempDic);
			}
			void Remove()
			{
				/// parent->removeUserType(name); - ToDo - X?
				ilist = vector<int>();
				itemList = vector< TypeArray<string> >();

				RemoveUserTypeList();
			}
			void RemoveUserTypeList() { /// chk memory leak test!!
				for (int i = 0; i < userTypeList.size(); i++) {
					for (int j = 0; j < userTypeList[i].size(); j++) {
						if (NULL != userTypeList[i].Get(j)) {
							delete userTypeList[i].Get(j); //
							userTypeList[i].Set(j, NULL);
						}
					}
				}
				// DO Empty..
				userTypeList = vector< TypeArray<UserType*> >();

				vector<int> temp;
				for (int i = 0; i < ilist.size(); ++i) {
					if (ilist[i] == 1)
					{
						temp.push_back(1);
					}
				}
				ilist = move( temp );
			}
			void RemoveUserTypeList(const string& varName)
			{
				int k = _GetIndex(ilist, 2, 0);
				vector<TypeArray<UserType*>> tempDic;
				for (int i = 0; i < userTypeList.size(); ++i) {
					if (varName != userTypeList[i].GetName()) {
						tempDic.push_back(userTypeList[i]);
						k = _GetIndex(ilist, 2, k + 1);
					}
					else {
						// remove usertypeitem, ilist left shift 1.
						for (int j = 0; j < userTypeList[i].size(); ++j) {
							if (NULL != userTypeList[i].Get(j)) {
								delete userTypeList[i].Get(j);
								userTypeList[i].Set(j, NULL);
							}
						}
						for (int j = k + 1; j < ilist.size(); ++j) {
							ilist[j - 1] = ilist[j];
						}
						ilist.resize(ilist.size() - 1);	
						k = _GetIndex(ilist, 2, k);
					}
				
				}
				userTypeList = move( tempDic );
			}

		public:
			bool empty()const { return ilist.empty(); }
			void AddItem(const string& name, const string& item) {
				/*int index = -1;
				if (!itemList.Search(TypeArray<string>(name), &index))
				{
					ilist.push_back(1);

					itemList.PushBack(TypeArray<string>(name));//
					itemList.Search(TypeArray<string>(name), &index);
				}
				itemList[index].Push(item);				
				*/
				TypeArray<string> temp( name );
				temp.Push( item );
				itemList.push_back(temp);
				ilist.push_back(1);
			}
			void AddUserTypeItem(const UserType& item) {
				/*
				int index = -1;
				if (!userTypeList.Search(TypeArray<UserType*>(item.GetName()), &index))
				{
					ilist.push_back(2);

					userTypeList.PushBack(TypeArray<UserType*>(item.GetName()));//
					userTypeList.Search(TypeArray<UserType*>(item.GetName()), &index);
				}
				*/
				UserType* temp( new UserType(item) );
				temp->parent = this;

				TypeArray<UserType*> temp2(item.GetName());
				temp2.Push(temp);

				ilist.push_back(2);

				userTypeList.push_back(temp2);
			}

			/// To Do!!
			vector<TypeArray<string>> GetItem(const string& name) const {
				vector<TypeArray<string>> temp;

				for (int i = 0; i < itemList.size(); ++i) {
					if (itemList[i].GetName() == name) {
						temp.push_back(itemList[i]);
					}
				}
				return temp;
			}
			bool SetItem(const string& name, const string& value) {
				int index = -1;
				
				for (int i = 0; i < itemList.size(); ++i) {
					if (itemList[i].GetName() == name)
					{
						itemList[i].Set(0, value);
						index = i;
					}
				}

				return -1 != index;
			}
			vector<TypeArray<UserType*>> GetUserTypeItem(const string& name) const { /// chk...
				vector<TypeArray<UserType*>> temp;

				for (int i = 0; i < userTypeList.size(); ++i) {
					if (userTypeList[i].GetName() == name) {
						temp.push_back(userTypeList[i]);
					}
				}

				return temp;
			}
		public:
			bool GetLastUserTypeItemRef(const string& name, TypeArray<UserType*>& ref) {
				int idx = -1;
				
				for (int i = 0; i < userTypeList.size(); ++i)
				{
					if (name == userTypeList[i].GetName()) {
						idx = i;
					}
				}
				if (idx > -1) {
					ref = userTypeList[idx];
				}
				return idx > -1;
			}
		private:
			/// save1 - like EU4 savefiles.
			void Save1(ostream& stream, UserType* ut) {
				int itemListCount = 0;
				int userTypeListCount = 0;

				for (int i = 0; i < ut->ilist.size(); ++i) {
					//cout << "ItemList" << endl;
					if (ut->ilist[i] == 1) {
						for (int j = 0; j < ut->itemList[itemListCount].size(); j++) {
							if (ut->itemList[itemListCount].GetName() != "")
								stream << ut->itemList[itemListCount].GetName() << "=";
							stream << ut->itemList[itemListCount].Get(j);
							if (j != ut->itemList[itemListCount].size() - 1)
								stream << "\n";
						}
						stream << "\n";
						itemListCount++;
					}
					else if (ut->ilist[i] == 2) {
						// cout << "UserTypeList" << endl;
						for (int j = 0; j < ut->userTypeList[userTypeListCount].size(); j++) {
							if (ut->userTypeList[userTypeListCount].GetName() != "")
								stream << ut->userTypeList[userTypeListCount].GetName() << "=";
							stream << "{\n";
							Save1(stream, ut->userTypeList[userTypeListCount].Get(j));
							stream << "\n";
							stream << " }" << "\n";
						}
						userTypeListCount++;
					}
				}
			}
			/// savw2 - for more seed loading data!
			void Save2(ostream& stream, UserType* ut) {
				int itemListCount = 0;
				int userTypeListCount = 0;

				for (int i = 0; i < ut->ilist.size(); ++i) {
					//cout << "ItemList" << endl;
					if (ut->ilist[i] == 1) {
						for (int j = 0; j < ut->itemList[itemListCount].size(); j++) {
							if (ut->itemList[itemListCount].GetName() != "")
								stream << ut->itemList[itemListCount].GetName() << " = ";
							stream << ut->itemList[itemListCount].Get(j);
							if (j != ut->itemList[itemListCount].size() - 1)
								stream << " ";
						}
						stream << "\n";
						itemListCount++;
					}
					else if (ut->ilist[i] == 2) {
						// cout << "UserTypeList" << endl;
						for (int j = 0; j < ut->userTypeList[userTypeListCount].size(); j++) {
							if (ut->userTypeList[userTypeListCount].GetName() != "")
								stream << ut->userTypeList[userTypeListCount].GetName() << " = ";
							stream << " {\n";
							Save2(stream, ut->userTypeList[userTypeListCount].Get(j));
							stream << "\n";
							stream << " }" << "\n";
						}
						userTypeListCount++;
					}
				}
			}
		public:
			void Save1(ostream& stream) {
				Save1(stream, this);
			}
			void Save2(ostream& stream) {
				Save2(stream, this);
			}
			string ItemListToString()const
			{
				string temp;
				int itemListCount = 0;
				
				for (int i = 0; i < itemList.size(); ++i) {
					for (int j = 0; j < itemList[itemListCount].size(); j++) {
						if (itemList[itemListCount].GetName() != "")
							temp = temp + itemList[itemListCount].GetName() + " = ";
						temp = temp + itemList[itemListCount].Get(j);
						if (j != itemList[itemListCount].size() - 1) {
							temp = temp + "/";
						}
					}
					if (i != itemList.size() - 1)
					{
						temp = temp + "/";
					}
					itemListCount++;
				}
				return temp;
			}
			string ItemListNamesToString()const
			{
				string temp;
				int itemListCount = 0;

				for (int i = 0; i < itemList.size(); ++i) {
					for (int j = 0; j < itemList[itemListCount].size(); j++) {
						if (itemList[itemListCount].GetName() != "")
							temp = temp + itemList[itemListCount].GetName();
						else
							temp = temp + " ";

						if (j != itemList[itemListCount].size() - 1) {
							temp = temp + "/";
						}
					}
					if (i != itemList.size() - 1)
					{
						temp = temp + "/";
					}
					itemListCount++;
				}
				return temp;
			}
			string UserTypeListNamesToString()const
			{
				string temp;
				int userTypeListCount = 0;

				for (int i = 0; i < userTypeList.size(); ++i) {
					for (int j = 0; j < userTypeList[userTypeListCount].size(); j++) {
						if (userTypeList[userTypeListCount].GetName() != "")
							temp = temp + userTypeList[userTypeListCount].GetName();
						else
							temp = temp + " "; // chk!! cf) wiz::load_data::Utility::Find function...
						if (j != userTypeList[userTypeListCount].size() - 1)
							temp = temp + "/";
					}
					if (i != itemList.size() - 1)
					{
						temp = temp + "/";
					}
					userTypeListCount++;
				}
				return temp;
			}
			string ToString()const
			{
				string temp;
				int itemListCount = 0;
				int userTypeListCount = 0;

				for (int i = 0; i < ilist.size(); ++i) {
					//cout << "ItemList" << endl;
					if (ilist[i] == 1) {
						for (int j = 0; j < itemList[itemListCount].size(); j++) {
							if (itemList[itemListCount].GetName() != "")
								temp = temp + itemList[itemListCount].GetName() + " = ";
							temp = temp + itemList[itemListCount].Get(j);
							if (j != itemList[itemListCount].size() - 1)
								temp = temp + " ";
						}
						temp = temp + " ";
						itemListCount++;
					}
					else if (ilist[i] == 2) {
						// cout << "UserTypeList" << endl;
						for (int j = 0; j < userTypeList[userTypeListCount].size(); j++) {
							if (userTypeList[userTypeListCount].GetName() != "")
								temp = temp + userTypeList[userTypeListCount].GetName() + " = ";
							temp = temp + " { ";
							temp = temp + (userTypeList[userTypeListCount].Get(j)->ToString()) + " ";
							temp = temp + " } " + " ";
						}
						userTypeListCount++;
					}
				}
				return temp;
			}
			friend ostream& operator<<(ostream& stream, const UserType& ut)
			{
				int itemListCount = 0;
				int userTypeListCount = 0;

				for (int i = 0; i < ut.ilist.size(); ++i) {
					//cout << "ItemList" << endl;
					if (ut.ilist[i] == 1) {
						for (int j = 0; j < ut.itemList[itemListCount].size(); j++) {
							if (ut.itemList[itemListCount].GetName() != "")
								stream << ut.itemList[itemListCount].GetName() << "=";
							stream << ut.itemList[itemListCount].Get(j);
							if (j != ut.itemList[itemListCount].size() - 1)
								stream << " ";
						}
						stream << "\n";
						itemListCount++;
					}
					else if (ut.ilist[i] == 2) {
						// cout << "UserTypeList" << endl;
						for (int j = 0; j < ut.userTypeList[userTypeListCount].size(); j++) {
							if (ut.userTypeList[userTypeListCount].GetName() != "")
								stream << ut.userTypeList[userTypeListCount].GetName() << "=";
							stream << "{\n";
							stream << *ut.userTypeList[userTypeListCount].Get(j) << "\n";
							stream << " }" << "\n";
						}
						userTypeListCount++;
					}
				}

				return stream;
			}
		};
	}
}

#endif