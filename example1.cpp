#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <armadillo>
#include <map> 
#define MAX_ALPHABET_LEN 80000
#define Kostyl_LEN 80000
#define Kasisky_len 3

using namespace std;
using namespace arma;

// NOTE: the C++11 "auto" keyword is not recommended for use with Armadillo objects and functions

ofstream Out_Put;
ifstream In_Put;


void Hill_Crypt();
void Hill_Decrypt();
void Vigin_part();
void Hill_part();
int Check_Key();
void Bigram_Create(string file_to_open);
void extended_euclid(int a, int b, int* x, int* y, int* d);
int Evkild(int a, int b);

void Vigin_crypt();
void Vigin_decrypt();
void Crack_Kasisky();
void Crack_Kasisky_3();
void Crack_Kasisky_2();
void Crack_Analys();
char* Global_Buffer = new char[Kostyl_LEN ];
int current_sf_len;

mat Key_Block(2, 2);
mat Reverse_Key_Block(2, 2);
int* base_message;
int* crypt_message;
int vig_key[3] = { 9,4,16 };
int Len_Bigram_Array;
map <char, int> bukvar;
map <string, int> bukvar_bigram;
char Key_input[5] = { 0,0,0,0 };




int main(int argc, char** argv)
  {
	base_message = new int[MAX_ALPHABET_LEN];
	crypt_message = new int[MAX_ALPHABET_LEN];
	setlocale(LC_ALL, "Russian");
	int flag =-1;
	while (flag != 0)
	{
		if (flag == 1) Hill_part();
		
		else
			if (flag == 2) Vigin_part();
			else
				if (flag == 3) Crack_Kasisky();
				else
					if (flag == 4) Crack_Analys();
			else printf("\n");
			
		printf("Usage:\n0. To stop\n1. To hill\n2. To Vigen\n3.To crack Vigen by Kasisky\n4.To crack Hill bigrams only with crypted\n");
		cin >> flag;
		cout << "Flag was " << flag << endl;
	}
	printf("That's all, folks\n");
	return 0;
}

void Crack_Kasisky()
{
	cout << "Choose len for word 2 or 3: ";
	int i = 0;
	cin >> i;
	switch (i)
	{
	case 2: Crack_Kasisky_2();
			break;
	case 3: Crack_Kasisky_3();
			break;
	default:
		cout << "WROOOOOOOOOOOONG\n";
		break;
	}
}

void Hill_part()
{
	printf("1 to crypt\n2 to decrypt\n");
	int flag;
	cin >> flag;
	if (flag == 1) Hill_Crypt();
	
	else
		if (flag == 2) Hill_Decrypt();
		
		else printf("Wrong input\n");

	return;
}


void Hill_Crypt()
{


	for (int i = 0; i < MAX_ALPHABET_LEN; i++)
	{
		Global_Buffer[i] = 0;
		base_message[i] = 0;
		crypt_message[i] = 0;
	}

	printf("Enter filename: ");
	string file_to_open;
	cin >> file_to_open;

	//----------------------------------

	In_Put.open(file_to_open , ios_base::binary);
	In_Put.read(Global_Buffer, MAX_ALPHABET_LEN);
	In_Put.close();
	cout << "Len = " << strlen(Global_Buffer) << endl;
	//cout << "Have read\n" << Global_Buffer<<endl;
	//Do table of freq
	for (int i = 0; i < strlen(Global_Buffer); i++)
	{

		if (bukvar.find(Global_Buffer[i]) == bukvar.end()) bukvar.insert(pair<char, int>(Global_Buffer[i], 1));
		else		
			bukvar.find(Global_Buffer[i])->second++;
			

		if (i % 2 == 0)
		{
			string key;
			key.assign(Global_Buffer + i, 2);	 

			if (bukvar_bigram.find(key) == bukvar_bigram.end()) bukvar_bigram.insert(pair <string, int>(key, 1));
			else
				bukvar_bigram.find(key)->second++;
		}
	}
	//print bukvar
	Out_Put.open(file_to_open + "_freq.txt");
	for (auto it = bukvar.begin(); it != bukvar.end(); it++)
	{
		cout << it->first << " - " << it->second << " - " << (int)it->first << endl;;
		Out_Put << it->first << ' ' << it->second << endl;
	}
	Out_Put.close();
	//print bigram
	Out_Put.open(file_to_open + "_bigram.txt");
	for (auto it = bukvar_bigram.begin(); it != bukvar_bigram.end(); ++it)
	{
		cout << it->first << " - " << it->second << " - | " << (int)it->first.at(0)<<" " <<(int)it->first.at(1) << endl;
		Out_Put << it->first << '\n' << it->second << endl;
	}
	Out_Put.close();

	//--------------------------------



	//cout <<"------------------\n";
	


	Out_Put.open(file_to_open + "_crypted.txt", ios_base::binary);

	// now cryptttt. first, we need key
	current_sf_len = bukvar.size();
	cout << "base = " << current_sf_len << endl;
	cout << "Enter key. N=2: ";
	cin >> Key_input;
	while (!Check_Key())
	{
		cout << "Error, wrong key, try again : ";
		cin >> Key_input;
	}

	cout << "Key accepted. Start crypting\n";

	//ok, we have key and its ok	

	//dont sleep, neo. 02653, roaches, silence is death
	int flag = strlen(Global_Buffer);
	

	for (int i = 0; i < flag; i++)
	{
		auto it = bukvar.begin();
		while (it->first != Global_Buffer[i] )
		{
			base_message[i]++;
			it++;			
		}
	}
	flag += flag % 2;
	/*
	for (int i = 0; i < flag; i++) printf("%2c ", Global_Buffer[i]);
	cout << endl;

	for (int i = 0; i < flag; i++) printf("%2d ", base_message[i]);
	cout << endl;
	*/

	for (int i = 0; i < flag; )
	{
		mat Block(2, 1);
		for (int j = 0; j < 2; j++) Block.at(j, 0) = base_message[i + j];
	
		//Block.print("Have: ");
		
		Block = Key_Block * Block;		

		for (int j = 0; j < 2; j++)
		{
			int toput = ((int)Block.at(j, 0)) % current_sf_len;
			int check = 0;
			for (auto it = bukvar.begin(); it != bukvar.end(); it++)
			{
				if (check == toput)
				{
					Out_Put << it->first;
					//printf("%2c ", it->first);
					crypt_message[i + j] = toput;
					break;
				}
				check++;
			}
		}

		//03:20 some ost from summer
		
		i += 2;
	}
	In_Put.close();
	Out_Put.close();
	cout << endl;
	//for (int i = 0; i < flag; i++) printf("%2d ", crypt_message[i]);
	cout << endl;

}



