
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
	//costu en yüksek vertexi bul
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
	/*input dosyasýnýn biçimi þu þekilde
	(a b c,d,e)
	a kendisine gidlen vertex
	b a'ya gitmenin maliyeti
	c,d,e a'ya gidilmeden önce ziyaret edilmesi gerekenler
	yani (a b c,d,e) yi açýk olarak þöyle alacaðýz vektörlere
	(a b c)
	(a b d)
	(a b e)
	yani aslýnda 3 tane edge var	*/

	while (getline(input, line)) {
		istringstream ss(line);
		string token;
		//Separate string based on commas and white spaces
		getline(ss, token, ' ');	//boþluða göre ayir
		ending_vert_set.push_back(stoi(token));
		getline(ss, token, ' ');
		edge_weights.push_back(stoi(token));
		getline(ss, token, ',');	//virgüle göre ayýr
		starting_vert_set.push_back(stoi(token));
		/*ilk okuduðumuz deðer kendisine gidilen vertex. ortadaki edge aðýrlýðý. sondaki ise kendisinden çýkýlan edge*/
		num_edges++;	//böylece bir edge imiz daha olmuþ oldu
		while (getline(ss, token, ',')) {
			/* (a b c) þeklinde deðil de (a b c,d,e) þeklinde girdi var */
			num_edges++;
			ending_vert_set.push_back(ending_vert_set.back());
			edge_weights.push_back(edge_weights.back());
			starting_vert_set.push_back(stoi(token));// c,d,e eklenecek
		}
		num_vertex++;  //inputtaki her line bi vertex
	}

	//indegree bi vertexe gelen toplam edge sayýsdýr
	int *indegree = new int[num_vertex];
	memset(indegree, 0, sizeof(int) * num_vertex);// arrayi sýfýrla doldur

												  /*v'nin her bir komþusu u için indegree[u] += 1*/
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
	//indegreesi 0 olan yani baþlangýç vertexlerini Q ya ekle. 
	for (j = 0; j<num_vertex; j++)
		if (indegree[j] == 0)
			Q.push(starting_vert_set[j]);

	int curr_vert_id, my_vert_ind, neigh_id;
	vector<int> critical_path;
	while (!Q.empty()) {
		curr_vert_id = Q.front();  //Q daki ilk vertexin indexini al
		Q.pop(); //burada vertexi sildik
		vector<int> neigh_set, weight_set;//neighbourlarý bul
		for (my_vert_ind = 0; my_vert_ind < num_edges; my_vert_ind++) {
			if (starting_vert_set[my_vert_ind] == curr_vert_id) {
				neigh_set.push_back(ending_vert_set[my_vert_ind]);
				weight_set.push_back(edge_weights[my_vert_ind]);
			}
		}
		//bu döngüde Q daki vertexin tüm komþularýný dolanýp ayný zamanda distance güncellemesi yapýyor
		for (i = 0; i < neigh_set.size(); i++) {
			neigh_id = neigh_set[i];
			if (distance[neigh_id] <  distance[curr_vert_id] + weight_set[i]) {
				distance[neigh_id] = distance[curr_vert_id] + weight_set[i];
			}
			indegree[neigh_id] -= 1;
			if (indegree[neigh_id] == 0)
				Q.push(neigh_id);//indegreesi 0 olan komþularý da Q'ya ekle graphý dolasýyoruz
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
	//bu döngüde tersten yani sondan baþa gelip max costun hangi vertexlerden geçerek oluþmuþ olduðunu buluyoruz

	while (max_cost>0) {
		distance[max_vertex] = -1;	//max_costu iptal ediyoruz, bundan sonraki en büyük costu bulsun
		max_vertex_new = find_max_vertex(distance, num_vertex);
		/*edge_weight setinde max cost ile bundan küçük en büyük cost arasýndaki fark kadar bi deðer varsa
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
			/*max_vertex_new id'li vertex ile bizi max costa götüren vertex arasýnda edge yok oluyor. max_vertex_new den
			küçük bi sonraki en büyüðe geç*/
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
