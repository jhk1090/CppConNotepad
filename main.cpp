#include <iostream>
#include <locale>
#include <filesystem>
#include <fstream>
#include <string>
#include "json\json.h"
#include <vector>
#include <algorithm>
#include <sstream>
#include <conio.h>
#include <direct.h>
#include <Windows.h>
#include <tuple>

using namespace std;
using namespace Json;
using namespace filesystem;


enum class RecieveReturn : int {
	wWrite = 0,
	nonwWrite,
	wEscape,
	nonwEscape,
	sSave,
	nonsSave,
	lLoad,
	nonlLoad,
};

// 키보드 핫키
string sc_delSentence;
// 이스케이프 핫키
string sc_escape;
// write 핫키
int sc_w_save;
int sc_w_load;
int sc_w_new;
int sc_w_back;
int sc_w_exit;
// save 핫키
int sc_s_up;
int sc_s_down;
int sc_s_fname_c;
int sc_s_enc_c;
int sc_s_dir_c;
int sc_s_select;
int sc_s_back;
int sc_s_save;
string sc_cursor;

// 시스템
string sys_ver;
string sys_area;
RecieveReturn sys_area_return = RecieveReturn::wWrite;
using SYS_INITIAL = tuple<int>;
SYS_INITIAL sys_initial(1);

// 기본 설정
string def_fileName;
string def_fileExt;
string def_fileEncoding;
string def_directory;

// 파일 저장 설정
string save_filename;
string save_encoding;
vector<string> allow_encoding({ "UTF-8", "ANSI" });
string save_directory;
vector<string> prev_directory;
vector<string> next_directory;
bool isSave = true;

// 파일 작성
vector<string> writeList;
string writeThing;

vector<string> str_setting;
vector<int> int_setting;

void SetToDefault(bool readfile = true) {
	if (readfile) {
		// data 파일 읽기
		ifstream json_open("data.json");
		CharReaderBuilder rbuilder;
		rbuilder["collectComments"] = false;
		unique_ptr<Json::CharReader> rwriter(rbuilder.newCharReader());
		Value value;
		JSONCPP_STRING errs;
		if (!parseFromStream(rbuilder, json_open, &value, &errs)) {
			cout << errs << endl;
		}

		// data 불러오기
		//str
		sc_delSentence = value["shortcut"].get("delSentence", "None").asString();
		sc_escape = value["shortcut"]["escape"].get("enter", "None").asString();
		sc_cursor = value["shortcut"].get("cursor", "None").asString();

		sc_w_save = value["shortcut"]["escape"]["area"]["write"].get("save", 0).asInt();
		sc_w_load = value["shortcut"]["escape"]["area"]["write"].get("load", 0).asInt();
		sc_w_new = value["shortcut"]["escape"]["area"]["write"].get("new", 0).asInt();
		sc_w_back = value["shortcut"]["escape"]["area"]["write"].get("back", 0).asInt();
		sc_w_exit = value["shortcut"]["escape"]["area"]["write"].get("exit", 0).asInt();
		sc_s_up = value["shortcut"]["escape"]["area"]["save"].get("up", 0).asInt();
		sc_s_down = value["shortcut"]["escape"]["area"]["save"].get("down", 0).asInt();
		sc_s_fname_c = value["shortcut"]["escape"]["area"]["save"].get("fname_c", 0).asInt();
		sc_s_enc_c = value["shortcut"]["escape"]["area"]["save"].get("enc_c", 0).asInt();
		sc_s_dir_c = value["shortcut"]["escape"]["area"]["save"].get("dir_c", 0).asInt();
		sc_s_select = value["shortcut"]["escape"]["area"]["save"].get("select", 0).asInt();
		sc_s_back = value["shortcut"]["escape"]["area"]["save"].get("back", 0).asInt();
		sc_s_save = value["shortcut"]["escape"]["area"]["save"].get("save", 0).asInt();
		
		sys_ver = value["system"].get("version", "None").asString();

		def_fileName = value["default"].get("fileName", "None").asString();
		def_fileExt = value["default"].get("fileExt", "None").asString();
		def_fileEncoding = value["default"].get("fileEncoding", "None").asString();
		def_directory = value["default"].get("directory", "None").asString();

		save_filename = def_fileName + "." + def_fileExt;
		save_encoding = def_fileEncoding;
		save_directory = def_directory;

		str_setting = {
			sc_delSentence,
			sc_escape,
			sc_cursor,
			sys_ver,
			def_fileName,
			def_fileExt,
			def_fileEncoding,
			def_directory,
			save_filename,
			save_encoding,
			save_directory
		};
		int_setting = {
			sc_w_save,
			sc_w_load,
			sc_w_new,
			sc_w_back,
			sc_w_exit,
			sc_s_up,
			sc_s_down,
			sc_s_fname_c,
			sc_s_enc_c,
			sc_s_dir_c,
			sc_s_select,
			sc_s_back,
			sc_s_save
		};

		rbuilder;
		rwriter;
		value;
		errs;
		json_open.close();
	}
	else {
		sc_delSentence = str_setting.at(0);
		sc_escape = str_setting.at(1);
		sc_w_save = int_setting.at(0);
		sc_w_load = int_setting.at(1);
		sc_w_new = int_setting.at(2);
		sc_w_back = int_setting.at(3);
		sc_w_exit = int_setting.at(4);
		sc_s_up = int_setting.at(5);
		sc_s_down = int_setting.at(6);
		sc_s_fname_c = int_setting.at(7);
		sc_s_enc_c = int_setting.at(8);
		sc_s_dir_c = int_setting.at(9);
		sc_s_select = int_setting.at(10);
		sc_s_back = int_setting.at(11);
		sc_s_save = int_setting.at(12);
		sc_cursor = str_setting.at(2);

		sys_ver = str_setting.at(3);

		def_fileName = str_setting.at(4);
		def_fileExt = str_setting.at(5);
		def_fileEncoding = str_setting.at(6);
		def_directory = str_setting.at(7);

		save_filename = str_setting.at(8);
		save_encoding = str_setting.at(9);
		save_directory = str_setting.at(10);
	}
}

