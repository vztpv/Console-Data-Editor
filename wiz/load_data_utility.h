
#ifndef LOAD_DATA_UTILITY_H
#define LOAD_DATA_UTILITY_H

#include <string>
#include <vector>
#include <fstream>
using namespace std;

namespace wiz {
	namespace load_data {

		class Utility
		{
		public:
			/// todo bool ,more stable!!
			static void AddSpace(const string& file1Name, const string& file2Name) {
				ifstream inFile;
				ofstream outFile;

				inFile.open(file1Name);
				outFile.open(file2Name);

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

				inFile.open(file1Name);
				if (inFile.fail()) { return false; }
				outFile.open(file2Name);
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
			static std::pair<bool, vector< UserType*> > Find(UserType* global, const string& position) /// option, option_offset
			{
				vector< UserType* > temp;
				if (position.empty()) { temp.push_back(global); return{ true, temp }; }

				StringTokenizer tokenizer(position, "/");
				vector<string> strVec;
				Deck<pair< UserType*, int>> utDeck;
				pair<UserType*, int> utTemp;
				utTemp.first = global;
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

	}
}

#endif