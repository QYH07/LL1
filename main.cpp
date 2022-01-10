#include<iostream>
#include<algorithm>
#include<set>
#include<map>
#include<iterator>
#include<fstream>
#include<string>
#include<stack>

using namespace std;

set<char> N, T;//N为非终结符号集；T为终结符号集
map<char, set<string>> P;//产生式集合

set<string> getP_right(char ch);
void initial_G();//初始化文法函数
void getFirstSet(char ch, set<char> &chFirst);//求first集函数
void getFollowSet(char ch, set<char> &chFollow, string str);//求follow集函数
void getFIRST_FOLLOW(map<char, set<char>> &FIRST, map<char, set<char>> &FOLLOW);//调用求first集和follow集函数
void createAnalyzeTable(map<pair<char, char>, string> &LL_M, map<char, set<char>> FIRST, map<char, set<char>> FOLLOW);//创建预测分析表函数
void getStringFirst(int n, string each, set<char> &strFirst, map<char, set<char>> FIRST);
void PredictAnalyze(map<pair<char, char>, string> LL_M);//预测分析过程函数
void error(int i, pair<char, char> a);//错误处理函数
void outputAnalyzeTable(map<pair<char, char>, string> LL_M);//输出分析表函数

int main(void)
{
	map<char, set<char>> FIRST;//保存First集变量
	map<char, set<char>> FOLLOW;//保存Follow集变量
	
	initial_G();
	getFIRST_FOLLOW(FIRST, FOLLOW);

	map<pair<char, char>, string> LL_M;//保存预测分析表变量
	createAnalyzeTable(LL_M, FIRST, FOLLOW);
	outputAnalyzeTable(LL_M);

	PredictAnalyze(LL_M);

	system("pause");
	return 0;
}

void PredictAnalyze(map<pair<char, char>, string> LL_M)
{
	string text;
	cout << "请输入需要分析的字符串:" << endl;
	cin >> text;

	text += '$';//输入缓冲区输入完毕
	stack<char> STACK;
	int ip = 0;
	char X, a;
	STACK.push('$');
	STACK.push('E');

	do {
		X = STACK.top();
		a = text[ip];
		if (T.count(X) == 1 || X == '$') {//X是终结符号||'$'
			if (a == 'n') {
				a = text[++ip];
				if (a == 'u') {
					a = text[++ip];
					if (a != 'm')
						error(1, { X, text[ip-1] });
					else a = 'n';
				}
				else error(1, { X, text[ip-1] });
			}
			if (X == a) {
				STACK.pop();
				ip++;
				if(X != 'n')
					cout << X << "匹配成功" << endl;
				else cout << "num匹配成功" << endl;
			}
			else error(1, { X, a });
		}
		else {
			pair<char, char> MX_a(X, a);
			auto formula = LL_M.find(MX_a);
			if (formula != LL_M.end()) {//M[X, a] = X->Y1Y2……
				auto fml_r = formula->second;
				STACK.pop();//从栈顶弹出X;
				//依次把Yk，Yk - 1，……Y1;
				for (int i = fml_r.length() - 1; i >= 0; i--) {
					if (fml_r[i] != '#') {
						STACK.push(fml_r[i]);
					}
				}
				if(fml_r != "n")
					cout << X << "->" << fml_r << endl;
				else cout << X << "->num" << endl;
			}
			else
				error(2, MX_a);
		}
	} while (X != '$');
	cout << "识别成功!" << endl;
}

