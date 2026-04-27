#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <ctime>

using namespace std;

// 坐标结构体，方便管理
struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

class Map {
public:
    int width, height;
    Point food;
    
    Map(int w, int h) : width(w), height(h) {
        srand(time(0));
        generate_food();
    }

    void generate_food() {
        // 随机生成食物位置
        food = { rand() % width, rand() % height };
    }

    // 检查坐标是否在地图内
    bool is_inside(Point p) {
        return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
    }
};

class Snake {
private:
    vector<Point> body;

public:
    Snake(int start_x, int start_y) {
        body.push_back({ start_x, start_y });
    }

    Point get_head() { return body.front(); }
    vector<Point> get_body() { return body; }

    // 核心算法：BFS 寻路
    Point find_next_step_bfs(Map& game_map) {
        Point head = get_head();
        Point target = game_map.food;

        if (head == target) return head;

        // BFS 必备：队列、访问标记、路径回溯记录
        queue<Point> q;
        q.push(head);

        // pre[x][y] 记录 (x,y) 是从哪个点走过来的，用于回溯路径
        vector<vector<Point>> pre(game_map.width, vector<Point>(game_map.height, { -1, -1 }));
        
        int dx[4] = { -1, 1, 0, 0 };
        int dy[4] = { 0, 0, -1, 1 };

        bool found = false;
        while (!q.empty()) {
            Point curr = q.front();
            q.pop();

            if (curr == target) {
                found = true;
                break;
            }

            for (int i = 0; i < 4; i++) {
                Point next = { curr.x + dx[i], curr.y + dy[i] };

                // 判定：在地图内、未访问过、且不是蛇身（简化处理，暂不考虑蛇身避障）
                if (game_map.is_inside(next) && pre[next.x][next.y].x == -1) {
                    pre[next.x][next.y] = curr;
                    q.push(next);
                }
            }
        }

        // 如果找到了路径，通过 pre 数组反向回溯到蛇头的下一步
        if (found) {
            Point step = target;
            while (!(pre[step.x][step.y] == head)) {
                step = pre[step.x][step.y];
            }
            return step;
        }

        // 没找到路径（比如被围死了），就随便往右走一步
        return { head.x + 1, head.y };
    }

    void move(Point next_step, bool ate_food) {
        body.insert(body.begin(), next_step);
        if (!ate_food) {
            body.pop_back();
        }
    }
};

class Game {
private:
    Map game_map;
    Snake player;
    int score;

public:
    Game() : game_map(10, 10), player(0, 0), score(0) {}

    void render() {
        // 简单的控制台渲染逻辑
        system("cls"); // 清屏 (Windows环境下)
        for (int i = 0; i < game_map.height; i++) {
            for (int j = 0; j < game_map.width; j++) {
                Point p = { j, i };
                if (p == player.get_head()) cout << "H"; // 蛇头
                else if (p == game_map.food) cout << "@"; // 食物
                else {
                    bool is_body = false;
                    for (auto b : player.get_body()) if (p == b) is_body = true;
                    cout << (is_body ? "o" : "."); 
                }
            }
            cout << endl;
        }
        cout << "得分: " << score << endl;
    }

    void run() {
        while (true) {
            render();
            Point next = player.find_next_step_bfs(game_map);
            
            bool ate = (next == game_map.food);
            if (ate) {
                score++;
                game_map.generate_food();
            }

            player.move(next, ate);

            // 死亡判定：撞墙
            if (!game_map.is_inside(next)) {
                cout << "游戏结束！最终得分: " << score << endl;
                break;
            }
            
            _sleep(200); // 暂停200毫秒，让肉眼能看清移动
        }
    }
};

int main() {
    Game myGame;
    myGame.run();
    return 0;
}