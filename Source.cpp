#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;

class Graph /*representation of undirected graph in adjacency list (using AVL tree)*/
{
private: /*definition of some structures in private, which will be used in this class (NOT outside the class)*/
	struct AVL_horizontal
	{
		string h_node;
		AVL_horizontal* left;
		AVL_horizontal* right;
		int height; /*for Balanced Factor of the AVL tree*/
	};
	struct AVL_vertical
	{
		string v_node;
		int degree;/*degree of each node in graph*/
		bool Is_in_created_cluster;/*To check whether the specific node already in some cluster/not ?*/
		AVL_horizontal* hor_root;/*AVL_hor_root*/
		AVL_vertical** heap_hor_head;/*points to the array of AVL_vertical pointers in which nodes are arranged
									 in descending order (sorted according to the degree)*/
		AVL_vertical* LEFT;
		AVL_vertical* RIGHT;
		int height;/*for Balanced Factor of the AVL tree*/
	};
	struct heap_vertical
	{
		AVL_vertical* NODE;/*points to some AVL_ver node.*/
		heap_vertical* left;
		heap_vertical* right;
	};
	struct cluster
	{
		AVL_vertical* NODE;
		cluster* left;
		cluster* right;
		int current_index;
		int height;
	};

private:/*private data members*/
	AVL_vertical* AVL_Root;
	heap_vertical* Heap_Root;
	int tot_unique_nodes;
	int tot_heap_nodes;
	int tot_clusters_formed;

private:/*private functions (which we do NOT want to be accessible outside the class)*/
	inline AVL_horizontal* insert_horizontal_vertex(string b)
	{
		AVL_horizontal* hor = new AVL_horizontal;
		hor->h_node = b;
		hor->left = hor->right = NULL;
		hor->height = 0;
		return hor;
	}
	inline AVL_vertical* insert_vertical_vertex(string a, string b)
	{
		AVL_vertical* ver = new AVL_vertical;
		ver->v_node = a;
		ver->hor_root = insert_horizontal_vertex(b);
		ver->degree = 1;
		ver->LEFT = ver->RIGHT = NULL;
		ver->height = 0;
		ver->heap_hor_head = NULL;
		ver->Is_in_created_cluster = false;
		return ver;
	}
	inline heap_vertical* create_heap_vertical_node(AVL_vertical* ver)
	{
		heap_vertical* new_heap_ver = new heap_vertical;
		new_heap_ver->left = new_heap_ver->right = NULL;
		new_heap_ver->NODE = ver;
		return new_heap_ver;
	}
	inline cluster* create_cluster(AVL_vertical* ver)
	{
		cluster* new_cluster = new cluster;
		new_cluster->left = new_cluster->right = NULL;
		new_cluster->NODE = ver;
		new_cluster->current_index = 0;
		new_cluster->height = 0;
		return new_cluster;
	}

	inline int Height_ver(AVL_vertical* temp)
	{
		/*this function is created for easily check the BF of node, and also
		return -1 when the node not exist*/
		if (temp)
			return temp->height;
		return -1;/*when the subtree does not exist, it means that height of
				  subtree is -1*/
	}
	inline int Height_hor(AVL_horizontal* temp)
	{
		if (temp)
			return temp->height;
		return -1;
	}
	inline int Height_cluster(cluster* temp)
	{
		if (temp)
			return temp->height;
		return -1;
	}
	inline void update_height_ver(AVL_vertical* temp)
	{
		temp->height = max(Height_ver(temp->LEFT), Height_ver(temp->RIGHT)) + 1;
	}
	inline void update_height_hor(AVL_horizontal* temp)
	{
		temp->height = max(Height_hor(temp->left), Height_hor(temp->right)) + 1;
	}
	inline void update_height_cluster(cluster* temp)
	{
		temp->height = max(Height_cluster(temp->left), Height_cluster(temp->right)) + 1;
	}