int Check_Key()
{
	//cout << "Checking key..." << endl;
	if (strlen(Key_input) != 4) return 0;
	int flag = -3;

	int Key_kode[4] = { 0,0,0,0 };
	//check, if all symbols are in table
	for (int j = 0; j < 4; j++) if (bukvar.find(Key_input[j]) != bukvar.end())
	{
		
		auto it = bukvar.begin();
		while (it->first != Key_input[j])
		{
			Key_kode[flag + 3]++;
			it++;
		}		 
		flag++;
	}
	current_sf_len = bukvar.size();
	if (flag != 1) return 0;
	//copy symbols to key matrix
	Key_Block << Key_kode[0] << Key_kode[1] << endr << Key_kode[2] << Key_kode[3];
	Key_Block.print("Key block:");
	//check if matrix det!=0....
	int deter = (int)det(Key_Block) % current_sf_len;
	if (deter < 0) deter += current_sf_len;
	cout << "Det: " << deter <<endl;
	if (det(Key_Block) == 0) return 0;
	cout << "Base: " << current_sf_len << endl;
	//...and NOD(det,base) == 1
	flag = Evkild(deter, current_sf_len);
	cout << "Evkild: " << flag << endl;
	if (flag != 1) return 0;
	//if all ok, we have our matrix and it have inverse.

	// HATE IT. Damn, spen all day googling what wrong

	//armad have inverse func, but cant in addition.
	Reverse_Key_Block << Key_Block.at(1, 1) << -Key_Block.at(0, 1)<<endr << -Key_Block.at(1, 0) << Key_Block.at(0, 0);
	
	
	//Reverse_Key_Block.print("ADJ block: ");
	//
		
	int x,y, d;
	extended_euclid(deter, current_sf_len, &d,&y, &x);

	d %= current_sf_len;
	if (d < 0) d += current_sf_len;

	cout << "Det ^-1: " << d << endl;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
		{
			//cout <<"i= "<<i+j<<" " <<(int)Reverse_Key_Block.at(i, j) << " * " << d << " mod " << current_sf_len << endl;
			//cout << "i= " << i + j << " " << Reverse_Key_Block.at(i, j) << " * " << d << " mod " << current_sf_len << endl;
			Reverse_Key_Block.at(i, j) = (int)(Reverse_Key_Block.at(i, j) * d) % current_sf_len;
			if (Reverse_Key_Block.at(i, j) < 0) Reverse_Key_Block.at(i, j) = Reverse_Key_Block.at(i, j) + current_sf_len;
			//cout << Reverse_Key_Block << endl;
		}
	Reverse_Key_Block.print("Reverse block: ");

	return flag;
}

int Evkild(int a, int b)
{
	if (b == 0) return a;

	return Evkild(b, a % b);
}

void Hill_Decrypt()
{

	for (int i = 0; i < Kostyl_LEN; i++)
	{
		Global_Buffer[i] = 0;
		base_message[i] = 0;
		crypt_message[i] = 0;
	}

	printf("Enter filename: ");
	string file_to_open;
	cin >> file_to_open;

	//----------------------------------

	In_Put.open(file_to_open, ios_base::binary);
	In_Put.read(Global_Buffer, Kostyl_LEN);
	In_Put.close();
	//cout << Global_Buffer << endl;
	printf("Enter bukvar name: ");
	
	cin >> file_to_open;
	In_Put.open(file_to_open);
	string key;
	string value;

	while ( getline(In_Put, key, ' ') ) 
	{
		getline(In_Put, value);
		//cout << "Got line" << endl;
		if ((int)key[0] == 0) key[0] = ' ';
		
		bukvar.insert( pair <char, int>(key[0], stoi(value) ) );
		cout <<key[0] << " - " << stoi(value) <<" "<< endl;
		key.erase();
		value.erase();
	}
	In_Put.open(file_to_open);
	//for spaces

	//----------------------------------



	Out_Put.open(file_to_open + "_uncrypted.txt", ios_base::binary);

	// now cryptttt. first, we need key
	current_sf_len = bukvar.size();
	cout << "base = " << current_sf_len << endl;
	cout << "Enter key. N=2: ";
	cin >> Key_input;
	while (!Check_Key())
	{
		cout << "Error, wrong key, try again : ";
		cin >> Key_input;
	}

	cout << "Key accepted. Start crypting\n";

	//ok, we have key and its ok	

	//dont sleep, neo. 02653, roaches, silence is death
	int flag = strlen(Global_Buffer);
	cout << "Seems ok\n";

	for (int i = 0; i < flag; i++)
	{
		auto it = bukvar.begin();
		while (it->first != Global_Buffer[i])
		{
			//cout << "i = " << i << " gl= " << (int)Global_Buffer[i] << " it-fr= " << (int)it->first<<endl;
			base_message[i]++;
			it++;
			
		}
	}
	flag += flag % 2;
	cout << "Seems ok\n";
	//for (int i = 0; i < flag; i++) printf("%2c ", Global_Buffer[i]);
	//cout << endl;

	//for (int i = 0; i < flag; i++) printf("%2d ", base_message[i]);
	//cout << endl;


	for (int i = 0; i < flag; )
	{
		mat Block(2, 1);
		for (int j = 0; j < 2; j++) Block.at(j, 0) = base_message[i + j];

		//Block.print("Have: ");
		Block = Reverse_Key_Block * Block;

		for (int j = 0; j < 2; j++)
		{
			int toput = ((int)Block.at(j, 0)) % current_sf_len;
			int check = 0;
			for (auto it = bukvar.begin(); it != bukvar.end(); it++)
			{
				if (check == toput)
				{
					Out_Put << it->first;
					//printf("%2c ", it->first);
					crypt_message[i + j] = toput;
					break;
				}
				check++;
			}
		}

		//03:20 some ost from summer

		i += 2;
	}
	In_Put.close();
	Out_Put.close();
	cout << endl;
	//for (int i = 0; i < flag; i++) printf("%2d ", crypt_message[i]);
	cout <<"DONE"<< endl;

}



