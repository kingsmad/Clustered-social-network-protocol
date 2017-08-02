/*************************************************************************
    > In god we trust
    > File Name: main.cpp
************************************************************************/
#include "src/socio_geo.h"

int main(int argc, char** argv) {
  freopen("../input.txt", "r", stdin);
  socio::Graph graph;
  graph.Init();

  int rounds = 100000;
  if (argv[1]) {
    sscanf(argv[1], "%d", &rounds);
  }

  printf("Running for total %d rounds\n", rounds);
  graph.Run(rounds);

  return 0;
}