class GUI {
public:
	GUI() {};

	// 상단의 상태바 구현
	void topBar() {
		cout << "=============== " << sys_area << topBar_isSave() << " - Notepad " << sys_ver << " ===============" << endl;
	}

	string topBar_isSave() {
		if (isSave) {
			return "[] ";
		}
		else {
			return "[*] ";
		}
	}

	void write() {
		cout << "===============[ 옵션 ]===============" << endl;
		cout << "- 저장        | \'ctrl+s\'" << endl;
		cout << "- 불러오기    | \'ctrl+o\'" << endl;
		cout << "- 새로 만들기 | \'ctrl+n\'" << endl;
		cout << "- 돌아가기    | \'esc\'" << endl;
		cout << "======================================" << endl;
		cout << "\'ctrl + x\'로 종료하기." << endl;
	}

	void save(int page, int indexFile) {
		cout << "===============[ 디렉토리 및 설정 ]===============" << endl;
		cout << "[ " << save_directory << " ]" << endl;
		cout << "--------------------------------------------------" << endl;
		showDir(page, indexFile);
		cout << "==================================================" << endl;
		cout << "파일명: " << save_filename << endl;
		cout << "사용 인코딩: " << save_encoding << endl;
		cout << "====================[ 단축키 ]====================" << endl;
		cout << "- 위, 아래 선택     | \'↑\', \'↓\'" << endl;
		cout << "- 파일명 변환       | \'ctrl+f\'" << endl;
		cout << "- 인코딩 변환       | \'ctrl+e\'" << endl;
		cout << "- 수동 디렉토리 변환| \'ctrl+d\'" << endl;
		cout << "- 선택된 폴더 열기  | \'ENTER\'" << endl;
		cout << "- 최종 저장         | \'ctrl+s\'" << endl;
		cout << "==================================================" << endl;
		cout << "\'esc\'로 돌아가기." << endl;
	}

	string save_select(int selectIndex) {
		int index = 0;
		if (save_directory != "C:\\" && save_directory != "C:/") {
			if (selectIndex == 0) {
				path parentDir = save_directory;
				return (parentDir.parent_path()).string();
			}
			index++;
		}

		for (const auto& dirEntry : directory_iterator(save_directory)) {
			if (index == selectIndex) {
				return (dirEntry.path()).generic_string();
			}
			index++;
		}
		return "";
	}

