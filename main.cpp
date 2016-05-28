
#include <iostream>
using namespace std;

#include <wiz/load_data.h>

#include <conio.h>
#include <Windows.h>


void FFLUSH() 
{
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail(), '\n');
}

void gotoxy(short x, short y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void setcolor(int color, int bgcolor)
{
	color = color & 0xF;
	bgcolor = bgcolor & 0xF;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
										(bgcolor << 4) | color);
}


void test2() {
	wiz::load_data::LoadData global;
	
	global.InitWizDB();

	global.AddData("", "x= { y = { ival = 0 } }");
	global.AddData("x/", "y={ id = 1 name = \"country AAA\" }");

	cout << (global.GetData("","")) << endl;

	global.AddData("", "player={ { x= 0 } }");
	//global.AddData("player/ ", "z = { x = 1 }");
	global.AddNoNameUserType("player/", "x=1" );
	cout << (global.GetData("player/", " ", "")) << endl;
	cout << "------------------------------------" << endl;
	cout << (global.GetData("","")) << endl;

	global.AddData("", "x={z=3}");

	cout << (global.GetData("","")) << endl;

	global.AddData("", "x={ y= 1 }");

	cout << (global.GetData("", "")) << endl;

	global.AddData("", "z=1");
	global.AddData("", "z={id=3 }");
	global.AddData("", "x={1 2 3 4}");

	cout << (global.GetData("", "")) << endl;

	///LoadData::SetData("x/y/", "id", "2"); // SetData( "x/y", "id", "3" );
	global.SetData("$", "id", "2", "");
	global.SetData("$/$/", "id", "4", "");
	cout << (global.GetData("", "")) << endl;

	global.SetData("", "x", "3", "");
	global.SetData("x", "", "2 3 4 5", "");

	cout << (global.GetData("", "")) << endl;

	///LoadData::RemoveData("x/y/");
	//if (false == LoadData::Remove("x/", "y")) { cout << "err" << endl; }

	cout << (global.GetData("x/", "y", "")) << endl;

	cout << (global.GetData("", "")) << endl;

	global.AddData("", "x={y=3}");

	cout << (global.GetData("", "TRUE")) << endl;

	global.SaveWizDB("test.txt");

	global.AllRemoveWizDB();
}


void eu4Test()
{

	wiz::load_data::LoadData global;
	for (int i = 0; i < 1; ++i) {
		global.InitWizDB();

		global.LoadWizDB("input.eu4"); // eu4
		cout << "load end" << endl;

		//if (false == LoadData::AddData("provinces/$/", "base_tax = 1.000", /// 1.000
		//						"NOTEXIST = { base_tax / }"))
		if (false == global.AddData("provinces/$/", "base_tax_is_5.000 = yes",
			"COMP> = { base_tax / 5.000 ~ 0 }")) {
			cout << "no found.." << endl;
			//_getch();
		}

		//LoadData::Remove("provinces/$/", "history", "NOTEQ = { core / \"RUS\" / }", 2); // 모든 경우에 다 다른다면?
		global.Remove("provinces/$/", "history", "AND = { EQ = { core / \"RUS\" ~ 1 }"
			"NOTEQ = { core / \"PLT\" ~ 2 } }");
		// ToDo - global.Remove("provinces/$/history/", "AND = { ISDATE _COMP> = { 1899.12.31 }  });
		cout << global.GetData("provinces/-100", "TRUE") << endl;

		cout << global.GetItemListNamesData("root/countries/C60/map_color", "TRUE") << endl;

		cout << global.GetUserTypeListNamesData("root/provinces/-100", "TRUE") << endl; 
		 
		//cout << global.SearchUserType("history", "TRUE") << endl;

		global.SaveWizDB("result.eu4", "1"); /// , 0
		cout << "save end" << endl;

		global.AllRemoveWizDB();

		cout << "all end" << endl;
	}
}
void stellarisTest()
{
	wiz::load_data::LoadData global;
	for (int i = 0; i < 1; ++i) {
		global.InitWizDB();

		global.LoadWizDB("gamestate"); // stellaris
		cout << "load end" << endl;

		//	cout << global.GetData("species/ /", "TRUE") << endl;

			// cout << global.SearchItem("name", "TRUE") << endl;
		//cout << "chk" << endl;
		//cout << global.GetData("species/ ",
		//	"EQ = { portrait /identity \"human\" ~ 2 }") << endl;
		//cout << "chk_" << endl;

		global.SaveWizDB("gamestate_result", "1"); /// , 0
		cout << "save end" << endl;

		global.AllRemoveWizDB();

		cout << "all end" << endl;
	}
}
class MData
{
public:
	bool isDir;
	string varName;
	int no; /// for UserType that has same name.!, rename?
public:
	explicit MData(const bool isDir = false, const string& varName = "", const int no = 0) : isDir(isDir), varName(varName), no(no)
	{ 

	}
};

