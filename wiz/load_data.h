
#ifndef LOAD_DATA_H_INCLUDED
#define LOAD_DATA_H_INCLUDED

#include <vector>
#include <stack>
#include <fstream>
#include <string>
#include <utility>
using namespace std;

#include <wiz/global.h>
#include <wiz/newArrays.h>
#include <wiz/Dictionary.h> /// change to map?

#include <wiz/cpp_string.h>
#include <wiz/queues.h>
#include <wiz/stacks.h>
#include <wiz/deck.h>


/// #define ArrayQueue list
const string LEFT = "{";
const string RIGHT = "}";
const string EQ = "=";

const int RIGHT_DO = 1;


namespace wiz {
	namespace load_data {
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
			Array<T> arr;
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
				arr = Array<T>();
				count = 0;
			}
			void Push(const T& val) { /// do not change..!!
				if (arr.empty()) {
					arr = Array<T>(1);
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
					cout << "error " << index << " " << count << endl;
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

		class UserType : public Type {
		public:
			const vector<int>& GetIList() const { return ilist; }
			vector<int>& GetIList() { return ilist; }
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
			Dictionary< TypeArray<string> > itemList;
			Dictionary< TypeArray<UserType*> > userTypeList;
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
				itemList = Dictionary< TypeArray<string> >();

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
				Dictionary<TypeArray<string>> tempDic;
				for (int i = 0; i < itemList.GetCount(); ++i) {
					if (varName != itemList[i].GetName()) {
						tempDic.PushBack(itemList[i]);
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
				itemList = tempDic;
			}
			void RemoveItemList() /// ALL
			{
				itemList = Dictionary<TypeArray<string>>();
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
				itemList = Dictionary< TypeArray<string> >();

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
				userTypeList = Dictionary< TypeArray<UserType*> >();

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
				Dictionary<TypeArray<UserType*>> tempDic;
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
		};
		/*
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
		*/

		class Utility
		{
		public:
			/// todo bool ,more stable!!
			static void AddSpace(const string& file1Name, const string& file2Name) {
				ifstream inFile;
				ofstream outFile;

				inFile.open(file1Name, ios::binary);
				outFile.open(file2Name, ios::binary);

				string temp;
				vector<string> strVec;

				while (getline(inFile, temp))
				{
					if (temp.empty() || temp == " ") { continue; }

					for (int j = 0; j < temp.size(); ++j)
					{
						if (temp[j] == '{') {
							outFile << " {\n";
						}
						else if (temp[j] == '}') {
							outFile << " }\n";
						}
						else if (temp[j] == '=') {
							outFile << " = ";
						}
						else
						{
							outFile << temp[j];
						}
					}
					outFile << " ";
					outFile << "\n";
				}

				inFile.close();
				outFile.close();
			}
			static bool PassSharp(const string& file1Name, const string& file2Name)
			{
				ifstream inFile;
				ofstream outFile;

				inFile.open(file1Name, ios::binary);
				if (inFile.fail()) { return false; }
				outFile.open(file2Name, ios::binary);
				if (outFile.fail()) { inFile.close(); return false; }

				string temp;

				while (getline(inFile, temp))
				{
					StringTokenizer tokenizer(temp, "#");

					if (tokenizer.countTokens() == 1 && tokenizer.isFindExist())
					{
						//
					}
					else if (tokenizer.countTokens() >= 1)
					{
						temp = tokenizer.nextToken();
						if (false == temp.empty())
						{
							outFile << temp << "\n";
						}
					}
				}

				inFile.close();
				outFile.close();
				return true;
			}

			static pair< bool, bool > PassSharp(ifstream& inFile, ArrayQueue<string>& strVec)
			{
				string temp;
				bool chk = (bool)getline(inFile, temp);
				if (chk) {
					StringTokenizer tokenizer(temp, "#");

					if (tokenizer.countTokens() == 1 && tokenizer.isFindExist())
					{
						//
					}
					else if (tokenizer.countTokens() >= 1)
					{
						temp = tokenizer.nextToken();
						if (false == temp.empty())
						{
							strVec.push(temp);
							return{ true, true };
						}
					}
				}
				return{ chk, false };
			}

			/// must lineNum > 0
			static pair<bool, int> Reserve2(ifstream& inFile, ArrayQueue<string>& strVec, const int num = 1)
			{
				string temp;
				int count = 0;

				for (int i = 0; i < num && (inFile >> temp); ++i) {
					strVec.push(temp);
					count++;
				}
				return{ count > 0, count };
			}
			/*
			pair< bool, int > Reserve(ifstream& inFile, ArrayQueue<string>& strVec, const int lineNum = 1)
			{
			int count = 0; // string num : not valid line num!
			pair<bool, bool> prTemp(false, false);

			for (int i = 0; i < lineNum; ++i) {
			ArrayQueue<string> temp;
			bool b1 = prTemp.first;
			bool b2 = prTemp.second;
			pair<bool, bool> bbTemp = PassSharp(inFile, temp);
			if (bbTemp.second) {
			for (int i = 0; i < temp.size(); ++i) {
			const string str = temp[i];
			StringTokenizer tokenizer(str, vector<string>{ " ", "\t", "\r", "\n" });
			for (int k = 0; k < tokenizer.countTokens(); ++k) {
			string str = tokenizer.nextToken();
			strVec.push(str);
			}
			count = count + tokenizer.countTokens();
			}
			}
			prTemp.first = b1 || bbTemp.first;
			}

			return{ prTemp.first, count };
			}
			*/
			static const string& Top(const ArrayQueue<string>& strVec)
			{
				return strVec[0];
			}
			static string Pop(ArrayQueue<string>& strVec)
			{
				return strVec.pop();
			}
			static int GetIndex(const ArrayQueue<string>& strVec, const string& str)
			{
				int idx = -1;

				for (int i = 0; i < strVec.size(); ++i)
				{
					const string x = strVec[i];
					idx++;
					if (x == str)
					{
						return idx;
					}
				}
				return -1;
			}

			static pair<bool, string> LookUp(const ArrayQueue<string>& strVec, const int idx = 1)
			{
				if (strVec.size() <= idx)
				{
					return{ false, "" };
				}
				return{ true, strVec[idx] };
			}
			/// must strVec[start] == up or down?
			/// now not use!!
			static pair<bool, int> IsMatched(const ArrayQueue<string>& strVec, const string& up, const string& down, const int start = 0, const int start_num = 0, int* pidx = NULL, int*pnum = NULL)
			{
				int num = start_num;
				int count = 0;
				int state = 0;
				int idx = -1;

				for (int i = start; i < strVec.size(); ++i) {
					if (strVec[i] == up) { state = 1; num++; }
					else if (strVec[i] == down) { state = 1;  num--; }
					if (state == 1 && num == 0)
					{
						if (pidx) { *pidx = i; }
						if (pnum) { *pnum = num; }
						return{ true, count };
					}
					count++;
					idx = i;
				}
				if (pidx) { *pidx = idx; }
				if (pnum) { *pnum = num; }
				return{ false, -1 };
			}

			static bool ChkData(const UserType* utTemp)
			{
				bool chk = true;
				const int itemListSize = utTemp->GetItemListSize();
				for (int i = 0; i < itemListSize; ++i) {
					if (utTemp->GetItemList(i).GetName() != ""
						&& utTemp->GetItemList(i).GetCount() > 1) {
						cout << utTemp->GetItemList(i).GetName() << endl;
						return false;
					}
				}
				const int UserTypeSize = utTemp->GetUserTypeListSize();
				for (int i = 0; i < UserTypeSize; ++i) {
					for (int j = 0; j < utTemp->GetUserTypeList(i).GetCount(); ++j) {
						chk = chk && ChkData(utTemp->GetUserTypeList(i).Get(j));
					}
				}

				return chk;
			}
			/// find userType! not itemList!
			static std::pair<bool, vector< UserType*> > Find(UserType& global, const string& position) /// option, option_offset
			{
				vector< UserType* > temp;
				if (position.empty()) { temp.push_back(&global); return{ true, temp }; }

				StringTokenizer tokenizer(position, "/");
				vector<string> strVec;
				Deck<pair< UserType*, int>> utDeck;
				pair<UserType*, int> utTemp;
				utTemp.first = &global;
				utTemp.second = 0;
				TypeArray<UserType*> utTemp2;

				for (int i = 0; i < tokenizer.countTokens(); ++i) {
					strVec.push_back(tokenizer.nextToken());
					if (" " == strVec[strVec.size() - 1])
					{
						strVec[strVec.size() - 1] = "";
					}
				}
				utDeck.push_front(utTemp);

				bool exist = false;
				while (false == utDeck.empty()) {
					utTemp = utDeck.pop_front();

					//	if (false == exist && utDeck.empty() && utTemp.second < strVec.size()  && false == utTemp.first->GetUserTypeItemRef(strVec[utTemp.second], utTemp2))
					//	{
					//		return{ false, vector<UserType*>() };
					//	}
					if (utTemp.second < strVec.size() && strVec[utTemp.second] == "$")
					{
						for (int j = utTemp.first->GetUserTypeListSize() - 1; j >= 0; --j) {
							for (int k = utTemp.first->GetUserTypeList(j).GetCount() - 1; k >= 0; --k) {
								UserType* x = utTemp.first->GetUserTypeList(j).Get(k);
								utDeck.push_front(make_pair(x, utTemp.second + 1));
							}
						}
					}
					else if (utTemp.second < strVec.size() && utTemp.first->GetUserTypeItemRef(strVec[utTemp.second], utTemp2)) {
						for (int j = utTemp2.GetCount() - 1; j >= 0; --j) {
							utDeck.push_front(make_pair(utTemp2.Get(j), utTemp.second + 1));
						}
					}

					if (utTemp.second == strVec.size()) {
						exist = true;
						temp.push_back(utTemp.first);
					}
				}
				if (false == exist) { return{ false, vector<UserType*>() }; }
				return{ true, temp };
			}
		public:
			// To Do
			// AddSpace : return string?
			static string AddSpace(const string& str)
			{
				string temp;

				for (int i = 0; i < str.size(); ++i)
				{
					if ('=' == str[i]) {
						temp.push_back(' ');
						temp.push_back('=');
						temp.push_back(' ');
					}
					else if ('{' == str[i]) {
						temp.push_back(' ');
						temp.push_back('{');
						temp.push_back(' ');
					}
					else if ('}' == str[i]) {
						temp.push_back(' ');
						temp.push_back('}');
						temp.push_back(' ');
					}
					else {
						temp.push_back(str[i]);
					}
				}

				return temp;
			}
			/// need testing!
			static string PassSharp(const string& str) {
				string temp;
				int state = 0;

				for (int i = 0; i < str.size(); ++i) {
					if (str[i] == '#') { state = 1; }
					else if (str[i] == '\n') { state = 0; }

					if (0 == state) {
						temp.push_back(str[i]);
					}
				}
				return temp;
			}
			/// need to rename!
			static string ChangeSpace(const string& str, const char result_ch) {
				string temp;
				int state = 0;

				for (int i = 0; i < str.size(); ++i)
				{
					if (0 == state && '\"' == str[i]) {
						state = 1;
						temp.push_back(str[i]);
					}
					else if (1 == state && '\"' != str[i] && (' ' == str[i] || '\t' == str[i])) {
						state = 1;
						temp.push_back(result_ch);
					}
					else if (1 == state && '\"' == str[i]) {
						state = 0;
						temp.push_back('\"');
					}
					else
					{
						temp.push_back(str[i]);
					}
				}

				return temp;
			}

			static void ChangeCharInString(string& str, const char target_ch, const char result_ch)
			{
				for (int i = 0; i < str.size(); ++i)
				{
					if (str[i] == target_ch)
					{
						str[i] = result_ch;
					}
				}
			}

			static void ReplaceAll(UserType* temp, const char target_ch, const char result_ch) {
				const int itemListSize = temp->GetItemListSize();
				const int userTypeListSize = temp->GetUserTypeListSize();

				for (int i = 0; i < itemListSize; ++i) {
					TypeArray<std::string>& itemList = temp->GetItemList(i);
					for (int j = 0; j < itemList.GetCount(); ++j) {
						ChangeCharInString(itemList.Get(j), target_ch, result_ch);
					}
				}
				for (int i = 0; i < userTypeListSize; ++i) {
					for (int j = 0; j < temp->GetUserTypeList(i).GetCount(); ++j) {
						ReplaceAll(temp->GetUserTypeList(i).Get(j), target_ch, result_ch);
					}
				}
			}

		};

		class InFIleReserver
		{
		private:
			ifstream* pInFile;
		public:
			int Num;
		public:
			explicit InFIleReserver(ifstream& inFile)
			{
				pInFile = &inFile;
				Num = 1;
			}
			bool end()const { return pInFile->eof(); }
		public:
			bool operator() (ArrayQueue<string>& strVec)
			{
				return Utility::Reserve2(*pInFile, strVec, Num).second > 0;
			}
		};
		class NoneReserver
		{
		private:
			int count;
		public:
			explicit NoneReserver() : count(0) { }
			bool operator() (ArrayQueue<string>& strVec)
			{
				count = 1;
				return false;
			}
			bool end()const { return 1 == count; }
		};


		class Condition {
		private:
			int braceNum;
			std::vector<string> tokenVec;
			ArrayStack<string> tokenStack;
			string condition;
			int i;
			UserType* position; // need set, get ter!!
			UserType* global;
			int option;
		public:
			explicit Condition(const string& condition, UserType* position, UserType* global, const int option = 0)
				: braceNum(0), condition(condition), i(0), position(position), option(option)
			{
				Init(condition);
			}
		private:
			string reverse(const string& str) {
				string temp;
				for (int i = str.size() - 1; i >= 0; --i) {
					temp.push_back(str[i]);
				}
				return temp;
			}
			bool IsInteger(const string& str) {
				int state = 0;
				for (int i = 0; i < str.size(); ++i) {
					switch (state)
					{
					case 0:
						if ('+' == str[i] || '-' == str[i]) {
							state = 0;
						}
						else if (str[i] >= '0' && str[i] <= '9')
						{
							state = 1;
						}
						else return false;
						break;
					case 1:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 1;
						}
						else return false;
					}
				}
				return true;
			}
			bool IsDouble(const string& str) {
				int state = 0;
				for (int i = 0; i < str.size(); ++i) {
					switch (state)
					{
					case 0:
						if ('+' == str[i] || '-' == str[i]) {
							state = 0;
						}
						else if (str[i] >= '0' && str[i] <= '9')
						{
							state = 1;
						}
						else return false;
						break;
					case 1:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 1;
						}
						else if (str[i] == '.') {
							state = 2;
						}
						else return false;
						break;
					case 2:
						if (str[i] >= '0' && str[i] <= '9') { state = 2; }
						else return false;
						break;
					}
				}
				return true;
			}
			bool IsDate(const string& str)
			{
				int state = 0;
				for (int i = 0; i < str.size(); ++i) {
					switch (state)
					{
					case 0:
						if (str[i] >= '0' && str[i] <= '9')
						{
							state = 1;
						}
						else return false;
						break;
					case 1:
						if (str[i] >= '0' && str[i] <= '9') {
							state = 1;
						}
						else if (str[i] == '.') {
							state = 2;
						}
						else return false;
						break;
					case 2:
						if (str[i] >= '0' && str[i] <= '9') { state = 2; }
						else if (str[i] == '.') {
							state = 3;
						}
						else return false;
						break;
					case 3:
						if (str[i] >= '0' && str[i] <= '9') { state = 3; }
						else return false;
						break;
					}
				}
				return true;
			}
			bool IsMinus(const string& str)
			{
				return str.empty() == false && str[0] == '-';
			}
			string GetType(const string& str) {
				if (IsInteger(str)) { return "INTEGER"; }
				else if (IsDouble(str)) { return "DOUBLE"; }
				else if (IsDate(str)) { return "DATE"; }
				else return "STRING";
			}
			string Compare(const string& str1, const string& str2, const int type = 0)
			{
				string type1 = GetType(str1);
				string type2 = GetType(str2);

				if (type1 != type2) {
					return "ERROR";
				}

				if ("STRING" == type1 || type == 1)
				{
					if (str1 < str2) {
						return "< 0";
					}
					else if (str1 == str2) {
						return "== 0";
					}
					return "> 0";
				}
				else if ("INTEGER" == type1)
				{
					if (IsMinus(str1) && !IsMinus(str2)) { return "< 0"; }
					else if (!IsMinus(str1) && IsMinus(str2)) { return "> 0"; }

					const bool minusComp = IsMinus(str1) && IsMinus(str2);

					if (false == minusComp) {
						string x = reverse(str1);
						string y = reverse(str2);

						if (x[0] == '+') { x = string(x.c_str() + 1); }
						if (y[0] == '+') { y = string(y.c_str() + 1); }

						if (x.size() < y.size()) {
							for (int i = 0; i < y.size() - x.size(); ++i) {
								x.push_back('0');
							}
						}
						else {
							for (int i = 0; i < x.size() - y.size(); ++i) {
								y.push_back('0');
							}
						}
						return Compare(reverse(x), reverse(y), 1);
					}
					else {
						return Compare(string(str2.c_str() + 1), string(str1.c_str() + 1));
					}
				}
				else if ("DOUBLE" == type1)
				{
					StringTokenizer tokenizer1(str1, ".");
					StringTokenizer tokenizer2(str2, ".");

					string x = tokenizer1.nextToken();
					string y = tokenizer2.nextToken();

					string z = Compare(x, y);
					if ("= 0" == z)
					{
						x = tokenizer1.nextToken();
						y = tokenizer2.nextToken();

						if (x.size() < y.size()) {
							for (int i = 0; i < y.size() - x.size(); ++i) {
								x.push_back('0');
							}
						}
						else {
							for (int i = 0; i < x.size() - y.size(); ++i) {
								y.push_back('0');
							}
						}
						return Compare(x, y, 1);
					}
					else
					{
						return z;
					}
				}
				else if ("DATE" == type1)
				{
					StringTokenizer tokenizer1(str1, ".");
					StringTokenizer tokenizer2(str2, ".");

					for (int i = 0; i < 3; ++i) {
						const string x = tokenizer1.nextToken();
						const string y = tokenizer2.nextToken();

						const string comp = Compare(x, y);

						if (comp == "< 0") { return comp; }
						else if (comp == "> 0") { return comp; }
					}
					return "== 0";
				}

				return "ERROR";
			}

			auto Get(const string& var, const string& position, UserType* utPosition, UserType* global) {
				string valTemp = position;
				StringTokenizer tokenizer(position, "/");
				UserType* utTemp = NULL;
				if (false == tokenizer.hasMoreTokens()) {
					utTemp = utPosition;
				}
				else {
					if (tokenizer.nextToken() != "root") {
						utTemp = utPosition;
					}
					else {
						valTemp = string(position.c_str() + 5);
						utTemp = global;
					}
				}
				return Utility::Find(*utTemp, valTemp);
			}
			string GetValue(const string& op, const string& var, const string& val, UserType* utPosition, UserType* global, const string& option = "0")
			{
				if (NULL == utPosition) { return "ERROR"; }
				if ("EXIST" == op) { /// option == 1?	
					auto x = Get(var, val, utPosition, global);
					if (x.first) {
						//if (x.second.size() > 1) { return "ERROR"; } ///
						for (int i = 0; i < x.second.size(); ++i) {
							if (x.second[i]->GetItem(var).GetCount() > 0)
							{
								return "TRUE";
							}
						}
						return "FALSE";
					}
					return "FALSE";
				}
				else if ("NOTEXIST" == op) { /// option == 2 ?
					auto x = Get(var, val, utPosition, global);
					if (x.first) {
						//if (x.second.size() > 1) { return "ERROR"; } ///

						for (int i = 0; i < x.second.size(); ++i) {
							if (0 < x.second[i]->GetItem(var).GetCount())
							{
								return "FALSE";
							}
						}
						return "TRUE";
					}
					return "FALSE";
				}

				return "ERROR";
			}
			string GetValue(const string& op, const string& var1, const string& position1, const string& var2, const string& position2,
				UserType* utPosition, UserType* global, const string& option = "0")
			{
				// COMP<, COMP>, EQ, NOTEQ
				if (NULL == utPosition) { return "ERROR"; }

				auto x = Get(var1, position1, utPosition, global);
				auto y = Get(var2, position2, utPosition, global);

				if (((x.first && x.second.size() > 1) || (y.first && y.second.size() > 1))) {
					return "ERROR";
				}

				TypeArray<string> value1 = x.second[0]->GetItem(var1);
				TypeArray<string> value2 = y.second[0]->GetItem(var2);

				if (value1.GetCount() == 0) {
					value1.Push(var1);
				}
				if (value2.GetCount() == 0) {
					value2.Push(var2);
				}

				if (option == "0" && (value1.GetCount() > 1 || value2.GetCount() > 1)) {
					return "ERROR";
				}

				if ("COMP<" == op) {
					if ("0" != option) { /// ToDo.. // 0. just 1-1, // 1. for any case // 2. for all case
						for (int i = 0; i < value1.GetCount(); ++i) {
							for (int j = 0; j < value2.GetCount(); ++j) {
								if (Compare(value1.Get(i), value2.Get(j)) == "< 0") {
									if ("1" == option) { return "TRUE"; }
								}
								else {
									if ("2" == option) { return "FALSE"; }
								}
							}
						}
						if ("1" == option) { return "FALSE"; }
						else if ("2" == option) {
							return "TRUE";
						}
					}
					else {
						if (Compare(value1.Get(0), value2.Get(0)) == "< 0") {
							return "TRUE";
						}
					}
					return "FALSE";
				}
				else if ("COMP>" == op) {
					if ("0" != option) { /// ToDo.. // 0. just 1-1, // 1. for any case // 2. for all case
						for (int i = 0; i < value1.GetCount(); ++i) {
							for (int j = 0; j < value2.GetCount(); ++j) {
								if (Compare(value1.Get(i), value2.Get(j)) == "> 0") {
									if ("1" == option) { return "TRUE"; }
								}
								else {
									if ("2" == option) { return "FALSE"; }
								}
							}
						}
						if ("1" == option) { return "FALSE"; }
						else if ("2" == option) {
							return "TRUE";
						}
					}
					else {
						if (Compare(value1.Get(0), value2.Get(0)) == "> 0") {
							return "TRUE";
						}
					}
					return "FALSE";
				}
				else if ("EQ" == op) {
					if ("0" != option) { /// ToDo.. // 0. just 1-1, // 1. for any case // 2. for all case
						for (int i = 0; i < value1.GetCount(); ++i) {
							for (int j = 0; j < value2.GetCount(); ++j) {
								if (Compare(value1.Get(i), value2.Get(j)) == "== 0") {
									if ("1" == option) { return "TRUE"; }
								}
								else {
									if ("2" == option) { return "FALSE"; }
								}
							}
						}
						if ("1" == option) { return "FALSE"; }
						else if ("2" == option) {
							return "TRUE";
						}
					}
					else {
						if (Compare(value1.Get(0), value2.Get(0)) == "== 0") {
							return "TRUE";
						}
					}
					return "FALSE";
				}
				else if ("NOTEQ" == op) {
					if ("0" != option) { /// ToDo.. // 0. just 1-1, // 1. for any case // 2. for all case
						for (int i = 0; i < value1.GetCount(); ++i) {
							for (int j = 0; j < value2.GetCount(); ++j) {
								if (Compare(value1.Get(i), value2.Get(j)) != "== 0") {
									if ("1" == option) { return "TRUE"; }
								}
								else {
									if ("2" == option) { return "FALSE"; }
								}
							}
						}
						if ("1" == option) { return "FALSE"; }
						else if ("2" == option) {
							return "TRUE";
						}
					}
					else {
						if (Compare(value1.Get(0), value2.Get(0)) != "== 0") {
							return "TRUE";
						}
					}
					return "FALSE";
				}

				return "ERROR";
			}
			string BoolOperation(const string& op, const string& x, const string& y)
			{
				if (x == "ERROR" || y == "ERROR") { return "ERROR"; }
				if ("NOT" == op) { return x == "TRUE" ? "FALSE" : "TRUE"; }
				else if ("AND" == op) {
					if (x == "TRUE" && y == "TRUE") { return "TRUE"; }
					else {
						return "FALSE";
					}
				}
				else if ("OR" == op) {
					if (x == "TRUE" || y == "TRUE") { return "TRUE"; }
					else {
						return "FALSE";
					}
				}
				else {
					return "ERROR";
				}
			}

			void Init(const string condition)
			{
				bool evalue = false;

				string str = Utility::AddSpace(condition);
				StringTokenizer tokenizer(str, { " ", "\t", "\n", "\r" });

				while (tokenizer.hasMoreTokens()) {
					tokenVec.push_back(tokenizer.nextToken());
				}
			}
		public:
			bool Next() {
				try {
					if (i >= tokenVec.size()) { return false; }
					if (condition.empty()) {
						return false;
					}
					{
						if ("=" != tokenVec[i]
							&& "{" != tokenVec[i]
							&& "}" != tokenVec[i]) {
							tokenStack.push(tokenVec[i]);
						}

						if ("{" == tokenVec[i]) {
							braceNum++;
						}
						else if ("}" == tokenVec[i]) {
							braceNum--;

							// COMP<, COMP>, EQ, NOTEQ
							if (tokenStack.size() >= 6 && ("COMP<" == tokenStack[tokenStack.size() - 6]
								|| "COMP>" == tokenStack[tokenStack.size() - 6]
								|| "EQ" == tokenStack[tokenStack.size() - 6]
								|| "NOTEQ" == tokenStack[tokenStack.size() - 6]))
							{
								const string op = tokenStack[tokenStack.size() - 6];
								const string var1 = tokenStack[tokenStack.size() - 5];
								const string position1 = tokenStack[tokenStack.size() - 4];
								const string var2 = tokenStack[tokenStack.size() - 3];
								const string position2 = tokenStack[tokenStack.size() - 2];
								const string option = tokenStack[tokenStack.size() - 1];
								tokenStack.pop();
								tokenStack.pop();
								tokenStack.pop();
								tokenStack.pop();
								tokenStack.pop();
								tokenStack.pop();

								tokenStack.push(GetValue(op, var1, position1, var2, position2, position, global, option));
							}
							else if (tokenStack.size() >= 3 && "NOT" != tokenStack[tokenStack.size() - 2]) {
								string op = tokenStack[tokenStack.size() - 3];
								const string operand1 = tokenStack[tokenStack.size() - 2];
								const string operand2 = tokenStack[tokenStack.size() - 1];

								if (op == "AND" || op == "OR") {
									tokenStack.pop();
									tokenStack.pop();
									tokenStack.pop();

									tokenStack.push(BoolOperation(op, operand1, operand2));
								}
								else { // EXIST, NOTEXIST 
									tokenStack.pop();
									tokenStack.pop();
									tokenStack.pop();

									tokenStack.push(GetValue(op, operand1, operand2, position, global));
								}
							}
							else if (tokenStack.size() >= 2 && "NOT" == tokenStack[tokenStack.size() - 2]) {
								const string op = tokenStack[tokenStack.size() - 2];
								const string operand1 = tokenStack[tokenStack.size() - 1];

								tokenStack.pop();
								tokenStack.pop();
								tokenStack.push(BoolOperation(op, operand1, operand1));
							}
						}
						++i;
					}
				}
				catch (const char* err) { cout << err << endl; return false; }
				catch (Error& e) { cout << e << endl; return false; }
				return true;
			}
		public:
			int NowIndex()const { return i; }
			const ArrayStack<string>& Now()const { return tokenStack; }
			ArrayStack<string>& Now() { return tokenStack; }
			bool IsNowImportant() const {
				return "}" == tokenVec[i] && "NOT" != tokenStack[tokenStack.size() - 2];
			}
		};


		class LoadData
		{
			/// core
		private:
			template <class Reserver>
			static void _LoadData(ArrayQueue<string>& strVec, Reserver& vecReserver, UserType& global) // first, strVec.empty() must be true!!?
			{
				int state = 0;
				int braceNum = 0;
				stack<int> state_reserve;
				stack<int> do_reserve;
				vector< UserType* > nestedUT(1);
				string var1, var2, val;

				bool varOn = false;

				nestedUT[0] = &global;
				{
					vecReserver(strVec);

					while (strVec.empty())
					{
						vecReserver(strVec);
						if (strVec.empty() && vecReserver.end()) {
							throw "Err nextToken does not exist";
						}
					}
				}

				while (false == strVec.empty()) {
					switch (state)
					{
					case 0:
						if (LEFT == Utility::Top(strVec)) {
							Utility::Pop(strVec);
							state = 1;
						}
						else {
							pair<bool, string> bsPair = Utility::LookUp(strVec, 1);
							if (bsPair.first) {
								if (EQ == bsPair.second) {
									var2 = Utility::Pop(strVec);
									Utility::Pop(strVec);
									state = 2;
								}
								else {
									var1 = Utility::Pop(strVec);
									nestedUT[braceNum]->AddItem("", var1);
									state = 0;
								}
							}
							else {
								var1 = Utility::Pop(strVec);
								nestedUT[braceNum]->AddItem("", var1);
								state = 0;
							}
						}
						break;
					case 1:
						if (RIGHT == Utility::Top(strVec)) {
							Utility::Pop(strVec);
							state = 0;
						}
						else {
							// syntax error.
							throw "syntax error 1 ";
						}
						break;
					case 2:
						if (LEFT == Utility::Top(strVec)) {
							Utility::Pop(strVec);

							///
							nestedUT[braceNum]->AddUserTypeItem(UserType(var2));
							TypeArray<UserType*> pTemp;
							nestedUT[braceNum]->GetUserTypeItemRef(var2, pTemp);

							braceNum++;

							/// new nestedUT
							if (nestedUT.size() == braceNum) /// changed 2014.01.23..
								nestedUT.push_back(NULL);

							/// initial new nestedUT.
							nestedUT[braceNum] = pTemp.Get(pTemp.GetCount() - 1);
							///
							state = 3;
						}
						else {
							val = Utility::Pop(strVec);

							nestedUT[braceNum]->AddItem(var2, val);

							state = 0;
						}
						break;
					case 3:
						if (RIGHT == Utility::Top(strVec)) {
							Utility::Pop(strVec);
							//
							nestedUT[braceNum] = NULL;
							braceNum--;

							state = 0;
						}
						else {
							/*
							pair<bool, int> biPair = IsMatched(strVec, "{", "}", 0, 1);
							if (false == biPair.first) {
							bool biPair2 = vecReserver(strVec);

							while (!biPair.first && biPair2)
							{
							biPair2 = vecReserver(strVec);
							biPair = IsMatched(strVec, "{", "}", 0, 1);
							}
							}
							if (biPair.first)
							*/
							{
								/// uisng struct?
								state_reserve.push(0);
								do_reserve.push(RIGHT_DO);
								state = 4;
							}
							//else
							{
								//	throw  "syntax error 2 ";
							}
						}
						break;
					case 4:
						if (LEFT == Utility::Top(strVec)) {
							Utility::Pop(strVec);

							nestedUT[braceNum]->AddUserTypeItem(UserType(""));
							TypeArray<UserType*> pTemp;
							nestedUT[braceNum]->GetUserTypeItemRef("", pTemp);

							braceNum++;

							/// new nestedUT
							if (nestedUT.size() == braceNum) /// changed 2014.01.23..
								nestedUT.push_back(NULL);

							/// initial new nestedUT.
							nestedUT[braceNum] = pTemp.Get(pTemp.GetCount() - 1);
							///
							//}

							state = 5;
						}
						else if (RIGHT == Utility::Top(strVec)) {
							Utility::Pop(strVec);
							state = state_reserve.top();
							state_reserve.pop();
							const int do_id = do_reserve.top();
							do_reserve.pop();
							//if (do_id == RIGHT_DO)
							{
								nestedUT[braceNum] = NULL;
								braceNum--;
							}
						}
						else {
							pair<bool, string> bsPair = Utility::LookUp(strVec, 1);
							if (bsPair.first) {
								if (EQ == bsPair.second) {
									// var2
									var2 = Utility::Pop(strVec);
									Utility::Pop(strVec); // pass EQ
									state = 6;
								}
								else {
									// var1
									var1 = Utility::Pop(strVec);
									nestedUT[braceNum]->AddItem("", var1);

									state = 4;
								}
							}
							else
							{
								// syntax error..
								throw "syntax error 3 ";
							}
						}
						break;
					case 5:
						if (RIGHT == Utility::Top(strVec)) {
							Utility::Pop(strVec);
							//
							//if (flag1 == 0) {
							nestedUT[braceNum] = NULL;
							braceNum--;
							// }
							//
							state = 4;
						}

						else {
							int idx = -1;
							int num = -1;

							/*(pair<bool, int> biPair = IsMatched(strVec, "{", "}", 0, 1, &idx, &num);
							if (false == biPair.first) {
							bool biPair2 = vecReserver(strVec);

							while (!biPair.first && biPair2)
							{
							biPair2 = vecReserver(strVec);

							biPair = IsMatched(strVec, "{", "}", idx + 1, num, &idx, &num);
							}
							}
							if (biPair.first)
							*/
							{
								/// uisng struct?
								state_reserve.push(4);

								{
									do_reserve.push(RIGHT_DO);
								}
								state = 4;
							}
							//else
							{
								//	throw "syntax error 4  ";
							}
						}
						break;
					case 6:
						if (LEFT == Utility::Top(strVec)) {
							Utility::Pop(strVec);

							///
							{
								nestedUT[braceNum]->AddUserTypeItem(UserType(var2));
								TypeArray<UserType*> pTemp;
								nestedUT[braceNum]->GetUserTypeItemRef(var2, pTemp);

								braceNum++;

								/// new nestedUT
								if (nestedUT.size() == braceNum) /// changed 2014.01.23..
									nestedUT.push_back(NULL);

								/// initial new nestedUT.
								nestedUT[braceNum] = pTemp.Get(pTemp.GetCount() - 1);
							}
							///
							state = 7;
						}
						else {
							val = Utility::Pop(strVec);

							nestedUT[braceNum]->AddItem(var2, val);
							if (strVec.empty())
							{
								//
							}
							else if (RIGHT == Utility::Top(strVec)) {
								Utility::Pop(strVec);

								{
									state = state_reserve.top();
									state_reserve.pop();
									const int do_id = do_reserve.top();
									do_reserve.pop();
									//if (do_id == RIGHT_DO)
									{
										nestedUT[braceNum] = NULL;
										braceNum--;
									}
								}
								{
									//state = 4;
								}
							}
							else {
								state = 4;
							}
						}
						break;
					case 7:
						if (RIGHT == Utility::Top(strVec)) {
							Utility::Pop(strVec);
							//

							nestedUT[braceNum] = NULL;
							braceNum--;
							//
							state = 4;
						}
						else {
							int idx = -1;
							int num = -1;
							/*
							pair<bool, int> biPair = IsMatched(strVec, "{", "}", 0, 1, &idx, &num);
							if (false == biPair.first) {
							bool biPair2 = vecReserver(strVec);

							while (!biPair.first && biPair2)
							{
							biPair2 = vecReserver(strVec);

							biPair = IsMatched(strVec, "{", "}", idx + 1, num, &idx, &num);
							}
							}
							if (biPair.first) */
							{
								/// uisng struct?
								state_reserve.push(4);

								{
									do_reserve.push(RIGHT_DO);
								}
								state = 4;
							}
							//else
							{
								//throw "syntax error 5 ";
							}
						}
						break;
					default:
						// syntax err!!

						throw "syntax error 6 ";
						break;
					}

					if (strVec.size() < 10) {
						vecReserver(strVec);

						while (strVec.empty())
						{
							vecReserver(strVec);
							if (strVec.empty() && vecReserver.end()) {
								// throw "Err nextToken does not exist2";
								break;
							}
						}
					}
				}
				if (state != 0) {
					throw "error final state is not 0!";
				}
				if (braceNum != 0) {
					throw string("chk braceNum is ") + toStr(braceNum);
				}
			}

		public:
			static bool LoadDataFromFile(const string& fileName, UserType& global) /// global should be empty?
			{
				ifstream inFile;
				inFile.open(fileName, ios::binary);
				if (true == inFile.fail())
				{
					inFile.close(); return false;
				}
				UserType globalTemp = global;
				ArrayQueue<string> strVec;

				try {
					Utility::Reserve2(inFile, strVec, 10000);
					InFIleReserver ifReserver(inFile);

					ifReserver.Num = 10000;
					_LoadData(strVec, ifReserver, globalTemp);

					inFile.close();
				}
				catch (Error e) { cout << e << endl; inFile.close(); return false; }
				catch (const char* err) { cout << err << endl; inFile.close(); return false; }
				catch (const string& e) { cout << e << endl; inFile.close(); return false; }
				catch (exception e) { cout << e.what() << endl; inFile.close(); return false; }
				catch (...) { cout << "예기치 못한 에러" << endl; inFile.close(); return false; }
				global = globalTemp;
				return true;
			}

			static bool LoadDataFromString(string str, UserType& ut)
			{
				UserType utTemp = ut;
				str = Utility::AddSpace(str);
				str = Utility::PassSharp(str);
				str = Utility::ChangeSpace(str, '^');
				/// ToDp - ""안에 여백이 있을 떄 다른 것으로 대체후 다시 변경
				/// ToDo -  #주석이 있다면? 없애는 함수 제작? - using str.find, String::Substr.
				/// ToDo - error 처리..
				StringTokenizer tokenizer(str, vector<string>{" ", "\t", "\r", "\n"});
				ArrayQueue<string> strVec;

				while (tokenizer.hasMoreTokens())
				{
					strVec.push(tokenizer.nextToken());
				}
				try {
					_LoadData(strVec, NoneReserver(), utTemp);
					Utility::ReplaceAll(&utTemp, '^', ' ');
				}
				catch (Error& e) { cout << e << endl; return false; }
				catch (const char* err) { cout << err << endl; return false; }
				catch (exception& e) { cout << e.what() << endl; return false; }
				catch (...) { cout << "예기치 못한 에러" << endl; return  false; }

				ut = utTemp;
				return true;
			}
		private:
			UserType global; // ToDo - remove!!
		public:
			// InitQuery or LoadQuery
			LoadData() { InitWizDB(); }
			virtual ~LoadData() { }
			//
			bool InitWizDB() {
				global = UserType("global");
				return true;
			}
			// allRemove Query 
			bool AllRemoveWizDB() {
				global = UserType("");
				return true;
			}
			// AddQuery AddData, AddUserTypeData?
			bool AddData(const string& position, const string& data, const string& condition = "") {
				UserType utTemp;

				if (false == LoadDataFromString(data, utTemp))
				{
					return false;
				}
				auto finded = Utility::Find(global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						int item_n = 0;
						int user_n = 0;

						/// chk temp test codes - > using flag? 1->Exist 2->Comparision?
						//if (finded.second[i]->GetItem("base_tax").GetCount() > 0) { continue; }
						///~end
						if (false == condition.empty()) {
							Condition cond(condition, finded.second[i], &global);

							while (cond.Next());

							if ("TRUE" != cond.Now()[0])
							{
								//cout << cond.Now()[0] << endl;
								continue;
							}
						}

						for (int k = 0; k < utTemp.GetIList().size(); ++k) {
							if (utTemp.GetIList()[k] == 1) {
								finded.second[i]->AddItemList(utTemp.GetItemList(item_n));
								item_n++;
							}
							else if (utTemp.GetIList()[k] == 2) {
								finded.second[i]->AddUserTypeList(utTemp.GetUserTypeList(user_n));
								user_n++;
							}
						}
					}
					return true;
				}
				else {
					return false;
				}
			}
			// SetQuery
			bool SetData(const string& position, const string& varName, const string& data, const string& condition = "")
			{
				auto finded = Utility::Find(global, position);

				if (finded.first) {
					/// todo - if varName is "" then data : val val val ... 
					if (varName == "") {
						UserType utTemp;
						if (false == LoadDataFromString(data, utTemp)) {
							return false;
						}
						const int n = utTemp.GetItem("").GetCount();
						for (int i = 0; i < finded.second.size(); ++i) {
							if (false == condition.empty()) {
								Condition cond(condition, finded.second[i], &global);

								while (cond.Next());

								if ("TRUE" != cond.Now()[0])
								{
									//	cout << cond.Now()[0] << endl;
									continue;
								}
							}
							finded.second[i]->RemoveItemList("");

							for (int j = 0; j < n; ++j) {
								finded.second[i]->AddItem("", utTemp.GetItem("").Get(j));
							}
						}

						return true;
					}
					else {
						for (int i = 0; i < finded.second.size(); ++i) {
							if (false == condition.empty()) {
								Condition cond(condition, finded.second[i], &global);

								while (cond.Next());

								if ("TRUE" != cond.Now()[0])
								{
									//	cout << cond.Now()[0] << endl;
									continue;
								}
							}
							finded.second[i]->SetItem(varName, data); /// chk??
						}

						return true;
					}
				}
				else {
					return false;
				}
			}
			/// 
			string GetData(const string& position, const string& condition) {
				string str;
				auto finded = Utility::Find(global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						if (false == condition.empty()) {
							Condition cond(condition, finded.second[i], &global);

							while (cond.Next());

							if ("TRUE" != cond.Now()[0])
							{
								//	cout << cond.Now()[0] << endl;
								continue;
							}
						}
						str = str + finded.second[i]->ToString() + "\n";
					}
					return str;
				}
				else {
					return "";
				}
			}

			/// varName = val - do
			/// varName = { val val val } - GetData(position+"/varName", ""); 
			/// varName = { var = val } - GetData(position+"/varname", var);
			string GetData(const string& position, const string& varName, const string& condition) // ??
			{
				string str;
				auto finded = Utility::Find(global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						if (false == condition.empty()) {
							Condition cond(condition, finded.second[i], &global);

							while (cond.Next());

							if ("TRUE" != cond.Now()[0])
							{
								//	cout << cond.Now()[0] << endl;
								continue;
							}
						}

						str = str + finded.second[i]->GetItem(varName).ToString() + "\n";
					}
				}
				return str;
			}
			/*
			bool RemoveData(const string& position) {
			auto finded = Find(global, position);
			if (finded.first) {
			for (int i = 0; i < finded.second.size(); ++i) {
			finded.second[i]->Remove(); // todo - 내부..
			}
			return true;
			}
			else {
			return false;
			}
			}
			*/
			bool Remove(const string& position, const string& var, const string& condition) {
				auto finded = Utility::Find(global, position);
				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						UserType* temp = finded.second[i];

						if (false == condition.empty()) {
							Condition cond(condition, finded.second[i], &global);

							while (cond.Next());

							if ("TRUE" != cond.Now()[0])
							{
								cout << cond.Now()[0] << endl;
								continue;
							}
						}

						temp->RemoveItemList(var);
						temp->RemoveUserTypeList(var);
					}
					return true;
				}
				else {
					return false;
				}
			}
			bool LoadWizDB(const string& fileName) {
				UserType globalTemp;
				// preprocessing
				Utility::PassSharp(fileName, "output.txt");
				cout << "PassSharp End" << endl;
				Utility::AddSpace("output.txt", "output2.txt");
				cout << "AddSpace End" << endl;

				{
					ifstream inFile;
					ofstream outFile;

					inFile.open("output2.txt", ios::binary);
					outFile.open("output3.txt", ios::binary);

					string temp;
					vector<string> strVec;

					while (getline(inFile, temp))
					{
						if (temp.empty() || temp == " ") { continue; }
						StringTokenizer tokenizer(temp, "=");
						const int num = tokenizer.countTokens();
						static int counter = 0;
						const bool existEQ = tokenizer.isFindExist();


						for (int i = 0; i < num; ++i) {
							if (i == 1 && existEQ) { outFile << " = "; }
							string str = tokenizer.nextToken();
							int idx = getFirstIndex(str, '\"');

							if (idx != -1) {
								for (int j = idx + 1; j < str.size(); ++j) {
									// chk - maybe has bug!
									if (str[j] == '\"') { break; }
									if (str[j] == ' ' || str[j] == '\t') {
										str[j] = '^';
									}
								}
							}
							if (existEQ && num == 1 && i == 0)
							{
								outFile << str << " = ";
							}
							else if (existEQ && num == 2 && i == 1) {
								for (int j = 0; j < str.size(); ++j)
								{
									if (str[j] == '{') {
										outFile << " { \n";
									}
									else if (str[j] == '}') {
										outFile << " } ";
									}
									else
									{
										outFile << str[j];
									}

								}
								outFile << " ";
							}
							else if (!existEQ && num == 1 && i == 0)
							{
								for (int j = 0; j < str.size(); ++j)
								{
									if (str[j] == '{') {
										outFile << " { \n";
									}
									else if (str[j] == '}') {
										outFile << " } ";
									}
									else
									{
										outFile << str[j];
									}

								}
								outFile << " ";
							}
							else {
								for (int j = 0; j < str.size(); ++j)
								{
									if (str[j] == '{') {
										outFile << " { \n";
									}
									else if (str[j] == '}') {
										outFile << " } ";
									}
									else if (str[j] == '=') {
										outFile << " = ";
									}
									else
									{
										outFile << str[j];
									}

								}
								outFile << " ";
							}
						}
						outFile << "\n";
					}
					inFile.close();
					outFile.close();
				}
				cout << "space in \"~\" -> ^ End" << endl;

				{
					ifstream inFile;
					inFile.open("output3.txt", ios::binary); // ~2

					int count = 0;
					int count2 = 0;
					string temp;
					while (inFile >> temp)
					{
						if (temp == "{") { count2++; }
						if (temp == "}") { count2--; }
						for (int i = 0; i < temp.size(); ++i)
						{
							if (temp[i] == '{') { count++; }
							else if (temp[i] == '}') { count--; }
						}
					}
					inFile.close();
					cout << count << " " << count2 << endl;
				}
				cout << "chk end" << endl;
				//	getch();

				// Scan + Parse
				if (false == LoadDataFromFile("output3.txt", globalTemp)) { return false; }
				cout << "LoadData End" << endl;

				/// ToDo - Change ^ to ' '
				{
					// for all, remove ^ in val
					Utility::ReplaceAll(&globalTemp, '^', ' ');
				}
				cout << "remove ^ end" << endl;
				global = globalTemp;
				return true;
			}
			// SaveQuery
			bool SaveWizDB(const string& fileName) {
				ofstream outFile;
				outFile.open(fileName + "temp", ios::binary);
				if (outFile.fail()) { return false; }

				outFile << global; /// SaveFile( fileName, data, use option 1 or 2? )

				outFile.close();

				{ // for eu4
					ifstream inFile;
					ofstream outFile;
					inFile.open(fileName + "temp", ios::binary);
					outFile.open(fileName, ios::binary);
					string temp;
					int line_size = 0;
					int line_count = 0;
					{
						while (getline(inFile, temp)) { line_size++; }
						inFile.close();
					}
					inFile.open(fileName + "temp", ios::binary);
					for (int i = 0; i < line_size; ++i)
					{
						getline(inFile, temp);
						if (temp == "") { continue; }
						int count = 0;
						for (const char ch : temp)
						{
							if (isWhitespace(ch)) { count++; }
						}
						if (count == temp.size()) { continue; }
						if (inFile.eof()) { break; }
						outFile << temp;
						//cout << temp << endl;
						if (i < line_size - 1) {
							outFile << "\n";
						}
					}

					inFile.close();
					outFile.close();
				}
				return true;
			}

			bool ChkData()
			{
				return Utility::ChkData(&global);
			}
		};
	}
}


#endif // LOAD_DATA_H_INCLUDED