	AVL_vertical* ver_rotate_clockwise(AVL_vertical* A)
	{/*single rotate with left*/

		AVL_vertical* B = A->LEFT;
		/*A is the node at which the problem occur, and B is the
		left child of A*/
		A->LEFT = B->RIGHT;
		B->RIGHT = A;
		/*after rotation, we have to update the height of those nodes
		which are modified after rotation*/
		update_height_ver(A);
		update_height_ver(B);
		return B;/*B is replaced at the position of A means that
				 B becomes new root (after rotating) instead of A*/
	}
	AVL_vertical* ver_rotate_anticlockwise(AVL_vertical* A)
	{/*SingleRotateWithRight*/

		AVL_vertical* B = A->RIGHT;
		/*A is the node at which the problem occur, and B is the
		right child of A*/
		A->RIGHT = B->LEFT;
		B->LEFT = A;
		update_height_ver(A);
		update_height_ver(B);
		return B;
	}
	AVL_vertical* ver_DoubleRotateWithLeft(AVL_vertical* A)
	{/*LR rotation*/
		A->LEFT = ver_rotate_anticlockwise(A->LEFT);/*firstly, RR rotation*/
		return ver_rotate_clockwise(A);/*then LL rotation*/
	}
	AVL_vertical* ver_DoubleRotateWithRight(AVL_vertical* A)
	{/*RL rotation*/
		A->RIGHT = ver_rotate_clockwise(A->RIGHT);/*LL rotation*/
		return ver_rotate_anticlockwise(A);/*then RR rotation*/
	}

	AVL_horizontal* hor_rotate_clockwise(AVL_horizontal* A)
	{/*single rotate with left*/

		AVL_horizontal* B = A->left;
		A->left = B->right;
		B->right = A;
		update_height_hor(A);
		update_height_hor(B);
		return B;
	}
	AVL_horizontal* hor_rotate_anticlockwise(AVL_horizontal* A)
	{/*SingleRotateWithRight*/

		AVL_horizontal* B = A->right;
		A->right = B->left;
		B->left = A;
		update_height_hor(A);
		update_height_hor(B);
		return B;
	}
	AVL_horizontal* hor_DoubleRotateWithLeft(AVL_horizontal* A)
	{/*LR rotation*/
		A->left = hor_rotate_anticlockwise(A->left);/*firstly, RR rotation*/
		return hor_rotate_clockwise(A);/*then LL rotation*/
	}
	AVL_horizontal* hor_DoubleRotateWithRight(AVL_horizontal* A)
	{/*RL rotation*/
		A->right = hor_rotate_clockwise(A->right);/*LL rotation*/
		return hor_rotate_anticlockwise(A);/*then RR rotation*/
	}

	cluster* cluster_rotate_clockwise(cluster* A)
	{/*single rotate with left*/

		cluster* B = A->left;
		A->left = B->right;
		B->right = A;
		update_height_cluster(A);
		update_height_cluster(B);
		return B;
	}
	cluster* cluster_rotate_anticlockwise(cluster* A)
	{/*SingleRotateWithRight*/

		cluster* B = A->right;
		A->right = B->left;
		B->left = A;
		update_height_cluster(A);
		update_height_cluster(B);
		return B;
	}
	cluster* cluster_DoubleRotateWithLeft(cluster* A)
	{/*LR rotation*/
		A->left = cluster_rotate_anticlockwise(A->left);/*firstly, RR rotation*/
		return cluster_rotate_clockwise(A);/*then LL rotation*/
	}
	cluster* cluster_DoubleRotateWithRight(cluster* A)
	{/*RL rotation*/
		A->right = cluster_rotate_clockwise(A->right);/*LL rotation*/
		return cluster_rotate_anticlockwise(A);/*then RR rotation*/
	}