void MStyleTest(const string& fileName)
{
	std::vector<wiz::load_data::TypeArray<wiz::load_data::UserType*>> utVec;
	std::vector<MData> mdVec;
	//std::vector<vector<MData>> mdVec2;
	wiz::load_data::TypeArray<wiz::load_data::UserType*> global;
	wiz::load_data::UserType utTemp;
	std::vector<wiz::load_data::UserType*> utVec2;
	std::vector<int> idxVec; //for store before idx
	std::vector<string> strVec; // for value..
	int braceNum = 0;
	int idx = 0;
	bool isFirst = true;
	int sizeOfWindow = 30;
	int Start = 0;
	int End = 0; 
	int state = 0;

	if (false == wiz::load_data::LoadData::LoadDataFromFile(fileName, utTemp))
	{
		cout << "fail to load data" << endl;
		return;
	}
	
	global.Push(&utTemp);

	utVec.push_back(global);

	utVec2.push_back(&utTemp);

	system("cls");
		
	int count_userType = 0;
	int count_item = 0;

	while (true) {
		if (isFirst) {
			mdVec = std::vector<MData>();
			count_userType = 0;
			count_item = 0;
			
			for (int h = 0; h < utVec[braceNum].size(); ++h) {
				for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
					MData mdTemp{ true, utVec[braceNum].Get(h)->GetUserTypeList(i).GetName(), h };
					if (mdTemp.varName.empty() && utVec[braceNum].Get(h)->GetUserTypeList(i).valid()) {
						mdTemp.varName = " ";
					}
					if (false == ( utVec[braceNum].Get(h)->GetUserTypeList(i).valid()))
					{
						mdTemp.varName = " ";
					}
					mdVec.push_back(mdTemp);
					count_userType++;
				}
			}
			for (int h = 0; h < utVec[braceNum].size(); ++h) {
				for (int i = 0; i < utVec[braceNum].Get(h)->GetItemListSize(); ++i) {
					MData mdTemp{ false, utVec[braceNum].Get(h)->GetItemList(i).GetName(), h };
					if (mdTemp.varName.empty() && utVec[braceNum].Get(h)->GetItemList(i).valid()) {
						mdTemp.varName = " ";
					}
					mdVec.push_back(mdTemp);
					count_item++;
				}
			}			
			setcolor(0, 0);
			system("cls");

			End = min( Start + sizeOfWindow - 1, mdVec.size()-1 );
			if (mdVec.empty()) {
				End = Start - 1;
			}
			// draw mdVec and cursor - chk!!
			else {
				for (int i = Start; i <= End; ++i) {
					if (mdVec[i].isDir) { setcolor(0, 10); }
					else { setcolor(0, 7); }
					if (false == mdVec[i].varName.empty() ) {
						cout << "  " << mdVec[i].varName;
					}
					else
					{
						cout << "  " << " ";
					}
					if (i != mdVec.size() - 1) { cout << endl; }
				}
		
				gotoxy(0, idx - Start);
				
				setcolor(0, 12);
				cout << "●";
				setcolor(0, 0);
				gotoxy(0, 0);
			}

			isFirst = false;
		}

		// move and chk enterkey. - todo!!
		{
			char ch = _getch();
			
			if ('q' == ch) { return; }

			// todo - add, remove, save
			if ( strVec.empty() && Start <= End && idx > 0 && ('w' == ch || 'W' == ch))
			{
				// draw mdVec and cursor - chk!!
				if (idx == Start) {
					system("cls");

					int count = 0;
					int newEnd = Start - 1;
					int newStart = max(0, newEnd - sizeOfWindow + 1);

					Start = newStart; End = newEnd;
					idx--;

					for (int i = Start; i <= End; ++i) {
						if (mdVec[i].isDir) { setcolor(0, 10); }
						else { setcolor(0, 7); }
						cout << "  " << mdVec[i].varName;
						if (mdVec[i].varName.empty()) { cout << " "; }
						if (i != mdVec.size() - 1) { cout << endl; }
						count++;
					}
					gotoxy(0, idx - Start);
					setcolor(0, 12);
					cout << "●";
					setcolor(0, 0);
				}
				else {
					gotoxy(0, idx - Start);
					setcolor(0, 0);
					cout << "  ";
					idx--;

					gotoxy(0, idx - Start);
					setcolor(0, 12);
					cout << "●";
					setcolor(0, 0);
				}
			}
			else if (
				strVec.empty() && Start <= End &&  ( idx < mdVec.size() - 1 )
				&& ('s' == ch || 'S' == ch) 
			)
			{
				if (idx == End) {
					system("cls");

					int count = 0;
					int newStart = End + 1;
					int newEnd = min( newStart + sizeOfWindow - 1, mdVec.size()-1 );

					Start = newStart; End = newEnd;
					idx++;

					for (int i = Start; i <= End; ++i) {
						if (mdVec[i].isDir) { setcolor(0, 10); }
						else { setcolor(0, 7); }
						cout << "  " << mdVec[i].varName;
						if (i != mdVec.size() - 1) { cout << endl; }
						count++;
					}
					gotoxy(0, 0);
					setcolor(0, 12);
					cout << "●";
					setcolor(0, 0);
				}
				else {
					gotoxy(0, idx - Start);
					setcolor(0, 0);
					cout << "  ";
					idx++;

					gotoxy(0, idx - Start);
					setcolor(0, 12);
					cout << "●";
					setcolor(0, 0);
				}
			}
			if (!strVec.empty() && Start <= End && idx > 0 && ('w' == ch || 'W' == ch))
			{
				// draw mdVec and cursor - chk!!
				if (idx == Start) {
					system("cls");

					int count = 0;
					int newEnd = Start - 1;
					int newStart = max(0, newEnd - sizeOfWindow + 1);

					Start = newStart; End = newEnd;
					idx--;

					for (int i = Start; i <= End; ++i) {
						setcolor(0, 7);
						cout << "  " << strVec[i];
						if (i != strVec.size() - 1) { cout << endl; }
						count++;
					}
					gotoxy(0, idx - Start);
					setcolor(0, 12);
					cout << "●";
					setcolor(0, 0);
				}
				else {
					gotoxy(0, idx - Start);
					setcolor(0, 0);
					cout << "  ";
					idx--;

					gotoxy(0, idx - Start);
					setcolor(0, 12);
					cout << "●";
					setcolor(0, 0);
				}
			}
			else if (
				!strVec.empty() && Start <= End && (idx < strVec.size() - 1)
				&& ('s' == ch || 'S' == ch)
				)
			{
				if (idx == End) {
					setcolor(0, 0);
					system("cls");

					int count = 0;
					int newStart = End + 1;
					int newEnd = min(newStart + sizeOfWindow - 1, strVec.size() - 1);

					Start = newStart; End = newEnd;
					idx++;

					for (int i = Start; i <= End; ++i) {
						setcolor(0, 7);
						cout << "  " << strVec[i];
						if (i != strVec.size() - 1) { cout << endl; }
						count++;
					}
					gotoxy(0, 0);
					setcolor(0, 12);
					cout << "●";
					setcolor(0, 0);
				}
				else {
					gotoxy(0, idx - Start);
					setcolor(0, 0);
					cout << "  ";
					idx++;

					gotoxy(0, idx - Start);
					setcolor(0, 12);
					cout << "●";
					setcolor(0, 0);
				}
			}
			else if ('\r' == ch || '\n' == ch) {
				/// To Do..
				gotoxy(0, 0);
				if (0 == count_userType && 0 == count_item)
				{
					// nothing..
				}
				else if (strVec.empty() && idx < count_userType) { // utVec[braceNum].Get(mdVec[idx].no)->GetUserTypeListSize()) {
					setcolor(0, 0);
					system("cls");

					// usertypelist
					braceNum++;
					idxVec.push_back(idx); /// idx?
					
					if (braceNum >= utVec.size()) {
						utVec.push_back(wiz::load_data::TypeArray<wiz::load_data::UserType*>());
						utVec2.push_back(NULL);
					}

					wiz::load_data::TypeArray< wiz::load_data::UserType*> ref;
					string strTemp = mdVec[idxVec[braceNum-1]].varName;
					if (strTemp == " " || false == utVec[braceNum - 1].Get(mdVec[idxVec[braceNum - 1]].no)->valid())
					{
						strTemp = "";
					}

					if (utVec[braceNum - 1].Get(mdVec[idxVec[braceNum - 1]].no)->GetUserTypeItemRef(idxVec[braceNum-1], ref))
					{
						//
					}
					utVec[braceNum] = ref;

					utVec2[braceNum - 1]->GetUserTypeItemRef(idxVec[braceNum-1], ref);
					utVec2[braceNum] = ref.Get(mdVec[idxVec[braceNum - 1]].no);

					Start = 0;
					idx = 0;
					isFirst = true;
				}
				else
				{	
					// itemlist
					// print

					if (
						!mdVec.empty() &&
						strVec.empty()
						) 
					{
						setcolor(0, 0);
						system("cls");

						string strTemp = mdVec[idx].varName;
						if (strTemp == " ") { strTemp = ""; }
						const int count = 1; // utVec[braceNum].Get(mdVec[idx].no)->GetItem(strTemp).size();
						setcolor(0, 7);
						
						for (int i = 0; i < count; ++i) {
							setcolor(0, 7); 
							
							auto x = utVec[braceNum].Get(mdVec[idx].no)->GetItemList(idx - count_userType);
							string temp = x.Get(0);
							cout << "  " << temp;
							strVec.push_back(temp);
							//}
							if (i != count - 1) { cout << endl; }
						}
					}

					if (state == 0) {
						gotoxy(0, 0);
						state = 1;

						idxVec.push_back(idx);
						
						idx = 0;
						Start = 0;
					}
					else if (state == 1) { /// cf) state = 2;
						gotoxy(0, idx); /// chk..
						Start = idx;
					}

					setcolor(0, 12);
					cout << "●";
					setcolor(0, 0);
					// idx = 0;
					End = min(Start + sizeOfWindow - 1, strVec.size() - 1);
					if (strVec.empty()) { End = Start - 1; }

					count_userType = 0;
					count_item = 0; /// chk??
				}
			}
			else {
				if ('q' == ch) { return; } // quit
				else if ('b' == ch && braceNum > 0 && strVec.empty() && state == 0) {  // back
					braceNum--; setcolor(0, 0);  system("cls"); isFirst = true;
					//Start = idxVec.back();

					idx = idxVec.back();
					idxVec.pop_back();

					if (0 <= idx - sizeOfWindow / 2)
					{
						Start = idx - sizeOfWindow / 2;
					}
					else {
						Start = 0;
					}
				}
				else if ('b' == ch && !idxVec.empty())
				{
					idx = idxVec.back();
					idxVec.pop_back();

					if (0 <= idx - sizeOfWindow / 2)
					{
						Start = idx - sizeOfWindow / 2;
					}
					else {
						Start = 0;
					}

					state = 0;
					setcolor(0, 0);
					system("cls");
					strVec.clear();
					isFirst = true;
				}
				else if ('e' == ch) {
					setcolor(0, 0);
					system("cls");
					setcolor(7, 0);

					cout << "edit mode" << endl;
					cout << "add - a, change - c, remove -r, save -s" << endl;
					ch = _getch();
	

					/// todo : edit - e    ch = getch();
					/// add - a, change- c, remove - r. save - s 
					/// isFirst = true;
					if ('a' == ch) { // add
						int select=-1;
						string var;
						string val;

						setcolor(0, 7);
						// need more test!!
						cout << "add UserType : 1, add Item : 2, add usertype that name is "": 3 your input : ";
						cin >> select;

						// add userType?
						if (1 == select) {
							// name of UserType.
							cout << "what is new UserType name? : ";
							cin >> var;
							utVec2[braceNum]->AddUserTypeItem(wiz::load_data::UserType(var));

							if (state == 1)
							{
								idx = idxVec.back();
								idxVec.pop_back();
								// max!
								if (0 <= idx - sizeOfWindow / 2)
								{
									Start = idx - sizeOfWindow / 2;
								}
								else {
									Start = 0;
								}
								strVec.clear();
								state = 0;
							}
						}
						// addd Item?
						else if (2 == select) {
							// var, val /// state에 따라?
							cout << "var : ";
							cin >> var;
							cout << "val : ";
							getchar(); //
							getline(cin, val);
							utVec2[braceNum]->AddItem(var, val);

							if (state == 1)
							{
								idx = idxVec.back();
								idxVec.pop_back();
								// max!
								if (0 <= idx - sizeOfWindow / 2)
								{
									Start = idx - sizeOfWindow / 2;
								}
								else {
									Start = 0;
								}
								strVec.clear();
								state = 0;
							}
						}
						else if (3 == select)
						{
							utVec2[braceNum]->AddUserTypeItem(wiz::load_data::UserType(""));

							if (state == 1)
							{
								idx = idxVec.back();
								idxVec.pop_back();
								// max!
								if (0 <= idx - sizeOfWindow / 2)
								{
									Start = idx - sizeOfWindow / 2;
								}
								else {
									Start = 0;
								}
								strVec.clear();
								state = 0;
							}
						}
					}
					else if ('c' == ch && Start <= End) { // change var or value
						// idx
						if (idx < count_userType) {
							string temp;
							setcolor(0, 7);
							cout << "change userType name : ";
							cin >> temp;

							int count = 0;
							for (int h = 0; h < utVec[braceNum].size(); ++h) {
								for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
									if (count == idx) {
										utVec[braceNum].Get(h)->GetUserTypeList(i).SetName(temp);
									}
									count++;
								}
							}
						}
						else {
							string temp;
							setcolor(0, 7);
							
							string name, value;
							if (1 == state) { // val
								cout << "change val : " << endl;
								getline(cin, temp);
								
								value = temp; 
								
								int count = 0;
								int count_userType = 0;
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
										count++;
										count_userType++;
									}
								}
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetItemListSize(); ++i) {
										if (idxVec.back() == count) {
											utVec[braceNum].Get(h)->GetItemList(i).Get(idx) = value;
										}
										count++;
									}
								}
								idx = idxVec.back();
								idxVec.pop_back();
								// max!
								if (0 <= idx - sizeOfWindow / 2)
								{
									Start = idx - sizeOfWindow / 2;
								}
								else {
									Start = 0;
								}
								strVec.clear();
								state = 0;
							}
							else if (0 == state) { // var
								cout << "change var : " << endl;
								cin >> temp;
								name = temp;
								int count = 0;
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
										count++;
									}
								}
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetItemListSize(); ++i) {
										if (idx == count) {
											utVec[braceNum].Get(h)->GetItemList(i).SetName(name);
										}
										count++;
									}
								}
							}
						}
					}
					else if ('r' == ch && Start <= End) { // remove
						if (idx < count_userType)
						{
							int count = 0;
							for (int h = 0; h < utVec[braceNum].size(); ++h) {
								for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
									if (count == idx) {
										string temp = mdVec[idx].varName;
										if (temp == " ") {
											temp = "";
										}
										utVec[braceNum].Get(h)->RemoveUserTypeList(temp);
									}
									count++;
								}
							}
							idx = 0;
							Start = 0;
						}
						else {
							if (state == 0) {
								int count = 0;
								int count_ut = 0;
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
										count++;
										count_ut++;
									}
								}
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetItemListSize(); ++i) {
										if (count == idx) {
											string temp = mdVec[idx].varName;
											if (temp == " ") { temp = ""; }

											utVec[braceNum].Get(h)->RemoveItemList(temp);
											/*
											utVec[braceNum].Get(h)->GetItemList(idx - count_ut).Remove(0);
											if (utVec[braceNum].Get(h)->GetItemList(idx - count_ut).size() == 0) {
												utVec[braceNum].Get(h)->GetItemList(idx - count_ut).Remove();
												utVec[braceNum].Get(h)->RemoveEmptyItem();
											}
											*/
										}
										count++;
									}
								}
								idx = 0;
								Start = 0;
							}
							else if (1 == state)
							{
								int count = 0;
								int count_ut = 0;
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetUserTypeListSize(); ++i) {
										count++;
										count_ut++;
									}
								}
								for (int h = 0; h < utVec[braceNum].size(); ++h) {
									for (int i = 0; i < utVec[braceNum].Get(h)->GetItemListSize(); ++i) {
										if (count == idxVec.back()) {
											string temp = mdVec[idxVec.back()].varName;
											if (temp == " ") { temp = ""; }
											
											utVec[braceNum].Get(h)->GetItemList(count - count_ut).Remove(0);
											if (utVec[braceNum].Get(h)->GetItemList(count - count_ut).size() == 0) {
												utVec[braceNum].Get(h)->GetItemList(count - count_ut).Remove();
												utVec[braceNum].Get(h)->RemoveEmptyItem();
											}
										}
										count++;
									}
								}

								idxVec.pop_back();
								idx = 0;
								// max!
								if (0 <= idx - sizeOfWindow / 2)
								{
									Start = idx - sizeOfWindow / 2;
								}
								else {
									Start = 0;
								}
								strVec.clear();
								state = 0;
							}
						}
					}
					else if ('s' == ch) { // save total data.
						string temp;

						setcolor(0, 7);
						cout << "save file name : ";
						FFLUSH();
						getline(cin, temp);

						wiz::load_data::LoadData::SaveWizDB(utTemp, temp, "1");

						if (1 == state)
						{
							idxVec.back();
							idxVec.pop_back();
							idx = 0;
							// max!
							if (0 <= idx - sizeOfWindow / 2)
							{
								Start = idx - sizeOfWindow / 2;
							}
							else {
								Start = 0;
							}
							strVec.clear();
							state = 0;
						}
					}
					/// else if( l? reload?
					isFirst = true; // redraw?
				}
				else if ('t' == ch ) { // pass???
					isFirst = true;
					setcolor(0, 0);
					system("cls");

					setcolor(7, 0);
					cout << "text edit mode" << endl;

					// Add, AddUserType, Set, Remove, RemoveAll ?.
					string temp;
					FFLUSH();
					getline(cin, temp);
					
					wiz::StringTokenizer tokenizer(temp, "|");
					vector<string> strVecTemp;
					
					while (tokenizer.hasMoreTokens()) {
						strVecTemp.push_back(tokenizer.nextToken());
					}
					if (!strVecTemp.empty()) {
						try {
							if ("add" == strVecTemp[0])
							{
								if (false == wiz::load_data::LoadData::AddData(utTemp, strVecTemp[1], strVecTemp[2], strVecTemp[3]))
								{
									cout << "faile to add" << endl; /// To Do to following code.
								}
							}
							else if ("addusertype" == strVecTemp[0])
							{
								wiz::load_data::LoadData::AddUserType(utTemp, strVecTemp[1], strVecTemp[2], strVecTemp[3], strVecTemp[4]);
							}
							else if ("set" == strVecTemp[0])
							{
								wiz::load_data::LoadData::SetData(utTemp, strVecTemp[1], strVecTemp[2], strVecTemp[3], strVecTemp[4]);
							}
							else if ("remove" == strVecTemp[0])
							{
								wiz::load_data::LoadData::Remove(utTemp, strVecTemp[1], strVecTemp[2], strVecTemp[3]);
							}
							else if ("removeall" == strVecTemp[0])
							{
								wiz::load_data::LoadData::Remove(utTemp, strVecTemp[1], strVecTemp[2]);
							}
						}
						catch (exception& e) {}
						catch (wiz::Error& e) {}
						catch (const char* e) {}
						catch (const string& e) {}
					}
					//
					idx = 0;
					Start = 0;
					if (1 == state)
					{
						idxVec.back();
						idxVec.pop_back();
						idx = 0;
						// max!
						if (0 <= idx - sizeOfWindow / 2)
						{
							Start = idx - sizeOfWindow / 2;
						}
						else {
							Start = 0;
						}
						strVec.clear();
						state = 0;
					}
				}
			}
		}
	}
}

int main(void)
{
	//test2(); // load from string!
	//_getch();
	 //eu4Test();
	//stellarisTest();
	
	string fileName;

	cout << "input name : ";
	cin >> fileName;
		
	try {
		MStyleTest(fileName);
	}
	catch (exception& e) { cout << e.what() << endl; }
	return 0;	
}