	void load(int page, int indexFile) {
		cout << "===============[ 디렉토리 및 설정 ]===============" << endl;
		cout << "[ " << save_directory << " ]" << endl;
		cout << "--------------------------------------------------" << endl;
		showDir(page, indexFile);
		cout << "==================================================" << endl;
		cout << "파일명: " << save_filename << endl;
		cout << "사용 인코딩: " << save_encoding << endl;
		cout << "====================[ 단축키 ]====================" << endl;
		cout << "- 위, 아래 선택     | \'↑\', \'↓\'" << endl;
		cout << "- 인코딩 변환       | \'ctrl+e\'" << endl;
		cout << "- 수동 디렉토리 변환| \'ctrl+d\'" << endl;
		cout << "- 선택  | \'ENTER\'" << endl;
		cout << "- 최종 저장         | \'ctrl+o\'" << endl;
		cout << "==================================================" << endl;
		cout << "\'esc\'로 돌아가기." << endl;
	}

	path load_select(int selectIndex) {
		int index = 0;
		if (save_directory != "C:\\" && save_directory != "C:/") {
			if (selectIndex == 0) {
				path parentDir = save_directory;
				return parentDir.parent_path();
			}
			index++;
		}

		for (const auto& dirEntry : directory_iterator(save_directory)) {
			if (index == selectIndex) {
				return dirEntry.path();
			}
			index++;
		}
		return "";
	}

	void showDir(int page = 1, int indexFile = 0) {
		int index = 0;
		/*int allowPage = showAllowPage();
		int allowRealRange = showIndex() - 1;*/

		if (save_directory != "C:\\" && save_directory != "C:/") {
			if (index == indexFile) {
				cout << "상위 폴더 |" << "\"..\"" << " [* 선택됨]" << endl;
			}
			else {
				cout << "상위 폴더 |" << "\"..\"" << endl;
			}
			index++;
		}
		for (const auto& dirEntry : directory_iterator(save_directory)) {
			/*if (page > 1 && (page - 1) * 10 + 1 <= index && index <= allowRealRange || page == 1 && 0 <= index && index <= 10) {*/
			if (index == indexFile) {
				if (dirEntry.is_directory())
					cout << "폴더      |" << (dirEntry.path()).filename() << " [* 선택됨]" << endl;
				else
					cout << "파일      |" << (dirEntry.path()).filename() << " [* 선택됨]" << endl;
			}
			else {
				if (dirEntry.is_directory())
					cout << "폴더      |" << (dirEntry.path()).filename() << endl;
				else
					cout << "파일      |" << (dirEntry.path()).filename() << endl;
			}
			index++;
		}
		/*cout << "---------------[ 페이지 " << page << " / " << allowPage << " ]-------------------" << endl;*/
	}

	int showIndex(int indexFile = -1) {
		int index = 0;

		if (save_directory != "C:\\" && save_directory != "C:/")
			index++;

		for (const auto& dirEntry : directory_iterator(save_directory))
			if (index == indexFile) {
				if (dirEntry.is_directory()) {
					return 1;
				}
				else {
					return 0;
				}
			}
			else {
				index++;
			}

		return index - 1;
	}
	
	/*int showAllowPage() {
		if ((showIndex() - 1) % 10 != 0 && showIndex() - 1 > 9)
			return (showIndex() - 1) / 10 + 1;
		else 
			return (showIndex() - 1) / 10;
	}*/
};

class Save {
private:
	int getKey;
	int indexFile;
	int page;

private:
	// 출처: https://doitnow-man.tistory.com/211