	void balance_the_subtree_after_insertion_hor(AVL_horizontal*& temp, string& n)
	{
		if (Height_hor(temp->left) - Height_hor(temp->right) == 2)/*if (bf==2)*/
		{
			if (n < temp->left->h_node)
				temp = hor_rotate_clockwise(temp);/*LL case*/
			else
				temp = hor_DoubleRotateWithLeft(temp);/*LR case*/
		}
		else if (Height_hor(temp->left) - Height_hor(temp->right) == -2)/*if (bf==-2)*/
		{
			if (n > temp->right->h_node)
				temp = hor_rotate_anticlockwise(temp);/*RR case*/
			else
				temp = hor_DoubleRotateWithRight(temp);/*RL case*/
		}
		else /*here, everthing is OK, means that tree is balanced, so do nothing*/
			;

		update_height_hor(temp);
	}
	void balance_the_subtree_after_insertion_cluster(cluster*& temp, AVL_vertical*& inserted_data)
	{
		if (Height_cluster(temp->left) - Height_cluster(temp->right) == 2)/*if (bf==2)*/
		{
			if (inserted_data->v_node < temp->left->NODE->v_node)
				temp = cluster_rotate_clockwise(temp);/*LL case*/
			else
				temp = cluster_DoubleRotateWithLeft(temp);/*LR case*/
		}
		else if (Height_cluster(temp->left) - Height_cluster(temp->right) == -2)/*if (bf==-2)*/
		{
			if (inserted_data->v_node > temp->right->NODE->v_node)
				temp = cluster_rotate_anticlockwise(temp);/*RR case*/
			else
				temp = cluster_DoubleRotateWithRight(temp);/*RL case*/
		}
		else /*here, everthing is OK, means that tree is balanced, so do nothing*/
			;

		update_height_cluster(temp);
	}
	void balance_the_subtree_after_insertion_ver(AVL_vertical*& temp, string& n)
	{
		/*this function check the balance factor of any node, and if
		not balanced then balance that node by rotation*/

		/*bf must be -1/0/1*/

		if (Height_ver(temp->LEFT) - Height_ver(temp->RIGHT) == 2)/*if (bf==2)*/
		{
			if (n < temp->LEFT->v_node)
				temp = ver_rotate_clockwise(temp);/*LL case*/
			else
				temp = ver_DoubleRotateWithLeft(temp);/*LR case*/
		}
		else if (Height_ver(temp->LEFT) - Height_ver(temp->RIGHT) == -2)/*if (bf==-2)*/
		{
			if (n > temp->RIGHT->v_node)
				temp = ver_rotate_anticlockwise(temp);/*RR case*/
			else
				temp = ver_DoubleRotateWithRight(temp);/*RL case*/
		}
		else /*here, everthing is OK, means that tree is balanced, so do nothing*/
			;

		update_height_ver(temp);/*after the insertion/deletion of any node, we have
							to update the height of every ancestor nodes on the
							path where we inserted/deleted the destination node*/
	}
	void insert_in_hor(string& n1, string& n2, AVL_horizontal*& hor, AVL_vertical*& ver)
	{
		if (hor)
		{
			if (hor->h_node > n2)
				insert_in_hor(n1, n2, hor->left, ver);
			else if (hor->h_node < n2)
				insert_in_hor(n1, n2, hor->right, ver);
			else
			{ /*2 nodes relation already exist, so do nothing*/
			}
			balance_the_subtree_after_insertion_hor(hor, n2);
		}
		else /*when the corresponding vertical`s horizontal node NOT exist*/
		{
			hor = insert_horizontal_vertex(n2);//,weight);

			ver->degree++;
			if (n1 == n2)
			{
				ver->degree++;/*degree of a self loop is 2*/
				tot_unique_nodes--;
			}
		}
	}
	void insert_in_the_cluster(cluster*& temp, AVL_vertical*& data_to_insert)
	{
		if (temp)
		{
			if (temp->NODE->v_node > data_to_insert->v_node)
				insert_in_the_cluster(temp->left, data_to_insert);
			else if (temp->NODE->v_node < data_to_insert->v_node)
				insert_in_the_cluster(temp->right, data_to_insert);
			else /*if (temp->v_node == n1) when it is desired node*/
				;
			balance_the_subtree_after_insertion_cluster(temp, data_to_insert);
		}
		else /*if desired node not found in graph*/
		{
			temp = create_cluster(data_to_insert);
		}
	}
	void insert_in_ver(string& n1, string& n2, AVL_vertical*& temp, bool& n1_exist)
	{
		if (temp)
		{
			/*Since, 2 strings can be compared, So by comparing 2 strings, I implemented the
			adjacency list using AVL tree so that insertion, and deletion is in log(N).
			so the code will be efficient. (I inserted the nodes in AVL tree on the basic
			of their string names)*/

			if (temp->v_node > n1)
				insert_in_ver(n1, n2, temp->LEFT, n1_exist);
			else if (temp->v_node < n1)
				insert_in_ver(n1, n2, temp->RIGHT, n1_exist);
			else /*if (temp->v_node == n1) when it is desired node*/
			{
				/*node1 already exists.*/
				n1_exist = true;
				insert_in_hor(n1, n2, temp->hor_root, temp);
			}
			balance_the_subtree_after_insertion_ver(temp, n1);/*when new node is inserted, then we check the
										 BF of every its ancestors, and balance the tree*/
		}
		else /*if desired node not found in graph*/
		{
			/*node1 already NOT exist in graph*/
			n1_exist = false;

			temp = insert_vertical_vertex(n1, n2);

			if (n1 == n2)
			{
				temp->degree++;/*degree of a self loop is 2*/
				tot_unique_nodes--;
			}
		}
	}

