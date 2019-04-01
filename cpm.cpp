
#include <stdio.h>
#include <string.h>
#include <queue>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include<windows.h>
#include<vector>


using namespace std;

int find_max_vertex(int *dist, int size)
{
	//costu en y�ksek vertexi bul
	int max = -1, vertex = -1;
	for (int i = 0; i< size; i++) {
		if (max < dist[i]) {
			max = dist[i];
			vertex = i;
		}
	}
	return vertex;
}

int main() {

	int i = 0, j;
	int num_vertex = 1;
	int num_edges = 0;
	HANDLE  hConsole;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	GetConsoleScreenBufferInfo(h, &csbiInfo);
	WORD wOldColorAttrs = csbiInfo.wAttributes;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	vector<int> starting_vert_set;
	vector<int> ending_vert_set;
	vector<int> edge_weights;

	ifstream input;
	input.open("input2.txt");
	string line;
	/*input dosyas�n�n bi�imi �u �ekilde
	(a b c,d,e)
	a kendisine gidlen vertex
	b a'ya gitmenin maliyeti
	c,d,e a'ya gidilmeden �nce ziyaret edilmesi gerekenler
	yani (a b c,d,e) yi a��k olarak ��yle alaca��z vekt�rlere
	(a b c)
	(a b d)
	(a b e)
	yani asl�nda 3 tane edge var	*/

	while (getline(input, line)) {
		istringstream ss(line);
		string token;
		//Separate string based on commas and white spaces
		getline(ss, token, ' ');	//bo�lu�a g�re ayir
		ending_vert_set.push_back(stoi(token));
		getline(ss, token, ' ');
		edge_weights.push_back(stoi(token));
		getline(ss, token, ',');	//virg�le g�re ay�r
		starting_vert_set.push_back(stoi(token));
		/*ilk okudu�umuz de�er kendisine gidilen vertex. ortadaki edge a��rl���. sondaki ise kendisinden ��k�lan edge*/
		num_edges++;	//b�ylece bir edge imiz daha olmu� oldu
		while (getline(ss, token, ',')) {
			/* (a b c) �eklinde de�il de (a b c,d,e) �eklinde girdi var */
			num_edges++;
			ending_vert_set.push_back(ending_vert_set.back());
			edge_weights.push_back(edge_weights.back());
			starting_vert_set.push_back(stoi(token));// c,d,e eklenecek
		}
		num_vertex++;  //inputtaki her line bi vertex
	}

	//indegree bi vertexe gelen toplam edge say�sd�r
	int *indegree = new int[num_vertex];
	memset(indegree, 0, sizeof(int) * num_vertex);// arrayi s�f�rla doldur

												  /*v'nin her bir kom�usu u i�in indegree[u] += 1*/
	for (j = 0; j<num_edges; j++)
		indegree[ending_vert_set[j]]++;

	/*SetConsoleTextAttribute(hConsole, 5);
	for (j = 0; j<num_vertex; j++)
	cout << "indegree " << j << "=" << indegree[j] << endl;
	cout << endl;
	*/
	queue<int> Q;
	int *distance = new int[num_vertex];
	memset(distance, 0, sizeof(int) * num_vertex);
	//indegreesi 0 olan yani ba�lang�� vertexlerini Q ya ekle. 
	for (j = 0; j<num_vertex; j++)
		if (indegree[j] == 0)
			Q.push(starting_vert_set[j]);

	int curr_vert_id, my_vert_ind, neigh_id;
	vector<int> critical_path;
	while (!Q.empty()) {
		curr_vert_id = Q.front();  //Q daki ilk vertexin indexini al
		Q.pop(); //burada vertexi sildik
		vector<int> neigh_set, weight_set;//neighbourlar� bul
		for (my_vert_ind = 0; my_vert_ind < num_edges; my_vert_ind++) {
			if (starting_vert_set[my_vert_ind] == curr_vert_id) {
				neigh_set.push_back(ending_vert_set[my_vert_ind]);
				weight_set.push_back(edge_weights[my_vert_ind]);
			}
		}
		//bu d�ng�de Q daki vertexin t�m kom�ular�n� dolan�p ayn� zamanda distance g�ncellemesi yap�yor
		for (i = 0; i < neigh_set.size(); i++) {
			neigh_id = neigh_set[i];
			if (distance[neigh_id] <  distance[curr_vert_id] + weight_set[i]) {
				distance[neigh_id] = distance[curr_vert_id] + weight_set[i];
			}
			indegree[neigh_id] -= 1;
			if (indegree[neigh_id] == 0)
				Q.push(neigh_id);//indegreesi 0 olan kom�ular� da Q'ya ekle graph� dolas�yoruz
		}
	}

	SetConsoleTextAttribute(hConsole, 2);
	cout << endl << "*********DISTANCES********" << endl;
	for (j = 0; j < num_vertex; j++)
		if (j != 0)
			cout << j << ". distance :" << distance[j] << endl;
	cout << endl;

	int max_vertex = find_max_vertex(distance, num_vertex);
	int max_cost = distance[max_vertex];

	SetConsoleTextAttribute(hConsole, 4);
	cout << endl << endl << "Total cost " << max_cost << endl << endl;

	int max_vertex_new;
	//bu d�ng�de tersten yani sondan ba�a gelip max costun hangi vertexlerden ge�erek olu�mu� oldu�unu buluyoruz

	while (max_cost>0) {
		distance[max_vertex] = -1;	//max_costu iptal ediyoruz, bundan sonraki en b�y�k costu bulsun
		max_vertex_new = find_max_vertex(distance, num_vertex);
		/*edge_weight setinde max cost ile bundan k���k en b�y�k cost aras�ndaki fark kadar bi de�er varsa
		critical pathin bir edgeini ve 2 vertexini buluyoruz*/
		int diff = max_cost - distance[max_vertex_new];
		bool edge_exist = false;
		for (i = 0; i < num_edges; i++) {
			int svi = starting_vert_set[i];
			int evi = ending_vert_set[i];
			if (max_vertex == evi && max_vertex_new == svi) {
				edge_exist = true;
				if (diff == edge_weights[i]) {
					critical_path.push_back(max_vertex);
					max_vertex = max_vertex_new;
					max_cost = distance[max_vertex_new];
				}
			}
		}
		if (edge_exist == false) {
			/*max_vertex_new id'li vertex ile bizi max costa g�t�ren vertex aras�nda edge yok oluyor. max_vertex_new den
			k���k bi sonraki en b�y��e ge�*/
			distance[max_vertex_new] = -1;
		}
	}

	SetConsoleTextAttribute(hConsole, 3);
	cout << "Path is as follows" << endl;

	for (i = critical_path.size() - 1; i >= 0; i--) {
		cout << "-->[" << critical_path[i] << "]";
	}

	SetConsoleTextAttribute(h, wOldColorAttrs);

	cout << endl;
	system("pause");
	return (0);
}