	// utf-8 문자 멀티바이트 전환
	string Utf8ToMultiByte(string utf8_str) {
		string resultString; char* pszIn = new char[utf8_str.length() + 1];
		strncpy_s(pszIn, utf8_str.length() + 1, utf8_str.c_str(), utf8_str.length());
		int nLenOfUni = 0, nLenOfANSI = 0;
		wchar_t* uni_wchar = NULL;
		char* pszOut = NULL;
		// 1. utf8 Length
		if ((nLenOfUni = MultiByteToWideChar(CP_UTF8, 0, pszIn, (int)strlen(pszIn), NULL, 0)) <= 0)
			return "";
		uni_wchar = new wchar_t[nLenOfUni + 1];
		memset(uni_wchar, 0x00, sizeof(wchar_t)*(nLenOfUni + 1));
		// 2. utf8 --> unicode
		nLenOfUni = MultiByteToWideChar(CP_UTF8, 0, pszIn, (int)strlen(pszIn), uni_wchar, nLenOfUni);
		// 3. ANSI(multibyte) Length
		if ((nLenOfANSI = WideCharToMultiByte(CP_ACP, 0, uni_wchar, nLenOfUni, NULL, 0, NULL, NULL)) <= 0)
		{ 
			delete[] uni_wchar;
			return "";
		} 
		pszOut = new char[nLenOfANSI + 1];
		memset(pszOut, 0x00, sizeof(char)*(nLenOfANSI + 1));
		// 4. unicode --> ANSI(multibyte)
		nLenOfANSI = WideCharToMultiByte(CP_ACP, 0, uni_wchar, nLenOfUni, pszOut, nLenOfANSI, NULL, NULL);
		pszOut[nLenOfANSI] = 0;
		resultString = pszOut;
		delete[] uni_wchar;
		delete[] pszOut;
		return resultString;
	}
	// 멀티바이트 문자 utf-8 변환
	string MultiByteToUtf8(string multibyte_str) {
		char* pszIn = new char[multibyte_str.length() + 1];
		strncpy_s(pszIn, multibyte_str.length() + 1, multibyte_str.c_str(), multibyte_str.length());
		string resultString; int nLenOfUni = 0, nLenOfUTF = 0;
		wchar_t* uni_wchar = NULL;
		char* pszOut = NULL;
		// 1. ANSI(multibyte) Length
		if ((nLenOfUni = MultiByteToWideChar(CP_ACP, 0, pszIn, (int)strlen(pszIn), NULL, 0)) <= 0)
			return "";
		uni_wchar = new wchar_t[nLenOfUni + 1];
		memset(uni_wchar, 0x00, sizeof(wchar_t)*(nLenOfUni + 1));
		// 2. ANSI(multibyte) ---> unicode
		nLenOfUni = MultiByteToWideChar(CP_ACP, 0, pszIn, (int)strlen(pszIn), uni_wchar, nLenOfUni);
		// 3. utf8 Length 
		if ((nLenOfUTF = WideCharToMultiByte(CP_UTF8, 0, uni_wchar, nLenOfUni, NULL, 0, NULL, NULL)) <= 0)
		{ 
			delete[] uni_wchar; 
			return ""; 
		} 
		pszOut = new char[nLenOfUTF + 1];
		memset(pszOut, 0, sizeof(char)*(nLenOfUTF + 1));
		// 4. unicode ---> utf8
		nLenOfUTF = WideCharToMultiByte(CP_UTF8, 0, uni_wchar, nLenOfUni, pszOut, nLenOfUTF, NULL, NULL);
		pszOut[nLenOfUTF] = 0;
		resultString = pszOut;
		delete[] uni_wchar;
		delete[] pszOut;
		return resultString; 
	}

public:
	Save() {
		getKey = 0;
		indexFile = 0;
		page = 1;
	};

