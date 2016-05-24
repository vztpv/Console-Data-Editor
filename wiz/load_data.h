
#ifndef LOAD_DATA_H_INCLUDED
#define LOAD_DATA_H_INCLUDED


#include <vector>
#include <stack>
#include <fstream>
#include <string>
#include <utility>
using namespace std;

#include <wiz/global.h>
#include <wiz/Dictionary.h> /// change to map?

#include <wiz/cpp_string.h>
#include <wiz/queues.h>
#include <wiz/stacks.h>
#include <wiz/deck.h>

#include <wiz/load_data_types.h>
#include <wiz/load_data_utility.h>
#include <wiz/load_data_reservers.h>
#include <wiz/load_data_condition.h>

namespace wiz {
	namespace load_data {
		const string LEFT = "{";
		const string RIGHT = "}";
		const string EQ = "=";

		const int RIGHT_DO = 1;
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
						if (
							strVec.empty() && 
							vecReserver.end()
							) {
							throw "Err nextToken does not exist"; // cf) or empty? file or empty? string!
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
							TypeArray<UserType*> pTemp;
							nestedUT[braceNum-1]->GetUserTypeItemRef(var2, pTemp);
							if (pTemp.GetCount() > 0)
							{
								pTemp.Get(pTemp.GetCount() - 1)->setValid(false);
							}
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

							UserType temp("");
							
							nestedUT[braceNum]->AddUserTypeItem(temp);
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
							TypeArray<UserType*> pTemp;
							nestedUT[braceNum-1]->GetUserTypeItemRef("", pTemp);
							if (pTemp.GetCount() > 0)
							{
								pTemp.Get(pTemp.GetCount() - 1)->setValid(false);
							}
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
							TypeArray<UserType*> pTemp;
							nestedUT[braceNum-1]->GetUserTypeItemRef(var2, pTemp);
							if (pTemp.GetCount() > 0)
							{
								pTemp.Get(pTemp.GetCount() - 1)->setValid(false);
							}

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

						while ( strVec.size() <= 1 ) // (strVec.empty())
						{
							vecReserver(strVec);
							if (
								//strVec.empty() && 
								vecReserver.end()
								) {
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
					InFIleReserver ifReserver(inFile);

					ifReserver.Num = 100000;
					_LoadData(strVec, ifReserver, globalTemp);
					/// ToDo - Change ^ to ' '
					{
						// for all, remove ^ in val
						Utility::ReplaceAll(&globalTemp,'^', ' ');
					}
					cout << "remove ^ end" << endl;

					inFile.close();
				}
				catch (Error e) { cout << e << endl; inFile.close(); return false; }
				catch (const char* err) { cout << err << endl; inFile.close(); return false; }
				catch (const string& e) { cout << e << endl; inFile.close(); return false; }
				catch (exception e) { cout << e.what() << endl; inFile.close(); return false; }
				catch (...) { cout << "예기치 못한 에러" << endl; inFile.close(); return false; }
				
				global = move( globalTemp );
				return true;
			}

			static bool LoadDataFromString(string str, UserType& ut)
			{
				UserType utTemp = ut;
				str = Utility::PassSharp(str);
				str = Utility::AddSpace(str);
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

				ut = move(utTemp);
				return true;
			}
		private:
			UserType global; // ToDo - remove!
		public:
			/// To Do.. static function -> ~.function.!
			// InitQuery or LoadQuery
			explicit LoadData() { InitWizDB(); }

			/// need testing!
			LoadData(const LoadData& ld)
			{
				global = ld.global;
			}
			virtual ~LoadData() { AllRemoveWizDB(); }

			/// need testing!
			LoadData& operator=(const LoadData& ld)
			{
				if (this == &ld) { return *this; }

				global = ld.global;
				return *this;
			}
			//
			bool InitWizDB() {
				return InitWizDB(global);
			}
			// allRemove Query 
			bool AllRemoveWizDB() {
				return AllRemoveWizDB(global);
			}
			// AddQuery AddData, AddUserTypeData?
			bool AddData(const string& position, const string& data, const string& condition = "") {
				return AddData(global, position, data, condition);
			}
			// 
			bool AddNoNameUserType(const string& position, const string& data, const string& condition = "")
			{
				return AddNoNameUserType(global, position, data, condition);
			}
			// SetQuery
			bool SetData(const string& position, const string& varName, const string& data, const string& condition = "")
			{
				return SetData(global, position, varName, data, condition);
			}
			/// 
			string GetData(const string& position, const string& condition) {
				return GetData(global, position, condition);
			}
			string GetItemListData(const string& position, const string& condition)
			{
				return GetItemListData(global, position, condition);
			}
			string GetItemListNamesData(const string& position, const string& condition)
			{
				return GetItemListNamesData(position, condition);
			}
			string GetUserTypeListNamesData(const string& position, const string& condition) 
			{
				return GetUserTypeListNamesData(global, position, condition);
			}
			/// varName = val - do
			/// varName = { val val val } - GetData(position+"/varName", ""); 
			/// varName = { var = val } - GetData(position+"/varname", var);
			string GetData(const string& position, const string& varName, const string& condition) // ??
			{
				return GetData(global, position, varName, condition);
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
				return Remove(global, position, var, condition);
			}
		
			bool LoadWizDB(const string& fileName) {
				return LoadWizDB(global, fileName);
			}
			// SaveQuery
			bool SaveWizDB(const string& fileName, const string option="0") { /// , int option
				return SaveWizDB(global, fileName, option);
			}

			/// To Do - ExistItem, ExistUserType, SetUserType? GetUserType?
			bool ExistData(const string& position, const string& varName, const string& condition) // ??
			{
				return ExistData(global, position, varName, condition);
			}
			bool ChkData()
			{
				return Utility::ChkData(&global);
			}
			
			/// ToDo - recursive function??
			string SearchItem(const string& var, const string& condition)
			{
				return SearchItem(global, var, condition);
			}
			string SearchUserType(const string& var, const string& condition)
			{
				return SearchUserType(global, var, condition);
			}
		private:
			void SearchItem(vector<string>& positionVec, const string& var, const string& nowPosition,
				UserType* ut, const string& condition)
			{
				SearchItem(global, positionVec, var, nowPosition, ut, condition);
			}
			void SearchUserType(vector<string>& positionVec, const string& var, const string& nowPosition,
				UserType* ut, const string& condition)
			{
				SearchUserType(global, positionVec, var, nowPosition, ut, condition);
			}
		private:
			static void SearchItem(UserType& global, vector<string>& positionVec, const string& var, const string& nowPosition,
				UserType* ut, const string& condition)
			{
				string _var = var;
				if (_var == " ") { _var = ""; }
				if (ut->GetItem(_var).size() > 0) {
					Condition cond(condition, ut, &global);

					while (cond.Next());

					if ("TRUE" == cond.Now()[0])
					{
						positionVec.push_back(nowPosition);
					}
				}

				for (int i = 0; i < ut->GetUserTypeListSize(); ++i) {
					string temp = ut->GetUserTypeList(i).GetName();
					if (temp == "") { temp = " "; }
					for (int j = 0; j < ut->GetUserTypeList(i).GetCount(); ++j) {
						SearchItem( 
							global,
							positionVec,
							_var,
							nowPosition + "/" + temp,
							ut->GetUserTypeList(i).Get(j),
							condition
						);
					}
				}
			}
			static void SearchUserType(UserType& global, vector<string>& positionVec, const string& var, const string& nowPosition,
				UserType* ut, const string& condition)
			{
				string _var = var;
				if (_var == " ") {
					_var = "";
				}
				if (ut->GetName() == _var) {
					Condition cond(condition, ut, &global);

					while (cond.Next());

					if ("TRUE" == cond.Now()[0])
					{
						positionVec.push_back(nowPosition);
					}
				}

				for (int i = 0; i < ut->GetUserTypeListSize(); ++i) {
					string temp = ut->GetUserTypeList(i).GetName();

					if (temp == "") { temp = " "; }
					for (int j = 0; j < ut->GetUserTypeList(i).GetCount(); ++j) {
						SearchUserType(
							global,
							positionVec,
							_var,
							nowPosition + "/" + temp,
							ut->GetUserTypeList(i).Get(j),
							condition
						);
					}
				}
			}

		public:
			static bool InitWizDB(UserType& global) {
				global = UserType("global");
				return true;
			}
			// allRemove Query 
			static bool AllRemoveWizDB(UserType& global) {
				global = UserType("");
				return true;
			}
			// AddQuery AddData, AddUserTypeData?
			static bool AddData(UserType& global, const string& position, const string& data, const string& condition = "") {
				UserType utTemp = UserType("global");
				bool isTrue = false;

				if (false == LoadDataFromString(data, utTemp))
				{
					return false;
				}
				auto finded = Utility::Find(&global, position);
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
						isTrue = true;
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			static bool AddNoNameUserType(UserType& global, const string& position, const string& data, const string& condition = "")
			{
				UserType utTemp = UserType("");
				bool isTrue = false;

				if (false == LoadDataFromString(data, utTemp))
				{
					return false;
				}
				auto finded = Utility::Find(&global, position);
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
						finded.second[i]->AddUserTypeItem(utTemp);

						isTrue = true;
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			static bool AddUserType(UserType& global, const string& position, const string& var, const string& data, const string& condition = "")
			{
				bool isTrue = false;
				auto finded = Utility::Find(&global, position);
				if (finded.first) {
					StringTokenizer tokenizer(var, "/");
					UserType utTemp = UserType("");
					if (false == LoadDataFromString(data, utTemp))
					{
						return false;
					}

					while(tokenizer.hasMoreTokens()) {
						string utName = tokenizer.nextToken();
						if (utName == " ") { utName = ""; }
						
						utTemp.SetName(utName);
						
						for (int i = 0; i < finded.second.size(); ++i) {
							int item_n = 0;
							int user_n = 0;

							/// chk temp test codes - > using flag? 1->Exist 2->Comparision?
							//if (finded.second[i]->GetItem("base_tax").GetCount() > 0) { continue; }
							///~end
							if (false == condition.empty()) {
								string _condition = condition;
								_condition = wiz::String::replace(_condition, "~~", utName); //

								Condition cond(_condition, finded.second[i], &global);

								while (cond.Next());

								if ("TRUE" != cond.Now()[0])
								{
									//cout << cond.Now()[0] << endl;
									continue;
								}
							}

							finded.second[i]->AddUserTypeItem(utTemp);

							isTrue = true;
						}
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			// SetQuery
			static bool SetData(UserType& global, const string& position, const string& varName, const string& data, const string& condition = "")
			{
				auto finded = Utility::Find(&global, position);
				bool isTrue = false;

				if (finded.first) {
					StringTokenizer tokenizer(varName, "/");
					UserType utTemp("");
					if (false == LoadDataFromString(data, utTemp)) {
						return false;
					}
					while( tokenizer.hasMoreTokens() ) {
						string _varName = tokenizer.nextToken();
						/// todo - if varName is "" then data : val val val ... 
						if (_varName == "" || _varName == " ") {
							const int n = utTemp.GetItem("").size();
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
									finded.second[i]->AddItem("", utTemp.GetItem("")[j].Get(0));
								}
								isTrue = true;
							}
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
								finded.second[i]->SetItem(_varName, data); /// chk??
								isTrue = true;
							}
						}
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			/// 
			static string GetData(UserType& global, const string& position, const string& condition) {
				string str;
				auto finded = Utility::Find(&global, position);
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
			static string GetItemListData(UserType& global, const string& position, const string& condition)
			{
				string str;
				auto finded = Utility::Find(&global, position);
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
						str = str + finded.second[i]->ItemListToString() + "\n";
					}
					return str;
				}
				else {
					return "";
				}
			}
			static string GetItemListNamesData(UserType& global, const string& position, const string& condition)
			{
				string str;
				auto finded = Utility::Find(&global, position);
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
						str = str + finded.second[i]->ItemListNamesToString() + "\n";
					}
					return str;
				}
				else {
					return "";
				}
			}
			static string GetUserTypeListNamesData(UserType& global, const string& position, const string& condition)
			{
				string str;
				auto finded = Utility::Find(&global, position);
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
						str = str + finded.second[i]->UserTypeListNamesToString() + "\n";
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
			static string GetData(UserType& global, const string& position, const string& varName, const string& condition) // ??
			{
				string str;
				string _var = varName;
				if (_var == " ") { _var = ""; }

				auto finded = Utility::Find(&global, position);
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

						const int num = finded.second[i]->GetItem(_var).size();
						for (int k = 0; k < num; ++k) {
							str = str + finded.second[i]->GetItem(_var)[k].Get(0) + "\n";
						}
					}
				}
				return str;
			}
			
			/// todo - SetUserTypeName?? // 합치는 함수??

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
			static bool Remove(UserType& global, const string& position, const string& var, const string& condition) {
				auto finded = Utility::Find(&global, position);
				bool isTrue = false;
				
				if (finded.first) {
					StringTokenizer tokenizer(var, "/");
					while (tokenizer.hasMoreTokens()) {
						string _var = tokenizer.nextToken();
						if (_var == " ") { _var = ""; }

						for (int i = 0; i < finded.second.size(); ++i) {
							UserType* temp = finded.second[i];

							if (false == condition.empty()) {
								Condition cond(condition, finded.second[i], &global);

								while (cond.Next());

								if ("TRUE" != cond.Now()[0])
								{
									// cout << cond.Now()[0] << endl;
									continue;
								}
							}

							temp->RemoveItemList(_var);
							temp->RemoveUserTypeList(_var);
							isTrue = true;
						}
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			static bool Remove(UserType& global, const string& position, const string& condition) {
				auto finded = Utility::Find(&global, position);
				bool isTrue = false;


				if (finded.first) {
					for (int i = 0; i < finded.second.size(); ++i) {
						UserType* temp = finded.second[i];

						if (false == condition.empty()) {
							Condition cond(condition, finded.second[i], &global);

							while (cond.Next());

							if ("TRUE" != cond.Now()[0])
							{
								// cout << cond.Now()[0] << endl;
								continue;
							}
						}

						temp->Remove();
						isTrue = true;
					}
					return isTrue;
				}
				else {
					return false;
				}
			}
			static bool LoadWizDB(UserType& global, const string& fileName) {
				UserType globalTemp = UserType("global");
				// preprocessing
				//Utility::PassSharp(fileName, "output.txt");
				//cout << "PassSharp End" << endl;
				//Utility::AddSpace("output.txt", "output2.txt");
				//cout << "AddSpace End" << endl;
				/*
				{
				ifstream inFile;
				ofstream outFile;

				inFile.open("output2.txt");
				outFile.open("output3.txt");

				string temp;

				/// chk maybe has bugs!, - "abcd" - ok, "abc""def" - not ok.
				while (inFile >> temp)
				{
				if (temp.empty()) { continue; }
				int idx = 0;
				if ((idx = temp.find('\"')) != string::npos) {
				if ((idx = temp.find('\"', idx + 1)) != string::npos) {
				outFile << temp;
				}
				else {
				outFile << temp;
				outFile << '^';

				while (inFile >> temp) {
				if ((idx = temp.find('\"')) != string::npos) {
				outFile << temp; outFile << " ";
				break;
				}
				else {
				outFile << temp;
				outFile << '^';
				}
				}
				}
				}
				else {
				outFile << temp;
				}
				outFile << " ";
				}
				inFile.close();
				outFile.close();
				}
				cout << "space in \"~\" -> ^ End" << endl;
				*/
				/*	{
				ifstream inFile;
				inFile.open("output3.txt"); // ~2

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
				*/
				//	getch();

				// Scan + Parse  // output3.txt
				if (false == LoadDataFromFile(fileName, globalTemp)) { return false; }
				cout << "LoadData End" << endl;

				global = move(globalTemp);
				return true;
			}
			// SaveQuery
			static bool SaveWizDB(UserType& global, const string& fileName, const string option = "0") { /// , int option
				ofstream outFile;
				if (fileName.empty()) { return false; }
				outFile.open(fileName + "temp", ios::binary);
				if (outFile.fail()) { return false; }

				/// saveFile
				if (option == "0")
					outFile << global; /// SaveFile( fileName, data, use option 1 or 2? )
				else if (option == "1") // for eu4.
					global.Save1(outFile); // cf) friend?
				else if (option == "2")
					global.Save2(outFile);

				outFile.close();

				{ // for eu4, last line remove!
					ifstream inFile;
					ofstream outFile;
					inFile.open(fileName + "temp");
					outFile.open(fileName);

					if (outFile.fail()) { inFile.close(); return false; }

					string temp;
					int line_size = 0;
					int line_count = 0;
					{
						while (getline(inFile, temp)) { line_size++; }
						inFile.close();
					}
					inFile.open(fileName + "temp");
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

			/// To Do - ExistItem, ExistUserType, SetUserType? GetUserType?
			static bool ExistData(UserType& global, const string& position, const string& varName, const string& condition) // ??
			{
				int count = 0;
				string _var = varName;
				if (_var == " ") { _var = ""; }

				auto finded = Utility::Find(&global, position);
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
						count = count + (finded.second[i]->GetItem(_var).size());
					}
				}
				return 0 != count;
			}
			static bool ChkData(UserType& global)
			{
				return Utility::ChkData(&global);
			}

			/// ToDo - global, position, var, condition + var is " "!
			// "root" -> position.
			static string SearchItem(UserType& global, const string& var, const string& condition)
			{
				vector<string> positionVec;
				string temp;

				SearchItem(global, positionVec, var, "root", &global, condition);

				for (int i = 0; i < positionVec.size(); ++i)
				{
					temp = temp + positionVec[i] + "\n";
				}

				return temp;
			}
			static string SearchUserType(UserType& global, const string& var, const string& condition)
			{
				vector<string> positionVec;
				string temp;

				SearchUserType(global, positionVec, var, "root", &global, condition);

				for (int i = 0; i < positionVec.size(); ++i)
				{
					temp = temp + positionVec[i] + "\n";
				}

				return temp;
			}
		};
	}
}


#endif // LOAD_DATA_H_INCLUDED
