#include <vector>
#include <iostream>
#include <set>


using namespace std;

#define int long long


void run() {
    // https://codeforces.com/problemset/problem/20/C?locale=ru
    // Не требуется сделать оптимально быструю версию, поэтому если вы получили:
    //
    // Превышено ограничение времени на тесте 31
    //
    // То все замечательно и вы молодец.

    int n,m;
    std::cin >> n;
    std::cin >> m;

    vector<vector<pair<int,int>>> g(n);
    set<pair<int, int>> edges;
    for (int i = 0; i < m; ++i) {
        int ai, bi, w;
        std::cin >> ai >> bi >> w;

        ai -= 1;
        bi -= 1;
        g[ai].push_back({bi,w});
        g[bi].push_back({ai,w});
    }
    edges.insert({0,0});

    const int INF = 1e18;
    vector<int> d(n,INF);
    d[0] = 0;
    vector<int> p(n);
    while(!edges.empty()){
        pair<int, int> e = *edges.begin();
        edges.erase(e);
        int len = e.first, v = e.second;
        for(auto to: g[v]){
            if(to.second + len < d[to.first]){
                edges.erase({d[to.first], to.first});
                edges.insert({to.second + len, to.first});
                d[to.first] = to.second + len;
                p[to.first] = v;
            }
        }
    }
    if(d[n-1] == INF) cout<<-1;
    else{
        vector<int> ans;
        ans.reserve(n);
        ans.push_back(n);
        int cur = n - 1;
        while(p[cur] != 0){
            ans.push_back(p[cur] + 1);
            cur = p[cur];
        }ans.push_back(1);
        for(int i = ans.size()-1;i>=0;--i) cout<<ans[i]<<" ";
    }

}

int32_t main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    run();
}