void initial_G()
{
	/*所有非终结符和终结符保存在symbol.txt中，所有产生式保存在formula.txt中*/
	string N_T = "symbol.txt", Prdt = "formula.txt", nstr, tstr, pstr;
	ifstream f1, f2;
	f1.open(N_T, ios::in);
	if (!f1.is_open()) {
		cout << "Initial Terminator and non Terminator Failed." << endl;
		system("pause");
		exit(-3);
	}
	else {
		getline(f1, nstr);
		for (int i = 0; i < nstr.length(); i++) {
			if (nstr[i] != ' ' && nstr[i] != '\n') N.insert(nstr[i]);
		}
		getline(f1, tstr);
		for (int i = 0; i < tstr.length(); i++) {
			if (tstr[i] != ' ' && tstr[i] != '\n') T.insert(tstr[i]);
		}
	}f1.close();

	f2.open(Prdt, ios::in);
	if (!f2.is_open()) {
		cout << "Initial Generative formula Failed." << endl;
		system("pause");
		exit(-3);
	}
	else {
		char S;
		int num;
		string fml;
		while (!f2.eof()) {
			f2 >> S >> num;
			set<string> p_r;
			for (int i = 0; i < num; i++) {
				f2 >> fml;
				p_r.insert(fml);
			}P.insert({ S, p_r });
		}
	}f2.close();
}

void error(int i, pair<char, char> a)
{
	pair<string, char> b;
	if (a.first == 'n') b.first = "num";
	else b.first = a.first;
	b.second = a.second;
	if (i == 1)//错误1
		cout << "分析栈栈顶符号是终结符号" << b.first << "，但与当前输入符号" << a.second << "不匹配." << endl;
	else//错误2
		cout << "分析栈栈顶符号是非终结符号" << b.first << "，当前输入符号为" << a.second << ",但分析表中M[" 
		<< b.first << "," << a.second << "]为空." << endl;

	system("pause");
	exit(-i);
}

void createAnalyzeTable(map<pair<char, char>, string> &LL_M, map<char, set<char>> FIRST, map<char, set<char>> FOLLOW)
{
	/*参照算法4.2创建预测分析表*/
	for (auto p = P.begin(); p != P.end(); p++) {
		auto p_r = p->second;
		auto p_l = p->first;
		//for(每个终结符a∈FIRST(α)) 将A->α放入表项M[A,a]中；
		for (auto each = p_r.begin(); each != p_r.end(); each++) {
			if (T.count((*each)[0]) != 0 && (*each)[0] != '#') {
				pair<char, char> temp(p_l, (*each)[0]);
				LL_M.insert({ temp, *each });
			}
			else if ((*each) == "#") {//空∈FIRST(α)
				auto p_lFollow = FOLLOW.find(p_l);
				auto chFollow = p_lFollow->second;
				for (auto ch = chFollow.begin(); ch != chFollow.end(); ch++) {
					pair<char, char> temp(p_l, *ch);
					LL_M.insert({ temp, *each });
				}
			}
			else {
				set<char> strFirst = {};
				getStringFirst(0, *each, strFirst, FIRST);
				for (auto ch = strFirst.begin(); ch != strFirst.end(); ch++) {
					pair<char, char> temp(p_l, *ch);
					if(*ch != '#')  LL_M.insert({ temp, *each });
				}
				if (strFirst.count('#') == 1) {
					auto p_lFollow = FOLLOW.find(p_l);
					auto chFollow = p_lFollow->second;
					for (auto ch = chFollow.begin(); ch != chFollow.end(); ch++) {
						pair<char, char> temp(p_l, *ch);
						LL_M.insert({ temp, *each });
					}
				}
			}
		}
	}
}

void outputAnalyzeTable(map<pair<char, char>, string> LL_M)
{
	cout << "LL(1)预测分析表:" << endl;
	for (auto i = LL_M.begin(); i != LL_M.end(); i++) {
		auto j = i->first;
		auto k = i->second;
		cout << "M[" << j.first << "," << j.second << "] = " << k << endl;
	}
}

void getStringFirst(int n, string each, set<char> &strFirst, map<char, set<char>> FIRST)
{
	auto ch_first = FIRST.find(each[n]);
	if (ch_first == FIRST.end()) return;
	auto ch_f_r = ch_first->second;
	set<char> temp = {};
	set_union(strFirst.begin(), strFirst.end(), ch_f_r.begin(), ch_f_r.end(), inserter(temp, temp.begin()));
	strFirst = temp;

	if (ch_f_r.count('#') != 0 && n < each.length())
		getStringFirst(n + 1, each, strFirst, FIRST);
}