	void destroy_hor(AVL_horizontal*& temp)/*we pass the pointer by reference.*/
	{
		if (temp)
		{
			destroy_hor(temp->left);
			destroy_hor(temp->right);
			delete temp;
			temp = NULL;
		}
	}
	void destroy_ver(AVL_vertical*& temp)/*we pass the pointer by reference.*/
	{
		if (temp)
		{
			destroy_ver(temp->LEFT);
			destroy_ver(temp->RIGHT);
			destroy_hor(temp->hor_root);/*deleting corresponding horizontal list*/
			if (temp->heap_hor_head)
				delete[]temp->heap_hor_head;
			delete temp;
			temp = NULL;
		}
	}
	void destroy_heap(heap_vertical*& temp)
	{
		if (temp)
		{
			destroy_heap(temp->left);
			destroy_heap(temp->right);
			delete temp;
			temp = NULL;
		}
	}
	void destroy_cluster(cluster*& temp)
	{
		if (temp)
		{
			destroy_cluster(temp->left);
			destroy_cluster(temp->right);
			delete temp;
			temp = NULL;
		}
	}
	AVL_vertical* get_vertical_node(string& n)
	{
		return (get_vertical(AVL_Root, n));
	}
	AVL_vertical* get_vertical(AVL_vertical* temp, string& ver_data)
	{
		if (temp)
		{
			if (temp->v_node > ver_data)
				return (get_vertical(temp->LEFT, ver_data));
			else if (temp->v_node < ver_data)
				return (get_vertical(temp->RIGHT, ver_data));
			else /*if (temp->v_node == ver_data)*/
				return (temp);
		}
		return (NULL);/*when ver_data NOT exist in tree*/
	}
	AVL_horizontal* get_horizontal_node(AVL_horizontal* hor, string& hor_data)
	{
		if (hor)
		{
			if (hor->h_node > hor_data)
				return (get_horizontal_node(hor->left, hor_data));
			else if (hor->h_node < hor_data)
				return (get_horizontal_node(hor->right, hor_data));
			else
				return (hor);
		}
		return (NULL);
	}