	RecieveReturn main() {
		if (save_directory == "C:") {
			save_directory = "C:/";
		}
		int chdir = _chdir(save_directory.c_str());
		sys_area = "저장";
		system("cls");
		GUI gui;
		gui.topBar();
		gui.save(page, indexFile);
		while (true) {
			getKey = _getch();
			if (getKey == sc_s_up) {
				if (indexFile != 0) {
					/*if (indexFile % 10 == 0) {
						page -= 1;
					}*/
					indexFile--;
					main();
				}
				else {
					indexFile = gui.showIndex();
					/*page = gui.showAllowPage();*/
					main();
				}
			}
			else if (getKey == sc_s_down) {
				if (indexFile != gui.showIndex()) {
					/*if (indexFile % 10 == 0) {
						page += 1;
					}*/
					indexFile++;
					main();
				}
				else {
					indexFile = 0;
					//page = 1;
					main();
				}
			}
			else if (getKey == sc_s_fname_c) {
				cout << "파일명 설정 | ";
				getline(cin, save_filename);
				main();
			}
			else if (getKey == sc_s_enc_c) {
				string setEncoding;
				cout << "인코딩 설정 | ";
				getline(cin, setEncoding);
				transform(setEncoding.begin(), setEncoding.end(), setEncoding.begin(), static_cast<int(*)(int)>(toupper));
				for (string i : allow_encoding) {
					if (i == setEncoding)
						save_encoding = setEncoding;
				}
				main();
			}
			else if (getKey == sc_s_dir_c) {
				string setDirectory;
				path pathDirectory;
				cout << "디렉토리 설정 | ";
				getline(cin, setDirectory);
				pathDirectory = setDirectory;
				if (exists(pathDirectory) && is_directory(pathDirectory)) {
					save_directory = pathDirectory.generic_string();
					indexFile = 0;
				}
				main();
			}
			else if (getKey == sc_s_select) {
				if (gui.showIndex(indexFile)) {
					save_directory = gui.save_select(indexFile);
					indexFile = 0;
					main();
				}
			}
			else if (getKey == sc_s_back) {
				return RecieveReturn::nonwEscape;
			}
			else if (getKey == sc_s_save) {
				ofstream save_file;
				save_file.open(save_filename, ifstream::out);

				for (string i : writeList) {
					string output;
					if (save_encoding == "UTF-8") {
						output = MultiByteToUtf8(i);
					}
					else if (save_encoding == "ANSI") {
						output = Utf8ToMultiByte(MultiByteToUtf8(i));
					}
					save_file << output << endl;
				}
				save_file.close();
				isSave = true;
				return RecieveReturn::nonwEscape;
			}
		}
	}
};

class Load {
public:
	Load() {};

	RecieveReturn main() {
		return RecieveReturn::wWrite;
	}
};

class Write {
private:
	int loop;
	int loopAtt;
	int getKey;
	string writeText;

private:
	string indentationLine(string value) {
		string addInd;
		int looping = to_string(loop).length() - value.length();
		for (int i = 0; i <= looping; i++) {
			addInd += " ";
		}
		return value + addInd;
	};

public:
	Write(int setLoop = 1) {
		loop = setLoop;
		loopAtt = 0;
		getKey = 0;
		writeText = "";
	};

	RecieveReturn escape() {
		sys_area = save_filename;
		system("cls");
		GUI gui;
		gui.topBar();
		gui.write();
		while (true) {
			getKey = _getch();
			if (getKey == sc_w_save) {
				return RecieveReturn::nonsSave;
			}
			else if (getKey == sc_w_load) {
				return RecieveReturn::nonlLoad;
			}
			else if (getKey == sc_w_new) {
				writeList.clear();
				writeThing = "";
				isSave = true;
				SetToDefault(false);
				sys_area = save_filename;
				return RecieveReturn::wWrite;
			}
			else if (getKey == sc_w_back) {
				return RecieveReturn::nonwWrite;
			}
			else if (getKey == sc_w_exit) {
				exit(0);
			}
		}
	}