void Vigin_part()
{
	cout << "VIGIN PART" << endl;
	printf("1 to crypt\n2 to decrypt\n");
	int flag;
	cin >> flag;
	if (flag == 1)
		 Vigin_crypt();
	
	else
		if (flag == 2)
			 Vigin_decrypt();
		else printf("Wrong input\n");
		
	return;
}


void Vigin_crypt()
{


	for (int i = 0; i < Kostyl_LEN; i++)
	{
		Global_Buffer[i] = 0;
		base_message[i] = 0;
		crypt_message[i] = 0;
	}

	printf("Enter filename: ");
	string file_to_open;
	cin >> file_to_open;

	//----------------------------------

	In_Put.open(file_to_open, ios_base::binary);
	In_Put.read(Global_Buffer, Kostyl_LEN);
	In_Put.close();

	//cout << "Have read\n" << Global_Buffer<<endl;
	//Do table of freq
	for (int i = 0; i < strlen(Global_Buffer); i++)
	{

		if (bukvar.find(Global_Buffer[i]) == bukvar.end()) bukvar.insert(pair<char, int>(Global_Buffer[i], 1));
		else
			bukvar.find(Global_Buffer[i])->second++;


		if (i % 2 == 0)
		{
			string key;
			key.assign(Global_Buffer + i, 2);

			if (bukvar_bigram.find(key) == bukvar_bigram.end()) bukvar_bigram.insert(pair <string, int>(key, 1));
			else
				bukvar_bigram.find(key)->second++;
		}
	}
	//print bukvar
	Out_Put.open(file_to_open + "_freq.txt");
	for (auto it = bukvar.begin(); it != bukvar.end(); it++)
	{
		//cout << it->first << " - " << it->second << endl;
		Out_Put << it->first << ' ' << it->second << endl;
	}
	Out_Put.close();
	//print bigram
	Out_Put.open(file_to_open + "_bigram.txt");
	for (auto it = bukvar_bigram.begin(); it != bukvar_bigram.end(); ++it)
	{
		//cout << it->first << " - " << it->second << endl;
		Out_Put << it->first << '\n' << it->second << endl;
	}
	Out_Put.close();

	//--------------------------------



	//cout <<"------------------\n";

	int flag = strlen(Global_Buffer);
	int rounder = 0;
	current_sf_len = bukvar.size();
	Out_Put.open(file_to_open + "_Vis_crypted.txt", ios_base::binary);

	for (int i = 0; i < flag; i++)
	{
		auto it = bukvar.begin();
		while (it->first != Global_Buffer[i])
		{
			base_message[i]++;
			it++;
		}
	}



	for (int i = 0; i < flag; i++) printf("%2c ", Global_Buffer[i]);
	cout << endl;

	for (int i = 0; i < flag; i++) printf("%2d ", base_message[i]);
	cout << endl;



	for (int i = 0; i < flag; i++)
	{
		auto it = bukvar.begin();
		crypt_message[i] = (base_message[i] + vig_key[rounder]) % current_sf_len;
		int j = 0;
		while (j< crypt_message[i])
		{
			j++;
			it++;
		}
		printf("%2c ", it->first);
		Out_Put << it->first;
		rounder++;
		rounder %= 3;
	}

	cout << endl;	
	for (int i = 0; i < strlen(Global_Buffer); i++) printf("%2d ", crypt_message[i]);
	cout << endl;
	In_Put.close();
	Out_Put.close();
	return;
}

void Vigin_decrypt()
{

	for (int i = 0; i < MAX_ALPHABET_LEN; i++)
	{
		Global_Buffer[i] = 0;
		base_message[i] = 0;
		crypt_message[i] = 0;
	}

	printf("Enter filename: ");
	string file_to_open;
	cin >> file_to_open;

	//----------------------------------

	In_Put.open(file_to_open, ios_base::binary);
	In_Put.read(Global_Buffer, MAX_ALPHABET_LEN);
	In_Put.close();
	//cout << Global_Buffer << endl;
	printf("Enter bukvar name: ");

	cin >> file_to_open;
	In_Put.open(file_to_open);
	string key;
	string value;

	while (getline(In_Put, key, ' '))
	{
		getline(In_Put, value);
		if ((int)key[0] == 0) key[0] = ' ';
		bukvar.insert(pair <char, int>(key[0], stoi(value)));
		cout << key[0] << " - " << stoi(value) << endl;
	}

	In_Put.open(file_to_open);

	Out_Put.open(file_to_open + "_Vis_uncrypted.txt", ios_base::binary);






	int flag = strlen(Global_Buffer);
	int rounder = 0;
	current_sf_len = bukvar.size();
	

	for (int i = 0; i < flag; i++)
	{
		auto it = bukvar.begin();
		while (it->first != Global_Buffer[i])
		{
			base_message[i]++;
			it++;
		}
	}



	//for (int i = 0; i < flag; i++) printf("%2c ", Global_Buffer[i]);
	//cout << endl;

	//for (int i = 0; i < flag; i++) printf("%2d ", base_message[i]);
	//cout << endl;



	for (int i = 0; i < flag; i++)
	{
		auto it = bukvar.begin();
		crypt_message[i] = (base_message[i] - vig_key[rounder] + current_sf_len) % current_sf_len;
		int j = 0;
		while (j < crypt_message[i])
		{
			j++;
			it++;
		}
		printf("%2c ", it->first);
		Out_Put << it->first;
		rounder++;
		rounder %= 3;
	}

	cout << endl;
//	for (int i = 0; i < strlen(Global_Buffer); i++) printf("%2d ", crypt_message[i]);
//	cout << endl;
	In_Put.close();
	Out_Put.close();
	return;
}

// расширенный алгоритм Евклида
void extended_euclid(int a, int b, int* x, int* y, int* d)

/* 
	calculates a * *x + b * *y = gcd(a, b) = *d 
*/
{
	int q, r, x1, x2, y1, y2;
	if (b == 0) 
	{
		*d = a, * x = 1, *y = 0;
		return;
	}
	x2 = 1, x1 = 0, y2 = 0, y1 = 1;

	while (b > 0)
	{

		q = a / b, r = a - q * b;

		*x = x2 - q * x1, * y = y2 - q * y1;

		a = b, b = r;

		x2 = x1, x1 = *x, y2 = y1, y1 = *y;

	}

	*d = a, * x = x2, * y = y2;
}