	void traverse_AVL_hor_to_make_hor_heap_list(AVL_horizontal*& AVL_hor_Root, AVL_vertical* insert_in_it, int& index)
	{
		if (AVL_hor_Root)
		{
			insert_in_it->heap_hor_head[index] = get_vertical_node(AVL_hor_Root->h_node);
			traverse_AVL_hor_to_make_hor_heap_list(AVL_hor_Root->left, insert_in_it, ++index);
			traverse_AVL_hor_to_make_hor_heap_list(AVL_hor_Root->right, insert_in_it, ++index);
		}
		else
			index--;
	}
	void traverse_AVL_ver_to_make_max_heap(heap_vertical*& new_heap_root, AVL_vertical*& AVL_ver, int& inserted_nodes)
	{
		if (AVL_ver)
		{
			insert_in_max_ver_heap(new_heap_root, AVL_ver, inserted_nodes, 1);
			inserted_nodes++;
			traverse_AVL_ver_to_make_max_heap(new_heap_root, AVL_ver->LEFT, inserted_nodes);
			traverse_AVL_ver_to_make_max_heap(new_heap_root, AVL_ver->RIGHT, inserted_nodes);
		}
	}
	bool insert_in_max_ver_heap(heap_vertical*& new_heap_temp, AVL_vertical*& node_to_insert, int inserted_entries, int index)
	{
		if (new_heap_temp)
		{
			if (insert_in_max_ver_heap(new_heap_temp->left, node_to_insert, inserted_entries, 2 * index))
			{
				/*comparing with new created node*/
				if (new_heap_temp->left->NODE->degree > new_heap_temp->NODE->degree)
					swap(new_heap_temp->NODE, new_heap_temp->left->NODE);/*predefined function*/
			}
			else if (insert_in_max_ver_heap(new_heap_temp->right, node_to_insert, inserted_entries, 2 * index + 1))
			{
				if (new_heap_temp->right->NODE->degree > new_heap_temp->NODE->degree)
					swap(new_heap_temp->NODE, new_heap_temp->right->NODE);/*predefined function*/
			}
			else
				return false;/*if the node do not inserted at left/right side, then return false*/

			return true;
		}
		else if (inserted_entries == (index - 1))
		{
			new_heap_temp = create_heap_vertical_node(node_to_insert);
			int arr_size = node_to_insert->degree;
			node_to_insert->heap_hor_head = new AVL_vertical * [arr_size];
			for (int i = 0; i < arr_size; i++)
				node_to_insert->heap_hor_head[i] = NULL;
			int arr_index = 0;
			traverse_AVL_hor_to_make_hor_heap_list(node_to_insert->hor_root, node_to_insert, arr_index);
			sort_in_descending(node_to_insert->heap_hor_head, arr_size);

			return true;/*returning true means that the value is inserted*/
		}
		else
			return false;
	}
	void sort_in_descending(AVL_vertical** arr, const int size)
	{
		/*selection sort*/

		int max_index;
		for (int i = 0; i < (size - 1); i++)
		{
			max_index = i;
			for (int j = i + 1; j < size; j++)
			{
				if (arr[max_index]->degree < arr[j]->degree)
					max_index = j;
			}
			swap(arr[max_index], arr[i]);
		}
	}


