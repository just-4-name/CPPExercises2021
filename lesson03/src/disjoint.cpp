#include "disjoint.h"

#define ROOT -1 // объявили макрос (считайте константу) равный минус единице - чтобы не было "волшебной переменной", а была именованная константа "корень дерева"

using namespace std;


DisjointSet::DisjointSet(int size)
{
    parents = std::vector<int>(size);
    ranks = std::vector<int>(size);
    sizes = std::vector<int>(size);
    // TODO - заполните вектора так чтобы на этапе конструирования эта система непересекающихся множеств состояла из:
    // size элементов, каждый из которых состоит в своем множестве (а значит ссылка на родителя у него - на корень, т.е. на ROOT, а какой тогда ранк и размер у множества каждого элемента?)
    // TODO заполнить parents
    // TODO заполнить ranks
    // TODO заполнить sizes
    for(int i=0; i<size;++i){
        parents[i] = ROOT;
        sizes[i] = 1;
        ranks[i] = 1;
    }
}

int	DisjointSet::get_set(int element)
{
    // TODO по номеру элемента нужно переходя по ссылкам на родителя дойти до самого верхнего элемента,
    // номер этого корневого элемента - номер множества на данный момент (кто вверху тот и главный, множество названо в его честь)
    if(parents[element] == ROOT) return element;
    return parents[element] = get_set(parents[element]);
}

int DisjointSet::count_differents() const
{
    // TODO посчитать сколько разных множеств (подсказка: в каждом множестве ровно один корень, а корень - это тот у кого родитель = ROOT)
    int count = 0;
    for (size_t i = 0; i < this->parents.size(); i++) {
        if(parents[i] == ROOT) count++;
    }
    return count;
}

int DisjointSet::get_set_size(int element)
{
    // TODO сообщить сколько элементов в множестве, которому принадлежит данный элемент (да, это очень просто)
    return sizes[get_set(element)];
}

int	DisjointSet::union_sets(int element0, int element1)
{
    // TODO узнать корневые элементы этих двух элементов и посмотрев на них - решить,
    // кого из них подвесить к другому (тем самым объединить два множества)
    // при этом стоит подвешивать менее высокое дерево к более высокому (т.е. учитывая ранк),
    // а так же важно не забыть после переподвешивания у корневого элемента обновить ранк и размер множества
    int a = get_set(element0), b = get_set(element1);
    if(a == b) return a;
    if(ranks[a]>ranks[b]) swap(a,b);
    if(ranks[a] == ranks[b]) ranks[b]++;
    parents[a] = b;
    sizes[b] += sizes[a];
    return b; // вернуть номер объединенного множества
}
