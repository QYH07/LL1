#include<iostream>
#include<algorithm>
#include<set>
#include<map>
#include<iterator>
#include<fstream>
#include<string>
#include<stack>

using namespace std;

set<char> N, T;//NΪ���ս���ż���TΪ�ս���ż�
map<char, set<string>> P;//����ʽ����

set<string> getP_right(char ch);
void initial_G();//��ʼ���ķ�����
void getFirstSet(char ch, set<char> &chFirst);//��first������
void getFollowSet(char ch, set<char> &chFollow, string str);//��follow������
void getFIRST_FOLLOW(map<char, set<char>> &FIRST, map<char, set<char>> &FOLLOW);//������first����follow������
void createAnalyzeTable(map<pair<char, char>, string> &LL_M, map<char, set<char>> FIRST, map<char, set<char>> FOLLOW);//����Ԥ���������
void getStringFirst(int n, string each, set<char> &strFirst, map<char, set<char>> FIRST);
void PredictAnalyze(map<pair<char, char>, string> LL_M);//Ԥ��������̺���
void error(int i, pair<char, char> a);//��������
void outputAnalyzeTable(map<pair<char, char>, string> LL_M);//�����������

int main(void)
{
	map<char, set<char>> FIRST;//����First������
	map<char, set<char>> FOLLOW;//����Follow������
	
	initial_G();
	getFIRST_FOLLOW(FIRST, FOLLOW);

	map<pair<char, char>, string> LL_M;//����Ԥ����������
	createAnalyzeTable(LL_M, FIRST, FOLLOW);
	outputAnalyzeTable(LL_M);

	PredictAnalyze(LL_M);

	system("pause");
	return 0;
}

void PredictAnalyze(map<pair<char, char>, string> LL_M)
{
	string text;
	cout << "��������Ҫ�������ַ���:" << endl;
	cin >> text;

	text += '$';//���뻺�����������
	stack<char> STACK;
	int ip = 0;
	char X, a;
	STACK.push('$');
	STACK.push('E');

	do {
		X = STACK.top();
		a = text[ip];
		if (T.count(X) == 1 || X == '$') {//X���ս����||'$'
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
					cout << X << "ƥ��ɹ�" << endl;
				else cout << "numƥ��ɹ�" << endl;
			}
			else error(1, { X, a });
		}
		else {
			pair<char, char> MX_a(X, a);
			auto formula = LL_M.find(MX_a);
			if (formula != LL_M.end()) {//M[X, a] = X->Y1Y2����
				auto fml_r = formula->second;
				STACK.pop();//��ջ������X;
				//���ΰ�Yk��Yk - 1������Y1;
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
	cout << "ʶ��ɹ�!" << endl;
}

void initial_G()
{
	/*���з��ս�����ս��������symbol.txt�У����в���ʽ������formula.txt��*/
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
	if (i == 1)//����1
		cout << "����ջջ���������ս����" << b.first << "�����뵱ǰ�������" << a.second << "��ƥ��." << endl;
	else//����2
		cout << "����ջջ�������Ƿ��ս����" << b.first << "����ǰ�������Ϊ" << a.second << ",����������M[" 
		<< b.first << "," << a.second << "]Ϊ��." << endl;

	system("pause");
	exit(-i);
}

void createAnalyzeTable(map<pair<char, char>, string> &LL_M, map<char, set<char>> FIRST, map<char, set<char>> FOLLOW)
{
	/*�����㷨4.2����Ԥ�������*/
	for (auto p = P.begin(); p != P.end(); p++) {
		auto p_r = p->second;
		auto p_l = p->first;
		//for(ÿ���ս��a��FIRST(��)) ��A->���������M[A,a]�У�
		for (auto each = p_r.begin(); each != p_r.end(); each++) {
			if (T.count((*each)[0]) != 0 && (*each)[0] != '#') {
				pair<char, char> temp(p_l, (*each)[0]);
				LL_M.insert({ temp, *each });
			}
			else if ((*each) == "#") {//�ա�FIRST(��)
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
	cout << "LL(1)Ԥ�������:" << endl;
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
	cout << "FIRST��:" << endl;
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
	cout << "FOLLOW��" << endl;
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
	if (T.count(ch) != 0) {//chΪ�ս��
		chFirst.insert(ch);
	}
	else {
		auto p_right = getP_right(ch);
		for (auto each = p_right.begin(); each != p_right.end(); each++) {
			if (T.count((*each)[0]) != 0)//Y1���ս��
				chFirst.insert((*each)[0]);
			else {
				int pos = 0;
				while (true) {
					set<char> temp;
					getFirstSet((*each)[pos], temp);
					auto pp_right = getP_right((*each)[pos++]);
					for (auto i = temp.begin(); i != temp.end(); i++)
						//Ԫ�طǿ� || (Ԫ��Ϊ�� && Yn�ǲ���ʽ�ַ������һ���ַ�)
						if (*i != '#' || (*i == '#' && pos == (*each).length())) chFirst.insert(*i);
					if (pos == (*each).length() || pp_right.count("#") == 0)//�ǲ���ʽ����ַ� || temp���޿�
						break;	
				}
			}
		}
	}
}

void getFollowSet(char ch, set<char> &chFollow, string str)
{
	if (ch == 'E') chFollow.insert('$');//chΪ��ʼ��ʱ����$����follow��
	if (str.find(ch) != str.npos) return;//��������ch��follow������return
	for (auto p = P.begin(); p != P.end(); p++) {
		auto p_r = p->second;
		for (auto each = p_r.begin(); each != p_r.end(); each++) {
			int pos = (*each).find(ch);
			if (pos != (*each).npos) {
				if (pos == (*each).length() - 1) {//ch��λ��Ϊ����ʽĩβ
					if(ch != p->first) getFollowSet(p->first, chFollow, str + ch);
				}
				else if (T.count((*each)[pos + 1]) != 0) {//ch�ĺ�һ���ַ�Ϊ�ս��
					chFollow.insert((*each)[pos + 1]);
				}
				else if (N.count((*each)[pos + 1]) != 0) {//ch�ĺ�һ���ַ�Ϊ���ս��
					int temp_pos = pos + 1;

					while (temp_pos < (*each).length()) {
						getFirstSet((*each)[temp_pos], chFollow);
						set<char> temp;
						getFirstSet((*each)[temp_pos], temp);

						if (temp.count('#') == 1 && temp_pos < (*each).length() - 1)
							;//temp�к��п� && û�б���������ʽβ
						else if (temp.count('#') == 1 && temp_pos == (*each).length() - 1)//temp�к��� && ����������ʽβ
							getFollowSet(p->first, chFollow, str + ch);
						else if (temp.count('#') == 0)//temp�в�����
							break;
						temp_pos++;
					}	
				}
			}
		}
	}
}