	void extract_ver_max()/*delete the maximum degree node (from the Heap Root)*/
	{
		if (!AVL_Root)
		{
			/*Graph already empty*/
			return;
		}
		if (!Heap_Root)
		{
			/*Heap already empty*/
			return;
		}
		delete_ver_heap_max_node(Heap_Root, 1);
		tot_heap_nodes--;
	}
	void delete_ver_heap_max_node(heap_vertical*& temp, int index)
	{
		/*this function perculating UP*/
		if (temp)
		{
			if (temp->left && temp->right)
			{
				/*agar temp->LEFT ka weight greater he, tu us ko ooper le aao, wrna
				temp->RIGHT ko ooper le aao (bhare weight wala node ooper aai ga)*/
				if (temp->left->NODE->degree > temp->right->NODE->degree)
				{
					swap(temp->NODE, temp->left->NODE);/*predefined function*/
					delete_ver_heap_max_node(temp->left, 2 * index);
				}
				else
				{
					swap(temp->NODE, temp->right->NODE);/*predefined function*/
					delete_ver_heap_max_node(temp->right, 2 * index + 1);
				}
			}
			else if (temp->left)
			{
				swap(temp->NODE, temp->left->NODE);/*predefined function*/
				delete_ver_heap_max_node(temp->left, 2 * index);
			}
			else /*if (temp->Left==NULL && temp->Right==NULL)*/
			{
				if (index == tot_heap_nodes)
				{
					delete temp;
					temp = NULL;
				}
				else
					swap_with_last_ver_heap_node(temp, Heap_Root, 1);
			}
		}
	}
	void swap_with_last_ver_heap_node(heap_vertical*& to_swap, heap_vertical*& last_node, int last_index)
	{
		if (last_index == tot_heap_nodes)
		{
			swap(to_swap->NODE, last_node->NODE);/*predefined function*/
			delete last_node;
			last_node = NULL;
		}
		if (last_node)
		{
			swap_with_last_ver_heap_node(to_swap, last_node->left, 2 * last_index);
			swap_with_last_ver_heap_node(to_swap, last_node->right, 2 * last_index + 1);
		}
	}
	void make_new_cluster(const double threshold_density, const double threshold_CP)
	{
		if (!Heap_Root)
			return;
		while (Heap_Root->NODE->Is_in_created_cluster)
		{
			extract_ver_max();/*deleting the maximum degree node.*/
			if (!Heap_Root)
				return;
		}
		cluster* new_cluster = NULL;
		cluster* not_satisfied_nodes_in_new_cluster = NULL;
		int tot_edges_in_new_cluster = 0;/*E*/
		int tot_nodes_in_new_cluster = 0;/*N*/
		int next_cluster_no = ++tot_clusters_formed;
		insert_in_the_cluster(new_cluster, Heap_Root->NODE);/*start highest degree node of Graph as the cluster.*/
		tot_nodes_in_new_cluster++;
		Heap_Root->NODE->Is_in_created_cluster = true;/*make Heap_Root->NODE the member of created_cluster*/
		AVL_vertical* max_deg_holder = NULL;/*max degree holder me jo node aai ga, ham ne us ko cluster me add krwana he.*/
		traverse_the_new_cluster_to_find_max_degree_node_from_its_neighbours(new_cluster, max_deg_holder, not_satisfied_nodes_in_new_cluster);

		while (max_deg_holder)
		{
			if (Is_CP_density_satisfied(new_cluster, max_deg_holder, tot_nodes_in_new_cluster, tot_edges_in_new_cluster, threshold_density, threshold_CP))
			{
				insert_in_the_cluster(new_cluster, max_deg_holder);
				max_deg_holder->Is_in_created_cluster = true;
			}
			else
			{
				insert_in_the_cluster(not_satisfied_nodes_in_new_cluster, max_deg_holder);
			}
			max_deg_holder = NULL;
			traverse_the_new_cluster_to_find_max_degree_node_from_its_neighbours(new_cluster, max_deg_holder, not_satisfied_nodes_in_new_cluster);
		}
		print_cluster_in_file(next_cluster_no, new_cluster);/*print the new cluster in the file*/

		destroy_cluster(new_cluster);
		destroy_cluster(not_satisfied_nodes_in_new_cluster);
	}
	bool Is_CP_density_satisfied(cluster*& new_cluster, AVL_vertical*& max_deg_holder, int& tot_nodes_in_new_cluster, int& tot_edges_in_new_cluster, const double threshold_density, const double threshold_CP)
	{
		int edges_bw_node_and_cluster = 0;
		set_edges_bw_node_and_cluster(max_deg_holder, new_cluster, edges_bw_node_and_cluster);
		tot_nodes_in_new_cluster++;
		tot_edges_in_new_cluster += edges_bw_node_and_cluster;

		double d = Density(tot_nodes_in_new_cluster, tot_edges_in_new_cluster);
		if (d < threshold_density)
		{
			tot_nodes_in_new_cluster--;
			tot_edges_in_new_cluster -= edges_bw_node_and_cluster;
			return false;/*density NOT satisfied*/
		}

		int Nc = tot_nodes_in_new_cluster - 1;
		double cp = CP(d, Nc, edges_bw_node_and_cluster);
		if (cp < threshold_CP)
		{
			tot_nodes_in_new_cluster--;
			tot_edges_in_new_cluster -= edges_bw_node_and_cluster;
			return false;/*CP NOT satisfied*/
		}
		return true;/*both density & CP satisfied*/
	}
	void set_edges_bw_node_and_cluster(AVL_vertical*& max_deg_holder, cluster*& new_cluster, int& edges)/*this function sets Ec*/
	{
		if (new_cluster)
		{
			if (Is_found_in_horizontal(new_cluster->NODE->hor_root, max_deg_holder->v_node))
				edges++;
			set_edges_bw_node_and_cluster(max_deg_holder, new_cluster->left, edges);
			set_edges_bw_node_and_cluster(max_deg_holder, new_cluster->right, edges);
		}
	}
	void print_cluster_in_file(int cluster_no, cluster*& new_cluster)
	{
		ofstream fout;
		fout.open("clusters_list.txt", ios::app);
		if (!fout)
		{
			fout.open("clusters_list.txt");/*create new file and open it.*/
		}
		fout << "Cluster # " << cluster_no << endl;
		print_in_cluster(fout, new_cluster);
		fout << endl << endl;
		fout.close();
	}
	void print_in_cluster(ofstream& fout, cluster*& new_cluster)
	{
		if (new_cluster)
		{
			fout << new_cluster->NODE->v_node << "\t";
			print_in_cluster(fout, new_cluster->left);
			print_in_cluster(fout, new_cluster->right);
		}
	}

