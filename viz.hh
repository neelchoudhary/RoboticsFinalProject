#ifndef VIZ_H
#define VIZ_H


class Node {
  public:
    Node()
        : wallHits(0), emptyHits(0), prior(0), currL(0), inverseSensorModel(0)
    {};

    Node(int _wallHits, int _emptyHits)
        : wallHits(_wallHits), emptyHits(_emptyHits)
    {}

    int wallHits;
    int emptyHits;
    double prior;
    double currL;
    double inverseSensorModel;
};

int viz_run(int argc, char **argv);
void viz_hit(float range, float angle);
void viz_drawMap(Node map[][300]);

#endif