void Crack_Kasisky_3()
{


	for (int i = 0; i < MAX_ALPHABET_LEN; i++)
	{
		Global_Buffer[i] = 0;
		base_message[i] = 0;
		crypt_message[i] = 0;
	}

	printf("Enter filename: ");
	string file_to_open;
	cin >> file_to_open;

	//----------------------------------
	//ijflmju990 ██
	In_Put.open(file_to_open, ios_base::binary);
	In_Put.read(Global_Buffer, Kostyl_LEN);
	In_Put.close();
	cout << Global_Buffer << endl;
	printf("Enter bukvar name: ");

	cin >> file_to_open;
	In_Put.open(file_to_open);
	string key;
	string value;

	while (getline(In_Put, key, ' '))
	{
		getline(In_Put, value);
		if ((int)key[0] == 0) key[0] = ' ';
		bukvar.insert(pair <char, int>(key[0], stoi(value)));
		//cout << key[0] << " - " << stoi(value) << endl;
	}
	int krik = 0;
	for (auto looker = bukvar.begin(); looker != bukvar.end(); looker++) printf("%3d ", krik++);
	cout << endl;
	for (auto looker = bukvar.begin(); looker != bukvar.end(); looker++) printf("%3c ", looker->first);
	cout << endl;
	for (auto looker = bukvar.begin(); looker != bukvar.end(); looker++) printf("%3d ", looker->second);
	cout << endl;
	In_Put.close();
	//cout << "Have read bukvar\n";
	int flag = strlen(Global_Buffer);	
	//cout << "len "<<flag<<endl;
	for (int i = 0; i < flag; i++)
	{
		auto it = bukvar.begin();
		while (it->first != Global_Buffer[i])
		{
			base_message[i]++;
			it++;
		}
	}
	/*
	for (int i = 0; i < flag; i++) printf("%2d ",i);
	cout << endl;
	for (int i = 0; i < flag; i++) printf("%2c ", Global_Buffer[i]);
	cout << endl;

	for (int i = 0; i < flag; i++) printf("%2d ", base_message[i]);
	cout << endl;	
	*/

	map <string, vector <int>> kasisky_test;
	for (int i = 0; i < flag- Kasisky_len; i++)
	{
		string key;
		vector <int> distanse;
		distanse.push_back(i);
		key.assign(Global_Buffer + i, Kasisky_len);
		int counter = 0;
		if (kasisky_test.find(key) == kasisky_test.end())
		{
			//███ new key.
			kasisky_test.insert(pair <string, vector<int>>(key, distanse));
			string lookup_key;
			for (int j = i + 1; j < flag - 3; j++)
			{
				lookup_key.assign(Global_Buffer + j, Kasisky_len);
				if (lookup_key == key)
				{
					kasisky_test.find(key)->second.push_back(j);
					counter++;
				}

			}
			if (counter == 0) kasisky_test.erase(key);
		}
		
	}
	//cout << endl;
	
	vector <int> max_occurs;
	for (auto it = kasisky_test.begin(); it != kasisky_test.end(); it++)
	{
		
	//	cout << it->first<<" ";		for (int dist :it->second)			cout << dist << " ";		cout << "\t|";
		
		for (int i = 0; i + 1 < it->second.size(); i++)
		{
		//	cout << it->second[i + 1] - it->second[i] << " ";
			int new_dist = it->second[i + 1] - it->second[i];
			for (int zog : max_occurs) if (zog == new_dist) { new_dist = 1; break; }
			if (new_dist != 1) max_occurs.push_back(new_dist);
		}
	//	cout << endl;
	}
	//Out_Put.open(file_to_open + "_Vis_uncrypted.txt", ios_base::binary);
	//███████████████████████████████████████████████████████
	//for (int dist : max_occurs) 		cout << dist << " ";
	//███████████████████████████████████████████████████████
	//cout << endl;
	int kasisky_otvet = 1;
	int lol = 1;
	while (max_occurs.size() != 1)
	{
		int mark = max_occurs.size()-1;
		//if (max_occurs[mark] == 0) mark--;
		int x= max_occurs[mark], y= max_occurs[mark - 1];
		
		//if (kasisky_otvet == 0)
	//	cout << endl << " SizeMax = " << mark << " X= " << x << " Y= " << y;
		extended_euclid(max_occurs[mark], max_occurs[mark - 1], &x, &y, &lol);
		if (lol != 1) kasisky_otvet = lol;
	//	cout <<" NOD = "<< kasisky_otvet << endl;


		//███████████████████████████████████████████████████████
		//for (int dist : max_occurs)		cout << dist << " ";cout <<" "<< kasisky_otvet<< endl;
		//███████████████████████████████████████████████████████
		max_occurs.pop_back();
		max_occurs.pop_back();
		max_occurs.push_back(kasisky_otvet);
		
	}



	cout << endl << "I think it: " << kasisky_otvet <<"..."<< endl;
	if (kasisky_otvet != 3)
	{
		cout << "Nooope☺☺☺☺ but lets try" << endl;
		//return;
	}
	cout << "Size is " << bukvar.size()<<endl;
	int vig_cracked[20] = { 0,0,0 };
	for (int i = 0; i < kasisky_otvet; i++)
	{
		map <char, int> vig_look;
		for (int j = i; j < flag;)
		{
			if (vig_look.find(Global_Buffer[j]) == vig_look.end()) vig_look.insert(pair <char, int>(Global_Buffer[j], 1));
			else
				vig_look.find(Global_Buffer[j])->second++;
			j += kasisky_otvet;
		}
		
		char key_MAX = 0;
		int freq_MAX = 0;
		int index_MAX = 0;
		int  open_index_MAX = 0;
		int  open_freq_MAX = 0;
		char open_key_MAX = 0;
		int index = 0;

		for (auto it = vig_look.begin(); it != vig_look.end(); it++)
		{
			if (freq_MAX < it->second)
			{
				key_MAX = it->first;
				freq_MAX = it->second;
			}
		}
	
		for (auto looker = bukvar.begin(); looker != bukvar.end(); looker++)
		{
			if (looker->first == key_MAX) break;
			index_MAX++;
		}


		for (auto looker = bukvar.begin(); looker != bukvar.end(); looker++)
		{
			if (looker->second > open_freq_MAX)
			{
				open_freq_MAX = looker->second;
				open_key_MAX = looker->first;
				open_index_MAX = index;
			}
			index++;
		}

		vig_cracked[i] = (index_MAX - open_index_MAX + bukvar.size())% bukvar.size();
		cout << "#I" << i << " = "<< (index_MAX - open_index_MAX + bukvar.size()) % bukvar.size();

		cout << endl;


	}





	int rounder = 0;
	current_sf_len = bukvar.size();




	Out_Put.open(file_to_open + "_Vis_CRACKED.txt", ios_base::binary);
	

	flag = strlen(Global_Buffer);
	cout << "Have:\n";
	for (int i = 0; i < flag; i++)
	{
		auto it = bukvar.begin();
		crypt_message[i] = (base_message[i] - vig_cracked[rounder] + current_sf_len) % current_sf_len;
		int j = 0;
		while (j < crypt_message[i])
		{
			j++;
			it++;
		}
		
		Out_Put << it->first;
		cout<< it->first;
		rounder++;
		rounder %= kasisky_otvet;
	}

	cout << "\nDONE\n";
	In_Put.close();
	Out_Put.close();

	return;
}