	inline double Density(int no_of_nodes, int no_of_edges)
	{
		/*this function accepts no of nodes in cluster, and no of edges in cluster, and
		calculates the density with the given data, and returns that density.*/
		double numerator = 2 * abs(no_of_edges);
		double denominator = (abs(no_of_nodes) * (abs(no_of_nodes) - 1));
		return (numerator / denominator);
	}
	inline double CP(double density, int Nc, int no_of_edges_bw_node_and_cluster)/*cluster property*/
	{
		double numerator = (abs(no_of_edges_bw_node_and_cluster));
		double denominator = density * abs(Nc);
		return (numerator / denominator);
	}

	bool Is_found_in_cluster(cluster*& temp, string& n)
	{
		if (temp)
		{
			if (temp->NODE->v_node > n)
				return Is_found_in_cluster(temp->left, n);
			else if (temp->NODE->v_node < n)
				return Is_found_in_cluster(temp->right, n);
			else /*if (temp->v_node == n)*/
				return true;
		}
		return false;
	}
	bool Is_found_in_horizontal(AVL_horizontal*& temp, string& n)
	{
		if (temp)
		{
			if (temp->h_node > n)
				return Is_found_in_horizontal(temp->left, n);
			else if (temp->h_node < n)
				return Is_found_in_horizontal(temp->right, n);
			else /*if (temp->v_node == n)*/
				return true;
		}
		return false;
	}
	bool Is_found_in_vertical(AVL_vertical*& temp, string& n)
	{
		if (temp)
		{
			if (temp->v_node > n)
				return Is_found_in_vertical(temp->LEFT, n);
			else if (temp->v_node < n)
				return Is_found_in_vertical(temp->RIGHT, n);
			else /*if (temp->v_node == n)*/
				return true;
		}
		return false;
	}
	void set_string_from_heap_hor(cluster*& new_cluster, cluster*& not_satisfied_nodes_in_new_cluster, AVL_vertical*& max_deg_holder)
	{
		AVL_vertical** search_in_it = new_cluster->NODE->heap_hor_head;

		int tot_size = new_cluster->NODE->degree;
		int current_pointed_at_index = new_cluster->current_index;

		for (int i = current_pointed_at_index; i < tot_size; i++)
		{
			if (search_in_it[i]->Is_in_created_cluster)
			{
				/*do nothing, and move forward*/
			}
			else if (Is_found_in_cluster(not_satisfied_nodes_in_new_cluster, search_in_it[i]->v_node))
			{
				/*do nothing, and move forward*/
			}
			else
			{
				if (max_deg_holder == NULL)
					max_deg_holder = search_in_it[i];
				if (search_in_it[i]->degree > max_deg_holder->degree)
					max_deg_holder = search_in_it[i];

				new_cluster->current_index = i;
				/*ab chunke curent_index tak ham ne traversing kar li he, tu next time current_index se
				aage traversing karein ge, kiu k current_index se pichle data ko ab ham ne nai dekhna.*/
				return;
			}
		}
	}
	void traverse_the_new_cluster_to_find_max_degree_node_from_its_neighbours(cluster*& new_cluster, AVL_vertical*& max_deg_holder, cluster*& not_satisfied_nodes_in_new_cluster)
	{
		/*new cluster k nodes k neighbours me jo bhi nodes hun ge, un me se sab se max degree
		wala node cluster me add ho ga (jo cluster me add hone k liay satisfy kar jae)*/
		if (new_cluster)
		{
			if (new_cluster->NODE->heap_hor_head)
				set_string_from_heap_hor(new_cluster, not_satisfied_nodes_in_new_cluster, max_deg_holder);

			traverse_the_new_cluster_to_find_max_degree_node_from_its_neighbours(new_cluster->left, max_deg_holder, not_satisfied_nodes_in_new_cluster);
			traverse_the_new_cluster_to_find_max_degree_node_from_its_neighbours(new_cluster->right, max_deg_holder, not_satisfied_nodes_in_new_cluster);
		}
	}
	void sort_nodes_in_descending_order()
	{
		/*this function creates all the vertical as well as horizontal nodes to max heap
		w.r.t degree of corresponding node*/

		if (Heap_Root)
		{
			return;/*Already Heap created*/
		}

		cout << "\nFirstly, Vertical nodes and its neighbors are sorting in descending\norder to make clusters\n";

		heap_vertical* new_heap_root = NULL;
		int inserted_nodes = 0;
		traverse_AVL_ver_to_make_max_heap(new_heap_root, AVL_Root, inserted_nodes);
		this->Heap_Root = new_heap_root;
		tot_heap_nodes = tot_unique_nodes;
		new_heap_root = NULL;
		/*now we have both AVL tree, as well as Max Heap, so whenever we have to search
		any node on the basis of node`s names, we can search from AVL tree in log(n),
		and we have also data is sorted in descending order in Max_HEAP*/
	}


public:/*public functions*/
	Graph()
	{
		this->AVL_Root = NULL;
		this->Heap_Root = NULL;
		this->tot_unique_nodes = 0;
		this->tot_heap_nodes = 0;
		this->tot_clusters_formed = 0;
	}
	void Insert_Edge(string node1, string node2)
	{
		/*insert with the method of AVL*/
		bool node1_already_exist = false;
		bool node2_already_exist = false;

		insert_in_ver(node1, node2, AVL_Root, node1_already_exist);/*vertical node1, horizontal node2*/
		insert_in_ver(node2, node1, AVL_Root, node2_already_exist);/*vertical node2, horizontal node1*/

		if (node1_already_exist && node2_already_exist) /*both node1, and node2 already exists.*/
		{
			/*do nothing*/
		}
		else if (node1_already_exist) /*node1 already exists, but node2 NOT exists.*/
		{
			tot_unique_nodes++;/*because 1 new node (node2) added in graph*/
		}
		else if (node2_already_exist) /*node2 already exists, but node1 NOT exists.*/
		{
			tot_unique_nodes++;/*because 1 new node (node1) added in graph*/
		}
		else /*both node1 & node2 already NOT exist*/
		{
			tot_unique_nodes += 2;/*because 2 new nodes (node1 & node2) added in graph*/
		}
	}
	int get_tot_nodes()
	{
		return tot_unique_nodes;
	}
	void make_all_clusters(const double threshold_d, const double threshold_CP)
	{
		if (!AVL_Root)
		{
			cout << "Graph is empty, No more clusters formed\n";
			return;
		}

		sort_nodes_in_descending_order();/*before making clusters, firstly we have to sort the nodes.*/

		ofstream fout;
		fout.open("clusters_list.txt");/*make new empty file to start printing new clusters in the file.*/
		fout.close();

		cout << "\nNow making the clusters\n";

		while (Heap_Root)
		{
			make_new_cluster(threshold_d, threshold_CP);
		}
		cout << "Total " << tot_clusters_formed << " clusters formed\n";
	}

