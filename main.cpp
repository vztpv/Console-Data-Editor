
#include <iostream>
#include <memory>
using namespace std;

#include <wiz/load_data.h>


void test2() {
	wiz::load_data::LoadData global;
	
	global.InitWizDB();

	global.AddData("", "x= { y = { ival = 0 } }");
	global.AddData("x/", "y={ id = 1 name = \"country AAA\" }");

	cout << (global.GetData("","")) << endl;

	global.AddData("", "x=1");

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

#include <conio.h>

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
			"COMP> = { base_tax / 5.000 / 0 }")) {
			cout << "no found.." << endl;
			//_getch();
		}

		//LoadData::Remove("provinces/$/", "history", "NOTEQ = { core / \"RUS\" / }", 2); // 모든 경우에 다 다른다면?
		global.Remove("provinces/$/", "history", "AND = { EQ = { core / \"RUS\" / 1 }"
			"NOTEQ = { core / \"PLT\" / 2 } }");
		cout << global.GetData("provinces/-100", "TRUE") << endl;

		cout << global.GetItemListData("root/provinces/-100", "TRUE") << endl;

		cout << global.GetUserTypeListData("root/provinces/-100", "TRUE") << endl;

		cout << global.SearchUserType("history", "TRUE") << endl;

		global.SaveWizDB("result.eu4", "1"); /// , 0
		cout << "save end" << endl;

		global.AllRemoveWizDB();

		cout << "all end" << endl;
	}
}
int main(void)
{
//	test2();
	//eu4Test();
	
	wiz::load_data::LoadData global;
	for (int i = 0; i < 1; ++i) {
		global.InitWizDB();

		global.LoadWizDB("gamestate"); // stellaris
		cout << "load end" << endl;
		
	//	cout << global.GetData("species/ /", "TRUE") << endl;

		// cout << global.SearchItem("name", "TRUE") << endl;
		cout << "chk"<<endl;
		cout << global.GetData("species/ ", 
					"EQ = { portrait /identity \"human\" / 2 }") << endl;
		cout << "chk_" << endl;

		global.SaveWizDB("gamestate_result", "1"); /// , 0
		cout << "save end" << endl;
		
		global.AllRemoveWizDB();

		cout << "all end" << endl;
	}
	
	_getch();
	return 0;
}
