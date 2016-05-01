
#ifndef LOAD_DATA_H_INCLUDED
#define LOAD_DATA_H_INCLUDED

#include <vector>
#include <fstream>
#include <string>
#include <utility>
using namespace std;

#include <wiz/global.h>
#include <wiz/newArrays.h>
#include <wiz/Dictionary.h> /// change to map?

namespace wiz {
	// name must not "".
	class Type {
	private:
		string name;
	public:
		explicit Type(const string& name = "") : name(name) { }
		Type(const Type& type)
			: name(type.name)
		{ }
		virtual ~Type() { }
		bool IsFail() const {
			return "" == name;
		}
		string GetName()const {
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
			return *this;
		}
	};

	template < class T >
	class TypeArray : public Type {
	private:
		wiz::Array<T> arr;
		int count;
	public:
		TypeArray(const TypeArray<T>& ta) : Type(ta)
		{
			arr = ta.arr;
			count = ta.count;
		}
	public:
		explicit TypeArray(const string& name = "") : Type(name), count(0) { }
	public:
		void Remove() {
			arr = wiz::Array<T>();
			count = 0;
		}
		void Push(const T& val) { /// do not change..!!
			if (arr.empty()) {
				arr = wiz::Array<T>(1);
			}
			if (count >= arr.size()) {
				arr.expand();
			}
			arr[count] = val;
			count++;
		}
		T& Get(const int index) {
			return arr[index];
		}
		const T& Get(const int index) const {
			if (index < 0 || index >= count || arr.size() <= 0) {
				cout << "index " << index << endl;
				throw "index not valid";
			}
			return arr[index];
		}
		void Set(const int index, const T& val) {
			if (index >= 0 && index < count) {
				arr[index] = val;
			}
			else
			{
				cout << "error " << index << " "<< count << endl;
			}
		}
		int GetCount()const {
			return count;
		}
	public:
		TypeArray<T>& operator=(const TypeArray<T>& ta)
		{
			Type::operator=(ta);
			TypeArray<T> temp = ta;

			arr = std::move(temp.arr);
			count = temp.count;
			return *this;
		}
		string ToString()const
		{
			string temp;

			for (int i = 0; i < GetCount(); ++i) {
				temp = temp + arr[i] + "\n";
			}

			return temp;
		}
	};

	class UserType;
	UserType LoadData(const string& fileName);

