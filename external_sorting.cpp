#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

typedef long long ll;
typedef vector<int> vi;
typedef vector<ll> vll;
typedef vector<string> vs;
typedef vector<bool> vb;
typedef vector<vi> vvi;
typedef pair<int, int> pii;
typedef pair<ll, int> pli;
typedef pair<ll,ll> pll;
typedef vector<pii> vpii;
typedef vector<pll> vpll;
typedef vector<pli> vpli;

class min_heap
{
public:
    vpli heap;

    bool check_size(ll value)
    {
        return (value < heap.size()) ; 
    }

    bool compare(ll ind1, ll ind2)
    {
        return (heap[ind1].first > heap[ind2].first);
    }

    void heapify(ll i)
    {
        ll min_index = -1;
        ll left_chid = 2*i + 1;
        ll right_chid = 2*i + 2;

        bool check_min = heap[left_chid].first < heap[i].first;

        if(check_size(left_chid) && check_min == true) 
        {
            //left
            min_index = left_chid; 
        }
        else 
        {
            min_index = i;
        }
            
        check_min = heap[right_chid].first < heap[min_index].first;

        if(check_size(right_chid) && check_min == true)
        {
            //right
            min_index = right_chid;
        } 

        //swapping 
        bool min_found{min_index != i}; 

        if(min_found == true)
        {
            swap(heap[i], heap[min_index]);
            heapify(min_index);
        }
    }

    void balance_heap()
    {
        ll index = heap.size()-1;
        ll parent{0};

        if(!(index & 1)) 
            parent = (index/2)-1;
        else 
            parent = index/2;

        while(parent > -1 && compare(parent, index))
        {
            swap(heap[parent], heap[index]);

            index = parent;

            if(!(index & 1)) 
                parent =  (index/2)-1;
            else 
                parent = index/2;
        }
    }

    void insert(pair<ll, int> item)
    {
        heap.push_back(item);
        balance_heap();
    }

    pli getMin()
    {
        pli first_node = heap[0];
        size_t last_index = heap.size()-1;
        pli last_node = heap[last_index];

        ll minimum = first_node.first;
        int chunk_num = first_node.second;
        pli res = make_pair(minimum, chunk_num);

        swap(heap[0], heap[last_index]);

        heap.pop_back();
        heapify(0);

        return res;
    }
};

ll get_count(string input)
{
    ifstream file(input);
    string incoming_data = "";
    ll count{0};

    while (getline(file, incoming_data, ','))
    {
        count++;
    }

    return count;
}

int main(int argc, char *argv[])
{
    string input_file, output_file, incoming_data = "";
    ll num_count{0}, chunks{0}, k_count{0};
    int temp_file_count{1}, file_index{0};
    vll numbers;

    //Handling command line arguments
    if(argc != 3) 
    {
        cout << "Invalid number of arguments.." << endl;
        exit(0);
    }
    else
    {
        input_file = argv[1];
        output_file = argv[2];
    }

    //reading the input file
    ifstream file(input_file);

    //Calculating the numbers and chunks
    k_count = get_count(input_file);

    chunks = k_count/100000 + ((k_count%100000) == 0 ? 0 : 1);
    cout << "Chunks : " << chunks << endl;

    //Creating temporary chunk files
    while(getline(file, incoming_data,','))
    {
        ll number = stoll(incoming_data, nullptr, 10); 
        numbers.push_back(number);
        num_count++;
        
        if(num_count == 100000)
        {
            sort(numbers.begin(), numbers.end());

            string index = to_string(temp_file_count);
            string temp_chunk = "temp_chunk_" + index;

            FILE* write_chunk = fopen(temp_chunk.c_str(), "w");

            for(auto number : numbers)
                fprintf(write_chunk, "%lld ", number);

            fclose(write_chunk);

            num_count = 0;
            temp_file_count++;

            numbers.clear();
        }
    }

    size_t remaining_nums = numbers.size();

    //When chunk has less than CHUNKSIZE numbers
    if(remaining_nums > 0)
    {   
        sort(numbers.begin(), numbers.end());

        string index = to_string(temp_file_count);
        string temp_chunk = "temp_chunk_" + index;

        FILE* write_chunk = fopen(temp_chunk.c_str(), "w");

        for(auto number : numbers)
            fprintf(write_chunk, "%lld ", number);

        fclose(write_chunk);

        numbers.clear();
        num_count = 0;
        temp_file_count++;
    }

    temp_file_count--;  //total number of chunks => temp_file_count

    //Merging the chunks to form output
    min_heap heap;
    pli arr[temp_file_count];

    //opening and writing into the output file
    FILE* output_file_sorted = fopen(output_file.c_str(), "w");

    if(output_file_sorted)
    {
        int i{0}, num_count_chunk{1};
        FILE* temp_files[temp_file_count];

        //opening and maintaining all temp files
        for(; i < temp_file_count; i++)
        {
            string file_num = to_string(i+1);
            string temp_chunk = "temp_chunk_" + file_num;

            FILE* temp = fopen(temp_chunk.c_str(), "r");
            temp_files[i] = temp;
        }

        //reading from temp files
        i = 0;
        for(; i < temp_file_count; i++)
        {
            int read = fscanf(temp_files[i], "%lld ", &arr[i].first);

            if(read != -1)
                arr[i].second = i;
            else 
                break;
        }

        //inserting into min_heap
        i = 0;
        for(; i < temp_file_count; i++)
        {
            heap.insert(arr[i]);
        }

        //Printing onto the output file
        while(file_index != temp_file_count)
        {
            pli item = heap.getMin();
            ll num = item.first;
            int chunk_num = item.second;

            // fprintf(output_file_sorted, "%lld\n", num);

            if(num_count_chunk != k_count)
            {
                fprintf(output_file_sorted, "%lld,", num);   
            }
            else
            {
                fprintf(output_file_sorted, "%lld", num);
            }

            num_count_chunk++;
            pli newItem;

            int read = fscanf(temp_files[chunk_num], "%lld ", &newItem.first);

            if(read == 1)
            {
                newItem.second = chunk_num;
                heap.insert(newItem);
            }
            else 
                file_index += 1;
        }

        //removing temporary files created
        i=0;
        for(; i < temp_file_count; i++)
        { 
            fclose(temp_files[i]);
            string index = to_string(i+1);
            string chunk_file = "temp_chunk_" + index;
            remove(chunk_file.c_str());
        }

        fclose(output_file_sorted);
    }
    else
    {
        cout << "Error in writing to output file..\n";
    }

    return 0;
}