void Crack_Kasisky_2()
{


	for (int i = 0; i < MAX_ALPHABET_LEN; i++)
	{
		Global_Buffer[i] = 0;
		base_message[i] = 0;
		crypt_message[i] = 0;
	}

	printf("Enter filename: ");
	string file_to_open;
	cin >> file_to_open;

	//----------------------------------
	//ijflmju990 ██
	In_Put.open(file_to_open, ios_base::binary);
	In_Put.read(Global_Buffer, Kostyl_LEN);
	In_Put.close();
	cout << Global_Buffer << endl;
	printf("Enter bukvar name: ");

	cin >> file_to_open;
	In_Put.open(file_to_open);
	string key;
	string value;

	while (getline(In_Put, key, ' '))
	{
		getline(In_Put, value);
		if ((int)key[0] == 0) key[0] = ' ';
		bukvar.insert(pair <char, int>(key[0], stoi(value)));
		//cout << key[0] << " - " << stoi(value) << endl;
	}
	int krik = 0;
	for (auto looker = bukvar.begin(); looker != bukvar.end(); looker++) printf("%3d ", krik++);
	cout << endl;
	for (auto looker = bukvar.begin(); looker != bukvar.end(); looker++) printf("%3c ", looker->first);
	cout << endl;
	for (auto looker = bukvar.begin(); looker != bukvar.end(); looker++) printf("%3d ", looker->second);
	cout << endl;
	In_Put.close();
	//cout << "Have read bukvar\n";
	int flag = strlen(Global_Buffer);
	//cout << "len "<<flag<<endl;
	for (int i = 0; i < flag; i++)
	{
		auto it = bukvar.begin();
		while (it->first != Global_Buffer[i])
		{
			base_message[i]++;
			it++;
		}
	}
	/*
	for (int i = 0; i < flag; i++) printf("%2d ",i);
	cout << endl;
	for (int i = 0; i < flag; i++) printf("%2c ", Global_Buffer[i]);
	cout << endl;

	for (int i = 0; i < flag; i++) printf("%2d ", base_message[i]);
	cout << endl;
	*/

	map <string, vector <int>> kasisky_test;
	for (int i = 0; i < flag - 2; i++)
	{
		string key;
		vector <int> distanse;
		distanse.push_back(i);
		key.assign(Global_Buffer + i, 2);
		int counter = 0;
		if (kasisky_test.find(key) == kasisky_test.end())
		{
			//███ new key.
			kasisky_test.insert(pair <string, vector<int>>(key, distanse));
			string lookup_key;
			for (int j = i + 1; j < flag - 3; j++)
			{
				lookup_key.assign(Global_Buffer + j, 2);
				if (lookup_key == key)
				{
					kasisky_test.find(key)->second.push_back(j);
					counter++;
				}

			}
			if (counter == 0) kasisky_test.erase(key);
		}

	}
	//cout << endl;

	vector <int> max_occurs;
	for (auto it = kasisky_test.begin(); it != kasisky_test.end(); it++)
	{

		//	cout << it->first<<" ";		for (int dist :it->second)			cout << dist << " ";		cout << "\t|";

		for (int i = 0; i + 1 < it->second.size(); i++)
		{
			//	cout << it->second[i + 1] - it->second[i] << " ";
			int new_dist = it->second[i + 1] - it->second[i];
			for (int zog : max_occurs) if (zog == new_dist) { new_dist = 1; break; }
			if (new_dist != 1) max_occurs.push_back(new_dist);
		}
		//	cout << endl;
	}
	//Out_Put.open(file_to_open + "_Vis_uncrypted.txt", ios_base::binary);
	//███████████████████████████████████████████████████████
	//for (int dist : max_occurs) 		cout << dist << " ";
	//███████████████████████████████████████████████████████
	//cout << endl;
	int kasisky_otvet = 1;
	int lol = 1;
	while (max_occurs.size() != 1)
	{
		int mark = max_occurs.size() - 1;
		//if (max_occurs[mark] == 0) mark--;
		int x = max_occurs[mark], y = max_occurs[mark - 1];

		//if (kasisky_otvet == 0)
	//	cout << endl << " SizeMax = " << mark << " X= " << x << " Y= " << y;
		extended_euclid(max_occurs[mark], max_occurs[mark - 1], &x, &y, &lol);
		if (lol != 1) kasisky_otvet = lol;
		//	cout <<" NOD = "<< kasisky_otvet << endl;


			//███████████████████████████████████████████████████████
			//for (int dist : max_occurs)		cout << dist << " ";cout <<" "<< kasisky_otvet<< endl;
			//███████████████████████████████████████████████████████
		max_occurs.pop_back();
		max_occurs.pop_back();
		max_occurs.push_back(kasisky_otvet);

	}



	cout << endl << "I think it: " << kasisky_otvet << "..." << endl;
	if (kasisky_otvet != 3)
	{
		cout << "Nooope☺☺☺☺ but lets try" << endl;
		//return;
	}
	cout << "Size is " << bukvar.size() << endl;
	int vig_cracked[20] = { 0,0,0 };
	for (int i = 0; i < kasisky_otvet; i++)
	{
		map <char, int> vig_look;
		for (int j = i; j < flag;)
		{
			if (vig_look.find(Global_Buffer[j]) == vig_look.end()) vig_look.insert(pair <char, int>(Global_Buffer[j], 1));
			else
				vig_look.find(Global_Buffer[j])->second++;
			j += kasisky_otvet;
		}

		char key_MAX = 0;
		int freq_MAX = 0;
		int index_MAX = 0;
		int  open_index_MAX = 0;
		int  open_freq_MAX = 0;
		char open_key_MAX = 0;
		int index = 0;

		for (auto it = vig_look.begin(); it != vig_look.end(); it++)
		{
			if (freq_MAX < it->second)
			{
				key_MAX = it->first;
				freq_MAX = it->second;
			}
		}

		for (auto looker = bukvar.begin(); looker != bukvar.end(); looker++)
		{
			if (looker->first == key_MAX) break;
			index_MAX++;
		}


		for (auto looker = bukvar.begin(); looker != bukvar.end(); looker++)
		{
			if (looker->second > open_freq_MAX)
			{
				open_freq_MAX = looker->second;
				open_key_MAX = looker->first;
				open_index_MAX = index;
			}
			index++;
		}

		vig_cracked[i] = (index_MAX - open_index_MAX + bukvar.size()) % bukvar.size();
		cout << "#I" << i << " = " << (index_MAX - open_index_MAX + bukvar.size()) % bukvar.size();

		cout << endl;


	}





	int rounder = 0;
	current_sf_len = bukvar.size();




	Out_Put.open(file_to_open + "_Vis_CRACKED.txt", ios_base::binary);


	flag = strlen(Global_Buffer);
	cout << "Have:\n";
	for (int i = 0; i < flag; i++)
	{
		auto it = bukvar.begin();
		crypt_message[i] = (base_message[i] - vig_cracked[rounder] + current_sf_len) % current_sf_len;
		int j = 0;
		while (j < crypt_message[i])
		{
			j++;
			it++;
		}

		Out_Put << it->first;
		cout << it->first;
		rounder++;
		rounder %= kasisky_otvet;
	}

	cout << "\nDONE\n";
	In_Put.close();
	Out_Put.close();

	return;
}