	~Graph()
	{
		destroy_ver(AVL_Root);/*deleting AVL vertical tree*/
		this->AVL_Root = NULL;
		destroy_heap(Heap_Root);/*deleting Heap*/
		this->Heap_Root = NULL;
		this->tot_unique_nodes = 0;
		tot_heap_nodes = 0;
		tot_clusters_formed = 0;
		cout << "destroyed\n";
		system("pause>0");
	}
};


void main()
{
	string file;
	cout << "Enter the filename to read the file : ";
	getline(cin, file);

	ifstream fin;
	fin.open(file);
	if (!fin)
	{
		cout << "Error! File not open. (File may not exist)\n";
		system("pause");
		return;
	}
	int choice;
	Graph graph1;

	string x, y;
	float weight;
	cout << "\nLoading data...";
	while (fin)
	{
		fin >> x >> y;
		fin >> weight;/*skipping*/
		graph1.Insert_Edge(x, y);
	}

	fin.close();

	system("cls");
	cout << "Representation of Undirected graph in adjacency list\n\n";
	cout << "Total " << graph1.get_tot_nodes() << " unique nodes in the graph" << endl;

	double d, CP;
	cout << endl << "To make clusters :\n";
	cout << "Enter threshold value for density : ";
	cin >> d;
	cout << "Enter threshold value for CP : ";
	cin >> CP;
	graph1.make_all_clusters(d, CP);


	system("pause");
}