	RecieveReturn main() {
		sys_area = save_filename;
		while (true) {
			system("cls");
			vector<string>::iterator listIter = writeList.begin();
			GUI gui;
			gui.topBar();

			for (vector<string>::iterator iter = writeList.begin();
				iter != writeList.end();
				++iter)
			{
				if (loopAtt == (iter - writeList.begin()) + 1)
					cout << indentationLine(to_string((iter - writeList.begin()) + 1)) << "| + " << *iter << endl;
				else
					cout << indentationLine(to_string((iter - writeList.begin()) + 1)) << "| " << *iter << endl;
			}

			// 메인 진행
			if (loopAtt == 0) {
				cout << loop << " | ";
				getline(cin, writeText);
			}
			else {
				cout << endl << loopAtt << "+| ";
				getline(cin, writeText);
			}

			// 단축 키워드 감지
			/// 문장 삭제 ( 2 + n )
			if (writeText.substr(0, 1) == sc_delSentence) {
				if (all_of(writeText.begin() + 1, writeText.end(), [](char i) { return isdigit(i); })) {
					int amountToRemove = 0;
					stringstream ssInt(writeText.substr(1));
					ssInt >> amountToRemove;
					if (0 < amountToRemove && amountToRemove <= static_cast<int>(writeList.size())) {
						if (loopAtt == 0) {
							if (loop != 1) {
								writeList.erase(listIter + (loop - (amountToRemove + 1)), listIter + (loop - 1));
								loop -= amountToRemove;
							}
						}
						else {
							if (loopAtt != 1) {
								writeList.erase(listIter + (loopAtt - (amountToRemove + 1)), listIter + (loopAtt - 1));
								loopAtt -= amountToRemove;
								loop -= amountToRemove;
							}
						}
					}
					else {
						if (loopAtt == 0) {
							if (loop != 1) {
								writeList.erase(listIter + (loop - 2));
								loop--;
							}
						}
						else {
							if (loopAtt != 1) {
								writeList.erase(listIter + (loopAtt - 2));
								loopAtt--;
								loop--;
							}
						}
					}
				}
				else
				{
					if (loopAtt == 0) {
						if (loop != 1) {
							writeList.erase(listIter + (loop - 2));
							loop--;
						}
					}
					else {
						if (loopAtt != 1) {
							writeList.erase(listIter + (loopAtt - 2));
							loopAtt--;
							loop--;
						}
					}
				}
			}
			/// 키보드 커서
			else if (writeText.substr(0, 1) == sc_cursor)
			{
				if (writeText.substr(1, 1) == sc_cursor) {
					if (writeText.length() == 2) {
						if (loopAtt == 0) {
							writeText = "^" + to_string(loop - 1);
						}
						else {
							writeText = "^" + to_string(loopAtt - 1);
						}
					}
					else {
						if (writeText.substr(2, 1) == "-") {
							if (all_of(writeText.begin() + 3, writeText.end(), [](char i) { return isdigit(i); })) {
								int selected = 0;
								stringstream ssInt(writeText.substr(3));
								ssInt >> selected;
								writeText = "^" + to_string(loopAtt + selected);
							}
						}
						else {
							if (all_of(writeText.begin() + 2, writeText.end(), [](char i) { return isdigit(i); })) {
								int selected = 0;
								stringstream ssInt(writeText.substr(2));
								ssInt >> selected;
								if (loopAtt == 0) {
									writeText = "^" + to_string(loop - selected);
								}
								else {
									writeText = "^" + to_string(loopAtt - selected);
								}
							}
						}
					}
				}

				if (all_of(writeText.begin() + 1, writeText.end(), [](char i) { return isdigit(i); })) {
					int selectedCursor = 0;
					stringstream ssInt(writeText.substr(1));
					ssInt >> selectedCursor;
					if (loop != 1 && 0 < selectedCursor && selectedCursor < loop) {
						loopAtt = selectedCursor;
					}
					else {
						loopAtt = 0;
					}
				}
				else
				{
					loopAtt = 0;
				}
			}
			/// 이스케이프
			else if (writeText.substr(0, 1) == sc_escape) {
				return RecieveReturn::nonwEscape;
			}
			// 만약 감지되지 않았다면
			else {
				loop++;

				if (loopAtt != 0) {
					loopAtt++;
					writeList.insert(listIter + (loopAtt - 2), writeText);
				}
				else {
					writeList.push_back(writeText);
				}
				sys_initial = (loop);
				isSave = false;
			}
		}
	}
};

int main()
{
	SetToDefault();
	while (true)
	{
		if (sys_area_return == RecieveReturn::wWrite) {
			Write write;
			sys_area_return = write.main();
		}
		else if (sys_area_return == RecieveReturn::nonwWrite) {
			Write write(get<0>(sys_initial));
			sys_area_return = write.main();
		}
		else if (sys_area_return == RecieveReturn::wEscape) {
			Write write;
			sys_area_return = write.escape();
		}
		else if (sys_area_return == RecieveReturn::nonwEscape) {
			Write write;
			sys_area_return = write.escape();
		}
		else if (sys_area_return == RecieveReturn::sSave) {
			Save save;
			sys_area_return = save.main();
		}
		else if (sys_area_return == RecieveReturn::nonsSave) {
			Save save;
			sys_area_return = save.main();
		}
		else if (sys_area_return == RecieveReturn::lLoad) {
			Load load;
			sys_area_return = load.main();
		}
		else if (sys_area_return == RecieveReturn::nonlLoad) {
			Load load;
			sys_area_return = load.main();
		}
	}
}