	class UserType : public Type {
	public:
		const vector<int>& GetIList() const { return ilist;  }
		vector<int>& GetIList() { return ilist;  }
		int GetItemListSize()const { return itemList.GetCount(); }
		int GetUserTypeListSize()const { return userTypeList.GetCount(); }
		TypeArray<string>& GetItemList(const int idx) { return itemList[idx]; }
		const TypeArray<string>& GetItemList(const int idx) const { return itemList[idx]; }
		TypeArray<UserType*>& GetUserTypeList(const int idx) { return userTypeList[idx]; }
		const TypeArray<UserType*>& GetUserTypeList(const int idx) const { return userTypeList[idx]; }
		void AddItemList(const TypeArray<string>& strTa)
		{
			for (int i = 0; i < strTa.GetCount(); ++i) {
				this->AddItem(strTa.GetName(), strTa.Get(i));
			}
		}
		void AddUserTypeList(const TypeArray<UserType*>& utTa)
		{
			for (int i = 0; i < utTa.GetCount(); ++i) {
				this->AddUserTypeItem(*utTa.Get(i));
			}
		}
	private:
		vector<int> ilist;
		wiz::Dictionary< TypeArray<string> > itemList;
		wiz::Dictionary< TypeArray<UserType*> > userTypeList;
	public:
		explicit UserType(const string& name = "") : Type(name) { }
		UserType(const UserType& ut) : Type(ut.GetName()) {
			Reset(ut);  // Initial
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
	private:
		void Reset(const UserType& ut) { /// UT 전체를 복사한다.
			ilist = ut.ilist;
			itemList = ut.itemList;

			for (int i = 0; i < ut.userTypeList.GetCount(); i++) {
				TypeArray<UserType*> temp(ut.userTypeList[i].GetName());
				for (int j = 0; j < ut.userTypeList[i].GetCount(); j++) {
					temp.Push(new UserType(*ut.userTypeList[i].Get(j)));
				}
				userTypeList.PushBack(temp);
			}
		}

		void _Remove()
		{
			ilist = vector<int>();
			itemList = wiz::Dictionary< TypeArray<string> >();

			RemoveUserTypeList();
		}

	private:
		int _GetIndex(const vector<int>& ilist, const int val, const int start=0)
		{
			for (int i = start; i < ilist.size(); ++i) {
				if (ilist[i] == val) { return i; }
			}
			return -1;
		}
	public:
		void RemoveItemList(const string& varName)
		{
			int k = _GetIndex( ilist, 1, 0 );
			wiz::Dictionary<TypeArray<string>> tempDic;
			for (int i = 0; i < itemList.GetCount(); ++i) {
				if (varName != itemList[i].GetName()) {
					tempDic.PushBack(itemList[i]);
				}
				else {
					// remove item, ilist left shift 1.
					for (int j = k+1; j < ilist.size(); ++j) {
						ilist[j-1] = ilist[j];
					}
					ilist.resize(ilist.size() - 1);
				}
				k = _GetIndex(ilist, 1, k+1);
			}
			itemList = tempDic;
		}
		void RemoveItemList() /// ALL
		{
			itemList = wiz::Dictionary<TypeArray<string>>();
			//
			vector<int> temp;
			for (int i = 0; i < ilist.size(); ++i) {
				if (ilist[i] == 2)
				{
					temp.push_back(2);
				}
			}
			ilist = temp;
		}
		void Remove()
		{
			/// parent->removeUserType(name); - ToDo - X?
			ilist = vector<int>();
			itemList = wiz::Dictionary< TypeArray<string> >();

			RemoveUserTypeList();
		}
		void RemoveUserTypeList() { /// chk memory leak test!!
			for (int i = 0; i < userTypeList.GetCount(); i++) {
				for (int j = 0; j < userTypeList[i].GetCount(); j++) {
					if (NULL != userTypeList[i].Get(j)) {
						delete userTypeList[i].Get(j); //
						userTypeList[i].Set(j, NULL);
					}
				}
			}
			// DO Empty..
			userTypeList = wiz::Dictionary< TypeArray<UserType*> >();
			
			vector<int> temp;
			for (int i = 0; i < ilist.size(); ++i) {
				if (ilist[i] == 1)
				{
					temp.push_back(1);
				}
			}
			ilist = temp;
		}
		void RemoveUserTypeList(const string varName)
		{
			int k = _GetIndex(ilist, 2, 0);
			wiz::Dictionary<TypeArray<UserType*>> tempDic;
			for (int i = 0; i < userTypeList.GetCount(); ++i) {
				if (varName != userTypeList[i].GetName()) {
					tempDic.PushBack(userTypeList[i]);
				}
				else {
					// remove usertypeitem, ilist left shift 1.
					for (int j = k + 1; j < ilist.size(); ++j) {
						ilist[j - 1] = ilist[j];
					}
					ilist.resize(ilist.size() - 1);
				}
				k = _GetIndex(ilist, 2, k + 1);
			}
			userTypeList = tempDic;
		}

	public:
		bool empty()const { return ilist.empty(); }
		void AddItem(const string& name, const string& item) {
			int index = -1;
			if (!itemList.Search(TypeArray<string>(name), &index))
			{
				ilist.push_back(1);

				itemList.PushBack(TypeArray<string>(name));//
				itemList.Search(TypeArray<string>(name), &index);
			}
			itemList[index].Push(item);
		}
		void AddUserTypeItem(const UserType& item) {
			int index = -1;
			if (!userTypeList.Search(TypeArray<UserType*>(item.GetName()), &index))
			{
				ilist.push_back(2);

				userTypeList.PushBack(TypeArray<UserType*>(item.GetName()));//
				userTypeList.Search(TypeArray<UserType*>(item.GetName()), &index);
			}
			UserType* temp = new UserType(item);
			userTypeList[index].Push(temp);
		}

		TypeArray<string> GetItem(const string& name) const {
			TypeArray<string> temp;
			int index = -1;
			if (itemList.Search(TypeArray<string>(name), &index))
			{
				temp = itemList[index];//
			}
			return temp;
		}
		bool SetItem(const string& name, const string& value) {
			int index = -1;
			if (itemList.Search(TypeArray<string>(name), &index))
			{
				for (int i = 0; i < itemList[index].GetCount(); ++i) {
					itemList[index].Set(i, value);//
				}
			}
			return -1 != index;
		}
		TypeArray<UserType> GetUserTypeItem(const string& name) const { /// chk...
			TypeArray<UserType> temp;

			int index = -1;
			if (userTypeList.Search(TypeArray<UserType*>(name), &index))
			{
				temp.SetName(userTypeList[index].GetName());
				for (int i = 0; i < userTypeList[index].GetCount(); i++) {
					temp.Push(*userTypeList[index].Get(i));
				}
			}
			return temp;
		}
	public:
		bool GetUserTypeItemRef(const string& name, TypeArray<UserType*>& ref) {
			int index = -1;
			if (userTypeList.Search(TypeArray<UserType*>(name), &index))
			{
				ref = userTypeList[index];
				return true;
			}
			return false;
		}
	public:
		string ToString()const
		{
			string temp;
			int itemListCount = 0;
			int userTypeListCount = 0;

			for (int i = 0; i < ilist.size(); ++i) {
				//cout << "ItemList" << endl;
				if (ilist[i] == 1) {
					for (int j = 0; j < itemList[itemListCount].GetCount(); j++) {
						if (itemList[itemListCount].GetName() != "")
							temp = temp + itemList[itemListCount].GetName() + " = ";
						temp = temp + itemList[itemListCount].Get(j);
						if (j != itemList[itemListCount].GetCount() - 1)
							temp = temp + " ";
					}
					temp = temp + " ";
					itemListCount++;
				}
				else if (ilist[i] == 2) {
					// cout << "UserTypeList" << endl;
					for (int j = 0; j < userTypeList[userTypeListCount].GetCount(); j++) {
						if (userTypeList[userTypeListCount].GetName() != "")
							temp = temp + userTypeList[userTypeListCount].GetName() + " = ";
						temp = temp + " { ";
						temp = temp + ( userTypeList[userTypeListCount].Get(j)->ToString() ) + " ";
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
					for (int j = 0; j < ut.itemList[itemListCount].GetCount(); j++) {
						if (ut.itemList[itemListCount].GetName() != "")
							stream << ut.itemList[itemListCount].GetName() << "=";
						stream << ut.itemList[itemListCount].Get(j);
						if (j != ut.itemList[itemListCount].GetCount() - 1)
							stream << " ";
					}
					stream << "\n";
					itemListCount++;
				}
				else if (ut.ilist[i] == 2) {
					// cout << "UserTypeList" << endl;
					for (int j = 0; j < ut.userTypeList[userTypeListCount].GetCount(); j++) {
						if (ut.userTypeList[userTypeListCount].GetName() != "")
							stream << ut.userTypeList[userTypeListCount].GetName() << "=";
						stream << "{\n";
						stream << *ut.userTypeList[userTypeListCount].Get(j) << "\n";
						stream << " } " << "\n";
					}
					userTypeListCount++;
				}
			}

			return stream;
		}
		friend UserType LoadData(const string& fileName);
	};

	/// chk .. #\n something.... problem!!
	// # notation
	bool PassSharp(ifstream& inFile, const string& str) {
		if ('#' == str[0]) {
			string temp;
			getline(inFile, temp); // enter key?
			return true;
		}
		return false;
	}
	pair< bool, string> GetToken(ifstream& inFile, const string& before_token, const bool use_before_token)
	{
		if (use_before_token) { return{ " " != before_token, before_token }; }
		string temp;
		bool chk = (bool)(inFile >> temp);
		return{ chk, temp };
	}

	UserType LoadData(const string& fileName) {
		vector< UserType* > nestedUT;
		UserType global("global"); /// fileName을 받아와서 global를 리턴하는 dll등을 해보자??
		ifstream inFile;
		int state = 0;
		string id;
		string val;
		unsigned int braceNum = 0;

		// init
		inFile.open(fileName);
		if (inFile.fail()) { throw "infile.fail.."; } /// throw error??

		nestedUT.push_back(&global);

		try {
			string str;
			while (inFile >> str) {
				if (PassSharp(inFile, str)) { continue; } // # notation.

				switch (state)
				{
				case 0:
					if ("}" == str) {
						nestedUT[braceNum] = NULL;
						braceNum--;
					}
					else {
						id = str;
						state = 1;
					}
					break;
				case 1:
					if ("=" != str) { throw str + " : str is not '='. "; }
					state = 2;
					break;
				case 2:
					if ("{" == str) {
						nestedUT[braceNum]->AddUserTypeItem(UserType(id));
						TypeArray<UserType*> pTemp;
						nestedUT[braceNum]->GetUserTypeItemRef(id, pTemp);

						braceNum++;

						/// new nestedUT
						if (nestedUT.size() == braceNum) /// changed 2014.01.23..
							nestedUT.push_back(NULL);

						/// initial new nestedUT.
						nestedUT[braceNum] = pTemp.Get(pTemp.GetCount() - 1); /// TypeArray는 들어온 순서대로 ...
					}
					else {
						val = str;
						nestedUT[braceNum]->AddItem(id, val);
					}
					state = 0;
					break;
				}
			}
			inFile.close();
			cout << "braceNum : " << braceNum << endl;
		}
		catch (char* err) {
			inFile.close();  /// chk!
			throw err;
		}
		return global;
	}

}


#endif // LOAD_DATA_H_INCLUDED