void getFIRST_FOLLOW(map<char, set<char>> &FIRST, map<char, set<char>> &FOLLOW)
{
	cout << "FIRST集:" << endl;
	for (auto ch = N.begin(); ch != N.end(); ch++) {
		set<char> chFirst;
		getFirstSet(*ch, chFirst);

		FIRST.insert({ *ch, chFirst });
		cout << "FIRST(" << *ch << ") = {";

		auto i = chFirst.begin();
		cout << *i;
		while (true) {
			i++;
			if (i != chFirst.end()) cout << "," << *i;
			else break;
		}cout << "}" << endl;
	}
	cout << "FOLLOW集" << endl;
	for (auto ch = N.begin(); ch != N.end(); ch++) {
		set<char> chFollow;
		string str = "";
		getFollowSet(*ch, chFollow, str);

		if (chFollow.count('#') != 0)
			for (auto temp = chFollow.begin(); temp != chFollow.end(); temp++)
				if (*temp == '#') {
					chFollow.erase(temp);
					break;
				}

		FOLLOW.insert({ *ch, chFollow });
		cout << "FOLLOW(" << *ch << ") = {";

		auto i = chFollow.begin();
		cout << *i;
		while (true) {
			i++;
			if (i != chFollow.end()) cout << "," << *i;
			else break;
		}cout << "}" << endl;
	}
}

set<string> getP_right(char ch)
{
	auto p_ch = P.find(ch);
	if (p_ch != P.end()) {
		auto p_right = p_ch->second;
		return p_right;
	}
}

void getFirstSet(char ch, set<char> &chFirst)
{
	if (T.count(ch) != 0) {//ch为终结符
		chFirst.insert(ch);
	}
	else {
		auto p_right = getP_right(ch);
		for (auto each = p_right.begin(); each != p_right.end(); each++) {
			if (T.count((*each)[0]) != 0)//Y1是终结符
				chFirst.insert((*each)[0]);
			else {
				int pos = 0;
				while (true) {
					set<char> temp;
					getFirstSet((*each)[pos], temp);
					auto pp_right = getP_right((*each)[pos++]);
					for (auto i = temp.begin(); i != temp.end(); i++)
						//元素非空 || (元素为空 && Yn是产生式字符串最后一个字符)
						if (*i != '#' || (*i == '#' && pos == (*each).length())) chFirst.insert(*i);
					if (pos == (*each).length() || pp_right.count("#") == 0)//非产生式最后字符 || temp中无空
						break;	
				}
			}
		}
	}
}

void getFollowSet(char ch, set<char> &chFollow, string str)
{
	if (ch == 'E') chFollow.insert('$');//ch为起始符时，将$加入follow集
	if (str.find(ch) != str.npos) return;//发现已求ch的follow集，则return
	for (auto p = P.begin(); p != P.end(); p++) {
		auto p_r = p->second;
		for (auto each = p_r.begin(); each != p_r.end(); each++) {
			int pos = (*each).find(ch);
			if (pos != (*each).npos) {
				if (pos == (*each).length() - 1) {//ch的位置为产生式末尾
					if(ch != p->first) getFollowSet(p->first, chFollow, str + ch);
				}
				else if (T.count((*each)[pos + 1]) != 0) {//ch的后一个字符为终结符
					chFollow.insert((*each)[pos + 1]);
				}
				else if (N.count((*each)[pos + 1]) != 0) {//ch的后一个字符为非终结符
					int temp_pos = pos + 1;

					while (temp_pos < (*each).length()) {
						getFirstSet((*each)[temp_pos], chFollow);
						set<char> temp;
						getFirstSet((*each)[temp_pos], temp);

						if (temp.count('#') == 1 && temp_pos < (*each).length() - 1)
							;//temp中含有空 && 没有遍历到产生式尾
						else if (temp.count('#') == 1 && temp_pos == (*each).length() - 1)//temp中含空 && 遍历到产生式尾
							getFollowSet(p->first, chFollow, str + ch);
						else if (temp.count('#') == 0)//temp中不含空
							break;
						temp_pos++;
					}	
				}
			}
		}
	}
}