void Crack_Analys()
{
	/*
	1.Собрать статистку n-грамм в исходном тексте (дун)
	2.Собрать статистику н-грамм в шифр тексте (в целом понятно что надо)
	3. Решить систему уравнений относительно ключа. Что за █████? 
	имхо
	О*К=С
	Имеем С. Пусть с макс частотой встречаемости.
	Берем О с макс частотой встречаемости.
	Тады К = О-1 * С. Логично? ВРоде да. 00:32, погнали. TYr, Northen Gate.
	*/

	//need coffe
	for (int i = 0; i < Kostyl_LEN; i++)
	{
		Global_Buffer[i] = 0;
		base_message[i] = 0;
		crypt_message[i] = 0;
	}

	printf("Enter filename: ");
	string file_to_open;
	cin >> file_to_open;

	//----------------------------------

	In_Put.open(file_to_open, ios_base::binary);
	In_Put.read(Global_Buffer, Kostyl_LEN);
	In_Put.close();
	//cout << Global_Buffer << endl;

	printf("Enter bukvar name: ");

	cin >> file_to_open;
	In_Put.open(file_to_open);
	string key;
	string value;

	while (getline(In_Put, key, ' '))
	{
		getline(In_Put, value);
		//cout << "Got line len " << key.size() <<endl;
		if ((int)key[0] == 0) key[0] = ' ';
		if (key.size() == 0) key += ' ';
		bukvar.insert(pair <char, int>(key[0], stoi(value)));
		//cout << key[0] << " - " << stoi(value)<<  endl;
		key.erase();
		value.erase();
	}
	In_Put.close();

	printf("Enter bi-bukvar name: ");

	cin >> file_to_open;
	In_Put.open(file_to_open);
	cout << endl;

	while (getline(In_Put, key))
	{
		//cout << "try get bi ";
		
		//cout << "Got line len " << key.size() << endl;
		//if ((int)key[0] == 0) key[0] = ' ';
		if (key.size() == 0)
		{
			string temp;
			getline(In_Put, temp);
			if (temp.size() == 0)
			{
			//	cout << "1 Double Tap\n";
				key = '\n' + '\n';
			}
			else
			{
			//	cout << "1 Single Tap\n";
				key = '\n' + temp;
			}
		//	cout << "Bugfix\n";
		
		}
		if (key.size() == 1)
		{
			string temp;
			getline(In_Put, temp);
			if (temp.size() == 0)
			{
				//cout << "2 Single Tap\n";
				key += '\n';
			}
			else
			{
				//cout << "2 Single Tap\n";
				key += temp;
			}
		}
		getline(In_Put, value);
		//cout << key << " - " << stoi(value)<<endl;
		bukvar_bigram.insert(pair <string, int>(key, stoi(value)));
	//	cout << " done " << endl;
		key.erase();
		value.erase();
	}
	In_Put.close();
	cout << "\nEnd reading\n";
	int flag = strlen(Global_Buffer);
	cout << "Global len " << flag << endl;
	//1 done
	//2 start

	map <string, int> new_bukvar_bigram;
	
	for (int i = 0; i < strlen(Global_Buffer); i++)
	{
			   		 
		if (i % 2 == 0)
		{
			string key;
			key.assign(Global_Buffer + i, 2);

			if (new_bukvar_bigram.find(key) == new_bukvar_bigram.end()) new_bukvar_bigram.insert(pair <string, int>(key, 1));
			else
				new_bukvar_bigram.find(key)->second++;
		}
	}

	int last_max_freqх[2] = { 0,0 };
	
	for (auto it = new_bukvar_bigram.begin(); it != new_bukvar_bigram.end(); it++) if (it->second > last_max_freqх[0]) last_max_freqх[0] = it->second;
	
	for (auto looker = bukvar_bigram.begin(); looker != bukvar_bigram.end(); looker++) if (looker->second > last_max_freqх[1]) last_max_freqх[1] = looker->second;
	
	int waaagh = 0;

	map <int, int> rate_of_freq_new;
	for (auto it = new_bukvar_bigram.begin(); it != new_bukvar_bigram.end(); it++)
	{
		if (rate_of_freq_new.find(it->second) == rate_of_freq_new.end()) rate_of_freq_new.insert(pair <int, int>(it->second, 1));
		else
			rate_of_freq_new.find(it->second)->second++;
	}

//	for (auto it = rate_of_freq_new.begin(); it != rate_of_freq_new.end(); it++) if (it->second > 1) rate_of_freq_new.erase(it->first);

	map <int, int> rate_of_freq_old;
	for (auto it = bukvar_bigram.begin(); it != bukvar_bigram.end(); it++)
	{
		if (rate_of_freq_old.find(it->second) == rate_of_freq_old.end()) rate_of_freq_old.insert(pair <int, int>(it->second, 1));
		else
			rate_of_freq_old.find(it->second)->second++;
	}
//	for (auto it = rate_of_freq_old.begin(); it != rate_of_freq_old.end(); it++) if (it->second > 1) rate_of_freq_old.erase(it->first);
	vector<string> X_array, X__aray, C_array, C__array;
	int got_pair = 0;
	
	auto looker_old = rate_of_freq_old.end();
	auto looker_new = rate_of_freq_new.end();
	while (got_pair != 10 && got_pair<10)
	{
		vector<string> temp_old, temp_new;
		for (auto it = bukvar_bigram.begin(); it != bukvar_bigram.end(); it++)
			if (it->second == looker_old->first) temp_old.push_back(it->first);

		for (auto it = new_bukvar_bigram.begin(); it != new_bukvar_bigram.end(); it++)
			if (it->second == looker_new->first) temp_new.push_back(it->first);

		if (temp_new.size() > 0 && temp_old.size() > 0)
		{
			for (auto look_temp_new = temp_new.begin(); look_temp_new != temp_new.end(); look_temp_new++)

				for (auto look_temp_old = temp_old.begin(); look_temp_old != temp_old.end(); look_temp_old++)
				{
					if ((*look_temp_old).size() != 0 && (*look_temp_new).size() != 0)
					{
						if (got_pair >= 10) break;
						X_array.push_back(*look_temp_old);
						C_array.push_back(*look_temp_new);
						got_pair++;
					}
				}
		}
		else
			printf("Oh, REALLY?....\n");
		looker_old--;
		looker_new--;
	}
	got_pair = 0;
	int x_have_free_pair = 0;
	string combinations[10][4];

	
	for (int i = 0; i < 10; i++)
	{
		
			for (int j=i; j<10; j++)
			if (C_array[i] != C_array[j] && X_array[i] != X_array[j])
			{

				combinations[got_pair][0] = X_array[i];
				combinations[got_pair][1] = C_array[i];
				combinations[got_pair][2] = X_array[j];
				combinations[got_pair][3] = C_array[j];
				got_pair++;
				if (got_pair == 10) break;
				//("\nnew comb %d\n", got_pair);
			}
			if (got_pair == 10) break;

	}
	
	printf(" X <-> Y\n");

	for (got_pair = 0; got_pair < 10; got_pair++)
	{
		printf("\nnew comb %d\n", got_pair);
		printf("|%2s| <-> |%2s|\n", combinations[got_pair][0], combinations[got_pair][1]);
		printf("|%2s| <-> |%2s|\n", combinations[got_pair][2], combinations[got_pair][3]);
		
	}
	
	for (; waaagh < 10; waaagh++)
	{
		cout << "START WAAAAGH. size="<< new_bukvar_bigram.size() << endl;
		string key_MAX[2];
		int freq_MAX[2] = { 0,0 };
		int index_MAX[2] = { 0,0 };
		int  open_index_MAX[2] = { 0,0 };
		int  open_freq_MAX[2] = { 0,0 };
		string open_key_MAX[2];
		int index = 0;
		open_key_MAX[0] = combinations[waaagh][0];
		open_key_MAX[1] = combinations[waaagh][2];
		key_MAX[0] = combinations[waaagh][1];
		key_MAX[1] = combinations[waaagh][3];




		int key_try[4] = { 0,0,0,0 };
		//key_try[0]=
		int x1, x2, x1_, x2_, c1, c2, c1_, c2_;
		x1 = x2 = x1_ = x2_ = c1 = c2 = c1_ = c2_ = 0;
		//x10
		cout << "Init done. crypted= >>"<< key_MAX[1]<<"<< >>"<< key_MAX[0]<<"<<"<<endl;
		cout << "Init done. open= >>" << open_key_MAX[1] << "<< >>" << open_key_MAX[0] << "<<" << endl;

		for (auto it = bukvar.begin(); it->first != open_key_MAX[0].at(0); it++)  x1_++;
		//cout << "Init done\n";
		//x20
		for (auto it = bukvar.begin(); it->first != open_key_MAX[0].at(1); it++)  x2_++;
		//x1
		for (auto it = bukvar.begin(); it->first != open_key_MAX[1].at(0); it++)  x1++;
		//x2
		for (auto it = bukvar.begin(); it->first != open_key_MAX[1].at(1); it++)  x2++;
	//	cout << "Init done\n";
		//c10
		for (auto it = bukvar.begin(); it->first != key_MAX[0].at(0); it++)  c1_++;
		//c20
		for (auto it = bukvar.begin(); it->first != key_MAX[0].at(1); it++)  c2_++;
		//c1
		for (auto it = bukvar.begin(); it->first != key_MAX[1].at(0); it++)  c1++;
		//c2
		for (auto it = bukvar.begin(); it->first != key_MAX[1].at(1); it++)  c2++;
		//printf("\n%3d %3d\n%3d %3d\n", x1, x1_, x2, x2_);
		//cout << "Init done\n";
		int bukvar_size = bukvar.size();
		//cout << "Base : " << bukvar_size<<endl;
		int det1, r0, r00,reverse_x2;
		det1 = (x1 * x2_ - x1_ * x2) % bukvar_size;
		if (det1 < 0) det1 += bukvar_size;
	//	cout << " det = " << det;
		extended_euclid(det1, bukvar_size, &det1, &r0, &r00);
		if (det1 < 0) det1 += bukvar_size;
	 //	cout << " det-1 = " << det1 << " NOD= " << r00 << endl;
		//K*X=C
		//K= C * X^-1
		//далее идет решение системы.
		if (r00 == 1) 
		{
			//extended_euclid(x2, bukvar_size, &reverse_x2, &r0, &r00);
			mat X_Block(2,2),XR_Block(2, 2), C_Block(2, 2);

			X_Block << x1 << x1_ << endr << x2 << x2_;
			C_Block << c1 << c1_ << endr << c2 << c2_;
			XR_Block << x2_ << -x1_ << endr << -x2 << x1;
			//X_Block.print("X block:");
			//C_Block.print("C block:");
			int d;
			//cout << "DB1 len=" << bukvar_size << endl;
			//cout << (int)det(X_Block) << endl;
			d= (int)det(X_Block) % bukvar_size;
			//cout << "DB2" << endl;
			if (d < 0) d += bukvar_size;
			
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					//cout <<"i= "<<i+j<<" " <<(int)XR_Block.at(i, j) << " * " << d << " mod " << current_sf_len << endl;
					//cout << "i= " << i + j << " " << Reverse_Key_Block.at(i, j) << " * " << d << " mod " << current_sf_len << endl;
					XR_Block.at(i, j) = (int)(XR_Block.at(i, j) * d) % bukvar_size;
					if (XR_Block.at(i, j) < 0) XR_Block.at(i, j) = XR_Block.at(i, j) + bukvar_size;
					//cout << Reverse_Key_Block << endl;
				}
			//cout << "Here?\n";
			//XR_Block.print("XReverse block: ");

			X_Block = C_Block * XR_Block;
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					X_Block.at(i, j) = ((int)X_Block.at(i, j) % bukvar_size + bukvar_size) % bukvar_size;
			//X_Block.print("X^(-1)*C block, key try");
			
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					key_try[i + j] = X_Block.at(i, j);
	//		cout << "X2= " << x2 << " X2-1= " << reverse_x2 << " NOD = " << r00 << endl;
			if (d == 0) r00 = 0;
			if (r00 == 1)
			{
				/*
				key_try[0] = ((x2_ * c1 - x2 * c1_) * det) % bukvar_size;
				if (key_try[0] < 0) key_try[0] += bukvar_size;
				//cout << "Check\n(( " << c1 << " - " << key_try[0] << " * " << x1 << " ) * " << reverse_x2 << " ) % " << bukvar_size << endl;
				key_try[1] = ((c1 - key_try[0] * x1) * reverse_x2) % bukvar_size;
				if (key_try[1] < 0) key_try[1] += bukvar_size;
				key_try[2] = ((x2_ * c2 - x2 * c2_) * det) % bukvar_size;
				if (key_try[2] < 0) key_try[2] += bukvar_size;
				key_try[3] = ((c2 - key_try[2] * x1) * reverse_x2) % bukvar_size;				
				if (key_try[3] < 0) key_try[3] += bukvar_size;
				*/

				//cout << key_try[0] << " " << key_try[1] << endl << key_try[2] << " " << key_try[3] << endl;
				/*
				16.0000   13.0000
				14.0000   21.0000
				*/
				for (int i = 0; i < 4; i++)
				{
					
					auto it = bukvar.begin();
					while (key_try[i])
					{
						key_try[i]--;
						it++;
					}
					Key_input[i] = it->first;
				}
				
				cout << "#" << waaagh + 1 << " Key is: " << Key_input << endl;
			   	

				if (Check_Key())
				{
					cout << "Key accepted. Start crypting\n";

					//ok, we have key and its ok	

					//dont sleep, neo. 02653, roaches, silence is death
					int flag = strlen(Global_Buffer);
					cout << "Seems ok\n";

					for (int i = 0; i < flag; i++)
					{
						auto it = bukvar.begin();
						while (it->first != Global_Buffer[i])
						{
							//cout << "i = " << i << " gl= " << (int)Global_Buffer[i] << " it-fr= " << (int)it->first<<endl;
							base_message[i]++;
							it++;

						}
					}
					flag += flag % 2;
					//cout << "Seems ok\n";
					//for (int i = 0; i < flag; i++) printf("%2c ", Global_Buffer[i]);
					//cout << endl;

					//for (int i = 0; i < flag; i++) printf("%2d ", base_message[i]);
					//cout << endl;


					for (int i = 0; i < flag && i<200; )
					{
						mat Block(2, 1);
						for (int j = 0; j < 2; j++) Block.at(j, 0) = base_message[i + j];

						//Block.print("Have: ");
						Block = Reverse_Key_Block * Block;

						for (int j = 0; j < 2; j++)
						{
							int toput = ((int)Block.at(j, 0)) % current_sf_len;
							int check = 0;
							for (auto it = bukvar.begin(); it != bukvar.end(); it++)
							{
								if (check == toput)
								{
									//Out_Put << it->first;
									printf("%2c", it->first);
									crypt_message[i + j] = toput;
									break;
								}
								check++;
							}
						}
						//03:20 some ost from summer
						i += 2;
					}
					cout << "\n#" << waaagh + 1 << " Key is: " << Key_input << endl;
					cout << "Save it? 1/0 ...";
					int choise;
					cin >> choise;
					if (choise||waaagh==10)
					{
						Out_Put.open("Cracked_Hill.txt");
						for (int i = 0; i < flag; )
						{
							mat Block(2, 1);
							for (int j = 0; j < 2; j++) Block.at(j, 0) = base_message[i + j];

							//Block.print("Have: ");
							Block = Reverse_Key_Block * Block;

							for (int j = 0; j < 2; j++)
							{
								int toput = ((int)Block.at(j, 0)) % current_sf_len;
								int check = 0;
								for (auto it = bukvar.begin(); it != bukvar.end(); it++)
								{
									if (check == toput)
									{
										Out_Put << it->first;
										//printf("%2c ", it->first);
										crypt_message[i + j] = toput;
										break;
									}
									check++;
								}
							}
							//03:20 some ost from summer
							i += 2;
						}
						Out_Put.close();
						waaagh = 101;
					}
					
					
					cout << endl;
				}

				else cout << "#" << waaagh + 1 << "Wrong key >" << Key_input <<"<"<< endl;

			}
			else
			{
				cout << "#" << waaagh + 1 <<"Don't have inverse for determ " << det1 << endl;
			}
		}
		else
		{
			cout << "#" << waaagh + 1<<"Don't have inverse for : "<<endl;
			printf("%3d %3d\n%3d %3d\n", x1, x1_, x2, x2_);
		}	
		
		//(new_bukvar_bigram.find(key_MAX[1]))
		//new_bukvar_bigram.erase(key_MAX[1]);
	}	

	if (waaagh != 101) cout << "No solution was found\n";

	return;
}


